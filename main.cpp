#include <libudev.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <unistd.h>


int init_udev(udev** udev)
{
    *udev = udev_new();

    return  (udev != nullptr);
}

//int scan()
//{

//}

int main (void)
{
    struct udev_enumerate *udev_enumeration = nullptr;
    struct udev_device *dev = nullptr;
    struct udev_monitor *udev_monitor = nullptr;
    int udev_monitor_fd = 0;


    // Init library
    struct udev *udev = nullptr;
    if (!init_udev(&udev))
    {
        return 0;
    }

    // Set up a udev monitor to monitor
    udev_monitor = udev_monitor_new_from_netlink(udev, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(udev_monitor, "tty", NULL);
    udev_monitor_enable_receiving(udev_monitor);
    // Get the file descriptor (fd) for the monitor.This fd will get passed to select()S
    udev_monitor_fd = udev_monitor_get_fd(udev_monitor);


    // Create a list of the devices in the tty subsystem
    udev_enumeration = udev_enumerate_new(udev);
    // Set filter
    udev_enumerate_add_match_subsystem(udev_enumeration, "tty");
    // Enumerate
    udev_enumerate_scan_devices(udev_enumeration);



    struct udev_list_entry *device_list_entries = nullptr;
    struct udev_list_entry *device_list_entry = nullptr;

    device_list_entries = udev_enumerate_get_list_entry(udev_enumeration);
    /* For each item enumerated, print out its information.
       udev_list_entry_foreach is a macro which expands to
       a loop. The loop will be executed for each member in
       devices, setting dev_list_entry to a list entry
       which contains the device's path in /sys. */


    udev_list_entry_foreach(device_list_entry, device_list_entries)
    {
        const char *path;
        struct udev_device *usb_dev;

        //Get the filename of the /sys entry for the device and create a udev_device object (dev) representing it
        path = udev_list_entry_get_name(device_list_entry);
        dev = udev_device_new_from_syspath(udev, path);

        // usb_device_get_devnode() returns the path to the device node itself in /dev. */


        /* The device pointed to by dev contains information about
           the hidraw device. In order to get information about the
           USB device, get the parent device with the
           subsystem/devtype pair of "usb"/"usb_device". This will
           be several levels up the tree, but the function will find
           it.*/
        usb_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
        if (!usb_dev)
        {
            //printf("Unable to find parent usb device.");
            //exit(1);
        }
        else
        {

            printf("Device Node Path: %s\n", udev_device_get_devnode(dev));

            /* From here, we can call get_sysattr_value() for each file
           in the device's /sys entry. The strings passed into these
           functions (idProduct, idVendor, serial, etc.) correspond
           directly to the files in the /sys directory which
           represents the USB device. Note that USB strings are
           Unicode, UCS2 encoded, but the strings returned from
           udev_device_get_sysattr_value() are UTF-8 encoded. */
            printf("  VID/PID: %s %s\n",
                   udev_device_get_sysattr_value(usb_dev,"idVendor"),
                   udev_device_get_sysattr_value(usb_dev, "idProduct"));
            printf("  %s\n  %s\n",
                   udev_device_get_sysattr_value(usb_dev,"manufacturer"),
                   udev_device_get_sysattr_value(usb_dev,"product"));
            printf("  serial: %s\n",
                   udev_device_get_sysattr_value(usb_dev, "serial"));
  printf("   Action: %s\n", udev_device_get_action(dev));
            /* unref only the original dev, not the parent device;
           the latter will be unreferenced automatically if set. */
            udev_device_unref(dev);
        }
    }
    /* Free the enumerator object */
    udev_enumerate_unref(udev_enumeration);

    /* Begin polling for udev events. Events occur when deevices
       attached to the system are added, removed, or change state.
       udev_monitor_receive_device() will return a device
       object representing the device which changed and what type of
       change occured.

       The select() system call is used to ensure that the call to
       udev_monitor_receive_device() will not block.

       The monitor was set up earler in this file, and monitoring is
       already underway.

       This section will run continuously, calling usleep() at the end
       of each pass. This is to demonstrate how to use a udev_monitor
       in a non-blocking way. */
    while (1)
    {
        // Set up the call to select(). In this case, select() will only operate on a single file descriptor, the one
        // associated with our udev_monitor. Note that the timeval object is set to 0, which will cause select() to not block.
        fd_set fds;
        struct timeval tv;
        int ret;

        FD_ZERO(&fds);
        FD_SET(udev_monitor_fd, &fds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        ret = select(udev_monitor_fd+1, &fds, NULL, NULL, &tv);

        /// Check if our file descriptor has received data. */
        if (ret > 0 && FD_ISSET(udev_monitor_fd, &fds))
        {
            // Make the call to receive the device. select() ensured that this will not block.
            dev = udev_monitor_receive_device(udev_monitor);
            if (dev)
            {
                struct udev_device *usb_dev;
                usb_dev = udev_device_get_parent_with_subsystem_devtype(dev, "usb", "usb_device");
                if (!usb_dev)
                {
                    //printf("Unable to find parent usb device.");
                    //exit(1);
                }
                else
                {
                    printf("\n\nDevice\n");
                    printf("   Node: %s\n", udev_device_get_devnode(dev));
                    printf("   Subsystem: %s\n", udev_device_get_subsystem(dev));
                    printf("   Devtype: %s\n", udev_device_get_devtype(dev));
                    printf("   Action: %s\n", udev_device_get_action(dev));

                   // From here, we can call get_sysattr_value() for each file in the device's /sys entry. The strings passed into these functions (idProduct, idVendor, serial, etc.)
                   // correspond directly to the files in the /sys directory which
                   // represents the USB device. Note that USB strings are Unicode, UCS2 encoded, but the strings returned from udev_device_get_sysattr_value() are UTF-8 encoded.

                    printf("   idVendor: %s\n",     udev_device_get_sysattr_value(usb_dev,"idVendor"));
                    printf("   idProduct: %s\n",    udev_device_get_sysattr_value(usb_dev, "idProduct"));
                    printf("   Manufacturer: %s\n", udev_device_get_sysattr_value(usb_dev,"manufacturer"));
                    printf("   Product: %s\n",      udev_device_get_sysattr_value(usb_dev,"product"));
                    printf("   Serial: %s\n",       udev_device_get_sysattr_value(usb_dev, "serial"));
                }
                // unref only the original dev, not the parent device, the latter will be unreferenced automatically if set. */
                udev_device_unref(dev);
            }
            else
            {
                printf("No Device from receive_device(). An error occured.\n");
            }
        }

        usleep(250*1000);
        fflush(stdout);
    }

    udev_monitor_unref(udev_monitor);
    udev_unref(udev);

    return 0;
}
