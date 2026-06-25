#include <linux/module.h>
#include <linux/init.h>

#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/completion.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Completion (complete_all) example");

#undef KERN_DEBUG
#define KERN_DEBUG "<1>"

static int complete_major = 250;
static int reader_count = 0;

DECLARE_COMPLETION(comp);

static ssize_t complete_read(struct file *filp, char __user *buf, size_t count,
			     loff_t *pos)
{
	pr_info("process %i (%s) going to sleep,waiting for writer/n",
		current->pid, current->comm);
	reader_count++;
	pr_info("In read ,before comletion: reader count = %d /n",
		reader_count);
	wait_for_completion(&comp);
	reader_count--;
	pr_info("awoken %s (%i) /n", current->comm, current->pid);
	pr_info("In read,after completion : reader count = %d /n",
		reader_count);
	if (reader_count == 0)
		reinit_completion(&comp);

	return 0;
}

static ssize_t complete_write(struct file *filp, const char __user *buf,
			      size_t count, loff_t *pos)
{
	pr_info("process %i (%s) awoking the readers.../n", current->pid,
		current->comm);
	pr_info("In write ,before do complete_all : reader count = %d /n",
		reader_count);

	if (reader_count != 0)
		complete_all(&comp);

	pr_info("In write ,after do complete_all : reader count = %d /n",
		reader_count);

	return count;
}

static const struct file_operations complete_fops = {
	.owner = THIS_MODULE,
	.read = complete_read,
	.write = complete_write,
};

static int complete_init(void)
{
	int result;

	result = register_chrdev(complete_major, "complete", &complete_fops);
	if (result < 0)
		return result;
	if (complete_major == 0)
		complete_major = result;

	pr_info("complete driver test init! complete_major=%d/n",
		complete_major);
	pr_info("¾²Ì¬³õÊ¼»¯completion/n");

	return 0;
}

static void complete_exit(void)
{
	unregister_chrdev(complete_major, "complete");
	pr_info("complete driver    is removed/n");
}

module_init(complete_init);
module_exit(complete_exit);
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_VERSION("1.0");
