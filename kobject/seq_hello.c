#include <net/net_namespace.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define PROC_NAME "test_proc"
#define MAX_LINES 10

typedef struct item {
	unsigned long key;
	unsigned char value;
} user_item;

user_item items[4];

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;
	pr_info("Invoke start\n");

	if (*pos == 0) {
		/* yes => return a non null value to begin the sequence */
		return &counter;
	} else {
		/* no => it's the end of the sequence, return end to stop reading */
		*pos = 0;
		return NULL;
	}
}

static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	pr_info("Invoke next\n");
	if (*pos < MAX_LINES) {
		(*tmp_v)++;
		(*pos)++;
		return tmp_v;
	} else {
		*pos = 0;
		return NULL;
	}
}

static void my_seq_stop(struct seq_file *s, void *v)
{
	pr_info("Invoke stop\n");
}

static int my_seq_show(struct seq_file *s, void *v)
{
	int i;
	loff_t *spos = (loff_t *)v;
	pr_info("Invoke show\n");
	for (i = 0; i < 4; i++) {
		items[i].key = *spos;
	}
	items[0].value = '0';
	items[1].value = '1';
	items[2].value = '2';
	items[3].value = '3';
	seq_printf(s, "%ld=%c,%ld=%c,%ld=%c,%ld=%c;\n", items[0].key,
		   items[0].value, items[1].key, items[1].value, items[2].key,
		   items[2].value, items[3].key, items[3].value);
	return 0;
}

static const struct seq_operations my_seq_ops = {
	.start = my_seq_start,
	.next = my_seq_next,
	.stop = my_seq_stop,
	.show = my_seq_show,
};

static int my_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_ops);
};

static const struct proc_ops my_file_ops = {

	.proc_open = my_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release
};

int init_module(void)
{
	struct proc_dir_entry *entry;
	entry = proc_create(PROC_NAME, 666, init_net.proc_net, &my_file_ops);
	pr_info("Initialze /proc/net/test_proc success!\n");
	return 0;
}

void cleanup_module(void)
{
	remove_proc_entry(PROC_NAME, init_net.proc_net);
	pr_info("Remove /proc/net/test_proc success!\n");
}
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Seq_file hello example");
MODULE_VERSION("1.0");
