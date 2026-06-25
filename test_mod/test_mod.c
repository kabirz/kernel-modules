// SPDX-License-Identifier: GPL-2.0
/*
 * Minimal Linux kernel test module.
 *
 * Build layout: Kbuild (build declaration) + Makefile (orchestration).
 * Demonstrates the two-pass kbuild parsing — see README.md.
 */

#include <linux/init.h>
#include <linux/module.h>

#define MOD_NAME "test_mod"

static int __init test_mod_init(void)
{
	pr_info("%s: loaded\n", MOD_NAME);
	return 0;
}

static void __exit test_mod_exit(void)
{
	pr_info("%s: unloaded\n", MOD_NAME);
}

module_init(test_mod_init);
module_exit(test_mod_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Minimal kernel test module");
MODULE_LICENSE("GPL");
