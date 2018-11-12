// http://www.signal11.us/oss/udev/
#include "QUdev.h"
#include <QDebug>

#define DEBUG_INFO


QUdev::QUdev(QObject *parent) : QObject(parent)
{
    initialize();
}

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
    udev_unref(ptrUdev);
    ptrUdev = nullptr;
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

    struct udev_enumerate *ptrEnum = nullptr;
    struct udev_list_entry *ptrDevicesList = nullptr;
    struct udev_list_entry *ptrDeviceEntry = nullptr;
    struct udev_device *udevDevice = nullptr;
    struct udev_device *udevParentDevice = nullptr;

    ptrEnum = udev_enumerate_new(ptrUdev);
    if (nullptr != ptrEnum)
    {
        udev_enumerate_add_match_subsystem(ptrEnum, getSubsystem().toLocal8Bit().constData());
        udev_enumerate_scan_devices(ptrEnum);

        ptrDevicesList = udev_enumerate_get_list_entry(ptrEnum);
        if (nullptr != ptrDevicesList)
        {
            udev_list_entry_foreach(ptrDeviceEntry, ptrDevicesList)
            {
                if (nullptr != ptrDeviceEntry)
                {
                    const char* path = nullptr;
                    path = udev_list_entry_get_name(ptrDeviceEntry);
                    if (nullptr != path)
                    {
                        udevDevice = udev_device_new_from_syspath(ptrUdev, path);
                        if (nullptr != udevDevice)
                        {
                            udevParentDevice = udev_device_get_parent_with_subsystem_devtype(udevDevice, getParentSubsystem().toLocal8Bit().constData(), "usb_device");
                            if (nullptr != udevParentDevice)
                            {
                                UdevDevice device;
                                device.path         = QString(udev_device_get_devnode(udevDevice));
                                device.vendorId     = QString(udev_device_get_sysattr_value(udevParentDevice, "idVendor"));
                                device.productId    = QString(udev_device_get_sysattr_value(udevParentDevice, "idProduct"));
                                device.product      = QString(udev_device_get_sysattr_value(udevParentDevice, "product"));
                                device.manufacturer = QString(udev_device_get_sysattr_value(udevParentDevice, "manufacturer"));
                                device.serial       = QString(udev_device_get_sysattr_value(udevParentDevice, "serial"));
#ifdef DEBUG_INFO
                                qDebug() << endl;
                                qDebug() << "Product:     " << qPrintable(device.product);
                                qDebug() << "Path:        " << qPrintable(device.path);
                                qDebug() << "Manufacturer:" << qPrintable(device.manufacturer);
                                qDebug() << "Serial:      " << qPrintable(device.serial);
                                qDebug() << "Vendor id:   " << qPrintable(device.vendorId);
                                qDebug() << "Product id:  " << qPrintable(device.productId);
#endif
                                devicesList.append(device);
                            }
                        }
                    }
                }
            }
        }
        udev_enumerate_unref(ptrEnum);
        ptrEnum = nullptr;
    }

    return devicesList;
}

//int main (void)
//{
//    struct udev_device *dev = nullptr;
//    struct udev_monitor *udev_monitor = nullptr;
//    int udev_monitor_fd = 0;


//    // Init library
//    struct udev *udev = nullptr;
//    if (0 != init_udev(&udev))
//    {
//        return 0;
//    }

//    // Set up a udev monitor to monitor
//    udev_monitor = udev_monitor_new_from_netlink(udev, "udev");
//    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "tty", NULL);
//    udev_monitor_enable_receiving(udev_monitor);
//    // Get the file descriptor (fd) for the monitor.This fd will get passed to select()S
//    udev_monitor_fd = udev_monitor_get_fd(udev_monitor);


//    // Create a list of the devices in the tty subsystem
//    struct udev_enumerate* udev_enumeration = udev_enumerate_new(udev);
//    // Set filter
//    udev_enumerate_add_match_subsystem(udev_enumeration, "tty");
//    // Enumerate
//    udev_enumerate_scan_devices(udev_enumeration);

//    //
//    struct udev_list_entry *device_list_entry = nullptr;
//    struct udev_list_entry *device_list_entries = udev_enumerate_get_list_entry(udev_enumeration);

//    udev_list_entry_foreach(device_list_entry, device_list_entries)
//    {
//        const char *path;
//        struct udev_device *usb_dev;

//        //Get the filename of the /sys entry for the device and create a udev_device object (dev) representing it
//        path = udev_list_entry_get_name(device_list_entry);
//        dev = udev_device_new_from_syspath(udev, path);

//        //  The device pointed to by dev contains information about the tty device. In order to get information about the
//        // USB device, get the parent device with the subsystem/devtype pair of "usb"/"usb_device".
//        // This will be several levels up the tree, but the function will find it.
//        usb_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
//        if (usb_dev)
//        {
//            printf("   Node: %s\n", udev_device_get_devnode(dev));
//            printf("   idVendor: %s\n",     udev_device_get_sysattr_value(usb_dev,"idVendor"));
//            printf("   idProduct: %s\n",    udev_device_get_sysattr_value(usb_dev, "idProduct"));
//            printf("   Manufacturer: %s\n", udev_device_get_sysattr_value(usb_dev,"manufacturer"));
//            printf("   Product: %s\n",      udev_device_get_sysattr_value(usb_dev,"product"));
//            printf("   Serial: %s\n",       udev_device_get_sysattr_value(usb_dev, "serial"));

//            /* unref only the original dev, not the parent device;
//           the latter will be unreferenced automatically if set. */
//            udev_device_unref(dev);
//        }
//    }
//    // Free the enumerator object
//    udev_enumerate_unref(udev_enumeration);



//    /* Begin polling for udev events. Events occur when devices
//       attached to the system are added, removed, or change state.
//       udev_monitor_receive_device() will return a device
//       object representing the device which changed and what type of
//       change occured.

//       The select() system call is used to ensure that the call to
//       udev_monitor_receive_device() will not block.

//       The monitor was set up earler in this file, and monitoring is
//       already underway.

//       This section will run continuously, calling usleep() at the end
//       of each pass. This is to demonstrate how to use a udev_monitor
//       in a non-blocking way. */
//    while (1)
//    {
//        // Set up the call to select(). In this case, select() will only operate on a single file descriptor, the one
//        // associated with our udev_monitor. Note that the timeval object is set to 0, which will cause select() to not block.
//        fd_set fds;
//        struct timeval tv;
//        int ret;

//        FD_ZERO(&fds);
//        FD_SET(udev_monitor_fd, &fds);
//        tv.tv_sec = 0;
//        tv.tv_usec = 0;

//        ret = select(udev_monitor_fd+1, &fds, NULL, NULL, &tv);

//        /// Check if our file descriptor has received data. */
//        if (ret > 0 && FD_ISSET(udev_monitor_fd, &fds))
//        {
//            // Make the call to receive the device. select() ensured that this will not block.
//            dev = udev_monitor_receive_device(udev_monitor);
//            if (dev)
//            {
//                struct udev_device *usb_dev;
//                usb_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
//                if (usb_dev)
//                {
//                    printf("\n\nDevice\n");
//                    printf("   Node: %s\n", udev_device_get_devnode(dev));
//                    printf("   Subsystem: %s\n", udev_device_get_subsystem(dev));
//                    printf("   Devtype: %s\n", udev_device_get_devtype(dev));
//                    printf("   Action: %s\n", udev_device_get_action(dev));

//                   // From here, we can call get_sysattr_value() for each file in the device's /sys entry. The strings passed into these functions (idProduct, idVendor, serial, etc.)
//                   // correspond directly to the files in the /sys directory which
//                   // represents the USB device. Note that USB strings are Unicode, UCS2 encoded, but the strings returned from udev_device_get_sysattr_value() are UTF-8 encoded.

//                    printf("   idVendor: %s\n",     udev_device_get_sysattr_value(usb_dev,"idVendor"));
//                    printf("   idProduct: %s\n",    udev_device_get_sysattr_value(usb_dev, "idProduct"));
//                    printf("   Manufacturer: %s\n", udev_device_get_sysattr_value(usb_dev,"manufacturer"));
//                    printf("   Product: %s\n",      udev_device_get_sysattr_value(usb_dev,"product"));
//                    printf("   Serial: %s\n",       udev_device_get_sysattr_value(usb_dev, "serial"));
//                }
//                // unref only the original dev, not the parent device, the latter will be unreferenced automatically if set. */
//                udev_device_unref(dev);
//            }
//        }

//        usleep(250*1000);
//        fflush(stdout);
//    }

//    udev_monitor_unref(udev_monitor);
//    udev_unref(udev);

//    return 0;
//}

