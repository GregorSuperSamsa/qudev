// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#include "qudev_monitor.h"
#include "qudev_context.h"
#include "qudev_device.h"

#include <libudev.h>
#include <QSocketNotifier>


QudevMonitor::QudevMonitor(Channel channel, QObject* parent) noexcept
    : QObject(parent),
    channel_(channel)
{

}

QudevMonitor::~QudevMonitor()
{
    stop();
}

bool QudevMonitor::start(const QudevFilters& filters) noexcept
{
    // Clean any previous state.
    stop();

    // Create and hold a udev context.
    context_.reset();
    if (auto ctx = QudevContext::create()) {
        context_.emplace(std::move(*ctx));
    } else {
        return false;
    }

    filters_ = filters;

    const char* grp = (channel_ == Channel::Kernel) ? "kernel" : "udev";
    monitor_ = udev_monitor_new_from_netlink(context_->get(), grp);
    if (!monitor_) {
        stop();
        return false;
    }

    if (!applyPreFilters(filters_)) {
        stop();
        return false;
    }

    const int rc = udev_monitor_enable_receiving(monitor_);
    if (rc < 0){
        stop();
        return false;
    }

    const int fd = udev_monitor_get_fd(monitor_);
    socket_ = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(socket_, &QSocketNotifier::activated, this, &QudevMonitor::onReadyRead);

    return true;
}

void QudevMonitor::stop() noexcept
{
    if (socket_) {
        socket_->disconnect(this);
        socket_->deleteLater();
        socket_ = nullptr;
    }

    if (monitor_) {
        udev_monitor_unref(monitor_);
        monitor_ = nullptr;
    }

    context_.reset();
}

bool QudevMonitor::applyPreFilters(const QudevFilters &filters) const noexcept
{
    const QByteArray subsystemBuf = filters.subsystem.toUtf8();
    const char* subsystemP = subsystemBuf.isEmpty() ? nullptr : subsystemBuf.constData();

    const QByteArray devtypeBuf = filters.devtype.toUtf8();
    const char* devtypeP = devtypeBuf.isEmpty() ? nullptr : devtypeBuf.constData();

    if (subsystemP)
    {
        const int rc = udev_monitor_filter_add_match_subsystem_devtype(monitor_, subsystemP, devtypeP);
        if (rc < 0) {
            return false;
        }
    }

    for (const auto& tag : filters.tags)
    {
        if (tag.isEmpty()) {
            continue;
        }

        const QByteArray tagBuf = tag.toUtf8();
        const int rc = udev_monitor_filter_add_match_tag(monitor_, tagBuf.constData());
        if (rc < 0) {
            return false;
        }
    }

    return true;
}

bool QudevMonitor::applyPostFilters(const QudevDevice& device, const QudevFilters &filters) const noexcept
{
    // subsystem
    if (!filters.subsystem.isEmpty() &&
        device.subsystem != filters.subsystem) {
        return false;
    }

    // devtype
    if (!filters.devtype.isEmpty() &&
        device.devtype != filters.devtype &&
        device.properties.value(QStringLiteral("DEVTYPE")) != filters.devtype) {
        return false;
    }

    // sysname
    if (!filters.sysname.isEmpty() &&
        device.sysname != filters.sysname) {
        return false;
    }

    // devnode
    if (!filters.devnode.isEmpty() &&
        device.devnode != filters.devnode) {
        return false;
    }

    // syspath
    if (!filters.syspathPrefix.isEmpty() &&
        !device.syspath.startsWith(filters.syspathPrefix)) {
        return false;
    }

    // action
    if (!filters.actions.isEmpty() &&
        !filters.actions.contains(device.action)) {
        return false;
    }

    // tags
    for (const auto& tag : filters.tags) {
        if (!device.tags.contains(tag)) {
            return false;
        }
    }

    // properties
    for (auto it = filters.properties.cbegin(); it != filters.properties.cend(); ++it) {
        if (device.properties.value(it.key()) != it.value()) {
            return false;
        }
    }

    // sysattrs
    for (auto it = filters.sysattrs.cbegin(); it != filters.sysattrs.cend(); ++it) {
        if (device.sysattrs.value(it.key()) != it.value()) {
            return false;
        }
    }

    // no match sys attributes
    for (auto it = filters.nomatchSysattrs.cbegin(); it != filters.nomatchSysattrs.cend(); ++it) {
        if (device.sysattrs.value(it.key()) == it.value()) {
            return false;
        }
    }

    return true;
}

void QudevMonitor::onReadyRead()
{
    if (!monitor_ || !context_) {
        return;
    }

    while (udev_device* rawData = udev_monitor_receive_device(monitor_))
    {
        QudevDevice device = buildDevice(*context_, rawData);
        udev_device_unref(rawData);

        if (applyPostFilters(device, filters_)) {
            emit deviceFound(device);
        }
    }

}

