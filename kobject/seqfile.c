#include <net/net_namespace.h> /* for use of init_net*/
#include <linux/module.h> /* Specifically, a module */
#include <linux/proc_fs.h> /* Necessary because we use proc fs */
#include <linux/seq_file.h> /* for seq_file */
#define PROC_NAME "my_seq_proc"

static void *my_seq_start(struct seq_file *s, loff_t *pos)
{
	static unsigned long counter = 0;
	pr_info("Invoke start\n");
	/* beginning a new sequence ? */
	if (*pos == 0) {
		/* yes => return a non null value to begin the sequence */
		pr_info("pos == 0\n");
		return &counter;
	} else {
		/* no => it's the end of the sequence, return end to stop reading */
		*pos = 0;
		pr_info("pos != 0\n");
		return NULL;
	}
}
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)
{
	unsigned long *tmp_v = (unsigned long *)v;
	pr_info("Invoke next\n");
	(*tmp_v)++;
	(*pos)++;
	return NULL;
}
static void my_seq_stop(struct seq_file *s, void *v)
{
	pr_info("Invoke stop\n");
	/* nothing to do, we use a static value in start() */
}
static int my_seq_show(struct seq_file *s, void *v)
{
	loff_t *spos = (loff_t *)v;
	pr_info("Invoke show\n");
	seq_printf(s, "%Ld\n", *spos);
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
	pr_info("Initialze my_seq_proc success!\n");
	return 0;
}
/**
 * This function is called when the module is unloaded.
 *
 */
void cleanup_module(void)
{
	remove_proc_entry(PROC_NAME, init_net.proc_net);
	pr_info("Remove my_seq_proc success!\n");
}
MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Seq_file proc example");
MODULE_VERSION("1.0");
