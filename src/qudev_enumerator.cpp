#include "qudev_enumerator.h"
#include "qudev_context.h"
#include "qudev_device.h"
#include "qudev_filters.h"

#include <libudev.h>

/**
 * @brief Set libudev prefilters for enumeration.
 * @return false if a libudev call failed; true otherwise.
 */
static bool applyPreFilters(udev_enumerate* en, const QudevFilters& filters)
{
    // subsystem
    if (!filters.subsystem.isEmpty())
    {
        const QByteArray subsystem = filters.subsystem.toUtf8();
        const int rc = udev_enumerate_add_match_subsystem(en, subsystem.constData());
        if (rc < 0) {
            return false;
        }
    }

    // sysname
    if (!filters.sysname.isEmpty())
    {
        const QByteArray sysname = filters.sysname.toUtf8();
        const int rc = udev_enumerate_add_match_sysname(en, sysname.constData());
        if (rc < 0) {
            return false;
        }
    }

    // properties (key=value)
    for (auto it = filters.properties.cbegin(); it != filters.properties.cend(); ++it)
    {
        if (it.key().isEmpty() || it.value().isEmpty()) {
            continue;
        }

        const QByteArray key = it.key().toUtf8();
        const QByteArray value = it.value().toUtf8();
        const int rc = udev_enumerate_add_match_property(en, key.constData(), value.constData());
        if (rc < 0) {
            return false;
        }
    }

    // tags (AND semantics across multiple calls)
    for (const auto& tag : filters.tags)
    {
        if (tag.isEmpty()) {
            continue;
        }

        const QByteArray tagBuf = tag.toUtf8();
        const int rc = udev_enumerate_add_match_tag(en, tagBuf.constData());
        if (rc < 0) {
            return false;
        }
    }

    // sysattrs (key=value)
    for (auto it = filters.sysattrs.cbegin(); it != filters.sysattrs.cend(); ++it)
    {
        if (it.key().isEmpty() || it.value().isEmpty()) {
            continue;
        }

        const QByteArray key = it.key().toUtf8();
        const QByteArray value = it.value().toUtf8();
        const int rc = udev_enumerate_add_match_sysattr(en, key.constData(), value.constData());
        if (rc < 0) {
            return false;
        }
    }

    // negative sysattrs (key!=value)
    for (auto it = filters.nomatchSysattrs.cbegin(); it != filters.nomatchSysattrs.cend(); ++it)
    {
        if (it.key().isEmpty() || it.value().isEmpty()) {
            continue;
        }

        const QByteArray key = it.key().toUtf8();
        const QByteArray value = it.value().toUtf8();
        const int rc = udev_enumerate_add_nomatch_sysattr(en, key.constData(), value.constData());
        if (rc < 0) {
            return false;
        }
    }

    // devtype: enumerator doesn’t have a dedicated API; DEVTYPE is exposed as a property
    if (!filters.devtype.isEmpty())
    {
        static const char* DEVTYPE = "DEVTYPE";
        const QByteArray value = filters.devtype.toUtf8();
        const int rc = udev_enumerate_add_match_property(en, DEVTYPE, value.constData());
        if (rc < 0) {
            return false;
        }
    }

    return true;
}

/**
 * @brief Post-filter parts that libudev cannot prefilter for enumeration.
 * @return true if @p device matches remaining criteria; false otherwise.
 */
static bool applyPostFilters(const QudevDevice& device, const QudevFilters& filters)
{
    if (!filters.devnode.isEmpty() &&
        filters.devnode != device.devnode) {
        return false;
    }

    if (!filters.syspathPrefix.isEmpty() &&
        !device.syspath.startsWith(filters.syspathPrefix)) {
        return false;
    }

    return true;
}

QudevEnumerator::QudevEnumerator(const QudevContext& ctx) noexcept
    : context(ctx)
{}

QList<QudevDevice> QudevEnumerator::scan(const QudevFilters& filters) const noexcept
{
    QList<QudevDevice> devices;

    // Create enumerate handle
    udev_enumerate* en = udev_enumerate_new(context.get());
    if (!en) {
        return devices;
    }

    if (!applyPreFilters(en, filters))
    {
        udev_enumerate_unref(en);
        return devices;
    }

    if (udev_enumerate_scan_devices(en) < 0)
    {
        udev_enumerate_unref(en);
        return devices;
    }

    // Iterate results
    for (udev_list_entry* it = udev_enumerate_get_list_entry(en);
         it; it = udev_list_entry_get_next(it))
    {

        const char* syspath = udev_list_entry_get_name(it);
        if (!syspath) {
            continue;
        }

        udev_device* d = udev_device_new_from_syspath(context.get(), syspath);
        if (!d) {
            continue;
        }

        QudevDevice device = buildDevice(context, d);
        udev_device_unref(d);

        if (!applyPostFilters(device, filters)) {
            continue;
        }

        devices.push_back(std::move(device));
    }

    udev_enumerate_unref(en);

    return devices;
}
