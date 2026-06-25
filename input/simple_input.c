// SPDX-License-Identifier: GPL-2.0
/*
 * simple_input.c — Simple input device driver example.
 * Demonstrates basic input device registration and event reporting.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/input.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct input_dev *input_dev;
static struct timer_list input_timer;

static void input_timer_func(struct timer_list *t)
{
	/* Generate a random key press/release event */
	static int key_value = 0;

	key_value = !key_value;

	input_report_key(input_dev, KEY_A, key_value);
	input_sync(input_dev);

	pr_info("simple_input: key event, value=%d\n", key_value);

	/* Reschedule timer */
	mod_timer(&input_timer, jiffies + HZ);
}

static int __init simple_init(void)
{
	int ret;

	/* Allocate input device */
	input_dev = input_allocate_device();
	if (!input_dev)
		return -ENOMEM;

	input_dev->name = "Simple Input Device";
	input_dev->phys = "simple-input/input0";
	input_dev->id.bustype = BUS_VIRTUAL;
	input_dev->id.vendor = 0x0000;
	input_dev->id.product = 0x0000;
	input_dev->id.version = 0x0001;

	/* Set capabilities */
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(KEY_A, input_dev->keybit);

	/* Register input device */
	ret = input_register_device(input_dev);
	if (ret) {
		pr_err("simple_input: failed to register device\n");
		input_free_device(input_dev);
		return ret;
	}

	/* Setup timer for periodic events */
	timer_setup(&input_timer, input_timer_func, 0);
	mod_timer(&input_timer, jiffies + HZ);

	pr_info("simple_input: device registered\n");
	return 0;
}

static void __exit simple_exit(void)
{
	timer_delete_sync(&input_timer);
	input_unregister_device(input_dev);
	pr_info("simple_input: device unregistered\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple input device driver example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");