// SPDX-License-Identifier: GPL-2.0
/*
 * hello.c — example module, a bit richer than test_mod.
 * Demonstrates a module parameter read at load time:
 *   sudo insmod hello/hello.ko repeat=3
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

static int repeat = 1;          /* how many times to greet */
module_param(repeat, int, 0644);
MODULE_PARM_DESC(repeat, "Number of times to print the greeting (default 1)");

static int __init hello_init(void)
{
	int i;

	for (i = 0; i < repeat; i++)
		pr_info("hello: greetings #%d\n", i + 1);
	return 0;
}

static void __exit hello_exit(void)
{
	pr_info("hello: leaving after %d greeting(s)\n", repeat);
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Example module with a parameter");
MODULE_LICENSE("GPL");
