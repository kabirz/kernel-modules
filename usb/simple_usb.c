// SPDX-License-Identifier: GPL-2.0
/*
 * simple_usb.c — Simple USB device driver example.
 * Demonstrates basic USB device registration and communication.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/usb.h>

#define VENDOR_ID 0x1234
#define PRODUCT_ID 0x5678

struct simple_usb {
	struct usb_device *udev;
	struct usb_interface *interface;
};

static int simple_usb_probe(struct usb_interface *interface,
			    const struct usb_device_id *id)
{
	struct simple_usb *dev;
	struct usb_device *udev = usb_get_dev(interface_to_usbdev(interface));

	dev = kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	dev->udev = udev;
	dev->interface = interface;

	usb_set_intfdata(interface, dev);

	pr_info("simple_usb: device connected: %s\n",
		udev->product ? udev->product : "unknown");

	return 0;
}

static void simple_usb_disconnect(struct usb_interface *interface)
{
	struct simple_usb *dev = usb_get_intfdata(interface);

	usb_set_intfdata(interface, NULL);

	if (dev) {
		pr_info("simple_usb: device disconnected\n");
		kfree(dev);
	}
}

static int simple_usb_suspend(struct usb_interface *interface,
			      pm_message_t message)
{
	pr_info("simple_usb: device suspended\n");
	return 0;
}

static int simple_usb_resume(struct usb_interface *interface)
{
	pr_info("simple_usb: device resumed\n");
	return 0;
}

static const struct usb_device_id simple_usb_id_table[] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{}
};
MODULE_DEVICE_TABLE(usb, simple_usb_id_table);

static struct usb_driver simple_usb_driver = {
	.name = "simple_usb",
	.probe = simple_usb_probe,
	.disconnect = simple_usb_disconnect,
	.suspend = simple_usb_suspend,
	.resume = simple_usb_resume,
	.id_table = simple_usb_id_table,
};

static int __init simple_init(void)
{
	int ret;

	ret = usb_register(&simple_usb_driver);
	if (ret) {
		pr_err("simple_usb: failed to register driver\n");
		return ret;
	}

	pr_info("simple_usb: driver registered\n");
	return 0;
}

static void __exit simple_exit(void)
{
	usb_deregister(&simple_usb_driver);
	pr_info("simple_usb: driver unregistered\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple USB device driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");