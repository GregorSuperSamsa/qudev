#pragma once

#include <QObject>
#include <optional>

#include "qudev_context.h"
#include "qudev_filters.h"

struct udev_monitor;
class QSocketNotifier;
struct QudevDevice;

/**
 * @file qudev_monitor.h
 * @brief Internal event monitor for libudev devices.
 */

/**
 * @brief Event monitor for libudev devices.
 *
 * QudevMonitor encapsulates a libudev monitor instance and exposes
 * device events via the @ref deviceFound() signal. It is an internal
 * helper used by @ref Qudev and is not meant to be used directly by
 * library consumers.
 */
class QudevMonitor : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Supported channels for device events.
     *
     * See libudev documentation for details.
     */
    enum class Channel {
        Kernel,
        Udev
    };

    /**
     * @brief Construct a QudevMonitor using @p channel.
     *
     * @param channel Channel type to use for monitoring.
     * @param parent  Optional QObject parent.
     */
    explicit QudevMonitor(Channel channel, QObject* parent = nullptr) noexcept;

    /// Destroys the monitor and releases system resources.
    ~QudevMonitor() override;

    /**
     * @brief Start monitoring for events.
     *
     * This will create and configure the underlying libudev monitor and
     * arrange for @ref onReadyRead() to be called when new events arrive.
     *
     * @param filters The @ref QudevFilters to be applied.
     *
     * @return @c true on success, @c false on failure.
     */
    bool start(const QudevFilters& filters) noexcept;

    /**
     * @brief Stop monitoring for events.
     *
     * It is safe to call this even if monitoring is not currently active.
     */
    void stop() noexcept;

signals:
    /**
     * @brief Emitted when a device event is received and passes all filters.
     *
     * @param device A fully populated @ref QudevDevice.
     */
    void deviceFound(const QudevDevice& device);

private:
    bool applyPreFilters(const QudevFilters& filters) const noexcept;
    bool applyPostFilters(const QudevDevice& device, const QudevFilters& filters) const noexcept;
    void onReadyRead();

private:
    udev_monitor* monitor_ = nullptr;
    std::optional<QudevContext> context_;
    QSocketNotifier* socket_ = nullptr;
    Channel channel_;
    QudevFilters filters_;
};
