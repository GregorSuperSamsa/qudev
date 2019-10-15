# QUdev

Qt wrapper around the linux libudev library. Used for monitoring and enumerating of tty devices


### Compiling

Add to your .pro file:
```
*LIBS += -ludev
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

// Start auto monitoring
udev->startMonitoring();

// Stop auto monitoring
udev->stopMonitoring();
```    
