#include "mainwindow.h"
#include "./ui_mainwindow.h"




MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ВРУЧНУЮ связываем кнопки с кодом
    connect(ui->btnEncrypt, &QPushButton::clicked, this, &MainWindow::on_btnEncrypt_clicked);
    connect(ui->btnDecrypt, &QPushButton::clicked, this, &MainWindow::on_btnDecrypt_clicked);
    connect(ui->surprise, &QPushButton::clicked, this, &MainWindow::on_surprise_clicked);
}


MainWindow::~MainWindow() { delete ui; }

// УНИВЕРСАЛЬНЫЙ ЦЕЗАРЬ (Работает и для шифрования, и для расшифровки)
QString MainWindow::caesarCipher(QString text, int step, bool encrypt) {
    QString result = "";
    int shift = encrypt ? step : -step;
    shift = (shift % 26 + 26) % 26; // Нормализация сдвига

    for (QChar c : text) {
        if (c.isLetter()) {
            char base = c.isUpper() ? 'A' : 'a';
            int code = c.toLatin1() - base;
            int shiftedCode = (code + shift) % 26;
            result.append(QChar(shiftedCode + base));
        } else {
            result.append(c);
        }
    }
    return result;
}

// КНОПКА ENCRYPT
void MainWindow::on_btnEncrypt_clicked() {
    QString input = ui->teInput->toPlainText();
    int key = ui->leKey->text().toInt();

    if (ui->comboMethods->currentIndex() == 0) { // Caesar
        QString out = caesarCipher(input, key, true);
        ui->teOutput->setPlainText(out); // ВЫВОДИМ В НОВОЕ ПОЛЕ
    }
}

// КНОПКА DECRYPT
void MainWindow::on_btnDecrypt_clicked() {
    QString input = ui->teInput->toPlainText();
    int key = ui->leKey->text().toInt();

    if (ui->comboMethods->currentIndex() == 0) {
        QString out = caesarCipher(input, key, false);
        ui->teOutput->setPlainText(out);
    }
}

void MainWindow::on_surprise_clicked() {
    this->setWindowTitle("Easter Egg Found! <><");
}

