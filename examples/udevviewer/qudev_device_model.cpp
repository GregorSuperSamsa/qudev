#include "qudev_device_model.h"


QudevDeviceModel::QudevDeviceModel(QObject* parent)
    : QAbstractItemModel(parent)
{
    root_ = makeRoot();
}

QudevDeviceModel::~QudevDeviceModel()
{
    delete root_;
}

QudevDeviceModel::Node* QudevDeviceModel::makeRoot()
{
    auto* n = new Node;
    n->type = SectionNode;
    n->display = QStringLiteral("root");
    return n;
}

void QudevDeviceModel::clear()
{
    beginResetModel();
    delete root_;
    root_ = makeRoot();
    endResetModel();

    emit countChanged();
}

int QudevDeviceModel::rowCount(const QModelIndex& parent) const
{
    Node* p = nodeFromIndex(parent);

    return p ? p->children.size() : 0;
}

bool QudevDeviceModel::hasChildren(const QModelIndex& parent) const
{
    Node* p = nodeFromIndex(parent);

    return p && !p->children.isEmpty();
}

Qt::ItemFlags QudevDeviceModel::flags(const QModelIndex& idx) const
{
    if (!idx.isValid()) {
        return Qt::NoItemFlags;
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex QudevDeviceModel::index(int row, int col, const QModelIndex& parent) const
{
    if (col != 0 || row < 0)
        return {};

    Node* p = nodeFromIndex(parent);
    if (!p || row >= p->children.size())
        return {};

    return createIndex(row, col, p->children.at(row));
}

QModelIndex QudevDeviceModel::parent(const QModelIndex& idx) const
{
    if (!idx.isValid())
        return {};

    Node* n = nodeFromIndex(idx);

    if (!n || !n->parent || n->parent == root_)
        return {};

    Node* p = n->parent;
    Node* gp = p->parent;
    int row = gp ? gp->children.indexOf(p) : 0;

    return createIndex(row, 0, p);
}

QHash<int, QByteArray> QudevDeviceModel::roleNames() const
{
    return {
        { Qt::DisplayRole, "display" },
        { Qt::UserRole+1,  "nodeType" },
        { Qt::UserRole+2,  "iconName" },
        { Qt::UserRole+3,  "key" },
        { Qt::UserRole+4,  "value" }
    };
}

static QString iconFor(const QudevDeviceModel::Node* n)
{
    switch (n->type) {
    case QudevDeviceModel::SubsystemNode:
        return QStringLiteral("devices_other");
    case QudevDeviceModel::DeviceNode:
        if (n->device.subsystem == QLatin1String("usb"))   return QStringLiteral("usb");
        if (n->device.subsystem == QLatin1String("block")) return QStringLiteral("storage");
        if (n->device.subsystem == QLatin1String("net"))   return QStringLiteral("router");
        return QStringLiteral("memory");
    case QudevDeviceModel::SectionNode:
        return QStringLiteral("info");
    case QudevDeviceModel::EntryNode:
        return QStringLiteral("label");
    }
    return QStringLiteral("label");
}

QVariant QudevDeviceModel::data(const QModelIndex& idx, int role) const
{
    if (!idx.isValid()) {
        return {};
    }

    Node* n = nodeFromIndex(idx);
    if (!n) {
        return {};
    }

    switch (role) {
    case Qt::DisplayRole:
        if (n->type == EntryNode) {
            if (!n->key.isEmpty()) {
                return n->key + QStringLiteral(": ") + n->value;
            }
            return n->value;
        }
        return n->display;
    case Qt::UserRole+1:
        return int(n->type);
    case Qt::UserRole+2:
        return iconFor(n);
    case Qt::UserRole+3:
        return n->key;
    case Qt::UserRole+4:
        return n->value;
    default:
        return {};
    }
}

QudevDeviceModel::Node* QudevDeviceModel::nodeFromIndex(const QModelIndex& idx) const
{
    return idx.isValid() ? static_cast<Node*>(idx.internalPointer()) : root_;
}

QModelIndex QudevDeviceModel::indexFromNode(Node* n, int column) const
{
    if (n == root_) {
        return {};
    }

    Node* p = n->parent;
    int row = p ? p->children.indexOf(n) : 0;

    return createIndex(row, column, n);
}

void QudevDeviceModel::addKVSection(Node* device, const QString& title, const QMap<QString, QString>& map)
{
    if (map.isEmpty()) {
        return;
    }

    Node* sec = nullptr;
    auto ensureSec = [&]() {
        if (!sec) {
            sec = new Node;
            sec->type = SectionNode;
            sec->display = title;
            sec->parent = device;
        }
    };

    for (auto it = map.cbegin(); it != map.cend(); ++it)
    {
        const QString& k = it.key();
        const QString& v = it.value();
        if (k.isEmpty() || v.isEmpty()) {
            continue;
        }

        ensureSec();
        auto* e = new Node;
        e->type = EntryNode;
        e->display = k;
        e->key = k;
        e->value = v;
        e->parent = sec;
        sec->children.push_back(e);
    }

    if (sec && !sec->children.isEmpty()) {
        device->children.push_back(sec);
    }
    else if (sec) {
        delete sec;
    }
}

void QudevDeviceModel::addListSection(Node* device, const QString& title, const QStringList& list)
{
    if (list.isEmpty()) {
        return;
    }

    Node* sec = nullptr;
    auto ensureSec = [&]() {
        if (!sec) {
            sec = new Node;
            sec->type = SectionNode;
            sec->display = title;
            sec->parent = device;
        }
    };

    for (int i = 0; i < list.size(); ++i)
    {
        const QString& v = list.at(i);
        if (v.isEmpty()) {
            continue;
        }

        ensureSec();
        auto* e = new Node;
        e->type = EntryNode;
        e->display = QString::number(i);
        e->key = QString::number(i);
        e->value = v;
        e->parent = sec;
        sec->children.push_back(e);
    }

    if (sec && !sec->children.isEmpty()) {
        device->children.push_back(sec);
    }
    else if (sec) {
        delete sec;
    }
}

QudevDeviceModel::Node* QudevDeviceModel::addOverview(Node* device, const QudevDevice& d)
{
    Node* sec = nullptr;
    auto ensureSec = [&]() {
        if (!sec) {
            sec = new Node;
            sec->type = SectionNode;
            sec->display = QStringLiteral("Overview");
            sec->parent = device;
        }
    };

    auto addS = [&](const QString& k, const QString& v){
        if (v.isEmpty()) {
            return;
        }
        ensureSec();
        auto* e = new Node;
        e->type = EntryNode;
        e->display = k;
        e->key = k;
        e->value = v;
        e->parent = sec;
        sec->children.push_back(e);
    };

    auto addB = [&](const QString& k, bool b){
        ensureSec();
        auto* e = new Node;
        e->type = EntryNode;
        e->display = k;
        e->key = k;
        e->value = b ? QStringLiteral("true") : QStringLiteral("false");
        e->parent = sec;
        sec->children.push_back(e);
    };

    auto addU = [&](const QString& k, quint64 u){
        ensureSec();
        auto* e = new Node;
        e->type = EntryNode;
        e->display = k;
        e->key = k;
        e->value = QString::number(u);
        e->parent = sec;
        sec->children.push_back(e);
    };

    // Strings → only if present
    addS(QStringLiteral("syspath"), d.syspath);
    addS(QStringLiteral("devnode"), d.devnode);
    addS(QStringLiteral("subsystem"), d.subsystem);
    addS(QStringLiteral("devtype"), d.devtype);
    addS(QStringLiteral("sysname"), d.sysname);
    addS(QStringLiteral("driver"), d.driver);
    addS(QStringLiteral("action"), d.action);
    addS(QStringLiteral("parent_syspath"),   d.parent_syspath);
    addS(QStringLiteral("parent_subsystem"), d.parent_subsystem);

    // Numbers/bools → keep (informative even if zero/false)
    addU(QStringLiteral("major"),  d.major);
    addU(QStringLiteral("minor"),  d.minor);
    addB(QStringLiteral("isBlock"), d.isBlock);
    addB(QStringLiteral("isChar"),  d.isChar);
    addU(QStringLiteral("seqnum"),  d.seqnum);

    if (sec && !sec->children.isEmpty()) {
        device->children.push_back(sec);
        return sec;
    }
    delete sec;

    return nullptr;
}

void QudevDeviceModel::addSections(Node* device, const QudevDevice& d)
{
    addOverview(device, d);
    addKVSection(device, QStringLiteral("Properties"), d.properties);
    addKVSection(device, QStringLiteral("Sysattrs"),   d.sysattrs);
    addListSection(device, QStringLiteral("Devlinks"), d.devlinks);
    addListSection(device, QStringLiteral("Tags"),     d.tags);
}

QudevDeviceModel::Node* QudevDeviceModel::addSubsystem(const QString& name)
{
    for (auto* s : root_->children) {
        if (s->type == SubsystemNode && s->display == name)
            return s;
    }

    auto* n = new Node;
    n->type = SubsystemNode;
    n->display = name;
    n->parent = root_;
    root_->children.push_back(n);

    return n;
}

QudevDeviceModel::Node* QudevDeviceModel::addDevice(Node* subsystem, const QudevDevice& d)
{
    auto label = d.devnode.isEmpty() ? d.syspath : d.devnode;
    auto* n = new Node;
    n->type = DeviceNode;
    n->display = label;
    n->device = d;
    n->parent = subsystem;
    subsystem->children.push_back(n);

    return n;
}

void QudevDeviceModel::rebuild(const QList<QudevDevice>& list)
{
    delete root_;
    root_ = makeRoot();

    QHash<QString, QList<QudevDevice>> buckets;
    for (const auto& d : list) {
        buckets[d.subsystem].push_back(d);
    }

    for (auto it = buckets.cbegin(); it != buckets.cend(); ++it) {
        Node* sub = addSubsystem(it.key());
        for (const auto& d : it.value()) {
            Node* dev = addDevice(sub, d);
            addSections(dev, d);
        }
    }
}

void QudevDeviceModel::setDevices(const QList<QudevDevice>& list)
{
    beginResetModel();
    rebuild(list);
    endResetModel();

    emit countChanged();
}

void QudevDeviceModel::deviceAdded(const QudevDevice& d)
{
    beginResetModel();

    QList<QudevDevice> flat;
    for (auto* s : root_->children) {
        for (auto* dv : s->children) {
            if (dv->type == DeviceNode) {
                flat.push_back(dv->device);
            }
        }
    }
    flat.push_back(d);

    rebuild(flat);
    endResetModel();

    emit countChanged();
}
