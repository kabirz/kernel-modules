#include <linux/module.h>

void my_print(int value);
void my_print(int value)
{
	pr_info("In %s-->%s, value = %d\n", __FILE__, __FUNCTION__, value);
}
EXPORT_SYMBOL(my_print);

/* 模块的入口函数 */
static int __init my_init(void)
{
	pr_info("Hello, old kernel 2.6.xxx\n");
	return 0;
}

/* 模块的出口函数 */
static void __exit my_exit(void)
{
	pr_info("Bye kernel\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
MODULE_DESCRIPTION("Simple test for module");
