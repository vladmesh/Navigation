#ifndef CONNECTIONUI_H
#define CONNECTIONUI_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QEvent>
#include <QCloseEvent>


namespace Ui {
class ConnectionUI;
}

class ConnectionUI : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectionUI(QWidget *parent = nullptr);
    ~ConnectionUI();

    void setLabelText(const QString &text);
    void setSignalQualityLabel(const QString &text, const QColor &color);
    QPushButton* getPushButton();
    void closeEvent(QCloseEvent* event) override;

private:
    Ui::ConnectionUI *ui;
};

#endif // CONNECTIONUI_H
