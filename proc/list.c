// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>

#define PROC_NAME "proc_list"

struct proc_head {
	int file_size;
	struct list_head head;
};

struct proc_item {
	char *str;
	struct list_head item;
};

static struct proc_head *filehead;

static int my_proc_show(struct seq_file *m, void *v)
{
	struct proc_item *tmp;

	list_for_each_entry(tmp, &filehead->head, item)
		seq_printf(m, "%s", tmp->str);

	seq_printf(m, "File size is %d bytes\n", filehead->file_size);
	return 0;
}

static int my_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, my_proc_show, NULL);
}

static ssize_t my_proc_write(struct file *file, const char __user *buf,
			     size_t count, loff_t *pos)
{
	struct proc_item *tmp;

	tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	tmp->str = memdup_user_nul(buf, count);
	if (IS_ERR(tmp->str)) {
		int ret = PTR_ERR(tmp->str);
		kfree(tmp);
		return ret;
	}

	list_add_tail(&tmp->item, &filehead->head);
	filehead->file_size += count;

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
	filehead = kzalloc(sizeof(*filehead), GFP_KERNEL);
	if (!filehead)
		return -ENOMEM;

	INIT_LIST_HEAD(&filehead->head);
	filehead->file_size = 0;

	if (!proc_create(PROC_NAME, 0666, NULL, &my_proc_fops)) {
		pr_err("Cannot create /proc/%s\n", PROC_NAME);
		kfree(filehead);
		return -ENOMEM;
	}

	return 0;
}

static void __exit my_exit(void)
{
	struct proc_item *tmp1, *tmp2;

	remove_proc_entry(PROC_NAME, NULL);

	list_for_each_entry_safe(tmp1, tmp2, &filehead->head, item) {
		list_del(&tmp1->item);
		kfree(tmp1->str);
		kfree(tmp1);
	}
	kfree(filehead);
}

module_init(my_init);
module_exit(my_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Proc file based on list_head");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
