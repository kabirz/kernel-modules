// SPDX-License-Identifier: GPL-2.0
/*
 * kernel_thread.c — Kernel thread example.
 * Demonstrates kernel thread creation, execution, and termination.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *my_thread;
static int thread_running = 1;

static int thread_func(void *data)
{
	int count = 0;

	pr_info("kernel_thread: thread started\n");

	while (!kthread_should_stop() && thread_running) {
		count++;
		pr_info("kernel_thread: running, count=%d\n", count);

		/* Sleep for 1 second */
		if (msleep_interruptible(1000))
			break;
	}

	pr_info("kernel_thread: thread stopped\n");
	return 0;
}

static int __init simple_init(void)
{
	my_thread = kthread_create(thread_func, NULL, "my_thread");
	if (IS_ERR(my_thread)) {
		pr_err("kernel_thread: failed to create thread\n");
		return PTR_ERR(my_thread);
	}

	wake_up_process(my_thread);
	pr_info("kernel_thread: thread created and started\n");
	return 0;
}

static void __exit simple_exit(void)
{
	thread_running = 0;
	kthread_stop(my_thread);
	pr_info("kernel_thread: thread stopped and cleaned up\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Kernel thread example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");