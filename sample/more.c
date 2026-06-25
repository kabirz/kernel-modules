#include <linux/module.h>

static int value = 5;
static char *name = "shrek";
module_param(value, int, 0444);
module_param(name, charp, 0644);

/* 模块的入口函数 */
static int __init my_init(void)
{
	pr_info("I'm %s, pay me %d dollars\n", name, value);
	return 0;
}

/* 模块的出口函数 */
static void __exit my_exit(void)
{
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module parameters example (multi-value)");
MODULE_VERSION("1.0");
