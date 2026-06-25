#include <linux/module.h>
#include <linux/init.h>

#include <linux/sched.h> /* current and everything */
#include <linux/fs.h> /* everything... */
#include <linux/completion.h>

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Completion example");

static int complete_major = 250;

DECLARE_COMPLETION(comp);

static ssize_t complete_read(struct file *filp, char __user *buf, size_t count,
			     loff_t *pos)
{
	pr_info("process %i (%s) going to sleep/n", current->pid,
		current->comm);
	wait_for_completion(&comp);
	pr_info("awoken %i (%s)/n", current->pid, current->comm);
	return 0; /* EOF */
}

static ssize_t complete_write(struct file *filp, const char __user *buf,
			      size_t count, loff_t *pos)
{
	pr_info("process %i (%s) awakening the readers.../n", current->pid,
		current->comm);
	complete(&comp);
	return count; /* succeed, to avoid retrial */
}

static const struct file_operations complete_fops = {
	.owner = THIS_MODULE,
	.read = complete_read,
	.write = complete_write,
};

static int complete_init(void)
{
	int result;

	/*
	 * Register your major, and accept a dynamic number
	 */
	result = register_chrdev(complete_major, "complete", &complete_fops);
	if (result < 0)
		return result;
	if (complete_major == 0)
		complete_major = result; /* dynamic */
	return 0;
}

static void complete_cleanup(void)
{
	unregister_chrdev(complete_major, "complete");
}

module_init(complete_init);
module_exit(complete_cleanup);
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_VERSION("1.0");
