#ifndef SENDERSETTINGSMENU_H
#define SENDERSETTINGSMENU_H

#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QHostAddress>


class SenderSettingsMenu : public QDialog
{
public:
    SenderSettingsMenu();
    int portNumber;
    QHostAddress IP;
private:

    void saveParams();
    void closeEvent(QCloseEvent* event) override {
        saveParams();
    }


    QLineEdit *ipLineEdit;
    QSpinBox *portSpinBox;
    QCheckBox *sendingCheckBox;
};

#endif // SENDERSETTINGSMENU_H
