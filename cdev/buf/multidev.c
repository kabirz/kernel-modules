#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>

#define DEV_NUM 16

static int char_open(struct inode *no, struct file *fp)
{
	return 0;
}

static int char_release(struct inode *no, struct file *fp)
{
	return 0;
}

static const struct file_operations char_ops = {
	.open = char_open,
	.release = char_release,
};

struct cdev dev[DEV_NUM];
struct class *cls;
dev_t dev_no;

static __init int my_init(void)
{
	int ret, i;

	ret = alloc_chrdev_region(&dev_no, 0, DEV_NUM, "led");
	if (ret < 0) {
		pr_info("alloc_chrdev_region chrdev_test1 error\n");
		return ret;
	}

	cls = class_create("led");
	if (IS_ERR(cls)) {
		ret = PTR_ERR(cls);
		goto class_err;
	}

	pr_info("MAJOR : %d\nMINOR : %d\n", MAJOR(dev_no), MINOR(dev_no));

	for (i = 0; i < DEV_NUM; i++) {
		cdev_init(&dev[i], &char_ops);
		ret = cdev_add(&dev[i], dev_no + i, 1);
		if (ret < 0) {
			pr_info("cdev_add error for dev %d\n", i);
			goto del_cdevs;
		}
	}

	for (i = 0; i < DEV_NUM; i++) {
		ret = PTR_ERR(device_create(cls, NULL, dev_no + i, NULL,
					    "led%d", i));
		if (ret < 0)
			goto destroy_devices;
	}

	return 0;

destroy_devices:
	while (i > 0) {
		i--;
		device_destroy(cls, dev_no + i);
	}
del_cdevs:
	for (i = 0; i < DEV_NUM; i++)
		cdev_del(&dev[i]);
	class_destroy(cls);
class_err:
	unregister_chrdev_region(dev_no, DEV_NUM);
	return ret;
}

static __exit void my_exit(void)
{
	int i;
	for (i = 0; i < DEV_NUM; i++) {
		device_destroy(cls, dev_no + i);
		cdev_del(&dev[i]);
	}

	class_destroy(cls);
	unregister_chrdev_region(dev_no, DEV_NUM);

	pr_info("Byebye module\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("This is simple test for module");
MODULE_VERSION("1.0");
