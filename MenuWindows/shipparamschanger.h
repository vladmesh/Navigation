#ifndef SHIPPARAMCHANGER_H_
#define SHIPPARAMCHANGER_H_

#include <QDialog>
#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QDoubleSpinBox>
#include <QVBoxLayout>
#include <QLabel>
#include <QGraphicsPolygonItem>
#include <QPolygonF>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QtMath>

class ShipParamsChangerModel : public QObject {
    Q_OBJECT
public:
    explicit ShipParamsChangerModel(QObject* parent = nullptr)
        : QObject(parent)
    {}

    bool loadFromFile() {
        QFile file("./shipEdges.txt");
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
            return false;

        coordinates.clear();
        QTextStream in(&file);
        while (!in.atEnd()) {
            int x, y;
            in >> x >> y;
            if (x == 0 && y == 0)
                continue;
            coordinates.append(QPointF(x, y));
        }
        return true;
    }

    bool saveToFile() const {
        QFile file("./shipEdges.txt");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        for (const auto& coord : coordinates) {
            if (coord.x() == 0 && coord.y() == 0)
                continue;
            out << coord.x() << " " << coord.y() << "\n";
        }
        return true;
    }

    void addCoordinate(int x, int y) {
        coordinates.append(QPointF(x, y));
    }

    void addCoordinate(const QPolygonF& figure) {
        coordinates.clear();
        for (const auto& point : figure) {
            // Если оба значения равны 0, пропускаем точку
            if (point.x() == 0 && point.y() == 0)
                continue;
            coordinates.append(point);
        }
    }

    QPolygonF getCoordinates() const {
        return coordinates;
    }

    void setCoordinates(const QPolygonF& newCoords) {
        coordinates = newCoords;
    }

    QString toText() const {
        QString text;
        for (const auto& coord : coordinates) {
            text += QString::number(coord.x()) + " " + QString::number(coord.y()) + "\n";
        }
        return text;
    }

    void resetToDefault() {
        coordinates.clear();
        coordinates<< QPointF(-50, -20)
                    << QPointF(-50, 20)
                    << QPointF(55, 20)
                    << QPointF(75, 0)
                    << QPointF(55, -20);
    }

private:
    QPolygonF coordinates;
};

////////////////////////////////////////////////////////////////////////

class ShipParamsChanger : public QDialog {
    Q_OBJECT

public:
    explicit ShipParamsChanger(QWidget* parent = nullptr, QGraphicsPolygonItem* MyShip = nullptr)
        : QDialog(parent), model(new ShipParamsChangerModel(this))
    {
        setWindowTitle("Set up ship parameters");

        textEdit = new QPlainTextEdit(this);
        saveButton = new QPushButton("Save Ship Contour", this);
        defaultButton = new QPushButton("Set to Default", this); // Новая кнопка
        heightLabel = new QLabel("Main Deck Height", this);
        heightSpinBox = new QDoubleSpinBox(this);

        heightSpinBox->setRange(0, 10);
        heightSpinBox->setSuffix(" m");
        heightSpinBox->setToolTip("Main Deck Height");

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->addWidget(textEdit);
        layout->addWidget(heightLabel);
        layout->addWidget(heightSpinBox);
        layout->addWidget(saveButton);
        layout->addWidget(defaultButton); // Добавляем кнопку в UI

        connect(saveButton, &QPushButton::clicked, this, &ShipParamsChanger::saveContour);
        connect(defaultButton, &QPushButton::clicked, this, &ShipParamsChanger::setToDefault); // Привязываем слот

        QPolygonF coors;
        if (MyShip) {
            coors = MyShip->polygon();
            model->addCoordinate(coors);
        } else {
            qDebug() << "Warning: MyShip is nullptr, polygon not loaded.";
        }
        textEdit->setPlainText(model->toText());
    }

    ~ShipParamsChanger() override {
        qDebug() << __FUNCTION__;
    }

    QPolygonF readPointsFromTextEdit()
    {
        QPolygonF points;
        QString text = textEdit->toPlainText();
        QStringList lines = text.split('\n', Qt::SkipEmptyParts);

        for (const QString& line : lines) {
            QStringList tokens = line.split(' ', Qt::SkipEmptyParts);
            if (tokens.size() != 2)
                continue;

            bool ok1 = false, ok2 = false;
            double d1 = tokens[0].toDouble(&ok1);
            double d2 = tokens[1].toDouble(&ok2);

            if (!ok1 || !ok2)
                continue;

            int x = static_cast<int>(d1);
            int y = static_cast<int>(d2);

            if (x == 0 && y == 0)
                continue;

            if (qAbs(x) > 300 || qAbs(y) > 300)
                continue;

            points.append(QPointF(x, y));
        }
        model->setCoordinates(points);
        return points;
    }


    void closeEvent(QCloseEvent* event) override {
        saveContour();
    }

    float getMainDeckHeight() const;

    QPolygonF getFigureCoordinates() {
        return model ? model->getCoordinates() : QPolygonF();
    }

private slots:
    void saveContour() {
        model->setCoordinates(readPointsFromTextEdit());
        model->saveToFile();
        mainDeckHeight = heightSpinBox->value();
        qDebug() << __FUNCTION__;
        this->accept();
    }

    void setToDefault() { // Новый слот
        qDebug() << "Resetting to default values...";
        model->resetToDefault();
        textEdit->setPlainText(model->toText());
        heightSpinBox->setValue(0);
    }

private:
    void loadFile() {
        if (model->loadFromFile()) {
            textEdit->setPlainText(model->toText());
        }
    }

    ShipParamsChangerModel* model;
    QPlainTextEdit* textEdit;
    QPushButton* saveButton;
    QPushButton* defaultButton; // Новая кнопка
    QLabel* heightLabel;
    QDoubleSpinBox* heightSpinBox;
    float mainDeckHeight = 0;
};

inline float ShipParamsChanger::getMainDeckHeight() const {
    return mainDeckHeight;
}

#endif // SHIPPARAMCHANGER_H_
