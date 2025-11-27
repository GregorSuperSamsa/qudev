// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#include "qudev_filters_model.h"

#include <QVariantMap>
#include <QVariantList>
#include <QJsonDocument>
#include <QJsonObject>


QudevFiltersModel::QudevFiltersModel(QObject* parent)
    : QObject(parent),
    settings_()
{
    loadSettings();
}

QVariantMap QudevFiltersModel::value() const
{
    return map_;
}

void QudevFiltersModel::setValue(const QVariantMap& map)
{
    if (map_ == map) {
        return;
    }

    map_ = map;
    saveSettings();

    emit changed();

}

void QudevFiltersModel::clearValue()
{
    if (map_.empty()) {
        return;
    }

    map_.clear();
    saveSettings();

    emit changed();
}

QudevFilters QudevFiltersModel::toQudevFilters() const
{
    auto toHash = [](const QVariant& value)
    {
        QHash<QString, QString> hash;
        const QVariantMap map = value.toMap();

        for (auto it = map.cbegin(); it != map.cend(); ++it) {
            hash.insert(it.key(), it.value().toString());
        }

        return hash;
    };


    QudevFilters f;

    f.subsystem     = map_.value("subsystem").toString();
    f.devtype       = map_.value("devtype").toString();
    f.sysname       = map_.value("sysname").toString();
    f.devnode       = map_.value("devnode").toString();
    f.syspathPrefix = map_.value("syspathPrefix").toString();
    f.actions       = map_.value("actions").toStringList();
    f.tags          = map_.value("tags").toStringList();
    f.properties      = toHash(map_.value("properties"));
    f.sysattrs        = toHash(map_.value("sysattrs"));
    f.nomatchSysattrs = toHash(map_.value("nomatchSysattrs"));

    return f;
}

void QudevFiltersModel::loadSettings()
{
    const QString json = settings_.value(settingsKey_).toString();
    if (json.isEmpty()) {
        map_.clear();
        emit changed();
        return;
    }

    const QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject()) {
        map_.clear();
        emit changed();
        return;
    }

    map_ = doc.object().toVariantMap();
    emit changed();
}

void QudevFiltersModel::saveSettings() const
{
    const QJsonDocument doc = QJsonDocument::fromVariant(map_);
    settings_.setValue(settingsKey_, QString::fromUtf8(doc.toJson(QJsonDocument::Compact)));
    settings_.sync();
}
