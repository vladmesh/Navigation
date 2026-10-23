#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include <QMutex>
#include <QMutexLocker>
#include <QDir>
#include <QStandardPaths>
#include <QTextEdit>
#include <QThread>
#include <QQueue>

// #define logmsg(msg) Logger::instance().logMessage(LogMessage(Info, msg))
#define logmsg(log) Logger::instance().addLogMsg(log);


/**
 * @class Logger
 * @brief Логгер для ведения журнала сообщений.
 *
 * Этот класс позволяет записывать сообщения в файл, отображать их в консоль
 * или окно для логов. Поддерживает многопоточную обработку сообщений.
 */
class Logger : public QObject
{
    Q_OBJECT
public:
    /**
     * @enum LogLevel
     * @brief Уровни логирования.
     */
    enum LogLevel {
        Debug,
        Info,
        Warning,
        Error
    };
    /**
     * @struct LogMessage
     * @brief Сообщение для логирования.
     *
     * Содержит указатель на вызывающий объект, уровень логирования и текст сообщения.
     */
    struct LogMessage{
        QObject* caller;
        LogLevel level;
        QString msg;
        LogMessage(LogLevel lvl, QString msg) : caller(nullptr), level(lvl), msg(msg) {};
        LogMessage(QObject* caller, LogLevel lvl, QString msg) : caller(caller), level(lvl), msg(msg) {};
    };

    Q_ENUM(LogLevel)

    /**
     * @brief Получение единственного экземпляра логгера.
     *
     * Использует паттерн одиночка (Singleton).
     * @return Ссылка на экземпляр Logger.
     */
    static Logger& instance();

    /**
     * @brief Логирование сообщения.
     *
     * @param log Сообщение для записи.
     */
    void logMessage(LogMessage log);

    //инициализирующие функции
    /**
     * @brief Создает файл для записи логов.
     */
    void createLogFile();
    /**
     * @brief Создает файл для записи логов.
     */
    void setPath(const QDir &newPath);
    /**
     * @brief Добавляет сообщение в очередь логов.
     *
     * @param log Текст сообщения.
     */
    void addLogMsg(QString log);
    /**
     * @brief Уничтожение экземпляра Logger.
     */
    void suicide();
private:
    Logger();
    ~Logger();

    //мультипоточность
    QMutex mutex; ///< Мьютекс для синхронизации доступа к логам.
    QThread logThread; ///< Поток для обработки сообщений.
    bool stopLogging; ///< Флаг для остановки потока.
    void run(); ///< Основной цикл обработки сообщений.

    //для работы с файлами
    QFile* logFile= nullptr; ///< Указатель на файл логов.
    QTextStream* logStream= nullptr; ///< Поток для записи в файл.
    QDir path = QDir(); ///< Путь для сохранения файлов.

    //для работы с сообщениями
    void appendTextToWindow(QString text); ///< Добавляет текст в окно логов.
    QQueue<LogMessage> logQueue; ///< Очередь сообщений для обработки.
    bool consoleOutput; ///< Флаг для вывода сообщений в консоль.
    QTextEdit* logWindow = nullptr; ///< Окно для отображения логов.
    int maxLines; ///< Максимальное количество строк в окне.
    void processQueue(); ///< Обрабатывает очередь сообщений.

    //бог знает зачем
    QString logLevelToString(LogLevel level); ///< Преобразование уровней логов в строки

    //suicide

signals:
};



#endif // LOGGER_H
