#include <linux/init.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/workqueue.h>

static void work_func(struct work_struct *work)
{
	pr_info("work working!\n");
}
static DECLARE_DELAYED_WORK(work3, work_func);
static int __init work_init(void)
{
	pr_info("work begin!\n");
	schedule_delayed_work(&work3, 3 * HZ);
	return 0;
}

static void __exit work_exit(void)
{
	cancel_delayed_work_sync(&work3);
	pr_info("work end!\n");
}

module_init(work_init);
module_exit(work_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Delayed workqueue example");
MODULE_VERSION("1.0");
