#pragma once

#include <memory>
#include <QObject>

#include "qudev_filters.h"

class QudevDevice;

/**
 * @file qudev.h
 * @brief Public façade class for the Qt/libudev wrapper.
 *
 * Qudev provides a thin, Qt-friendly wrapper around libudev that exposes:
 *
 *  - Synchronous device enumeration via @ref Qudev::enumerate().
 *  - Event-based monitoring of device changes via @ref Qudev::startMonitoring()
 *    and the @ref Qudev::deviceFound(const QudevDevice& device) signal.
 *
 * The class itself is synchronous and not thread-safe. It must be used
 * from a single thread (typically the GUI thread or a dedicated worker
 * thread). Asynchronous usage should be implemented by client code
 * (see the example service in @c examples/udevviewer).
 */

/**
 * @brief High-level Qt wrapper around libudev.
 *
 * This class owns the underlying libudev context and provides methods
 * to enumerate devices and receive hotplug events. It does not perform
 * any internal threading; callers are expected to wrap it in their own
 * worker thread if they require asynchronous behavior.
 */
class Qudev : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Construct a new Qudev instance.
     *
     * @param parent Optional QObject parent.
     */
    explicit Qudev(QObject* parent = nullptr);

    /// Destructor. Ensures the underlying libudev context is released.
    ~Qudev() override;

    /**
     * @brief Enumerate devices matching the current filters.
     *
     * This function performs a synchronous snapshot of devices using the
     * libudev enumerator. It is a blocking operation.
     *
     * @return A list of matching devices. The list may be empty if no
     *         devices matched the filters or if enumeration failed.
     */
    QList<QudevDevice> enumerate();

    /**
     * @brief Start monitoring for udev events.
     *
     * Once monitoring is started successfully, the @ref deviceFound()
     * signal will be emitted whenever libudev reports a matching event.
     *
     * @return @c true on success, @c false if monitoring could not be
     *         started (for example if creating the libudev monitor failed).
     */
    bool startMonitoring();

    /**
     * @brief Stop monitoring for udev events.
     *
     * If monitoring is currently active, this function stops observation
     * of device events and releases the underlying monitor handle.
     * It is safe to call this even if monitoring is not active.
     */
    void stopMonitoring();

    /**
     * @brief Set the filters to apply when enumerating and monitoring.
     *
     * Existing filters are replaced by @p filters. Subsequent calls to
     * @ref enumerate() and @ref startMonitoring() will use the new
     * configuration.
     *
     * @param filters New filter set to apply.
     */
    void setFilters(const QudevFilters& filters);

    /**
     * @brief Return the currently configured filter set.
     *
     * @return A const reference to the internal filter object.
     */
    const QudevFilters& filters() const;

    /**
     * @brief Clear all filters.
     *
     * After calling this, enumeration and monitoring will match all
     * devices until new filters are configured.
     */
    void clearFilters();

signals:
    /**
     * @brief Emitted when a matching device event is observed.
     *
     * @param device Device representation for the observed event.
     */
    void deviceFound(const QudevDevice& device);

private:
    struct Private;
    std::unique_ptr<Private> d_;

    QudevFilters filters_;
    bool ensureContext();
};
