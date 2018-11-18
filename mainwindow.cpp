#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QObject>
#include <QVector>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udev = new QUdev(this);
    QObject::connect(udev, SIGNAL(udevDeviceFound(QUdev::UdevDevice)), this, SLOT(onUdevDeviceFount(QUdev::UdevDevice)));

    if (udev->initialize())
    {
        udev->setSubsystem("tty");
        udev->setParentSubsystem("usb");
    }
}

void MainWindow::onUdevDeviceFount(QUdev::UdevDevice device)
{
    QString s;
    s.append("Found udev device in monitoring mode:").append('\n').append('\n');
    if (device.isValid)
    {
        s.append("Product:     ").append(device.product).append('\n');
        s.append("Path:        ").append(device.path).append('\n');
        s.append("Manufacturer:").append(device.manufacturer).append('\n');
        s.append("Serial:      ").append(device.serial).append('\n');
        s.append("Vendor id:   ").append(device.vendorId).append('\n');
        s.append("Product id:  ").append(device.productId).append('\n');

        QString action;
        switch (int(device.action))
        {
        case QUdev::UDEV_DEVICE_ACTION_ADD:
            action = "UDEV_DEVICE_ACTION_ADD";
            break;
        case QUdev::UDEV_DEVICE_ACTION_CHANGE:
            action = "UDEV_DEVICE_ACTION_CHANGE";
            break;
        case QUdev::UDEV_DEVICE_ACTION_OFFLINE:
            action = "UDEV_DEVICE_ACTION_OFFLINE";
            break;
        case QUdev::UDEV_DEVICE_ACTION_ONLINE:
            action = "UDEV_DEVICE_ACTION_ONLINE";
            break;
        case QUdev::UDEV_DEVICE_ACTION_REMOVE:
            action = "UDEV_DEVICE_ACTION_REMOVE";
            break;
        case QUdev::UDEV_DEVICE_ACTION_UNKNOWN:
        default:
            action = "UDEV_DEVICE_ACTION_UNKNOWN";
            break;
        }
        s.append("Action:      ").append(action).append('\n');
        s.append('\n');
    }

    ui->plainTextEdit->appendPlainText(s);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_buttonEnumerate_clicked()
{
    QString s;

    s.append("Enumerated udev devices:").append('\n').append('\n');
    ui->plainTextEdit->appendPlainText(s);
    s.clear();

    QVector<QUdev::UdevDevice> devices = udev->getUdevDeviceList();
    for (int i = 0; i < devices.count(); i++)
    {
        if (devices.at(i).isValid)
        {
            s.clear();
            s.append("Product:     ").append(devices.at(i).product).append('\n');
            s.append("Path:        ").append(devices.at(i).path).append('\n');
            s.append("Manufacturer:").append(devices.at(i).manufacturer).append('\n');
            s.append("Serial:      ").append(devices.at(i).serial).append('\n');
            s.append("Vendor id:   ").append(devices.at(i).vendorId).append('\n');
            s.append("Product id:  ").append(devices.at(i).productId).append('\n');
            QString action;
            switch (int(devices.at(i).action))
            {
            case QUdev::UDEV_DEVICE_ACTION_ADD:
                action = "UDEV_DEVICE_ACTION_ADD";
                break;
            case QUdev::UDEV_DEVICE_ACTION_CHANGE:
                action = "UDEV_DEVICE_ACTION_CHANGE";
                break;
            case QUdev::UDEV_DEVICE_ACTION_OFFLINE:
                action = "UDEV_DEVICE_ACTION_OFFLINE";
                break;
            case QUdev::UDEV_DEVICE_ACTION_ONLINE:
                action = "UDEV_DEVICE_ACTION_ONLINE";
                break;
            case QUdev::UDEV_DEVICE_ACTION_REMOVE:
                action = "UDEV_DEVICE_ACTION_REMOVE";
                break;
            case QUdev::UDEV_DEVICE_ACTION_UNKNOWN:
            default:
                action = "UDEV_DEVICE_ACTION_UNKNOWN";
                break;
            }
            s.append("Action:      ").append(action).append('\n');
            s.append('\n');
            ui->plainTextEdit->appendPlainText(s);
        }
    }
}

void MainWindow::on_buttonStartMonitoring_clicked()
{
    udev->startMonitoring();
}

void MainWindow::on_buttonStopMonitoring_clicked()
{
    udev->stopMonitoring();
}
