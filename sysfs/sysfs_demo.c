// SPDX-License-Identifier: GPL-2.0
/*
 * sysfs_demo.c — Sysfs interface example.
 * Demonstrates creating sysfs entries for kernel-userspace communication.
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static struct kobject *demo_kobj;
static int demo_value = 0;
static char demo_text[256] = "Hello from kernel!";

static ssize_t demo_value_show(struct kobject *kobj,
			       struct kobj_attribute *attr, char *buf)
{
	return sysfs_emit(buf, "%d\n", demo_value);
}

static ssize_t demo_value_store(struct kobject *kobj,
				struct kobj_attribute *attr, const char *buf,
				size_t count)
{
	int ret;

	ret = kstrtoint(buf, 10, &demo_value);
	if (ret)
		return ret;

	pr_info("sysfs_demo: value changed to %d\n", demo_value);
	return count;
}

static ssize_t demo_text_show(struct kobject *kobj, struct kobj_attribute *attr,
			      char *buf)
{
	return sysfs_emit(buf, "%s\n", demo_text);
}

static ssize_t demo_text_store(struct kobject *kobj,
			       struct kobj_attribute *attr, const char *buf,
			       size_t count)
{
	size_t len = min(count, sizeof(demo_text) - 1);

	memcpy(demo_text, buf, len);
	demo_text[len] = '\0';

	pr_info("sysfs_demo: text changed to '%s'\n", demo_text);
	return count;
}

static struct kobj_attribute demo_value_attr =
	__ATTR(value, 0644, demo_value_show, demo_value_store);

static struct kobj_attribute demo_text_attr =
	__ATTR(text, 0644, demo_text_show, demo_text_store);

static struct attribute *demo_attrs[] = {
	&demo_value_attr.attr,
	&demo_text_attr.attr,
	NULL,
};

static struct attribute_group demo_attr_group = {
	.attrs = demo_attrs,
};

static int __init simple_init(void)
{
	int ret;

	demo_kobj = kobject_create_and_add("demo_sysfs", kernel_kobj);
	if (!demo_kobj)
		return -ENOMEM;

	ret = sysfs_create_group(demo_kobj, &demo_attr_group);
	if (ret) {
		kobject_put(demo_kobj);
		return ret;
	}

	pr_info("sysfs_demo: sysfs entries created at /sys/kernel/demo_sysfs/\n");
	return 0;
}

static void __exit simple_exit(void)
{
	sysfs_remove_group(demo_kobj, &demo_attr_group);
	kobject_put(demo_kobj);
	pr_info("sysfs_demo: sysfs entries removed\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_AUTHOR("kabirz <jxwazxzhp@126.com>");
MODULE_DESCRIPTION("Sysfs interface example");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");