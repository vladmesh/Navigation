#ifndef CUSTOMDOUBLESPINBOX_H
#define CUSTOMDOUBLESPINBOX_H

#include <QDoubleSpinBox>
#include <QValidator>
#include <QRegularExpressionValidator>


class CustomDoubleSpinBox : public QDoubleSpinBox {
    Q_OBJECT

public:
    explicit CustomDoubleSpinBox(QWidget* parent = nullptr) :
        QDoubleSpinBox(parent), validator(new QRegularExpressionValidator(this))
    {
        validator->setRegularExpression(QRegularExpression("-?\\d{1,}(?:[,.]\\d*)?"));
    }
    virtual QValidator::State validate(QString & text, int & pos) const {
        bool tmp;
        text.toDouble(&tmp);
        if (tmp)
            text = QString::number(std::floor(text.toDouble()*10)/10);
            if (text.contains('.')) {
                text.replace('.', ',');
            }
        return validator->validate(text,pos);
    }

private:
    QRegularExpressionValidator* validator = nullptr;
};

#endif // CUSTOMDOUBLESPINBOX_H
