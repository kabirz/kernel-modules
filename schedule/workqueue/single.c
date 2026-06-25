#include <linux/module.h>
#include <linux/init.h>
#include <linux/workqueue.h>

static struct workqueue_struct *queue = NULL;
static struct work_struct work;

static void work_handler(struct work_struct *data)
{
	pr_alert("work handler function.\n");
}

static int __init test_init(void)
{
	queue = create_singlethread_workqueue("hello world");
	if (!queue)
		return -ENOMEM;

	INIT_WORK(&work, work_handler);
	schedule_work(&work);
	return 0;
}

static void __exit test_exit(void)
{
	destroy_workqueue(queue);
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Single-threaded workqueue example");
MODULE_VERSION("1.0");
module_init(test_init);
module_exit(test_exit);
