#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/slab.h>

static int chropen;
static struct cdev chardev;
static struct class *my_class;
static int len;
static dev_t dev;

static char *to;

static int char_open(struct inode *inode, struct file *file)
{
	if (chropen == 0)
		chropen++;
	else {
		pr_alert("Another process open the char device\n");
		return -EBUSY;
	}
	try_module_get(THIS_MODULE);
	return 0;
}

static int char_release(struct inode *inode, struct file *file)
{
	chropen--;
	module_put(THIS_MODULE);
	return 0;
}

static ssize_t char_read(struct file *filp, char __user *buffer, size_t length,
			 loff_t *offset)
{
	if (copy_to_user(buffer, to, length))
		return -EFAULT;
	return length;
}

static ssize_t char_write(struct file *filp, const char __user *buffer,
			  size_t length, loff_t *offset)
{
	to = (char *)kmalloc(length + 1, GFP_KERNEL);
	if (!to)
		return -ENOMEM;

	memset(to, '\0', length + 1);
	if (copy_from_user(to, buffer, length)) {
		kfree(to);
		to = NULL;
		return -EFAULT;
	}

	pr_info("to = %s\n", to);
	return length;
}

static const struct file_operations char_ops = {
	.read = char_read,
	.write = char_write,
	.open = char_open,
	.release = char_release,
};

static int __init char_init(void)
{
	int ret;

	pr_alert("Initing......\n");

	cdev_init(&chardev, &char_ops);
	chardev.owner = THIS_MODULE;

	ret = alloc_chrdev_region(&dev, 0, 10, "xmimx");
	if (ret) {
		pr_alert("Register char dev error\n");
		return ret;
	}

	chropen = 0;
	len = 0;

	ret = cdev_add(&chardev, dev, 1);
	if (ret) {
		pr_alert("Add char dev error!\n");
		goto err_unregister;
	}

	my_class = class_create("xmimx_class");
	if (IS_ERR(my_class)) {
		pr_info("Err: failed in creating class.\n");
		ret = PTR_ERR(my_class);
		goto err_del_cdev;
	}

	device_create(my_class, NULL, dev, NULL, "xmimx");
	return 0;

err_del_cdev:
	cdev_del(&chardev);
err_unregister:
	unregister_chrdev_region(dev, 10);
	return ret;
}

static void __exit module_close(void)
{
	len = 0;
	pr_alert("Unloading..........\n");

	device_destroy(my_class, dev);
	class_destroy(my_class);
	cdev_del(&chardev);
	unregister_chrdev_region(dev, 10);
}

module_init(char_init);
module_exit(module_close);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("This is a simple cdev driver");
MODULE_VERSION("1.0");
