// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#include "qudev_service.h"
#include <QVariant>
#include <QHash>
#include <QThread>


// Private worker that lives entirely in a dedicated QThread
// and owns the single Qudev instance.
class QudevWorker : public QObject
{
    Q_OBJECT

public:
    explicit QudevWorker(QObject* parent = nullptr)
        : QObject(parent)
    {
        qudev_.setParent(this);
        connect(&qudev_, &Qudev::deviceFound, this, &QudevWorker::deviceFound);
    }

public slots:
    void setFilters(const QudevFilters& filters)
    {
        qudev_.setFilters(filters);
    }

    void scan()
    {
        const QList<QudevDevice> devices = qudev_.enumerate();
        emit scanFinished(devices);
    }

    void startMonitoring()
    {
        const bool ok = qudev_.startMonitoring();
        emit monitoringStateChanged(ok);
    }

    void stopMonitoring()
    {
        qudev_.stopMonitoring();
        emit monitoringStateChanged(false);
    }

signals:
    void scanFinished(const QList<QudevDevice>& devices);
    void deviceFound(const QudevDevice& device);
    void monitoringStateChanged(bool active);

private:
    Qudev qudev_;
};


QudevService::QudevService(QObject* parent)
    : QObject(parent)
{
    workerThread_ = new QThread(this);
    worker_       = new QudevWorker;
    worker_->moveToThread(workerThread_);


    connect(workerThread_, &QThread::finished, worker_, &QObject::deleteLater);
    workerThread_->start();

    connect(this, &QudevService::requestScan,            worker_, &QudevWorker::scan);
    connect(this, &QudevService::requestStartMonitoring, worker_, &QudevWorker::startMonitoring);
    connect(this, &QudevService::requestStopMonitoring,  worker_, &QudevWorker::stopMonitoring);
    connect(this, &QudevService::filtersChanged,         worker_, &QudevWorker::setFilters);

    connect(worker_, &QudevWorker::scanFinished,           this, &QudevService::onScanFinished);
    connect(worker_, &QudevWorker::deviceFound,            this, &QudevService::deviceFound);
    connect(worker_, &QudevWorker::monitoringStateChanged, this, &QudevService::onMonitoringStateChanged);
}

QudevService::~QudevService()
{
    if (workerThread_) {
        workerThread_->quit();
        workerThread_->wait();
    }
}

// Async scan
bool QudevService::scan()
{
    if (!worker_ || scanning_)
        return false;

    scanning_ = true;
    emit scanningChanged();

    emit requestScan();

    return true;
}

void QudevService::onScanFinished(const QList<QudevDevice>& devices)
{
    scanning_ = false;

    emit scanningChanged();
    emit scanFinished(devices);
}

bool QudevService::scanning() const
{
    return scanning_;
}

// Async monitoring
bool QudevService::startMonitoring()
{
    if (!worker_ || monitoring_) {
        return false;
    }

    emit requestStartMonitoring();
    return true;
}

void QudevService::stopMonitoring()
{
    if (!worker_ || !monitoring_) {
        return;
    }

    emit requestStopMonitoring();
}

void QudevService::onMonitoringStateChanged(bool active)
{
    if (monitoring_ == active)
        return;

    monitoring_ = active;
    emit monitoringChanged();
}

bool QudevService::monitoring() const
{
    return monitoring_;
}

QudevFilters QudevService::filters() const
{
    return filters_;
}

void QudevService::setFilters(const QudevFilters &filters)
{
    filters_ = filters;
    emit filtersChanged(filters_);
}

#include "qudev_service.moc"
