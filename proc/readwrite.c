// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

#define PROC_NAME "proc_rw"

static char *str;

static int my_proc_show(struct seq_file *m, void *v)
{
	if (str)
		seq_printf(m, "%s", str);
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, my_proc_show, NULL);
}

static ssize_t my_proc_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *pos)
{
	char *tmp;

	tmp = memdup_user_nul(buf, count);
	if (IS_ERR(tmp))
		return PTR_ERR(tmp);

	kfree(str);
	str = tmp;

	return count;
}

static const struct proc_ops my_proc_fops = {

	.proc_open = my_proc_open,
	.proc_read = seq_read,
	.proc_write = my_proc_write,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int __init my_init(void)
{
	if (!proc_create(PROC_NAME, 0666, NULL, &my_proc_fops)) {
		pr_err("Cannot create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}
	return 0;
}

static void __exit my_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
	kfree(str);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Read-write proc file example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
