#include "qudev_context.h"
#include <libudev.h>

std::optional<QudevContext> QudevContext::create() noexcept {
    if (auto* p = udev_new())
        return QudevContext(p);
    return std::nullopt;
}

QudevContext::~QudevContext() { reset(); }

void QudevContext::reset() noexcept {
    if (ctx_) {
        udev_unref(ctx_);
        ctx_ = nullptr;
    }
}

QudevContext::QudevContext(QudevContext&& other) noexcept : ctx_(other.ctx_) {
    other.ctx_ = nullptr;
}

QudevContext& QudevContext::operator=(QudevContext&& other) noexcept {
    if (this != &other) {
        reset();
        ctx_ = other.ctx_;
        other.ctx_ = nullptr;
    }
    return *this;
}
