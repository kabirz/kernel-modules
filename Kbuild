# SPDX-License-Identifier: GPL-2.0
#
# Top-level Kbuild — lists every module directory in this project.
# Each "obj-m += <name>/" line makes kbuild descend into <name>/ and build the
# module(s) declared by that directory's own Kbuild.
#
# To add a new module:
#   1. create <name>/ with its sources + a per-module Kbuild (obj-m += <name>.o)
#   2. add one line here: obj-$(CONFIG_<NAME>_MODULE) += <name>/
#
# 注意：此文件由顶层 Makefile 生成，不要手动编辑。

# 从 .config 文件读取配置（过滤掉注释和空行）
CONFIGS := $(shell grep -E '^CONFIG_.*=[my]' .config 2>/dev/null | sed 's/=.*//' || true)
$(foreach c,$(CONFIGS),$(eval $(c)=m))

obj-$(CONFIG_HELLO_MODULE) += hello/
obj-$(CONFIG_TEST_MOD_MODULE) += test_mod/
obj-$(CONFIG_CDEV_MODULE) += cdev/
obj-$(CONFIG_KOBJECT_MODULE) += kobject/
obj-$(CONFIG_MULTI_MODULE) += multi/
obj-$(CONFIG_PROC_MODULE) += proc/
obj-$(CONFIG_RANDOM_MODULE) += random/
obj-$(CONFIG_SCHEDULE_MODULE) += schedule/
obj-$(CONFIG_SIGNAL_MODULE) += signal/
obj-$(CONFIG_MISC_MODULE) += misc/
obj-$(CONFIG_BLOCK_MODULE) += block/
obj-$(CONFIG_NET_MODULE) += net/
obj-$(CONFIG_USB_MODULE) += usb/
obj-$(CONFIG_INPUT_MODULE) += input/
obj-$(CONFIG_GPIO_MODULE) += gpio/
obj-$(CONFIG_IRQ_MODULE) += irq/
obj-$(CONFIG_THREAD_MODULE) += thread/
obj-$(CONFIG_LOCK_MODULE) += lock/
obj-$(CONFIG_MMAP_MODULE) += mmap/
obj-$(CONFIG_NETLINK_MODULE) += netlink/
obj-$(CONFIG_SYSFS_MODULE) += sysfs/
obj-$(CONFIG_PLATFORM_MODULE) += platform/

# 用户空间测试程序
obj-$(CONFIG_TEST_PROGRAMS) += test/
