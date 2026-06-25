// SPDX-License-Identifier: GPL-2.0
/*
 * mmap_demo.c — Memory mapping example.
 * Demonstrates mmap operation for user-space memory mapping.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/slab.h>

#define DEVICE_NAME "mmap_demo"
#define BUFFER_SIZE (4096 * 2)

static char *buffer;

static int mmap_demo_open(struct inode *inode, struct file *file)
{
	pr_info("mmap_demo: device opened\n");
	return 0;
}

static int mmap_demo_release(struct inode *inode, struct file *file)
{
	pr_info("mmap_demo: device closed\n");
	return 0;
}

static int mmap_demo_mmap(struct file *file, struct vm_area_struct *vma)
{
	unsigned long size = vma->vm_end - vma->vm_start;

	if (size > BUFFER_SIZE)
		return -EINVAL;

	if (remap_pfn_range(vma, vma->vm_start,
			    page_to_phys(virt_to_page(buffer)) >> PAGE_SHIFT,
			    size, vma->vm_page_prot))
		return -EAGAIN;

	pr_info("mmap_demo: mapped %lu bytes at 0x%lx\n",
		size, vma->vm_start);
	return 0;
}

static ssize_t mmap_demo_read(struct file *file, char __user *buf,
			      size_t count, loff_t *ppos)
{
	size_t len;

	if (*ppos >= BUFFER_SIZE)
		return 0;

	len = min_t(size_t, count, BUFFER_SIZE - *ppos);
	if (copy_to_user(buf, buffer + *ppos, len))
		return -EFAULT;

	*ppos += len;
	return len;
}

static ssize_t mmap_demo_write(struct file *file, const char __user *buf,
			       size_t count, loff_t *ppos)
{
	size_t len;

	if (*ppos >= BUFFER_SIZE)
		return -ENOMEM;

	len = min_t(size_t, count, BUFFER_SIZE - *ppos);
	if (copy_from_user(buffer + *ppos, buf, len))
		return -EFAULT;

	*ppos += len;
	return len;
}

static const struct file_operations mmap_demo_fops = {
	.owner = THIS_MODULE,
	.open = mmap_demo_open,
	.release = mmap_demo_release,
	.read = mmap_demo_read,
	.write = mmap_demo_write,
	.mmap = mmap_demo_mmap,
};

static struct miscdevice mmap_demo_dev = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &mmap_demo_fops,
};

static int __init mmap_demo_init(void)
{
	int ret;

	buffer = kmalloc(BUFFER_SIZE, GFP_KERNEL);
	if (!buffer)
		return -ENOMEM;

	memset(buffer, 0, BUFFER_SIZE);
	strcpy(buffer, "Hello from kernel space!");

	ret = misc_register(&mmap_demo_dev);
	if (ret) {
		kfree(buffer);
		return ret;
	}

	pr_info("mmap_demo: device registered\n");
	return 0;
}

static void __exit mmap_demo_exit(void)
{
	misc_deregister(&mmap_demo_dev);
	kfree(buffer);
	pr_info("mmap_demo: device unregistered\n");
}

module_init(mmap_demo_init);
module_exit(mmap_demo_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Memory mapping example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");
