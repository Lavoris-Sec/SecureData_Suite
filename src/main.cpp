#include "ui/MainWindow.h"
#include <QApplication>
#include <QLabel>
#include <QMovie>
#include <QTimer>
#include <QtGlobal>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    const bool enableSplash = qEnvironmentVariableIsEmpty("SDS_NO_SPLASH");
    QLabel splashLabel;
    QMovie movie(":/cat-eating-chips-windows-xp-meme.gif");

    if (enableSplash && movie.isValid()) {
        splashLabel.setWindowFlags(Qt::SplashScreen | Qt::FramelessWindowHint);
        splashLabel.setAttribute(Qt::WA_TranslucentBackground);
        splashLabel.setMovie(&movie);
        splashLabel.setScaledContents(true);
        splashLabel.resize(560, 420);
        movie.start();
        splashLabel.show();
        app.processEvents();
    }

    sds::MainWindow window;

    if (enableSplash && movie.isValid()) {
        QTimer::singleShot(2000, &splashLabel, &QWidget::close);
        QTimer::singleShot(2000, &window, &QWidget::show);
    } else {
        window.show();
    }

    return app.exec();
}
