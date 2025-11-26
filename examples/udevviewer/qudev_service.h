// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#pragma once

#include <QObject>
#include <QList>

#include <qudev.h>
#include <qudev_device.h>
#include <qudev_filters.h>

class QThread;
class QudevWorker;

/**
 * @file qudev_service.h
 * @brief QML-facing service for the udevviewer example.
 */

/**
 * @brief Thin service that wraps the Qudev façade for QML/C++ consumers.
 *
 * Responsibilities:
 *  - Owns a single @ref Qudev instance.
 *  - Exposes a QML-friendly API (@ref scan(), @ref startMonitoring,
 *    @ref stopMonitoring()).
 *  - Manages @ref QudevFilters based on data provided by the UI.
 */
class QudevService : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool scanning   READ scanning   NOTIFY scanningChanged)
    Q_PROPERTY(bool monitoring READ monitoring NOTIFY monitoringChanged)

public:
    explicit QudevService(QObject* parent = nullptr);
    ~QudevService() override;


    Q_INVOKABLE bool scan();
    Q_INVOKABLE bool startMonitoring();
    Q_INVOKABLE void stopMonitoring();

    bool scanning() const;
    bool monitoring() const;

    QudevFilters filters() const;
    void setFilters(const QudevFilters& filters);

signals:
    void scanFinished(QList<QudevDevice> devices);
    void deviceFound(const QudevDevice& device);

    void scanningChanged();
    void monitoringChanged();
    
    void requestScan();
    void requestStartMonitoring();
    void requestStopMonitoring();
    void filtersChanged(const QudevFilters& filters);

private slots:
    void onScanFinished(const QList<QudevDevice> &devices);
    void onMonitoringStateChanged(bool active);

private:
    QThread*      workerThread_ = nullptr;
    QudevWorker*  worker_       = nullptr;
    QudevFilters  filters_;
    bool          scanning_     = false;
    bool          monitoring_   = false;

};
