#include <linux/module.h>
#include <linux/fs.h> /* file_operations */
#include <linux/cdev.h> /* cdev */
#include <linux/proc_fs.h>
#include <linux/uaccess.h> /* copy_to|from_user */

#define BUF_SIZE 100
#define MEM_MAJOR 153

/* 私有结构体，每个设备一个 */
struct mem_priv {
	char *start, *end;
	size_t size;
	int wp;
	dev_t dev_id;
	struct cdev mem_cdev;
};

static struct mem_priv *dev;

static int mem_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int mem_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t mem_read(struct file *filp, char __user *buf, size_t count,
			loff_t *f_pos)
{
	count = min(count, (size_t)(dev->wp - *f_pos));
	if (0 == count)
		return 0;

	/* copy_to_user(to,from,count) */
	if (copy_to_user(buf, (dev->start + *f_pos), count))
		return -EFAULT;

	*f_pos += count;
	return count;
}

static ssize_t mem_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *f_pos)
{
	count = min(count, (size_t)(dev->size - dev->wp));
	if (count == 0)
		return 0;

	/* (to, from, count) */
	if (copy_from_user((dev->start + dev->wp), buf, count))
		return -EFAULT;

	dev->wp += count;
	return count;
}

static const struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = mem_open,
	.release = mem_release,
	.read = mem_read,
	.write = mem_write,
};

static int __init my_init(void)
{
	dev = (struct mem_priv *)kzalloc(sizeof(*dev), GFP_KERNEL);
	if (!dev)
		return -ENOMEM;

	/* 分配并初始化缓冲区 */
	dev->size = BUF_SIZE;
	dev->start = (char *)kzalloc(dev->size, GFP_KERNEL);
	if (!dev->start) {
		kfree(dev);
		return -ENOMEM;
	}
	dev->end = dev->start + dev->size;
	dev->wp = 0;

	/* 分配设备号并注册 */
	dev->dev_id = MKDEV(MEM_MAJOR, 0);
	cdev_init(&dev->mem_cdev, &my_fops);
	dev->mem_cdev.owner = THIS_MODULE;
	if (cdev_add(&dev->mem_cdev, dev->dev_id, 1)) {
		kfree(dev->start);
		kfree(dev);
		return -EFAULT;
	}

	return 0;
}

static void __exit my_exit(void)
{
	cdev_del(&dev->mem_cdev);
	kfree(dev->start);
	kfree(dev);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Memory char device driver with ioctl");
MODULE_VERSION("1.0");
