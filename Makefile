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
#   "warning: the compiler differs from the one used to build the kernel"
# NOTE: uses := (not ?=) because GNU make ships a built-in CC=cc that would
# otherwise shadow this default. The command-line still wins: make CC=gcc
CC := x86_64-linux-gnu-gcc

PWD := $(shell pwd)

.PHONY: all modules clean install help cdb

all modules:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) clean

install:
	$(MAKE) -C $(KDIR) M=$(PWD) CC=$(CC) modules_install

# Regenerate the Clang compilation database (compile_commands.json) for clangd.
# Requires a prior successful build — it reads the .cmd files kbuild produced.
cdb:
	@$(KDIR)/scripts/clang-tools/gen_compile_commands.py $(PWD) >/dev/null \
		&& echo "Generated compile_commands.json" \
		|| echo "Error: run 'make' first to produce .cmd files"

help:
	@echo "Targets:"
	@echo "  make          build all modules (recursively)"
	@echo "  make clean    remove build artifacts"
	@echo "  make install  install module(s) via $(KDIR)"
	@echo "  make cdb      regenerate compile_commands.json (for clangd)"
	@echo ""
	@echo "Override kernel tree: make KDIR=/path/to/linux"
