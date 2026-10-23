# Windows runtime set for Ship.exe (Qt 6.10.2, MinGW 64-bit)

The smallest set of DLLs that `Ship.exe` needs to start on a clean Windows machine
when it is built with **Qt 6.10.2 `win64_mingw` + MinGW 13.1.0 x86_64 (`tools_mingw1310`)**.
It is used by the `windows-mingw-smoke` job in `.github/workflows/pr-ci.yml`: the files are
copied next to `Ship.exe` and the app is started with `--smoke`.

These files are ABI-compatible only with a MinGW build of the same Qt version.
They do not work with the MSVC build (`windows-msvc` job, Qt 6.8.3).

## Contents

| File | Needed because | Source |
|---|---|---|
| `Qt6Core.dll` | imported by Ship.exe | owner's Qt 6.10.2 MinGW install |
| `Qt6Gui.dll` | imported by Ship.exe | owner |
| `Qt6Widgets.dll` | imported by Ship.exe | owner |
| `Qt6Network.dll` | imported by Ship.exe (QUdpSocket, QTcpSocket) | owner |
| `Qt6SerialPort.dll` | imported by Ship.exe (QSerialPort) | owner |
| `Qt6Positioning.dll` | imported by Ship.exe (QGeoCoordinate) | owner |
| `Qt6Core5Compat.dll` | imported by Ship.exe (QRegExp) | owner |
| `Qt6Svg.dll` | dependency of `imageformats/qsvg.dll` (splash logo `:/Data/logoGeodevice.svg`) | owner |
| `libgcc_s_seh-1.dll`, `libstdc++-6.dll`, `libwinpthread-1.dll` | MinGW 13.1.0 C++ runtime, imported by Ship.exe and Qt | official Qt 6.10.2 `win64_mingw` (aqt), `bin/` |
| `platforms/qwindows.dll` | platform plugin for a normal desktop start | official Qt 6.10.2 `win64_mingw` (aqt), `plugins/` |
| `platforms/qoffscreen.dll` | platform plugin for headless runs (`QT_QPA_PLATFORM=offscreen`) | aqt |
| `imageformats/qsvg.dll` | loads the SVG splash logo | aqt |
| `imageformats/qico.dll` | loads the window icon `:/Data/icon.ico` | aqt |

All files are byte-identical to the official Qt 6.10.2 `win64_mingw` packages; the CI job checks
this with sha256 against the Qt it installs.

Not included, not needed for start: `styles/` (the app sets the built-in `Fusion` style),
`iconengines/qsvgicon.dll` (icons are created from `.ico`), `tls/`, `networkinformation/`
(no HTTPS / QNetworkInformation), `Qt6Location.dll` (listed in `Ship.pro`, but no symbol is used,
so the linker drops the import), `opengl32sw.dll`, `d3dcompiler_47.dll`, translations.
Without `qwindows.dll` the app runs only with `QT_QPA_PLATFORM=offscreen`.

## Usage

```
copy deploy\win64-mingw\*.dll  <dir with Ship.exe>\
xcopy /E deploy\win64-mingw\platforms     <dir>\platforms\
xcopy /E deploy\win64-mingw\imageformats  <dir>\imageformats\
<dir>\Ship.exe --smoke
```

`--smoke` exits after 3 s with code 0 if the splash logo, the icon and the main window were
created, and prints `SMOKE OK` to stderr. Without the flag the app starts as usual.

To update the set for another Qt version: install the matching `win64_mingw` Qt with aqt and
take the same file names from `bin/` and `plugins/`. Run `.github/scripts/smoke-e2e.ps1`;
it also checks that removing any single file breaks the start.

## License

Qt libraries are used under the GNU LGPL v3 (https://www.qt.io/licensing/,
https://doc.qt.io/qt-6/lgpl.html). They are unmodified binaries of Qt 6.10.2; the source is
available at https://download.qt.io/official_releases/qt/6.10/6.10.2/. The MinGW runtime
DLLs are distributed under the GCC Runtime Library Exception and the MinGW-w64 license.
