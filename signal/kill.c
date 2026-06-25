#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fcntl.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/fs.h>

static char my_buf[20] = "Hello,kernel!";
static struct fasync_struct *kill_async_queue;

static int kill_open(struct inode *inode, struct file *file)
{
	return 0;
}

static ssize_t kill_read(struct file *file, char __user *buf, size_t count,
			 loff_t *ppos)
{
	size_t len = min(count, sizeof(my_buf));

	if (copy_to_user(buf, my_buf, len))
		return -EFAULT;

	return len;
}

static ssize_t kill_write(struct file *file, const char __user *buf,
			  size_t count, loff_t *ppos)
{
	kill_fasync(&kill_async_queue, SIGIO, POLL_IN);
	return 3;
}

static int kill_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int local_kill_fasync(int fd, struct file *filp, int on)
{
	return fasync_helper(fd, filp, on, &kill_async_queue);
}

static const struct file_operations kill_fops = {
	.owner = THIS_MODULE,
	.llseek = noop_llseek,
	.open = kill_open,
	.write = kill_write,
	.read = kill_read,
	.release = kill_release,
	.fasync = local_kill_fasync,
};

static struct miscdevice kill_dev = {
	.minor = 250,
	.name = "kill",
	.fops = &kill_fops,
};

static int __init kill_init(void)
{
	return misc_register(&kill_dev);
}

static void __exit kill_exit(void)
{
	misc_deregister(&kill_dev);
}

module_init(kill_init);
module_exit(kill_exit);
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Kernel async notification example");
MODULE_VERSION("1.0");
