#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/uaccess.h>

#define ANDROID_ACCESSORY_VID 0x18d1
#define ANDROID_ACCESSORY_INTERFACE_CLASS 0xff
#define ANDROID_ACCESSORY_INTERFACE_SUBCLASS 0xff
#define ANDROID_ACCESSORY_INTERFACE_PROTO 0x00

static const struct usb_device_id id_table[] = {
	{ USB_VENDOR_AND_INTERFACE_INFO(ANDROID_ACCESSORY_VID, ANDROID_ACCESSORY_INTERFACE_CLASS,ANDROID_ACCESSORY_INTERFACE_SUBCLASS,ANDROID_ACCESSORY_INTERFACE_PROTO) },
	{ },
};
MODULE_DEVICE_TABLE(usb, id_table);


static struct usb_serial_driver adk_serial_device = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "adk_serial",
	},
	.id_table = id_table,
	.num_ports = 1,
};

static struct usb_serial_driver * const serial_drivers[] = {
	&adk_serial_device, NULL
};

module_usb_serial_driver(serial_drivers, id_table);

MODULE_AUTHOR("Pasi Saarinen <phasip@gmail.com>");
MODULE_DESCRIPTION("USB-serial driver for Android devices in accessory mode");
MODULE_LICENSE("GPL");
