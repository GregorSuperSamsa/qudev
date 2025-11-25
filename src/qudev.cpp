#include <qudev.h>

#include <optional>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include "qudev_context.h"
#include "qudev_enumerator.h"
#include "qudev_monitor.h"
#include "qudev_device.h"
#include "qudev_filters.h"


struct Qudev::Private {
    std::unique_ptr<QudevContext> ctx;
    std::unique_ptr<QudevMonitor> mon;
};

Qudev::Qudev(QObject* parent) : QObject(parent),
    d_{std::make_unique<Private>()}
{

}

Qudev::~Qudev() { stopMonitoring(); }

bool Qudev::ensureContext() {
    if (d_->ctx) return true;
    if (auto ctx = QudevContext::create()) {
        d_->ctx = std::make_unique<QudevContext>(std::move(*ctx));
        return true;
    }
    qWarning() << "[Qudev] Failed to create QUdevContext";
    return false;
}

QList<QudevDevice> Qudev::enumerate()
{
    QList<QudevDevice> list;

    if (!ensureContext()) {
        return list;
    }

    QudevEnumerator enumerator(*d_->ctx);
    return enumerator.scan(filters_);
}

bool Qudev::startMonitoring()
{
    if (!ensureContext()) {
        return false;
    }

    stopMonitoring();
    d_->mon = std::make_unique<QudevMonitor>(QudevMonitor::Channel::Udev, this);

    if (!d_->mon->start(filters_)) {
        qWarning() << "[Qudev] Failed to start monitor";
        d_->mon.reset();
        return false;
    }

    connect(d_->mon.get(), &QudevMonitor::deviceFound, this, &Qudev::deviceFound);

    return true;
}

void Qudev::stopMonitoring() {
    if (d_->mon) { d_->mon->stop(); d_->mon.reset(); }
}

const QudevFilters& Qudev::filters() const
{
    return filters_;
}

void Qudev::setFilters(const QudevFilters &filters)
{
    filters_ = filters;
}

void Qudev::clearFilters()
{
    setFilters({});
}


