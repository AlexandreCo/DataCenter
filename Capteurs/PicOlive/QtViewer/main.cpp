#include <QtGui/QApplication>
#include <QSplashScreen>
#include "mainwindow.h"
#include "qdebug.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSplashScreen *splash = new QSplashScreen;
    splash->setPixmap(QPixmap(":/img/meteo.png"));
    splash->show();
    MainWindow w;
    QString sStyleSheet = " QTabWidget::pane { /* The tab widget frame */        border-top: 2px solid #C2C7CB;    }";

    //w.showMaximized();
    
    w.setStyleSheet(sStyleSheet);
    w.show();
    // Loaded modules
    splash->showMessage("Loaded modules", Qt::AlignLeft, Qt::white);
   // Establishing connections
    splash->showMessage("Established connections", Qt::AlignLeft, Qt::white);
    splash->finish(&w);
    delete splash;
    return a.exec();
}
