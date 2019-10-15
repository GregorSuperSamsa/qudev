# QUdev

Qt wrapper around the linux libudev library. Used for monitoring and enumerating of tty devices.


### Compiling

Add to your .pro file:
```
LIBS += -ludev
```

### Usage
```
#include <QUdev.h>

// Create object 
QUdev udev

// Initialize
udev->initialize()
    
// Set subsystem to monitor
udev->setSubsystem("tty");
        
// Set parent subsystem to monitor
udev->setParentSubsystem("usb");

// Get list of all found devices
udev->getUdevDeviceList();

```

### Realtime monitoring
```
// Start realtime monitoring of the selected subsystem
udev->startMonitoring();

// Stop realtime monitoring of the selected subsystem
udev->stopMonitoring();

```

If there is a change in the observed subsystem, a *SIGNAL* is emitted
```
udevDeviceFound(QUdev::UdevDevice);
```
