#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

#define DEF_LEN 100 /* 缓冲区的默认大小 */
#define DEF_MAJOR 55 /* 默认主设备号 */
#define DEV_NUM 100 /* 驱动可能支持的设备数量 */
#define MEM_NUM 3 /* 驱动当前支持的设备数量 */

#define MEM_TYPE 'Z'
#define MEM_RESET _IO(MEM_TYPE, 1)
#define MEM_RESIZE _IOW(MEM_TYPE, 2, int)
//#define MEM_RESET	1
//#define MEM_RESIZE	2

static int major = DEF_MAJOR;
module_param(major, int, 0444);

static int size[MEM_NUM] = { DEF_LEN, DEF_LEN * 2, DEF_LEN * 3 };
static int count;
module_param_array(size, int, &count, 0444);

struct mem_priv {
	char *start, *end;
	int buf_len;
	int wp; /* write position */
	int open_count;
	dev_t dev_id;
	struct cdev mem_cdev;
};

static struct mem_priv *devs[MEM_NUM];

/* /proc文件的读函数 (modern seq_file API) */
static int mem_proc_show(struct seq_file *m, void *v)
{
	int i;

	for (i = 0; i < MEM_NUM; i++) {
		seq_printf(m, "-----DEV %d:%d-----\n", MAJOR(devs[i]->dev_id),
			   MINOR(devs[i]->dev_id));
		seq_printf(m, "buf_len = %d bytes; from 0x%p to 0x%p\n",
			   devs[i]->buf_len, devs[i]->start, devs[i]->end);
		seq_printf(m, "wp = %d; open_count = %d\n\n", devs[i]->wp,
			   devs[i]->open_count);
	}

	return 0;
}

static int mem_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, mem_proc_show, NULL);
}

static const struct proc_ops mem_proc_fops = {
	.proc_open = mem_proc_open,
	.proc_read = seq_read,
	.proc_lseek = seq_lseek,
	.proc_release = single_release,
};

static int mem_open(struct inode *inode, struct file *filp)
{
	struct mem_priv *dev =
		container_of(inode->i_cdev, struct mem_priv, mem_cdev);

	dev->open_count++;
	filp->private_data = dev;

	return 0;
}

static int mem_release(struct inode *inode, struct file *filp)
{
	struct mem_priv *dev = filp->private_data;
	dev->open_count--;
	return 0;
}

static ssize_t mem_read(struct file *filp, char __user *buf, size_t count,
			loff_t *f_pos)
{
	struct mem_priv *dev = filp->private_data;

	/* 当缓冲区被reset后，可能出现读指针在写指针之上的情况
	   将读指针恢复到0即可 */
	if (*f_pos > dev->wp)
		*f_pos = 0;

	if (count == 0 || *f_pos == dev->wp)
		return 0;

	count = min(count, (size_t)(dev->wp - *f_pos));

	/* copy to user(to, from, count) */
	if (copy_to_user(buf, (dev->start + *f_pos), count))
		return -EFAULT;

	*f_pos += count;
	return count;
}

static ssize_t mem_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *f_pos)
{
	struct mem_priv *dev = filp->private_data;

	if (count == 0 || dev->wp == dev->buf_len)
		return 0;

	count = min(count, (size_t)(dev->buf_len - dev->wp));

	/* copy from user (to, from, count) */
	if (copy_from_user((dev->start + dev->wp), buf, count))
		return -EFAULT;

	dev->wp += count;

	return count;
}

static long mem_ioctl(struct file *filp, unsigned int req, unsigned long arg)
{
	struct mem_priv *dev = filp->private_data;
	char *tmp;

	if (_IOC_TYPE(req) != MEM_TYPE) {
		pr_info("Cannot recognize ioctl number 0x%x\n", req);
		return -ENOTTY;
	}

	switch (_IOC_NR(req)) {
	case 1: /* reset buffer */
		dev->wp = 0;
		memset(dev->start, 0, dev->buf_len);
		break;
	case 2: /* resize buffer */
		if (arg > 0x400000) {
			pr_info("Buffer too long\n");
			return -ENOMEM;
		}
		tmp = (char *)kzalloc(arg, GFP_KERNEL);
		if (tmp == NULL)
			return -ENOMEM;
		kfree(dev->start);
		dev->start = tmp;
		dev->buf_len = arg;
		dev->end = dev->start + dev->buf_len;
		dev->wp = 0;
		break;
	default: /* unrecognize number */
		pr_info("request number error\n");
		return -ENOTTY;
	}

	return 0;
}

static const struct file_operations mem_fops = {
	.owner = THIS_MODULE,
	.open = mem_open,
	.release = mem_release,
	.read = mem_read,
	.write = mem_write,
	.unlocked_ioctl = mem_ioctl,
};

static void devs_release(struct mem_priv **tmp, int dev_num);

/* 初始化多个缓冲区，并注册到VFS */
static int devs_init(struct mem_priv **tmp, int dev_num)
{
	int i, ret;
	struct mem_priv *dev;

	for (i = 0; i < dev_num; i++) {
		dev = (struct mem_priv *)kzalloc(sizeof(*dev), GFP_KERNEL);
		if (NULL == dev) {
			ret = -ENOMEM;
			goto err_alloc;
		}

		dev->start = (char *)kzalloc(size[i], GFP_KERNEL);
		if (NULL == dev->start) {
			ret = -ENOMEM;
			kfree(dev);
			goto err_alloc;
		}
		dev->buf_len = size[i];
		dev->end = dev->start + dev->buf_len;
		dev->wp = 0;
		dev->open_count = 0;

		/* init & add cdev */
		dev->dev_id = MKDEV(major, i + 10);
		cdev_init(&dev->mem_cdev, &mem_fops);
		dev->mem_cdev.owner = THIS_MODULE;
		ret = cdev_add(&dev->mem_cdev, dev->dev_id, 1);
		if (ret) {
			kfree(dev->start);
			kfree(dev);
			goto err_alloc;
		}

		tmp[i] = dev;
	}

	return 0;

err_alloc:
	devs_release(tmp, i);
	return ret;
}

/* 释放缓冲区，并从VFS中注销 */
static void devs_release(struct mem_priv **tmp, int dev_num)
{
	int i;
	for (i = 0; i < dev_num; i++) {
		if (tmp[i]) {
			cdev_del(&tmp[i]->mem_cdev);
			kfree(tmp[i]->start);
			kfree(tmp[i]);
		}
	}
}

static int __init my_init(void)
{
	int ret;
	dev_t base_id;

	/* register chrdev region */
	if (major) {
		base_id = MKDEV(major, 0);
		ret = register_chrdev_region(base_id, DEV_NUM, "char_test");
	} else {
		ret = alloc_chrdev_region(&base_id, 0, DEV_NUM, "char_test");
		major = MAJOR(base_id);
	}

	if (ret) {
		pr_err("Cannot allocate chrdev region from %d:%d to %d:%d\n",
		       major, 0, major, DEV_NUM);
		return ret;
	}

	/* create /proc/mem_test */
	if (!proc_create("mem_test", 0, NULL, &mem_proc_fops)) {
		pr_err("Cannot create /proc/mem_test\n");
		ret = -ENOMEM;
		goto err_proc;
	}

	/* init all devices */
	ret = devs_init(devs, MEM_NUM);
	if (ret)
		goto err_alloc;

	return 0;

err_alloc:
	remove_proc_entry("mem_test", NULL);
err_proc:
	unregister_chrdev_region(base_id, DEV_NUM);

	return ret;
}

static void __exit my_exit(void)
{
	dev_t dev_base = MKDEV(major, 0);

	/* release chrdev region */
	unregister_chrdev_region(dev_base, DEV_NUM);
	remove_proc_entry("mem_test", NULL);
	devs_release(devs, MEM_NUM);
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Multi-buffer char driver with proc and ioctl support");
MODULE_VERSION("1.0");
