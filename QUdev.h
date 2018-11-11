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

    void setSubsystem(const QString subsystem);
    QString getSubsystem() const;

    void setParentSubsystem(const QString parentSubsystem);
    QString getParentSubsystem() const;

    struct UdevDevice
    {
        QString path;
        QString vendorId;
        QString productId;
        QString manufacturer;
        QString product;
        QString serial;
    };
    QVector<UdevDevice> enumerate();

private:
    struct udev *ptrUdev = nullptr;

    QString subsystem;
    QString parentSubsystem;

    bool initialize();
    void uninitialize();

signals:

};

#endif // QUDEV_H
