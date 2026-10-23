#ifndef ITEMSLOADER_H
#define ITEMSLOADER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include "fabric.h"
#include "itemsstorage.h"

/**
 * @class ItemsLoader
 * @brief Класс для загрузки, создания и сохранения объектов и соединений из JSON-файлов.
 *
 * Класс `ItemsLoader` отвечает за чтение и запись элементов `FixedItem`, наследников и соединений в формате JSON.
 * Реализует паттерн Singleton для обеспечения единственного экземпляра класса.
 */
class ItemsLoader : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор класса ItemsLoader.
     * @param parent Указатель на родительский объект.
     */
    explicit ItemsLoader(QObject *parent = nullptr);

    /**
     * @brief Получает единственный экземпляр `ItemsLoader`.
     * @return Ссылка на экземпляр класса.
     */
    static ItemsLoader& instance();

    /**
     * @brief Конвертирует JSON-объект в объект `Connection`.
     * @param obj JSON-объект, содержащий информацию о соединении.
     * @return Указатель на созданный объект `Connection`.
     */
    static Connection* jsonToConnection(const QJsonObject &obj);

    /**
     * @brief Читает элементы из JSON-файла и создаёт объекты `FixedItem` или его наследников
     * @param file Указатель на объект файла JSON.
     * @return Вектор указателей на загруженные объекты `FixedItem`.
     */
    static QVector<QPointer<FixedItem>> readFromJSON(QFile* file);


    /**
     * @brief Записывает элементы в JSON-объект.
     * @param ItemsVector Вектор указателей на элементы `FixedItem` или его наследников.
     * @return JSON-объект, содержащий данные элементов.
     */
    static QJsonObject writeToJSON(QVector<QPointer<FixedItem>> ItemsVector);


    // /**
    //  * @brief Читает элемент из JSON-объекта и создаёт объекты `FixedItem` или его наследников
    //  * @param obj Указатель на объект JSON.
    //  * @return Вектор указателей на загруженные объекты `FixedItem`.
    //  */
    // static QPointer<FixedItem> readOneObjFromJSONObject(QJsonObject obj);


    /**
     * @brief Конвертирует объект наследников `Connection` в JSON.
     * @param conn Указатель на объект `Connection`.
     * @return JSON-объект, представляющий данные соединения.
     */
    static QJsonObject connectionToJson(Connection *conn);


    /**
     * @brief Сохраняет JSON-объект в файл.
     * @param jsonObject JSON-объект для сохранения.
     * @param filePath Путь к файлу для сохранения.
     * @return `true`, если сохранение прошло успешно, `false` в противном случае.
     */
    static bool saveJsonObjectToFile(QJsonObject &jsonObject, QString &filePath);

    /**
     * @brief Создаёт объект `FixedItem` на основе данных из JSON.
     * @param jsonObject JSON-значение, представляющее элемент.
     * @return Указатель на созданный объект `FixedItem`.
     */
    static FixedItem* createItemFromJson(QJsonValue jsonObject);

    /**
     * @brief Устанавливает приоритет создания для элементов.
     * @param ItemsVault Вектор указателей на элементы `FixedItem`.
     */
    static void setCreationPriority(QVector<QPointer<FixedItem>> &ItemsVault);

    static QDir dirpath;

    static ItemsStorage vault;
signals:
         // Определите здесь сигналы, если необходимо
};

#endif // ITEMSLOADER_H
