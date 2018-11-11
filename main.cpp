#include "mainwindow.h"
#include <QApplication>
#include "QUdev.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QUdev udev;

    udev.setSubsystem("tty");
    udev.setParentSubsystem("usb");
    udev.enumerate();


    return a.exec();
}
