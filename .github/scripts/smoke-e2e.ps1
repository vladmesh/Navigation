# Smoke e2e for Ship.exe with the runtime set from deploy/win64-mingw.
#
#   smoke-e2e.ps1 -Exe build\release\Ship.exe -Runtime deploy\win64-mingw -Work e2e
#
# 1. Copies Ship.exe and every *.dll from -Runtime (keeping platforms/, imageformats/)
#    into a clean directory.
# 2. Runs "Ship.exe --smoke" with PATH reduced to system directories, once with
#    QT_QPA_PLATFORM=offscreen and once with QT_QPA_PLATFORM=windows.
#    Pass: exit code 0, "SMOKE OK" in stderr, no plugin/DLL load errors, and every
#    Qt / MinGW module of the live process is loaded from the test directory.
# 3. Minimality: removes each file of the set in turn and expects the run to fail.
param(
    [Parameter(Mandatory = $true)][string]$Exe,
    [Parameter(Mandatory = $true)][string]$Runtime,
    [string]$Work = "e2e",
    [int]$TimeoutSec = 30,
    [switch]$SkipMinimality
)
$ErrorActionPreference = "Stop"

# Suppress "System Error: xxx.dll was not found" dialogs; the mode is inherited by children.
Add-Type -Namespace Win32 -Name ErrMode -MemberDefinition '[DllImport("kernel32.dll")] public static extern uint SetErrorMode(uint mode);'
[void][Win32.ErrMode]::SetErrorMode(0x0001 -bor 0x0002 -bor 0x8000)

$RuntimeFull = (Resolve-Path $Runtime).Path
$ExeFull = (Resolve-Path $Exe).Path
$SetFiles = Get-ChildItem -Path $RuntimeFull -Recurse -File -Filter *.dll |
    ForEach-Object { $_.FullName.Substring($RuntimeFull.Length + 1) } | Sort-Object

function New-TestDir([string]$dir, [string]$skip) {
    if (Test-Path $dir) { Remove-Item -Recurse -Force $dir }
    New-Item -ItemType Directory -Force $dir | Out-Null
    Copy-Item $ExeFull $dir
    foreach ($rel in $SetFiles) {
        if ($rel -eq $skip) { continue }
        $dst = Join-Path $dir $rel
        New-Item -ItemType Directory -Force (Split-Path $dst) | Out-Null
        Copy-Item (Join-Path $RuntimeFull $rel) $dst
    }
    return (Resolve-Path $dir).Path
}

function Invoke-Smoke([string]$dir, [string]$platform) {
    $saved = @{ PATH = $env:PATH; QT_QPA_PLATFORM = $env:QT_QPA_PLATFORM; QT_PLUGIN_PATH = $env:QT_PLUGIN_PATH; QT_QPA_FONTDIR = $env:QT_QPA_FONTDIR }
    $env:PATH = "$env:SystemRoot\System32;$env:SystemRoot;$env:SystemRoot\System32\Wbem"
    $env:QT_QPA_PLATFORM = $platform
    $env:QT_PLUGIN_PATH = $null
    # offscreen does not use the Windows font API; point it at the system fonts.
    $env:QT_QPA_FONTDIR = "$env:SystemRoot\Fonts"
    $env:QT_FORCE_STDERR_LOGGING = "1"
    $err = Join-Path $dir "stderr.txt"
    $out = Join-Path $dir "stdout.txt"
    try {
        $p = Start-Process -FilePath (Join-Path $dir "Ship.exe") -ArgumentList "--smoke" -WorkingDirectory $dir `
            -RedirectStandardError $err -RedirectStandardOutput $out -NoNewWindow -PassThru
        $null = $p.Handle  # keep the handle so ExitCode is available
        $modules = @{}
        $sw = [Diagnostics.Stopwatch]::StartNew()
        while (-not $p.HasExited -and $sw.Elapsed.TotalSeconds -lt $TimeoutSec) {
            # Process.Modules is cached by .NET; Refresh() re-reads the module list.
            try { $p.Refresh(); foreach ($m in $p.Modules) { $modules[$m.FileName.ToLower()] = $true } } catch {}
            Start-Sleep -Milliseconds 150
        }
        $timedOut = -not $p.HasExited
        if ($timedOut) { Stop-Process -Id $p.Id -Force; $p.WaitForExit() }
        $code = $p.ExitCode
    } finally {
        foreach ($k in $saved.Keys) { Set-Item "env:$k" $saved[$k] }
    }
    $log = ""
    if (Test-Path $err) { $log += Get-Content -Raw $err }
    if (Test-Path $out) { $log += Get-Content -Raw $out }
    return [pscustomobject]@{ Code = $code; TimedOut = $timedOut; Log = "$log"; Modules = @($modules.Keys | Sort-Object) }
}

$failures = 0
$fullDir = New-TestDir $Work ""
Write-Host "Test directory: $fullDir"
Write-Host "Runtime set ($($SetFiles.Count) files):"; $SetFiles | ForEach-Object { Write-Host "  $_" }

foreach ($platform in @("offscreen", "windows")) {
    Write-Host ""
    Write-Host "=== Ship.exe --smoke, QT_QPA_PLATFORM=$platform"
    $r = Invoke-Smoke $fullDir $platform
    Write-Host $r.Log
    Write-Host ("exit code: {0} (0x{1:X8}), timed out: {2}" -f $r.Code, $r.Code, $r.TimedOut)
    $ok = $true
    if ($r.TimedOut -or $r.Code -ne 0) { Write-Host "::error::[$platform] Ship.exe exit code $($r.Code)"; $ok = $false }
    if ($r.Log -notmatch "SMOKE OK") { Write-Host "::error::[$platform] no 'SMOKE OK' in log"; $ok = $false }
    $bad = ($r.Log -split "`n") | Where-Object { $_ -match "(?i)could not (load|find)|cannot (load|find)|failed to load|not found|no Qt platform plugin" }
    if ($bad) { Write-Host "::error::[$platform] load errors in log:"; $bad | ForEach-Object { Write-Host "  $_" }; $ok = $false }

    $qtMods = $r.Modules | Where-Object { $_ -match '\\(qt6[a-z0-9]+|libgcc_s_seh-1|libstdc\+\+-6|libwinpthread-1|q[a-z0-9]+)\.dll$' -and $_ -notmatch '\\windows\\' }
    Write-Host "Qt/MinGW modules loaded by the process:"
    $qtMods | ForEach-Object { Write-Host "  $_" }
    $foreign = $qtMods | Where-Object { -not $_.StartsWith($fullDir.ToLower()) }
    if ($foreign) { Write-Host "::error::[$platform] modules loaded outside the test directory: $($foreign -join ', ')"; $ok = $false }
    $expectedPlugin = if ($platform -eq "windows") { "platforms\qwindows.dll" } else { "platforms\qoffscreen.dll" }
    foreach ($need in @("qt6core.dll", "qt6gui.dll", "qt6widgets.dll", "libstdc++-6.dll", $expectedPlugin, "imageformats\qsvg.dll", "imageformats\qico.dll")) {
        if (-not ($qtMods -contains (Join-Path $fullDir.ToLower() $need))) { Write-Host "::error::[$platform] $need was not loaded"; $ok = $false }
    }
    if ($ok) { Write-Host "[$platform] PASS" } else { $failures++ }
}

if (-not $SkipMinimality) {
    Write-Host ""
    Write-Host "=== Minimality: each file removed in turn must break the run"
    foreach ($rel in $SetFiles) {
        $platform = if ($rel -eq "platforms\qwindows.dll") { "windows" } else { "offscreen" }
        $dir = New-TestDir "$Work-without" $rel
        $r = Invoke-Smoke $dir $platform
        $broken = $r.TimedOut -or $r.Code -ne 0 -or ($r.Log -notmatch "SMOKE OK")
        $first = (($r.Log -split "`n") | Where-Object { $_ -match "SMOKE |qt\.qpa|(?i)cannot|could not" } | Select-Object -First 1)
        if (-not $first -and $r.Code -eq 0xC0000135) { $first = "STATUS_DLL_NOT_FOUND" }
        $line = "without {0,-28} [{1,-9}] exit=0x{2:X8} -> {3}  {4}" -f $rel, $platform, $r.Code, $(if ($broken) { "fails (required)" } else { "STILL RUNS" }), $first
        Write-Host $line
        if (-not $broken) { Write-Host "::error::$rel is not required for the smoke run"; $failures++ }
    }
    Remove-Item -Recurse -Force "$Work-without"
}

if ($failures -gt 0) { Write-Host "::error::smoke-e2e: $failures check(s) failed"; exit 1 }
Write-Host "smoke-e2e: all checks passed"
