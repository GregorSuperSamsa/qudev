#ifndef QUDEV_H
#define QUDEV_H

#include <libudev.h>
#include <QObject>
#include <QVector>


class QUdev : public QObject
{
    Q_OBJECT

public:
    explicit QUdev(QObject *parent = nullptr);
    ~QUdev();

    bool initialize();
    void uninitialize();

    void setSubsystem(const QString subsystem);
    QString getSubsystem() const;

    void setParentSubsystem(const QString parentSubsystem);
    QString getParentSubsystem() const;

    enum UDEV_DEVICE_ACTION_TYPE
    {
        UDEV_DEVICE_ACTION_ADD,
        UDEV_DEVICE_ACTION_REMOVE,
        UDEV_DEVICE_ACTION_CHANGE,
        UDEV_DEVICE_ACTION_ONLINE,
        UDEV_DEVICE_ACTION_OFFLINE,
        UDEV_DEVICE_ACTION_UNKNOWN
    };
    UDEV_DEVICE_ACTION_TYPE getUdevDeviceAction(udev_device* ptrUdevDevice);

    struct UdevDevice
    {
        QString path;
        QString vendorId;
        QString productId;
        QString manufacturer;
        QString product;
        QString serial;
        UDEV_DEVICE_ACTION_TYPE action = UDEV_DEVICE_ACTION_UNKNOWN;
        bool isValid = false;
    };
    QVector<UdevDevice> enumerate();
    UdevDevice getUdevDeviceAttributes(udev_device* ptrUdevDevice);

    void monitor();

private:
    struct udev *ptrUdev = nullptr;

    QString subsystem;
    QString parentSubsystem;

private slots:
    void onMatchFound(QString s);

signals:
    void matchFound(QString s);

};

#endif // QUDEV_H
