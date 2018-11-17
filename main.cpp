#include "mainwindow.h"
#include <QApplication>
#include "QUdev.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QUdev udev;

    if (udev.initialize())
    {
        udev.setSubsystem("tty");
        udev.setParentSubsystem("usb");
        udev.enumerate();

        udev.monitor();
    }

    return a.exec();
}
