#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#define NUM 10

static int get_random_number(void)
{
	char randNum[NUM];
	int i = 0;

	pr_alert("Get some real random number.\n");
	for (i = 0; i < NUM; i++) {
		get_random_bytes(&randNum[i], sizeof(char));
		pr_alert("We get random number: %d\n", randNum[i]);
	}
	return 0;
}

static void random_exit(void)
{
	pr_alert("quit get_random_num.\n");
}

module_init(get_random_number);
module_exit(random_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Random number module");
MODULE_VERSION("1.0");
