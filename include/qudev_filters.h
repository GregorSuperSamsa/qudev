// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#pragma once

#include <QString>
#include <QStringList>
#include <QHash>
#include <QSet>

/**
 * @file qudev_filters.h
 * @brief Filter specification used to restrict which devices are reported.
 *
 * QudevFilters describes a set of criteria applied when enumerating or
 * monitoring devices. Some fields are used to configure libudev's own
 * enumerator (pre-filters), while others are applied in user space
 * (post-filters).
 */

/**
 * @brief Device filter configuration for enumeration and monitoring.
 *
 * A filter can contain several independent criteria. If multiple criteria
 * are set, they are combined with a logical AND: a device must satisfy
 * all non-empty fields in order to match.
 *
 * Some fields are only applied as post-filters (for example @ref devnode
 * and @ref syspathPrefix) because libudev does not provide a direct
 * pre-filter for them.
 */
struct QudevFilters
{
    /// Subsystem name, e.g. "usb", "block", "net", ...
    QString     subsystem;
    /// Devtype, e.g. "usb_device", "disk", "partition", ...
    QString     devtype;
    /// Sysname, e.g. "1-2", "sda", ...
    QString     sysname;
    /// Device node path, e.g. "/dev/sda" (post-filter only).
    QString     devnode;
    /// Prefix match on /sys path (post-filter only).
    QString     syspathPrefix;

    /// Device must contain all listed tags.
    QStringList tags;
    /// Event action must be in this list ("add", "remove", "change", ...).
    QStringList actions;

    /// Udev properties that must match exactly (key/value pairs).
    QHash<QString, QString> properties;
    /// Sysfs attributes that must match exactly (key/value pairs).
    QHash<QString, QString> sysattrs;
    /// Sysfs attributes that must not match the given value.
    QHash<QString, QString> nomatchSysattrs;

    /**
     * @brief Check whether this filter is effectively empty.
     *
     * @return @c true if no criteria are set and the filter will match
     *         all devices, @c false otherwise.
     */
    bool empty() const {
        return subsystem.isEmpty() &&
               devtype.isEmpty() &&
               sysname.isEmpty() &&
               devnode.isEmpty() &&
               syspathPrefix.isEmpty() &&
               tags.isEmpty() &&
               actions.isEmpty() &&
               properties.isEmpty() &&
               sysattrs.isEmpty() &&
               nomatchSysattrs.isEmpty();
    }
};

/**
 * @brief Compare two QStringLists as sets (order-insensitive).
 *
 * This helper ignores element ordering when determining equality. Does NOT care about duplicates!
 */
static inline bool equalUnordered(const QStringList& x, const QStringList& y)
{
    return QSet<QString>(x.cbegin(), x.cend()) == QSet<QString>(y.cbegin(), y.cend());
}

/**
 * @brief Compare two filter sets for equality.
 *
 * Order does not matter for list-like fields (@ref QudevFilters::tags, @ref QudevFilters::actions),
 * and the underlying Qt containers already provide order-insensitive
 * semantics for the hash-based maps.
 */
inline bool operator==(const QudevFilters& a, const QudevFilters& b)
{
    return a.subsystem       == b.subsystem
           && a.devtype         == b.devtype
           && a.sysname         == b.sysname
           && a.devnode         == b.devnode
           && a.syspathPrefix   == b.syspathPrefix
           && equalUnordered(a.tags,    b.tags)
           && equalUnordered(a.actions, b.actions)
           && a.properties      == b.properties
           && a.sysattrs        == b.sysattrs
           && a.nomatchSysattrs == b.nomatchSysattrs;
}

/**
 * @brief Inequality operator for QudevFilters.
 *
 * Equivalent to negating @ref operator==().
 */
inline bool operator!=(const QudevFilters& a, const QudevFilters& b)
{
    return !(a == b);
}
