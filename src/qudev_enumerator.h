#pragma once

#include <QList>
#include "qudev_filters.h"

class QudevContext;
struct QudevDevice;

/**
 * @file qudev_enumerator.h
 * @brief Internal helper for taking libudev device snapshots.
 */

/**
 * @brief Snapshot enumerator for libudev devices.
 *
 * This class performs a one-shot enumeration of devices using libudev.
 * It is an internal helper and not part of the public API surface.
 */
class QudevEnumerator
{
public:
    /**
     * @brief Construct an enumerator using an existing libudev context.
     *
     * The enumerator never takes ownership of the context; the caller
     * is responsible for ensuring that @p ctx outlives this object.
     *
     * @param ctx Reference to an existing @ref QudevContext.
     */
    explicit QudevEnumerator(const QudevContext& ctx) noexcept;

    /**
     * @brief Enumerate devices matching @p filters.
     *
     * This method runs libudev's enumeration with the given filter set,
     * and then applies any additional post-filters defined in
     * @ref QudevFilters.
     *
     * @param filters Filter set to apply (see @ref QudevFilters).
     * @return List of matching devices; empty on failure.
     */
    QList<QudevDevice> scan(const QudevFilters& filters) const noexcept;

private:
    const QudevContext& context;
};
