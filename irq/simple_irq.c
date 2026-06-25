// SPDX-License-Identifier: GPL-2.0
/*
 * simple_irq.c — Simple interrupt handler example.
 * Demonstrates basic interrupt handling using request_irq and tasklets.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/slab.h>

#define IRQ_NUM 1 /* Example IRQ number */

static int irq_count = 0;
static struct tasklet_struct my_tasklet;

static void tasklet_func(struct tasklet_struct *t)
{
	pr_info("simple_irq: tasklet executed\n");
}

static irqreturn_t simple_irq_handler(int irq, void *dev_id)
{
	irq_count++;
	pr_info("simple_irq: interrupt %d received, count=%d\n", irq,
		irq_count);

	/* Schedule tasklet */
	tasklet_schedule(&my_tasklet);

	return IRQ_HANDLED;
}

static int __init simple_init(void)
{
	int ret;

	/* Initialize tasklet */
	tasklet_setup(&my_tasklet, tasklet_func);

	/* Request IRQ */
	ret = request_irq(IRQ_NUM, simple_irq_handler, IRQF_SHARED,
			  "simple_irq", (void *)&irq_count);
	if (ret) {
		pr_err("simple_irq: failed to request IRQ %d\n", IRQ_NUM);
		tasklet_kill(&my_tasklet);
		return ret;
	}

	pr_info("simple_irq: IRQ %d registered\n", IRQ_NUM);
	return 0;
}

static void __exit simple_exit(void)
{
	free_irq(IRQ_NUM, (void *)&irq_count);
	tasklet_kill(&my_tasklet);
	pr_info("simple_irq: IRQ %d unregistered\n", IRQ_NUM);
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Simple interrupt handler example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");