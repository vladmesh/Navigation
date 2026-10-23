#include "p190headerchanger.h"

P190HeaderChanger::P190HeaderChanger(QWidget *parent)
    : QWidget(parent)
{
    // Задаем моноширинный шрифт
    QFont font("Courier New");
    font.setStyleHint(QFont::Monospace);

    // Создаем текстовое поле
    textEdit = new QPlainTextEdit(this);
    textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
    textEdit->setFont(font);

    // Загружаем текст из файла
    loadFile(QCoreApplication::applicationDirPath() + "/ModifiedSurveyInfo.txt");

    // Создаем строку ввода для Insert Line
    auto *inputLineEdit = new QLineEdit(this);

    // Создаем кнопку "Insert Line"
    auto *insertButton = new QPushButton("Insert Line", this);
    connect(insertButton, &QPushButton::clicked, this, [this, inputLineEdit]() {
        insertLine(inputLineEdit->text());
    });

    // Создаем кнопку "Apply" для сохранения файла
    auto *applyButton = new QPushButton("Apply", this);
    connect(applyButton, &QPushButton::clicked, this, &P190HeaderChanger::saveFile);

    // Новая кнопка для генерации заголовков
    auto *generateHeadersButton = new QPushButton("Generate Headers", this);
    connect(generateHeadersButton, &QPushButton::clicked, this, &P190HeaderChanger::generateHeaders);

    // Устанавливаем layout
    auto *layout = new QVBoxLayout(this);
    layout->addWidget(textEdit);
    layout->addWidget(inputLineEdit);
    layout->addWidget(insertButton);
    layout->addWidget(applyButton);
    layout->addWidget(generateHeadersButton);

    setLayout(layout);
    setFixedSize(600, 400); // Фиксированный размер окна
    setWindowTitle("P1/90 Header Changer");

    // Инициализируем объект p190HeaderAnalyzer.
    // Предполагается, что конструктор p190HeaderAnalyzer корректно создаёт и инициализирует свой vault.
}

void P190HeaderChanger::generateHeaders()
{
    // Предполагаем, что headerAnalyzer->vault->dataVault является контейнером элементов (например, QList<QPointer<FixedItem>>)
    // Проходим по всем элементам в dataVault
    headerAnalyzer.analizeVault();
    for (const auto &item : headerAnalyzer.Vault->ItemsVault) {
        // Получаем строку заголовка для текущего элемента
        QString header = headerAnalyzer.getHeaderStringFromItem(item);

        // Если строка не пуста и её еще нет в textEdit, то добавляем
        if (!header.isEmpty() && !textEdit->toPlainText().contains(header)) {
            insertLine(header);
        }
    }
}


void P190HeaderChanger::insertLine(QString line)
{
    if (!line.startsWith("H")) return;

    int headerCode = getHeaderCode(line);
    content = textEdit->toPlainText();
    QStringList lines = content.split('\n');


    int pos = 0;
    while (pos < lines.size() && headerCode >= getHeaderCode(lines[pos++]));
    lines.insert(pos-1, line);

    content.clear();
    for (auto i:lines) {
        if (i.isEmpty()) continue;
        if (i.length() < 80) {
            line = line.leftJustified(80, ' ');
        }
        content += i.left(80) + '\n'; // Учитываем ширину строки
    }

    textEdit->setPlainText(content);
}


void P190HeaderChanger::loadFile(const QString &fileName) {


    if (headerProcessor.loadFile(fileName).isEmpty()) {
        QMessageBox::warning(this, "Error", "Cannot open file for reading.\nReload programm");
    } else {
        updateTextEdit();
    }
}

void P190HeaderChanger::saveFile() {
    QFile file(QCoreApplication::applicationDirPath() + "/ModifiedSurveyInfo.txt");
    qDebug() <<__FUNCTION__ << QCoreApplication::applicationDirPath() + "/ModifiedSurveyInfo.txt";
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        content = textEdit->toPlainText();
        QStringList lines = content.split('\n');

        for (QString &line : lines) {
            if (line.isEmpty()) continue;
            if (line.length() < 80) {
                line = line.leftJustified(80, ' '); // Дополняем пробелами до 80 символов
            }
            out << line.left(80) << '\n'; // Усекаем до 80 символов
        }
    } else {
        QMessageBox::warning(this, "Error", "Cannot open file for writing.");
    }
    delete this;
}

void P190HeaderChanger::setVault(ItemsStorage *NewVault)
{
    this->headerAnalyzer.setVault(NewVault);
}

inline int P190HeaderChanger::getHeaderCode(const QString &line)
{
    bool resOk;
    int headerCode = line.mid(1,4).toInt(&resOk);
    return resOk? headerCode:0;
}

void P190HeaderChanger::updateTextEdit()
{
    textEdit->clear();
    textEdit->setPlainText(content);
}
