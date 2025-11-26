// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#pragma once

#include <QObject>
#include <QVariant>
#include <QSettings>

#include "qudev_filters.h"


/**
 * @file qudev_device_filters_model.h
 * @brief QML-facing model for editing and persisting QudevFilters.
 */

/**
 * @brief Adapter between QML filter data and QudevFilters.
 *
 * The QudevFiltersModel exposes a QVariantMap property @ref QudevFiltersModel::value() const that
 * can be bound to from QML (e.g. the FiltersDrawer). It also provides
 * load/save helpers backed by QSettings so that filter configuration can
 * persist across application runs.
 */
class QudevFiltersModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantMap value READ value WRITE setValue NOTIFY changed)

public:
    /**
     * @brief Construct a filters model with an optional parent.
     */
    explicit QudevFiltersModel(QObject* parent = nullptr);

    /**
     * @brief Return the current filter configuration as a map.
     *
     * The map keys are designed to be easy to work with from QML, and
     * are later translated into a @ref QudevFilters instance via
     * @ref toQudevFilters().
     */
    QVariantMap value() const;

    /**
     * @brief Replace the current filter configuration map.
     *
     * This does not automatically persist the configuration; call
     * @ref saveSettings() to write it to QSettings.
     *
     * @param map New configuration.
     */
    Q_INVOKABLE void setValue(const QVariantMap& map);

    /**
     * @brief Clear the current filter configuration map.
     *
     * This does not clear any stored settings; it only affects the live
     * instance. Use @ref saveSettings() if you want to persist the cleared
     * state as well.
     */
    Q_INVOKABLE void clearValue();

    /**
     * @brief Load filter settings from QSettings.
     */
    Q_INVOKABLE void loadSettings();

    /**
     * @brief Save filter settings to QSettings.
     */
    Q_INVOKABLE void saveSettings() const;

    /**
     * @brief Convert the current QVariantMap to a QudevFilters instance.
     *
     * This is used by the service layer to apply filters to the library.
     */
    QudevFilters toQudevFilters() const;

signals:
    /**
     * @brief Emitted whenever the filter map changes.
     */
    void changed();

private:
    QVariantMap map_;
    mutable QSettings settings_;
    QString settingsKey_ = QStringLiteral("qudev/filters");
};
