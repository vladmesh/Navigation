#ifndef CIRCULARBUFFER_H
#define CIRCULARBUFFER_H

#include <QGeoCoordinate>

//кольцевой буфер для расчета координат
/**
 * @class CircularBuffer
 * @brief Кольцевой буфер для хранения координат и расчета среднего азимута.
 *
 * Этот класс реализует кольцевой буфер фиксированного размера, предназначенный для хранения географических координат объектов с соединением.
 * Обеспечивает добавление новых координат и расчет среднего азимута между самой старой и самой новой точками в буфере.
 */
class CircularBuffer {
public:
    /**
     * @brief Конструктор класса CircularBuffer.
     * @param size Максимальное количество элементов в буфере.
     */
    CircularBuffer(size_t size) : size(size), buffer(size), currentIndex(0), isFull(false) {}

    /**
     * @brief Добавляет новое значение в буфер.
     *
     * Если буфер полон, новое значение заменяет самое старое.
     * @param coord Координата типа QGeoCoordinate для добавления в буфер.
     */
    void add(const QGeoCoordinate& coord) {
        buffer[currentIndex] = coord;
        currentIndex = (currentIndex + 1) % size;

        if (currentIndex == 0) {
            isFull = true;
        }
    }


    /**
     * @brief Возвращает текущее количество элементов в буфере.
     * @return Количество элементов, хранящихся в буфере.
     */
    size_t count() const {
        return isFull ? size : currentIndex;
    }

    /**
     * @brief Возвращает координату по индексу.
     *
     * Индекс интерпретируется циклично, т.е. если `index` больше текущего размера буфера,
     * будет использован `index % size`.
     * @param index Индекс координаты для получения.
     * @return Ссылка на координату типа QGeoCoordinate.
     */
    const QGeoCoordinate& get(size_t index) const {
        return buffer[index % size];
    }

    /**
     * @brief Вычисляет азимут между самой старой и самой новой точками в буфере.
     *
     * Если буфер содержит менее двух точек, возвращает -1.0.
     * Азимут вычисляется в градусах и преобразуется в радианы перед возвратом.
     * @return Средний азимут в радианах или -1.0, если данных недостаточно.
     */
    double calculateAzimuth() const {
        if (count() < 2) {
            return -1.0;
        }

        size_t oldestIndex = isFull ? currentIndex : 0;
        size_t newestIndex = (currentIndex == 0) ? size - 1 : currentIndex - 1;

        const QGeoCoordinate& oldestCoord = get(oldestIndex);
        const QGeoCoordinate& newestCoord = get(newestIndex);

        double azimuth = oldestCoord.azimuthTo(newestCoord);

        if (azimuth >= 0.0) {
            return qDegreesToRadians(azimuth);
        } else {
            return -1.0;
        }
    }

private:
    size_t size; ///< Максимальное количество элементов в буфере.
    QVector<QGeoCoordinate> buffer; ///< Вектор для хранения координат.
    size_t currentIndex; ///< Текущий индекс для добавления нового элемента.
    bool isFull; ///< Флаг, указывающий, что буфер заполнен.
};
#endif // CIRCULARBUFFER_H
