#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/fs.h>
////////////////////////////////////////////////////////////////////////////////
/* 
 * Yao.GUET
 * http://blog.csdn.net/Yao_GUET
 * a simple platform character driver
 */
////////////////////////////////////////////////////////////////////////////////
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Yao.GUET");
MODULE_DESCRIPTION("Simple platform character driver example");
MODULE_VERSION("1.0");
////////////////////////////////////////////////////////////////////////////////
static int chrdev_open(struct inode *inode, struct file *file)
{
	pr_alert("chrdev open!\n");
	return 0;
}

static int chrdev_release(struct inode *inode, struct file *file)
{
	pr_alert("chrdev release!\n");
	return 0;
}

static long chrdev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	pr_alert("chrdev release!\n");
	return 0;
}

// Kernel interface
static const struct file_operations chrdev_fops = {
	.owner = THIS_MODULE,
	.unlocked_ioctl = chrdev_ioctl,
	.open = chrdev_open,
	.release = chrdev_release,
};

#define CHRDEV_NAME "chrdev"
// driver interface
static struct class *chrdev_class = NULL;
static struct device *chrdev_device = NULL;
static dev_t chrdev_devno;
static struct cdev chrdev_cdev;

static int chrdev_probe(struct platform_device *dev)
{
	int ret;

	pr_alert("chrdev probe!\n");

	// alloc character device number
	ret = alloc_chrdev_region(&chrdev_devno, 0, 1, CHRDEV_NAME);
	if (ret) {
		pr_alert(" alloc_chrdev_region failed!\n");
		goto err_alloc;
	}
	pr_alert(" major:%d minor:%d\n", MAJOR(chrdev_devno),
		 MINOR(chrdev_devno));

	cdev_init(&chrdev_cdev, &chrdev_fops);
	chrdev_cdev.owner = THIS_MODULE;
	// add a character device
	ret = cdev_add(&chrdev_cdev, chrdev_devno, 1);
	if (ret) {
		pr_alert(" cdev_add failed!\n");
		goto err_cdev;
	}

	// create the device class
	chrdev_class = class_create(CHRDEV_NAME);
	if (IS_ERR(chrdev_class)) {
		pr_alert(" class_create failed!\n");
		ret = PTR_ERR(chrdev_class);
		goto err_cdev;
	}

	// create the device node in /dev
	chrdev_device = device_create(chrdev_class, NULL, chrdev_devno, NULL,
				      CHRDEV_NAME);
	if (IS_ERR(chrdev_device)) {
		pr_alert(" device_create failed!\n");
		ret = PTR_ERR(chrdev_device);
		goto err_class;
	}

	pr_alert(" chrdev probe ok!\n");
	return 0;

err_class:
	class_destroy(chrdev_class);
err_cdev:
	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(chrdev_devno, 1);
err_alloc:
	return ret;
}

static void chrdev_remove(struct platform_device *dev)
{
	pr_alert(" chrdev remove!\n");

	cdev_del(&chrdev_cdev);
	unregister_chrdev_region(chrdev_devno, 1);

	device_destroy(chrdev_class, chrdev_devno);
	class_destroy(chrdev_class);
}

// platform_device and platform_driver must has a same name!
// or it will not work normally
static struct platform_driver chrdev_platform_driver = {
    .probe  =   chrdev_probe,
    .remove =   chrdev_remove,
    .driver =   {
        .name   =   CHRDEV_NAME,
        .owner  =   THIS_MODULE,
    },
};

static struct platform_device chrdev_platform_device = { .name = CHRDEV_NAME,
							 .id = 0,
							 .dev = {} };

static __init int chrdev_init(void)
{
	int ret = 0;
	pr_alert("chrdev init!\n");

	ret = platform_device_register(&chrdev_platform_device);
	if (ret) {
		pr_alert(" platform_device_register failed!\n");
		return ret;
	}

	ret = platform_driver_register(&chrdev_platform_driver);
	if (ret) {
		pr_alert(" platform_driver_register failed!\n");
		return ret;
	}
	pr_alert(" chrdev_init ok!\n");
	return ret;
}

static __exit void chrdev_exit(void)
{
	pr_alert("chrdev exit!\n");
	platform_driver_unregister(&chrdev_platform_driver);
}

module_init(chrdev_init);
module_exit(chrdev_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Platform driver example");
