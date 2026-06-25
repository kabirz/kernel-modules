#include <linux/init.h>
#include <linux/module.h>
#include <linux/hrtimer.h>
#include <linux/workqueue.h>

static struct hrtimer hrt_timer;
static struct work_struct hrt_work;
static ktime_t ktime;

static void work_func(struct work_struct *work)
{
	pr_info("work addr is %p\n", work);
}

static enum hrtimer_restart hrt_func(struct hrtimer *htimer)
{
	pr_info("hrtimer functon\n");
	hrtimer_start(&hrt_timer, ktime, HRTIMER_MODE_REL);
	schedule_work(&hrt_work);
	return HRTIMER_NORESTART;
}

static int __init hrt_init(void)
{
	ktime = ktime_set(5, 0);
	hrtimer_setup(&hrt_timer, hrt_func, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&hrt_timer, ktime, HRTIMER_MODE_REL);
	pr_info("hrtimer init\n");
	INIT_WORK(&hrt_work, work_func);
	pr_info("work init is %p\n", &hrt_work);
	return 0;
}

static void __exit hrt_exit(void)
{
	hrtimer_cancel(&hrt_timer);
	pr_info("hrtimer cancle\n");
}

module_init(hrt_init);
module_exit(hrt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("HR timer with workqueue example");
MODULE_VERSION("1.0");
