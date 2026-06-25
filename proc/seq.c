// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/list.h>

#define PROC_NAME "proc_seq"

struct proc_head {
	struct list_head phead;
	int item_count;
	int str_count;
};

struct proc_item {
	struct list_head pitem;
	char *pbuf;
	int num;
};

static struct proc_head *my_head;

static void *my_seq_start(struct seq_file *seq, loff_t *pos)
{
	struct proc_item *entry;

	if (*pos == 0)
		seq_printf(seq, "List has %d items, total %d bytes\n",
			   my_head->item_count, my_head->str_count);

	++*pos;
	pr_debug("Start:pos=%d\n", (int)*pos);
	list_for_each_entry(entry, &my_head->phead, pitem) {
		if (*pos == entry->num) {
			pr_debug("Start:pos=%d\n", (int)*pos);
			return entry;
		}
	}

	return NULL;
}

static void *my_seq_next(struct seq_file *seq, void *v, loff_t *pos)
{
	struct proc_item *entry;

	++*pos;
	pr_debug("Next:pos=%d\n", (int)*pos);
	list_for_each_entry(entry, &my_head->phead, pitem) {
		if (*pos == entry->num) {
			pr_debug("Next:pos=%d\n", (int)*pos);
			return entry;
		}
	}

	return NULL;
}

static void my_seq_stop(struct seq_file *m, void *v)
{
}

static int my_seq_show(struct seq_file *m, void *v)
{
	struct proc_item *tmp = v;

	pr_debug("Show\n");
	seq_printf(m, "%s", tmp->pbuf);
	return 0;
}

static const struct seq_operations my_seq_fops = {
	.start = my_seq_start,
	.next = my_seq_next,
	.stop = my_seq_stop,
	.show = my_seq_show,
};

static int proc_seq_open(struct inode *inode, struct file *file)
{
	return seq_open(file, &my_seq_fops);
}

static ssize_t proc_seq_write(struct file *filp, const char __user *buf,
			      size_t count, loff_t *f_pos)
{
	struct proc_item *tmp;

	tmp = kzalloc(sizeof(*tmp), GFP_KERNEL);
	if (!tmp)
		return -ENOMEM;

	INIT_LIST_HEAD(&tmp->pitem);

	tmp->pbuf = kzalloc(count, GFP_KERNEL);
	if (!tmp->pbuf) {
		kfree(tmp);
		return -ENOMEM;
	}

	if (copy_from_user(tmp->pbuf, buf, count)) {
		kfree(tmp->pbuf);
		kfree(tmp);
		return -EFAULT;
	}

	list_add(&tmp->pitem, &my_head->phead);
	my_head->item_count++;
	my_head->str_count += count;
	tmp->num = my_head->item_count;

	return count;
}

static const struct proc_ops proc_seq_file_fops = {
	.proc_open = proc_seq_open,
	.proc_read = seq_read,
	.proc_write = proc_seq_write,
	.proc_lseek = seq_lseek,
	.proc_release = seq_release,
};

static int __init my_init(void)
{
	my_head = kzalloc(sizeof(*my_head), GFP_KERNEL);
	if (!my_head)
		return -ENOMEM;

	my_head->item_count = 0;
	my_head->str_count = 0;
	INIT_LIST_HEAD(&my_head->phead);

	if (!proc_create(PROC_NAME, 0666, NULL, &proc_seq_file_fops)) {
		kfree(my_head);
		pr_err("Cannot create /proc/%s\n", PROC_NAME);
		return -ENOMEM;
	}

	return 0;
}

static void __exit my_exit(void)
{
	struct proc_item *tmp1, *tmp2;

	remove_proc_entry(PROC_NAME, NULL);

	list_for_each_entry_safe(tmp1, tmp2, &my_head->phead, pitem) {
		list_del(&tmp1->pitem);
		kfree(tmp1->pbuf);
		kfree(tmp1);
	}
	kfree(my_head);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Seq_file based proc file example");
MODULE_VERSION("1.0");
