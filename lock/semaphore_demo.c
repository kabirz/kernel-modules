// SPDX-License-Identifier: GPL-2.0
/*
 * semaphore_demo.c — Semaphore example.
 * Demonstrates semaphore usage for limiting concurrent access.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static DEFINE_SEMAPHORE(my_sem, 1);
static int active_threads = 0;
static struct task_struct *threads[5];
static int thread_running = 1;

static int thread_func(void *data)
{
	int id = (int)(long)data;

	while (!kthread_should_stop() && thread_running) {
		down(&my_sem);
		active_threads++;
		pr_info("semaphore_demo: thread %d acquired semaphore, active=%d\n",
			id, active_threads);

		msleep(500);

		active_threads--;
		up(&my_sem);
		pr_info("semaphore_demo: thread %d released semaphore, active=%d\n",
			id, active_threads);

		msleep(100);
	}
	return 0;
}

static int __init simple_init(void)
{
	int i;

	for (i = 0; i < 5; i++) {
		threads[i] = kthread_run(thread_func, (void *)(long)i,
					 "sem_thread%d", i);
		if (IS_ERR(threads[i])) {
			pr_err("semaphore_demo: failed to create thread %d\n",
			       i);
			while (i-- > 0)
				kthread_stop(threads[i]);
			return PTR_ERR(threads[i]);
		}
	}

	pr_info("semaphore_demo: 5 threads started\n");
	return 0;
}

static void __exit simple_exit(void)
{
	int i;

	thread_running = 0;
	for (i = 0; i < 5; i++)
		kthread_stop(threads[i]);

	pr_info("semaphore_demo: all threads stopped\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Semaphore example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");