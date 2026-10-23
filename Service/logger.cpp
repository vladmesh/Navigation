#include "logger.h"

Logger::Logger() {

    consoleOutput = false;
    stopLogging = false;

#ifdef QT_DEBUG
    consoleOutput = true;
#endif

    // Создаем окно для вывода логов
    logWindow = new QTextEdit();
    logWindow->setReadOnly(true);  // Окно только для чтения
    logWindow->setWindowTitle("Log");
    logWindow->resize(400,250);
    // logWindow->show();            // Показать окно
    logWindow->move(0, 210);

    connect(&logThread, &QThread::started, this, &Logger::run);
    moveToThread(&logThread);
    logThread.start();
    // logWindow->hide();//прячем логгер пока
}

Logger::~Logger()
{
    // logStream->flush();
    // logFile->close();
    stopLogging = true;
    logThread.quit();
    logThread.wait();

    if (logFile) {
        logFile->close();
        delete logFile;
    }
    if (logStream) {
        delete logStream;
    }
}

void Logger::setPath(const QDir &newPath)
{
    path = newPath;
}

void Logger::addLogMsg(QString text)
{
    LogMessage log(LogLevel::Info, text);
    logQueue.enqueue(log);
}

Logger &Logger::instance()
{
    static Logger instance;
    if (!instance.logThread.isRunning()) {
        instance.logThread.start();
    }
    return instance;
}

void Logger::createLogFile()
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QString fileName = QString("%1_%2_%3_%4.log")
                           .arg(currentDateTime.time().hour())
                           .arg(currentDateTime.time().minute(), 2, 10, QChar('0'))  // Добавляем ведущий ноль для минут
                           .arg(currentDateTime.date().day(),2, 10, QChar('0'))
                           .arg(currentDateTime.date().month(),2, 10, QChar('0'));
    logFile = new QFile(path.absolutePath() + "/"+ fileName);
    logFile->open(QIODevice::WriteOnly);
    logStream = new QTextStream(logFile);

    QMutexLocker locker(&mutex);
}

void Logger::appendTextToWindow(QString text)
{
    QString curText = text + "\n" + logWindow->toPlainText();
    QStringList lines = curText.split("\n");
    if (lines.size() > maxLines) {
        lines = lines.mid(0, maxLines);
        logWindow->setText(lines.join("\n"));
    }
    logWindow->moveCursor(QTextCursor::Start);
}

void Logger::processQueue()
{
    QMutexLocker locker(&mutex);
    while (!logQueue.isEmpty()) {
        LogMessage log = logQueue.dequeue();
        logMessage(log);
    }

}

void Logger::run()
{
    while (!stopLogging) {
        processQueue();
        // QThread::sleep(1);
    }
}


void Logger::logMessage(LogMessage log)
{
    QMutexLocker locker(&mutex);
    QString callerName = "";
    if (log.caller != nullptr) {
        callerName = log.caller->objectName();
    }
    QString logtext = QString("%1 %2\n%3 %4").arg(callerName)
        .arg(QDateTime::currentDateTime().
            toString("yyyy-MM-dd HH:mm:ss"))
        .arg(logLevelToString(log.level))
        .arg(log.msg);
    if (logFile->isOpen()) {
        *logStream << logtext << "/n";
        logStream->flush();
    }
    if (consoleOutput) {
        qDebug() << logtext;
    }

    appendTextToWindow(logtext);
}


QString Logger::logLevelToString(LogLevel level)
{
    switch (level) {
    case 0:
        return "Debug";
    case 1:
        return "Info";
    case 2:
        return "Warning";
    case 3:
        return "Error";
    default:
        return "Hiring me was an error";
    }
}

void Logger::suicide()
{
    delete this;
}

