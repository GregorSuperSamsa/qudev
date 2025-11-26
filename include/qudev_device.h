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
#include <QMap>
#include <QVariant>
#include <QVariantMap>
#include <QMetaType>

struct udev_device;
class QudevContext;

/**
 * @file qudev_device.h
 * @brief Qt-friendly representation of a single libudev device.
 *
 * This header defines the @ref QudevDevice value type, which collects commonly
 * used libudev properties into a copyable struct that can be passed through
 * Qt's meta-object system and used from C++ or QML.
 */

/**
 * @brief Value type representing a libudev device and its metadata.
 *
 * All fields are public for convenience. The struct is intentionally
 * lightweight and trivially copyable. It is safe to store in containers,
 * emit through signals, or expose to QML.
 */
struct QudevDevice
{
    Q_GADGET
public:
    /// Absolute path in /sys for this device (e.g. "/sys/block/sda").
    QString syspath;
    /// Device node path in /dev (e.g. "/dev/sda"), if applicable.
    QString devnode;
    /// Udev subsystem name (e.g. "block", "net", "usb").
    QString subsystem;
    /// Udev devtype (e.g. "disk", "partition", "usb_device").
    QString devtype;
    /// Udev sysname (e.g. "sda", "eth0", "1-2").
    QString sysname;
    /// Kernel driver name associated with the device, if any.
    QString driver;

    /// Major and minor numbers for block/character devices.
    quint32 major = 0;
    quint32 minor = 0;

    /// Udev properties key/value map.
    QMap<QString, QString> properties;
    /// Sysfs attributes key/value map.
    QMap<QString, QString> sysattrs;

    /// Alternate device links (usually additional /dev/* symlinks).
    QStringList devlinks;
    /// Udev tags attached to this device.
    QStringList tags;

    /// Parent device syspath, if known.
    QString parent_syspath;
    /// Parent device subsystem, if known.
    QString parent_subsystem;

    /// Udev event action ("add", "remove", "change", ...).
    QString action;
    /// Udev event sequence number.
    quint64 seqnum = 0;

    /// Convenience flag indicating whether this is a block device.
    bool isBlock = false;
    /// Convenience flag indicating whether this is a character device.
    bool isChar = false;

};
Q_DECLARE_METATYPE(QudevDevice)

/**
 * @brief Build a QudevDevice from a raw libudev handle.
 *
 * This helper is used internally by the library to translate libudev's
 * @c udev_device structure into a higher-level @ref QudevDevice.
 *
 * @param ctx Associated libudev context.
 * @param d   Raw @c udev_device pointer (not owning).
 * @return A populated QudevDevice instance.
 */
QudevDevice buildDevice(const QudevContext& ctx, udev_device* d);

