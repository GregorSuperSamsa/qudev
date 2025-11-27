// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#include "qudev_device_search_model.h"
#include "qudev_device_model.h"

#include <QAbstractItemModel>


QudevDeviceSearchModel::QudevDeviceSearchModel(QObject* parent)
    : QSortFilterProxyModel(parent)
{
    setDynamicSortFilter(true);
}

QString QudevDeviceSearchModel::filterText() const
{
    return filterText_;
}

void QudevDeviceSearchModel::setFilterText(const QString& text)
{
    if (text == filterText_)
        return;

    filterText_ = text;
    filterTextLower_ = filterText_.toLower();
    emit filterTextChanged();
    invalidateFilter();
}

bool QudevDeviceSearchModel::filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const
{
    if (filterTextLower_.isEmpty()) {
        return true;
    }

    const QAbstractItemModel* src = sourceModel();
    if (!src) {
        return true;
    }

    QModelIndex idx = src->index(sourceRow, 0, sourceParent);
    if (!idx.isValid()) {
        return false;
    }

    return deviceSubtreeMatches(idx);
}

bool QudevDeviceSearchModel::deviceSubtreeMatches(const QModelIndex& anyIndex) const
{
    const QAbstractItemModel* src = sourceModel();
    if (!src || !anyIndex.isValid()) {
        return false;
    }

    QModelIndex deviceIdx = anyIndex;

    while (deviceIdx.isValid())
    {
        const QVariant typeVar = src->data(deviceIdx, Qt::UserRole + 1);
        if (typeVar.isValid()) {
            const auto nodeType = static_cast<QudevDeviceModel::NodeType>(typeVar.toInt());
            if (nodeType == QudevDeviceModel::DeviceNode) {
                break;
            }
        }
        deviceIdx = deviceIdx.parent();
    }

    if (!deviceIdx.isValid()) {
        deviceIdx = anyIndex;
    }

    return rowMatchesRecursive(deviceIdx);
}

bool QudevDeviceSearchModel::rowMatchesRecursive(const QModelIndex& sourceIndex) const
{
    const QAbstractItemModel* src = sourceModel();
    if (!src || !sourceIndex.isValid()) {
        return false;
    }

    const int columnCount = src->columnCount(sourceIndex.parent());
    for (int col = 0; col < columnCount; ++col)
    {
        QModelIndex idx = src->index(sourceIndex.row(), col, sourceIndex.parent());
        if (!idx.isValid())
            continue;

        if (indexMatches(idx))
            return true;
    }

    const int childRows = src->rowCount(sourceIndex);
    for (int r = 0; r < childRows; ++r)
    {
        QModelIndex child = src->index(r, 0, sourceIndex);
        if (!child.isValid())
            continue;

        if (rowMatchesRecursive(child))
            return true;
    }

    return false;
}

bool QudevDeviceSearchModel::indexMatches(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return false;
    }

    const QAbstractItemModel* src = sourceModel();
    if (!src) {
        return false;
    }

    const QVariant v = src->data(index, Qt::DisplayRole);
    const QString s = v.toString();
    if (s.isEmpty()) {
        return false;
    }

    return s.toLower().contains(filterTextLower_);
}
