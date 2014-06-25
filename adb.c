#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/usb.h>
#include <linux/slab.h>
#include <linux/errno.h>

#define DEBUG_TAG "ADK-ADB"
#define ADB_MINOR_BASE	16 /* Base address for the minor number */

#define VID	0x18d1	//VID in ADK + ADB mode
#define PID	0x2d01	//PID in ADK + ADB mode
#define CL	0xff	//Class
#define SC	0x42	//Subclass
#define PR	0x01	//Protocol

/* table of devices that work with this driver */
static struct usb_device_id adb_devices [] = {
	{ USB_DEVICE_AND_INTERFACE_INFO(VID, PID, CL, SC, PR) },
	{ } /* Terminating entry */
};

MODULE_DEVICE_TABLE (usb, adb_devices);

/* Structure to hold all of our device specific stuff */
struct adb_device {
	struct usb_device 	*udev;		/* usb device for this device */
	struct usb_interface 	*interface;	/* interface for this device */
	unsigned char 		minor;		/* minor value */
	unsigned char *		bulk_in_buffer;	/* the buffer to in data */
	size_t			bulk_in_size;	/* the size of the in buffer */
	__u8			bulk_in_add;	/* bulk in endpoint address */
	__u8			bulk_out_add;	/* bulk out endpoint address */
	struct kref		kref;		/* module references counter */
};

static struct usb_driver adb_driver;

static int
print (char *msg)
{
	return printk("%s : %s\n", DEBUG_TAG, msg);
}

static inline void
adb_delete (struct adb_device *dev)
{
	print("adb: deleting dev");
	kfree(dev);
}

static int
adb_open (struct inode *inode, struct file *file)
{
	struct adb_device *dev = NULL;
	struct usb_interface *interface;
	int subminor;
	int retval = 0;

	print("open: Device opened");
	subminor = iminor(inode);

	interface = usb_find_interface(&adb_driver, subminor);
	if (!interface) {
		printk("Can't find device for minor %d!", subminor);
		retval = -ENODEV;
		goto exit;
	}

	dev = usb_get_intfdata(interface);
	if (!dev) {
		print("Device NULL!");
		retval = -ENODEV;
		goto exit;
	} else {
		print("Non Null device found");
	}

	file->private_data = dev;

exit:
	return retval;
}

static int
adb_release (struct inode *inode, struct file *file)
{
	struct adb_device *dev = NULL;
	int retval = 0;

	print("adb_release: Device released");
	dev = file->private_data;

	if (!dev) {
		print("Dev is NULL!");
		retval = -ENODEV;
		goto exit;
	}

	if (!dev->udev) {
		print("Device unplugged before the file was released!");
		goto exit;
	}

exit:
	return retval;
}

static ssize_t
adb_write (struct file *file, const char __user *user_buf,
	 size_t count, loff_t *ppos)
{
	int retval = 0;
	
	print("Write requested");
	print("Operation not supported");
	
	return retval;
}
static ssize_t
adb_read (struct file *file, char __user *user_buf,
	 size_t count, loff_t *ppos)
{
	int retval = 0;
	struct adb_device *dev;
	
	dev = file->private_data;
	
	/* do a blocking bulk read to get data from the device */
	retval = usb_bulk_msg(dev->udev,
		      usb_rcvbulkpipe(dev->udev, dev->bulk_in_add),
		      dev->bulk_in_buffer,
		      min(dev->bulk_in_size, count),
		      &count, HZ*20);
	
	printk("ADB: Bulk read return code: %d\n", retval);

	/* if the read was successful, copy the data to user space */
	if (!retval) {
	    if (copy_to_user(user_buf, dev->bulk_in_buffer, count))
		retval = -EFAULT;
	    else
		retval = count;
	}
	
	return retval;
}

static struct file_operations adb_fops = {
	.owner =	THIS_MODULE,
	.write =	adb_write,
	.read =		adb_read,
	.open =		adb_open,
	.release =	adb_release,
};

static struct usb_class_driver adb_usb_class = {
	.name = "adb%d",
	.fops = &adb_fops,
	.minor_base = ADB_MINOR_BASE,
};

static void
set_bulk_address (
	struct adb_device *dev,
	struct usb_interface *interface)
{
	struct usb_endpoint_descriptor *endpoint;
	struct usb_host_interface *iface_desc;
	int i;
	
	iface_desc = interface->cur_altsetting;
	for (i = 0; i < iface_desc->desc.bNumEndpoints; ++i) {
		endpoint = &iface_desc->endpoint[i].desc;
		
		/* check for bulk endpoint */
		if ((endpoint->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) 
			== USB_ENDPOINT_XFER_BULK){
			
			/* bulk in */
			if(endpoint->bEndpointAddress & USB_DIR_IN) {
				dev->bulk_in_add = endpoint->bEndpointAddress;
				dev->bulk_in_size = endpoint->wMaxPacketSize;
				dev->bulk_in_buffer = kmalloc(dev->bulk_in_size,
							 	GFP_KERNEL);
				if (!dev->bulk_in_buffer)
					print("Could not allocate bulk buffer");
			}
			
			/* bulk out */
			else
				dev->bulk_out_add = endpoint->bEndpointAddress;	
		}
	}
}

static int
adb_probe (struct usb_interface *interface, const struct usb_device_id *id)
{
	struct adb_device *dev;
	int retval = -ENODEV;
	print("Device attached");
	
	dev = kmalloc(sizeof(*dev), GFP_KERNEL);
	if (dev == NULL) {
		retval = -ENOMEM;
		goto exit;
	}
	
	printk("Product id is: %04x \n", id->idProduct);

	memset(dev, 0x00, sizeof(*dev));
	
	dev->udev = interface_to_usbdev(interface);
	if (!dev->udev) {
		retval = -ENODEV;
		goto exit;
	}
	
	print("Attached as an accessory");
	dev->interface = interface;
	kref_init(&dev->kref);
	
	/* save our data pointer in this interface device. */
	usb_set_intfdata(interface, dev);
	
	kref_init(&dev->kref);
	set_bulk_address(dev, interface);
	printk("Bulk values: %04x, %04x\n", dev->bulk_in_add, dev->bulk_out_add);
	
	/* register the device now */
	retval = usb_register_dev(interface, &adb_usb_class);
	if (retval) {
		usb_set_intfdata(interface, NULL);
		goto error;
	}

	dev->minor = interface->minor;

	printk("New ADB device attached to /dev/adb%d\n", interface->minor);

exit:
	return retval;

error:
	adb_delete(dev);
	return retval;
}

static void
adb_disconnect (struct usb_interface *interface){
	struct adb_device *dev;
	int minor=0;

	dev = usb_get_intfdata(interface);

	/* Give back our minor. */
	if(dev != NULL)
		usb_deregister_dev(interface, &adb_usb_class);
	else
		printk("adb_disconnect: Dev is null!\n");

	printk("ADB device /dev/adb%d now disconnected\n", minor);
}

static struct usb_driver adb_driver = {
	.name = "adb_driver",
	.id_table = adb_devices,
	.probe = adb_probe,
	.disconnect = adb_disconnect,
};

static int __init
adb_init (void)
{
	int result;
	print("Init called!");

	result = usb_register(&adb_driver);
	if (result)
		print("Registering driver failed!");
	else
		print("Driver registered successfully!");

	return result;
}

static void __exit
adb_exit (void)
{
	usb_deregister(&adb_driver);
	print("Exit called!");
}

module_init(adb_init);
module_exit(adb_exit);

MODULE_AUTHOR("Praveen Kumar Pendyala");
MODULE_LICENSE("GPL");
