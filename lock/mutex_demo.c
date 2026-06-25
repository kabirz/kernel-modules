// SPDX-License-Identifier: GPL-2.0
/*
 * mutex_demo.c — Mutex example.
 * Demonstrates mutex usage for protecting shared data.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/kthread.h>

static DEFINE_MUTEX(my_mutex);
static int shared_data = 0;
static struct task_struct *task1;
static struct task_struct *task2;
static int thread_running = 1;

static int thread_func1(void *data)
{
	while (!kthread_should_stop() && thread_running) {
		mutex_lock(&my_mutex);
		shared_data++;
		pr_info("mutex_demo: thread1, data=%d\n", shared_data);
		mutex_unlock(&my_mutex);

		msleep(100);
	}
	return 0;
}

static int thread_func2(void *data)
{
	while (!kthread_should_stop() && thread_running) {
		mutex_lock(&my_mutex);
		shared_data--;
		pr_info("mutex_demo: thread2, data=%d\n", shared_data);
		mutex_unlock(&my_mutex);

		msleep(150);
	}
	return 0;
}

static int __init simple_init(void)
{
	mutex_init(&my_mutex);

	task1 = kthread_run(thread_func1, NULL, "mutex1");
	if (IS_ERR(task1)) {
		pr_err("mutex_demo: failed to create thread1\n");
		return PTR_ERR(task1);
	}

	task2 = kthread_run(thread_func2, NULL, "mutex2");
	if (IS_ERR(task2)) {
		kthread_stop(task1);
		pr_err("mutex_demo: failed to create thread2\n");
		return PTR_ERR(task2);
	}

	pr_info("mutex_demo: threads started\n");
	return 0;
}

static void __exit simple_exit(void)
{
	thread_running = 0;
	kthread_stop(task1);
	kthread_stop(task2);
	mutex_destroy(&my_mutex);
	pr_info("mutex_demo: final data=%d\n", shared_data);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Mutex example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");