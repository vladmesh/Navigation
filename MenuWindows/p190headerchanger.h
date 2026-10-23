#ifndef P190HEADERCHANGER_H
#define P190HEADERCHANGER_H

#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>
#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QLineEdit>

#include "p190headerprocessor.h"
#include "p190headeranalyzer.h"

class P190HeaderChanger : public QWidget
{
    Q_OBJECT
public:
    explicit P190HeaderChanger(QWidget *parent = nullptr);
    void insertLine(QString line);
    void setVault(ItemsStorage* NewVault);

    void generateHeaders();
private slots:
    void loadFile(const QString &fileName);
    void saveFile();

private:
    P190HeaderProcessor headerProcessor;
    p190HeaderAnalyzer headerAnalyzer;

    inline int getHeaderCode(const QString &line);
    QPlainTextEdit *textEdit;
    QString& content = headerProcessor.content;

    void updateTextEdit();


signals:
};

#endif // P190HEADERCHANGER_H
