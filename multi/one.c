#include <linux/init.h>
#include <linux/module.h>

int printf_m(void);
int printf_m(void)
{
	return 0;
}
EXPORT_SYMBOL_GPL(printf_m);
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Exported symbol example");
MODULE_VERSION("1.0");
