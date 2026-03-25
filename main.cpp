#include "mainwindow.h"
#include <QApplication>
#include <QSplashScreen>
#include <QMovie>
#include <QLabel>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // Создаем окно-метку для гифки
    QLabel *splashLabel = new QLabel();
    splashLabel->setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
    splashLabel->setAttribute(Qt::WA_TranslucentBackground);

    QMovie *movie = new QMovie(":/cat-eating-chips-windows-xp-meme.gif"); // ПРОВЕРЬ ПУТЬ ТУТ
    splashLabel->setMovie(movie);
    splashLabel->setScaledContents(true);
    splashLabel->resize(500, 400);

    movie->start();
    splashLabel->show();

    a.processEvents(); // Отрисовываем немедленно

    MainWindow w;

    // Закрываем кота и показываем окно через 3 сек
    QTimer::singleShot(3000, splashLabel, &QWidget::close);
    QTimer::singleShot(3000, &w, &QWidget::show);

    return a.exec();
}