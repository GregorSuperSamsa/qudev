#include "qudev_device.h"
#include "qudev_context.h"

#include <libudev.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <QVariantMap>


QudevDevice buildDevice(const QudevContext&, udev_device* d)
{
    const auto toQString = [](const char* s) {
        return QString::fromLocal8Bit(s ? s : "");
    };

    QudevDevice device;

    device.syspath   = toQString(udev_device_get_syspath(d));
    device.devnode   = toQString(udev_device_get_devnode(d));
    device.subsystem = toQString(udev_device_get_subsystem(d));
    device.devtype   = toQString(udev_device_get_devtype(d));
    device.sysname   = toQString(udev_device_get_sysname(d));
    device.driver    = toQString(udev_device_get_driver(d));
    device.action    = toQString(udev_device_get_action(d)).toLower();
    device.seqnum    = udev_device_get_seqnum(d);

    // devnum → major/minor
    dev_t dn = udev_device_get_devnum(d);
    if (dn) {
        device.major = major(dn);
        device.minor = minor(dn);
    }

    device.isBlock = (udev_device_get_devnode(d)
                      && udev_device_get_devnum(d)
                      && udev_device_get_devtype(d)
                      && device.devtype!="");

    device.isChar  = (udev_device_get_devnode(d)
                     && udev_device_get_devnum(d));

    // properties
    for (udev_list_entry* e = udev_device_get_properties_list_entry(d); e; e = udev_list_entry_get_next(e))
        device.properties.insert(toQString(udev_list_entry_get_name(e)), toQString(udev_list_entry_get_value(e)));

    // sysattrs
    for (udev_list_entry* e = udev_device_get_sysattr_list_entry(d); e; e = udev_list_entry_get_next(e)) {
        const char* k = udev_list_entry_get_name(e);
        device.sysattrs.insert(toQString(k), toQString(udev_device_get_sysattr_value(d, k)));
    }

    // devlinks
    for (udev_list_entry* e = udev_device_get_devlinks_list_entry(d); e; e = udev_list_entry_get_next(e))
        device.devlinks << toQString(udev_list_entry_get_name(e));

    // tags
    for (udev_list_entry* e = udev_device_get_tags_list_entry(d); e; e = udev_list_entry_get_next(e))
        device.tags << toQString(udev_list_entry_get_name(e));

    // parent summary
    if (auto* p = udev_device_get_parent(d)) {
        device.parent_syspath = toQString(udev_device_get_syspath(p));
        device.parent_subsystem = toQString(udev_device_get_subsystem(p));
    }

    return device;
}
