// SPDX-License-Identifier: MIT
// SPDX-FileCopyrightText: 2025 Georgi Georgiev, Samsa Ltd. <georgi@samsa.io>
//
// qudev - Qt wrapper around libudev
//
// This file is part of the qudev project.
// See the LICENSE file in the project root for full license text.

#pragma once

#include <QAbstractItemModel>
#include <QVector>
#include <QVariant>
#include <QHash>
#include <QByteArray>
#include <QString>
#include <QList>

#include "qudev_device.h"
#include "qudev_filters.h"


/**
 * @file qudev_device_model.h
 * @brief Tree model used by the udevviewer example to visualize devices.
 */

/**
 * @brief Hierarchical model of udev devices for use with QML TreeView.
 *
 * The model groups devices by subsystem and exposes several levels:
 *
 *  - Subsystem nodes
 *  - Device nodes
 *  - Section nodes (e.g. "Overview", "Properties", "Sysfs Attributes")
 *  - Entry nodes (key/value rows)
 *
 * The underlying data structure is a simple tree of Node objects
 * owned by the model. The model is intended for read-only use from
 * the QML side.
 */
class QudevDeviceModel : public QAbstractItemModel
{
    Q_OBJECT

    /// Number of top-level device rows (for convenience in QML).
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)

public:
    /**
     * @brief Node type enumeration used to distinguish items in the tree.
     */
    enum NodeType {
        SubsystemNode,
        DeviceNode,
        SectionNode,
        EntryNode
    };
    Q_ENUM(NodeType)

    /**
     * @brief Construct an empty device model.
     *
     * @param parent Optional QObject parent.
     */
    explicit QudevDeviceModel(QObject* parent=nullptr);
    ~QudevDeviceModel() override;

    /**
     * @brief Replace the model contents with the given device list.
     *
     * This resets the entire tree structure and rebuilds it based on @p list.
     *
     * @param list New device list to visualise.
     */
    Q_INVOKABLE void setDevices(const QList<QudevDevice>& list);

    /**
     * @brief Triggered by the QudevService when a new device is found.
     *
     * @param d The @ref QudevDevice found
     */
    Q_INVOKABLE void deviceAdded(const QudevDevice& d);

    /**
     * @brief Clear all devices from the model.
     */
    Q_INVOKABLE void clear();

    // QAbstractItemModel interface
    QModelIndex index(int row, int col, const QModelIndex& parent) const override;
    QModelIndex parent(const QModelIndex& idx) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& = QModelIndex()) const override { return 1; }
    QVariant data(const QModelIndex& idx, int role) const override;
    Qt::ItemFlags flags(const QModelIndex& idx) const override;
    bool hasChildren(const QModelIndex& parent) const override;
    QHash<int, QByteArray> roleNames() const override;

signals:
    void selectionChanged();
    void countChanged();

public:
    /**
     * @brief Internal tree node representation.
     *
     * Nodes are owned by the model and deleted recursively via the
     * destructor.
     */
    struct Node {
        NodeType type = SectionNode;
        QString  display;
        QString  key;
        QString  value;
        QudevDevice device;
        Node* parent = nullptr;
        QVector<Node*> children;
        ~Node() { qDeleteAll(children); }
    };

    /// Root node of the tree (may be @c nullptr when the model is empty).
    Node* root_ = nullptr;

    /// Helpers to build and navigate the tree structure.
    Node* makeRoot();
    void  rebuild(const QList<QudevDevice>& list);
    Node* addSubsystem(const QString& name);
    Node* addDevice(Node* subsystem, const QudevDevice& d);
    void  addSections(Node* device, const QudevDevice& d);
    Node* addOverview(Node* device, const QudevDevice& d);
    void  addKVSection(Node* device, const QString& title, const QMap<QString,QString>& map);
    void  addListSection(Node* device, const QString& title, const QStringList& list);

    Node* nodeFromIndex(const QModelIndex& idx) const;
    QModelIndex indexFromNode(Node* n, int column=0) const;

    mutable QModelIndex currentSelection_;
};
