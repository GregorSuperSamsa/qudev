// http://www.signal11.us/oss/udev/
#include "QUdev.h"
#include <QDebug>
#include <QtConcurrent/QtConcurrentRun>

#define DEBUG_INFO


QUdev::QUdev(QObject *parent) : QObject(parent) {}

QUdev::~QUdev()
{
    uninitialize();
}

bool QUdev::initialize()
{
    ptrUdev = udev_new();
    return (nullptr != ptrUdev);
}

void QUdev::uninitialize()
{
    if (ptrUdev != nullptr)
    {
        udev_unref(ptrUdev);
        ptrUdev = nullptr;
    }
}

void QUdev::setSubsystem(const QString subsystem)
{
    this->subsystem = subsystem;
}

QString QUdev::getSubsystem() const
{
    return subsystem;
}

void QUdev::setParentSubsystem(const QString parentSubsystem)
{
    this->parentSubsystem = parentSubsystem;
}

QString QUdev::getParentSubsystem() const
{
    return parentSubsystem;
}

QVector<QUdev::UdevDevice> QUdev::enumerate()
{
    QVector <UdevDevice> devicesList(0);
    struct udev_enumerate  *ptrUdevEnumeration = nullptr;
    struct udev_list_entry *ptrUdevDeviceList  = nullptr;
    struct udev_list_entry *ptrUdevDeviceEntry = nullptr;
    struct udev_device     *ptrUdevDevice      = nullptr;


    ptrUdevEnumeration = udev_enumerate_new(ptrUdev);
    if (nullptr != ptrUdevEnumeration)
    {
        udev_enumerate_add_match_subsystem(ptrUdevEnumeration, getSubsystem().toLatin1().constData());
        udev_enumerate_scan_devices(ptrUdevEnumeration);

        ptrUdevDeviceList = udev_enumerate_get_list_entry(ptrUdevEnumeration);
        if (nullptr != ptrUdevDeviceList)
        {
            udev_list_entry_foreach(ptrUdevDeviceEntry, ptrUdevDeviceList)
            {
                if (nullptr != ptrUdevDeviceEntry)
                {
                    const char* path = nullptr;
                    path = udev_list_entry_get_name(ptrUdevDeviceEntry);
                    if (nullptr != path)
                    {
                        ptrUdevDevice = udev_device_new_from_syspath(ptrUdev, path);
                        UdevDevice device = getUdevDeviceAttributes(ptrUdevDevice);
                        if (device.isValid)
                        {
                            devicesList.append(device);
                        }
                    }
                }
            }
        }

        udev_enumerate_unref(ptrUdevEnumeration);
        ptrUdevEnumeration = nullptr;
    }
    return devicesList;
}

QUdev::UdevDevice QUdev::getUdevDeviceAttributes(udev_device* ptrUdevDevice)
{
    UdevDevice device;

    if (nullptr != ptrUdevDevice)
    {
        struct udev_device *ptrUdevParentDevice = nullptr;

        ptrUdevParentDevice = udev_device_get_parent_with_subsystem_devtype(ptrUdevDevice, getParentSubsystem().toLocal8Bit().constData(), "usb_device");
        if (nullptr != ptrUdevParentDevice)
        {
            device.action       = getUdevDeviceAction(ptrUdevDevice);
            device.path         = QString(udev_device_get_devnode(ptrUdevDevice));
            device.vendorId     = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "idVendor")).toUpper();
            device.productId    = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "idProduct")).toUpper();
            device.vendorId     = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "idVendor")).toUpper();
            device.productId    = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "idProduct")).toUpper();
            device.product      = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "product"));
            device.manufacturer = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "manufacturer"));
            device.serial       = QString(udev_device_get_sysattr_value(ptrUdevParentDevice, "serial"));
            device.isValid = true;
#ifdef DEBUG_INFO
            qDebug() << endl;
            qDebug() << "Product:     " << qPrintable(device.product);
            qDebug() << "Path:        " << qPrintable(device.path);
            qDebug() << "Manufacturer:" << qPrintable(device.manufacturer);
            qDebug() << "Serial:      " << qPrintable(device.serial);
            qDebug() << "Vendor id:   " << qPrintable(device.vendorId);
            qDebug() << "Product id:  " << qPrintable(device.productId);
            qDebug() << "Action:      " << device.action;
#endif
        }
    }

    return device;
}

QUdev::UDEV_DEVICE_ACTION_TYPE QUdev::getUdevDeviceAction(udev_device* ptrUdevDevice)
{
    UDEV_DEVICE_ACTION_TYPE actionType = UDEV_DEVICE_ACTION_UNKNOWN;

    if (nullptr != ptrUdevDevice)
    {
        QString actionString = QString(udev_device_get_action(ptrUdevDevice)).trimmed().toUpper();
        if (!actionString.isEmpty())
        {
            if ("ADD" == actionString)
                actionType = UDEV_DEVICE_ACTION_ADD;
            else if ("REMOVE" == actionString)
                actionType = UDEV_DEVICE_ACTION_REMOVE;
            else if ("CHANGE" == actionString)
                actionType = UDEV_DEVICE_ACTION_CHANGE;
            else if ("ONLINE" == actionString)
                actionType = UDEV_DEVICE_ACTION_ONLINE;
            else if ("OFFLINE" == actionString)
                actionType = UDEV_DEVICE_ACTION_OFFLINE;
        }
    }

    return actionType;
}

void QUdev::monitor()
{
    QObject::connect(this, SIGNAL(matchFound(QString)), this, SLOT(onMatchFound(QString)));

    QtConcurrent::run([=]
    {
        struct udev_device *dev;
        struct udev_monitor *mon;

        mon = udev_monitor_new_from_netlink(ptrUdev, "udev");
        udev_monitor_filter_add_match_subsystem_devtype(mon, "tty", NULL);
        udev_monitor_enable_receiving(mon);

        while (1)
        {
            dev = udev_monitor_receive_device(mon);
            if (nullptr != dev)
            {
                getUdevDeviceAttributes(dev);

                udev_device_unref(dev);
            }

            QThread::usleep(1000*1000);
        }
    });

}

void QUdev::onMatchFound(QString s)
{
    qDebug() << s;
}
