#include "mainwindow.h"
#include <QApplication>


MainWindow* gMainWindowPointer = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    gMainWindowPointer = &w;
    w.show();

    return a.exec();
}
