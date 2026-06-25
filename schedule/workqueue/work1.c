// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list timer;

static void work_func(struct work_struct *work)
{
	pr_info("work begin!\n");
}

static DECLARE_WORK(work1, work_func);

static void do_func(struct timer_list *t)
{
	schedule_work(&work1);
	mod_timer(&timer, jiffies + 2 * HZ);
}

static int __init work_init(void)
{
	timer_setup(&timer, do_func, 0);
	timer.expires = jiffies + 2 * HZ;
	add_timer(&timer);
	return 0;
}

static void __exit work_exit(void)
{
	timer_delete(&timer);
}

module_init(work_init);
module_exit(work_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@128.com>");
MODULE_DESCRIPTION("Workqueue with DECLARE_WORK and timer example");
MODULE_VERSION("1.0");
