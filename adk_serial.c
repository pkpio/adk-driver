#include <linux/kernel.h>
#include <linux/tty.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/usb/serial.h>
#include <linux/uaccess.h>

static const struct usb_device_id id_table[] = {
	{ USB_DEVICE(0x18d1, 0x2d01) },
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
