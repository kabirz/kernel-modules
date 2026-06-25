#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

struct buf_st {
	int user;
	dev_t no;
	char buf[1024];
	struct cdev dev;
};

static ssize_t cbuf_read(struct file *fp, char __user *buffer, size_t count,
			 loff_t *off)
{
	struct buf_st *bs = fp->private_data;
	pr_info("Read:%s", bs->buf);

	if (copy_to_user(buffer, bs->buf, count))
		return -EFAULT;

	return count;
}

static ssize_t cbuf_write(struct file *fp, const char __user *buffer,
			  size_t count, loff_t *off)
{
	struct buf_st *bs = fp->private_data;

	pr_info("Write:%s", buffer);
	if (copy_from_user(bs->buf, buffer, count))
		return -EFAULT;

	return count;
}

static long cbuf_ioctl(struct file *fp, unsigned int cmd, unsigned long val)
{
	pr_info("In ioctl.");
	return 0;
}

static int cbuf_open(struct inode *no, struct file *fp)
{
	struct buf_st *bs = container_of(no->i_cdev, struct buf_st, dev);
	fp->private_data = (void *)bs;

	bs->user++;

	return 0;
}

static int cbuf_release(struct inode *no, struct file *fp)
{
	struct buf_st *bs = container_of(no->i_cdev, struct buf_st, dev);

	bs->user--;

	return 0;
}

static const struct file_operations cbuf_ops = {
	.owner = THIS_MODULE,
	.open = cbuf_open,
	.release = cbuf_release,
	.read = cbuf_read,
	.write = cbuf_write,
	.unlocked_ioctl = cbuf_ioctl,
};

struct buf_st cbuf;

static __init int test_init(void)
{
	int ret;

	memset(cbuf.buf, '\0', 1024);
	ret = alloc_chrdev_region(&cbuf.no, 2, 1, "cbuf");
	if (ret < 0) {
		pr_info("alloc_chrdev_region() error\n");
		return -EPERM;
	}

	cdev_init(&cbuf.dev, &cbuf_ops);
	ret = cdev_add(&cbuf.dev, cbuf.no, 1);
	if (ret < 0) {
		pr_info("cdev_add() error\n");
		goto add_error;
	}
	pr_info("MAJOR:%d, MINOR:%d\n", MAJOR(cbuf.no), MINOR(cbuf.no));

	return 0;

add_error:
	unregister_chrdev_region(cbuf.no, 1);
	return -EPERM;
}

static __exit void test_exit(void)
{
	cdev_del(&cbuf.dev);
	unregister_chrdev_region(cbuf.no, 1);
}

module_init(test_init);
module_exit(test_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("This is a simple cdev driver");
MODULE_VERSION("1.0");
