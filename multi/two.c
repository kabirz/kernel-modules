#include <linux/init.h>
#include <linux/module.h>

extern int printf_m(void);
static int __init print_init(void)
{
	return printf_m();
}

static void __exit print_exit(void)
{
}

module_init(print_init);
module_exit(print_exit);
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Module using exported symbol");
MODULE_VERSION("1.0");
