#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // Qt 6.x中高DPI缩放默认启用，不再需要手动设置

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
