// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/jiffies.h>

#define PROC_NAME "proc_ro"

static int my_proc_show(struct seq_file *m, void *v)
{
	seq_printf(m, "Kernel time is %ld\n", jiffies);
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, my_proc_show, NULL);
}

static const struct proc_ops my_proc_fops = {

	.proc_open = my_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init my_init(void)
{
	if (!proc_create(PROC_NAME, 0444, NULL, &my_proc_fops)) {
		pr_err("Cannot create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	return 0;
}

static void __exit my_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Read-only proc file example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
