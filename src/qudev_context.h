#pragma once

#include <optional>

struct udev;

/**
 * @file qudev_context.h
 * @brief RAII wrapper around a libudev context pointer.
 */

/**
 * @brief Lightweight wrapper around a libudev context (udev*).
 *
 * This class owns a single @c udev* handle and guarantees that
 * @c udev_unref() is called on destruction. It is non-copyable but
 * movable, and is used internally to share libudev state across
 * various helpers.
 */
class QudevContext final
{
public:
    /**
     * @brief Create a new libudev context.
     * @return std::optional<QudevContext> Engaged on success; empty on failure.
     */
    static std::optional<QudevContext> create() noexcept;

    /// @name Move-only semantics
    ///@{
    QudevContext(QudevContext&&) noexcept;
    QudevContext& operator=(QudevContext&&) noexcept;
    QudevContext(const QudevContext&) = delete;
    QudevContext& operator=(const QudevContext&) = delete;
    ///@}

    /**
     * @brief Destructor; releases the context if present.
     */
    ~QudevContext();

    /**
     * @brief Get the raw libudev handle.
     */
    udev* get() const noexcept { return ctx_; }

    /**
     * @brief Check whether this wrapper currently owns a valid udev handle.
     * @return true if  non-null; false otherwise.
     */
    bool valid() const noexcept { return ctx_ != nullptr; }

private:
    /// Construct from an already-created udev* (takes ownership).
    explicit QudevContext(udev* p) noexcept : ctx_(p) {}

    /// Release and nullify the current handle (if any).
    void reset() noexcept;

    udev* ctx_ = nullptr; //!< Owned libudev context handle.
};
