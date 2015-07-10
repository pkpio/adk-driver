adk-serial-driver
=======
Fork from [adk-driver](https://github.com/praveendath92/adk-driver/) ( Kernel driver to set device into ADK (Accessory Development Kit) mode )

Contains same **adk** driver (with adk manifacturer string changed to "adk_serial_linux" and some log messages removed).

Adds **adk_serial** driver that creates ttyUSB files for android devices in accessory mode.


Using as a terminal
---------------
Compile and install [adk-serial-app](https://github.com/Phasip/adk-serial-app) to your android device.

Compile and install the modules, when a android device is connected it will 
show up as a /dev/ttyUSBx where x is a number. 

Use udev to ensure that the device name is consistent, place code below in /etc/udev/rules.d/99-android_serial.rules

    SUBSYSTEM=="tty", ATTRS{idVendor}=="18d1", ATTRS{idProduct}=="2d01", SYMLINK+="android_serial"

Launch getty (or similar) on the device at boot. Eg arch linux

    systemctl enable getty@android_serial

(Other dists may use /etc/inittab to configure ttys)

Done!
Connect your Android device by usb and login!

Author 
---------------
Pasi Saarinen

License
---------------
Licensed under GPL v2 

