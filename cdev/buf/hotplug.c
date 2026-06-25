#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/device.h>

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

struct class *my_class;
int __init init_module(void)
{
	int major;
	int ret;

	memset(cbuf.buf, '\0', 1024);

	major = register_chrdev(0, "char1", &cbuf_ops);
	if (major < 0)
		return major;

	cbuf.no = MKDEV(major, 1);

	my_class = class_create("test");
	if (IS_ERR(my_class)) {
		ret = PTR_ERR(my_class);
		goto class_err;
	}

	ret = PTR_ERR(device_create(my_class, NULL, cbuf.no, NULL, "char1"));
	if (ret < 0)
		goto device_err;

	pr_info("Create devices:MAJOR:%d, MINOR:%d\n", MAJOR(cbuf.no),
		MINOR(cbuf.no));
	return 0;

device_err:
	class_destroy(my_class);
class_err:
	unregister_chrdev(MAJOR(cbuf.no), "char1");
	return ret;
}

void __exit cleanup_module(void)
{
	device_destroy(my_class, cbuf.no);
	class_destroy(my_class);
	unregister_chrdev(MAJOR(cbuf.no), "char1");
	pr_info("Destroy devices:MAJOR:%d, MINOR:%d\n", MAJOR(cbuf.no),
		MINOR(cbuf.no));
}

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("This is a simple hot cdev driver");
MODULE_VERSION("1.0");
