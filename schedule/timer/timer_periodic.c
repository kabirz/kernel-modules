// SPDX-License-Identifier: GPL-2.0
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list timer;

static void timer_func(struct timer_list *t)
{
	pr_info("timer function\n");
	mod_timer(&timer, jiffies + 2 * HZ);
}

static int __init timer_init(void)
{
	timer_setup(&timer, timer_func, 0);
	timer.expires = jiffies + 2 * HZ;
	add_timer(&timer);
	return 0;
}

static void __exit timer_exit(void)
{
	timer_delete(&timer);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Timer sample using timer_setup API");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
