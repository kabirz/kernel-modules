// SPDX-License-Identifier: GPL-2.0

#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list timer;
static struct workqueue_struct *wq;
static struct work_struct init_work;

static void work_func(struct work_struct *work)
{
	pr_info("queue working\n");
}

static void timer_fun(struct timer_list *t)
{
	queue_work(wq, &init_work);
}

static int __init queue_init(void)
{
	timer_setup(&timer, timer_fun, 0);
	timer.expires = jiffies + 2 * HZ;
	add_timer(&timer);

	INIT_WORK(&init_work, work_func);
	wq = create_singlethread_workqueue("queue");
	if (!wq)
		return -ENOMEM;

	return 0;
}

static void __exit queue_exit(void)
{
	timer_delete(&timer);
	flush_workqueue(wq);
	destroy_workqueue(wq);
}

module_init(queue_init);
module_exit(queue_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Workqueue with single thread and timer example");
MODULE_VERSION("1.0");
