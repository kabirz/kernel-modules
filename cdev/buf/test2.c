#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>

static dev_t dev_no;

static __init int test_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev_no, 2, 1, "cdev_test2");
	if (ret < 0) {
		pr_info("alloc_chrdev_region() error\n");
		return -EPERM;
	}

	pr_info("MAJOR:%d, MINOR:%d\n", MAJOR(dev_no), MINOR(dev_no));

	return 0;
}

static __exit void test_exit(void)
{
	unregister_chrdev_region(dev_no, 1);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("This is cdev");
MODULE_VERSION("1.0");
