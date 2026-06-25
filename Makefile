# SPDX-License-Identifier: GPL-2.0
#
# Makefile — build orchestration wrapper.
#
# Read by the user-invoked make (first pass). It delegates to a prebuilt
# kernel tree to compile the out-of-tree module declared in Kbuild.
# NOTE: obj-m is intentionally NOT set here — it lives in Kbuild only, so
# the two passes never interfere.

# Path to a prebuilt kernel tree (config + headers + Module.symvers).
# Default: the running kernel's build tree. Override, e.g. against a local tree:
#   make KDIR=/home/zed/code/linux-7.1.1
KDIR ?= /lib/modules/$(shell uname -r)/build

# Match the compiler used to build the running kernel. On Debian/Ubuntu the
# kernel is built with the triplet-prefixed gcc; using it silences:
# "warning: the compiler differs from the one used to build the kernel"
CC := x86_64-linux-gnu-gcc

PWD := $(shell pwd)

.PHONY: all modules clean install help cdb menuconfig defconfig oldconfig savedefconfig

all modules:
	@if [ ! -f .config ]; then $(MAKE) defconfig; fi
	@$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) modules 2>&1 \
		| grep -v "Skipping BTF generation for.*due to unavailability of vmlinux" \
		|| [ $$? = 1 ]

# 生成默认配置（所有模块设为 m）
defconfig:
	defconfig --kconfig Kconfig defconfig

# 交互式 TUI 配置界面
menuconfig:
	menuconfig Kconfig

# 保存最小配置
savedefconfig:
	savedefconfig --kconfig Kconfig --filename defconfig

# 更新配置（新选项使用默认值）
oldconfig:
	olddefconfig Kconfig

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) modules_install

# Regenerate the Clang compilation database (compile_commands.json) for clangd.
cdb:
	@$(KDIR)/scripts/clang-tools/gen_compile_commands.py $(PWD) >/dev/null \
		&& echo "Generated compile_commands.json" \
		|| echo "Error: run 'make' first to produce .cmd files"

help:
	@echo "Targets:"
	@echo "  make              构建所有模块（递归）"
	@echo "  make menuconfig   交互式 TUI 配置界面"
	@echo "  make defconfig    生成默认配置（所有模块设为 m）"
	@echo "  make oldconfig    更新配置（新选项使用默认值）"
	@echo "  make savedefconfig 保存最小配置到 defconfig 文件"
	@echo "  make clean        清理构建产物"
	@echo "  make install      安装模块"
	@echo "  make cdb          重新生成 compile_commands.json (用于 clangd)"
	@echo ""
	@echo "依赖："
	@echo "  pip3 install kconfiglib"
	@echo ""
	@echo "覆盖内核树：make KDIR=/path/to/linux"
