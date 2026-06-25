// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list timer;
static struct work_struct init_work;

static void work_func(struct work_struct *work)
{
	pr_info("init working\n");
}

static void do_func(struct timer_list *t)
{
	schedule_work(&init_work);
	mod_timer(&timer, jiffies + 2 * HZ);
}

static int __init init_init(void)
{
	pr_info("init begin\n");
	timer_setup(&timer, do_func, 0);
	timer.expires = jiffies + 2 * HZ;
	add_timer(&timer);

	INIT_WORK(&init_work, work_func);
	return 0;
}

static void __exit init_exit(void)
{
	timer_delete(&timer);
	pr_info("init end\n");
}

module_init(init_init);
module_exit(init_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Workqueue with timer example");
MODULE_VERSION("1.0");
