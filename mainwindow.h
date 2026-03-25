#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnEncrypt_clicked(); // Слот для кнопки ENCRYPT
    void on_btnDecrypt_clicked(); // Слот для кнопки DECRYPT
    void on_surprise_clicked();    // Слот для пасхалки

private:
    Ui::MainWindow *ui;
    // Наша функция-движок для Цезаря
    QString caesarCipher(QString text, int step, bool encrypt);
};
#endif // MAINWINDOW_H