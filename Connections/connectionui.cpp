#include "connectionui.h"
#include "ui_connectionui.h"

ConnectionUI::ConnectionUI(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ConnectionUI)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_DeleteOnClose);
}

ConnectionUI::~ConnectionUI()
{
    qDebug() << __FUNCTION__;
    delete ui;
}

void ConnectionUI::setLabelText(const QString &text)
{
    ui->label->setText(text);
}

void ConnectionUI::setSignalQualityLabel(const QString &text, const QColor &color)
{
    ui->label_2->setText(text);
    QPalette palette = ui->label_2->palette();
    palette.setColor(QPalette::Window, color);
    ui->label_2->setAutoFillBackground(true);
    ui->label_2->setPalette(palette);
}

QPushButton* ConnectionUI::getPushButton()
{
    return ui->pushButton;
}

void ConnectionUI::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}
