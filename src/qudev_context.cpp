// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

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
