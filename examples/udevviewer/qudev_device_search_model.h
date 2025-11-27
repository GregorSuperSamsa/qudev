// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#pragma once

#include <QSortFilterProxyModel>
#include <QString>


/**
 * @file qudev_device_search_model.h
 * @brief Proxy model providing free-text search over the device tree.
 */

/**
 * @brief Proxy model that implements free-text search across device subtrees.
 *
 * QudevDeviceSearchModel is intended to sit between the underlying
 * QudevDeviceModel and a QML TreeView. It exposes a @ref filterText
 * Q_PROPERTY that can be bound to a search field. Whenever the filter
 * text changes, the proxy recomputes which device subtrees should be
 * visible.
 */
class QudevDeviceSearchModel : public QSortFilterProxyModel
{
    Q_OBJECT

    /**
     * @brief Free-text search string applied to the device tree.
     *
     * When this property is changed, the proxy invalidates its filter
     * and recomputes visibility for all device subtrees. Matching is
     * case-insensitive and performed against the DisplayRole string of
     * each node.
     */
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)

public:
    /**
     * @brief Construct a device search model.
     *
     * @param parent Optional QObject parent.
     */
    explicit QudevDeviceSearchModel(QObject* parent = nullptr);

    QString filterText() const;

    /**
     * @brief Sets the free-text filter used for searching device subtrees.
     *
     * @param text New filter text. An empty string disables filtering and shows all devices.
     */
    void setFilterText(const QString& text);

signals:
    /**
     * @brief Emitted whenever @ref filterText is changed.
     */
    void filterTextChanged();

protected:
    /**
     * @brief Reimplemented from QSortFilterProxyModel to apply device-level filtering.
     *
     * @param sourceRow    Row index in the source model.
     * @param sourceParent Parent index in the source model.
     *
     * @return @c true if the row should be visible in the proxy, @c false otherwise.
     */
    bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;

private:
    /**
     * @brief Whether the device subtree that contains @p anyIndex matches.
     *
     * @param anyIndex Index somewhere within a device subtree.
     *
     * @return @c true if any node in the corresponding device subtree matches
     * the current filter text, @c false otherwise.
     */
    bool deviceSubtreeMatches(const QModelIndex& anyIndex) const;

    /**
     * @brief Recursively checks @p sourceIndex and all its descendants for a match.
     *
     * @param sourceIndex Root index of the subtree to inspect.
     *
     * @return @c true if any node in the subtree matches the filter, @c false otherwise.
     */
    bool rowMatchesRecursive(const QModelIndex& sourceIndex) const;

    /**
     * @brief Whether a single index's display text matches the filter.
     *
     * @param index Index to test.
     *
     * @return @c true if the display text contains the filter text, @c false otherwise.
     */
    bool indexMatches(const QModelIndex& index) const;

    QString filterText_;
    QString filterTextLower_;
};
