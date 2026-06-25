// SPDX-License-Identifier: GPL-2.0

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/workqueue.h>

static struct hrtimer vibe_timer;
static struct work_struct vibe_work;

static void vibe_work_func(struct work_struct *work)
{
	pr_info("vibe_work_func: msleep(50)\n");
	msleep(50);
}

static enum hrtimer_restart vibrator_timer_func(struct hrtimer *timer)
{
	struct timespec64 uptime;

	ktime_get_ts64(&uptime);
	pr_info("Time:%lld.%02ld\n", (s64)uptime.tv_sec,
		uptime.tv_nsec / (NSEC_PER_SEC / 100));
	pr_info("vibrator_timer_func\n");
	schedule_work(&vibe_work);
	return HRTIMER_NORESTART;
}

static int __init ker_driver_init(void)
{
	struct timespec64 uptime;

	pr_info("ker_driver_init\n");
	hrtimer_setup(&vibe_timer, vibrator_timer_func, CLOCK_MONOTONIC,
		      HRTIMER_MODE_REL);
	hrtimer_start(&vibe_timer, ktime_set(2, 0), HRTIMER_MODE_REL);

	ktime_get_ts64(&uptime);
	pr_info("Time:%lld.%02ld\n", (s64)uptime.tv_sec,
		uptime.tv_nsec / (NSEC_PER_SEC / 100));

	INIT_WORK(&vibe_work, vibe_work_func);
	return 0;
}

static void __exit ker_driver_exit(void)
{
	hrtimer_cancel(&vibe_timer);
}

module_init(ker_driver_init);
module_exit(ker_driver_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Kernel driver with hrtimer and workqueue");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
