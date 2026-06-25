#include <linux/init.h>
#include <linux/module.h>
#include <linux/hrtimer.h>

static struct hrtimer hrt_timer;
static ktime_t ktime;
static int count = 0;
static enum hrtimer_restart hrt_func(struct hrtimer *htimer)
{
	pr_info("hrtimer functon\n");
	if (count++ != 5)
		return HRTIMER_RESTART;
	else
		return HRTIMER_NORESTART;
}

static int __init hrt_init(void)
{
	ktime = ktime_set(1, 0);
	hrtimer_setup(&hrt_timer, hrt_func, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	hrtimer_start(&hrt_timer, ktime, HRTIMER_MODE_REL);
	pr_info("hrtimer init\n");
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
MODULE_DESCRIPTION("Periodic hrtimer example");
MODULE_VERSION("1.0");
