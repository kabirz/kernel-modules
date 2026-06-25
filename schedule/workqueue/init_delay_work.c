#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>

static struct delayed_work init_work;
static void work_func(struct work_struct *work)
{
	pr_info("init working\n");
}

static int __init init_init(void)
{
	pr_info("init begin\n");
	INIT_DELAYED_WORK(&init_work, work_func);
	schedule_delayed_work(&init_work, 2 * HZ);
	return 0;
}

static void __exit init_exit(void)
{
	cancel_delayed_work_sync(&init_work);
	pr_info("init end\n");
}

module_init(init_init);
module_exit(init_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Initial delayed work example");
MODULE_VERSION("1.0");
