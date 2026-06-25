#include <linux/module.h>

extern void my_print(int);

/* 模块的入口函数 */
static int __init my_init(void)
{
	pr_info("In %s, call my_print()\n", __FILE__);
	my_print(5);
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
MODULE_DESCRIPTION("Module parameters example");
MODULE_VERSION("1.0");
