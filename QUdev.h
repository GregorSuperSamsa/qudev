#ifndef QUDEV_H
#define QUDEV_H

#include <libudev.h>
#include <QObject>
#include <QVector>
#include <QFuture>


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
    QVector<UdevDevice> getUdevDeviceList();
    UdevDevice getUdevDevice(udev_device* ptrUdevDevice);

    void startMonitoring();
    void stopMonitoring();

private:
    struct udev *ptrUdevLib = nullptr;

    QString subsystem;
    QString parentSubsystem;

    QFuture<void> monitorTask;
    bool isMonitoring = false;
    void monitor();

signals:
    void udevDeviceFound(QUdev::UdevDevice device);

};

#endif // QUDEV_H
