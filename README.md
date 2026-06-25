# kernel-modules

A **multi-module** out-of-tree Linux kernel module project. Each module lives in
its own subdirectory and is built via kbuild **directory recursion**, so adding a
new module is just "create a directory + register one line".

## Layout

```
kernel-modules/
├── Makefile          # Top-level build wrapper (orchestration)
├── Kbuild            # Top-level: obj-m += <module>/ for each module
├── .gitignore
├── README.md
└── test_mod/         # One directory per module
    ├── Kbuild        # Per-module: obj-m += test_mod.o
    └── test_mod.c
```

## Two Kbuild files, two passes

kbuild parses the project in two passes (visible in the `make` log as `make[1]`
vs `make[2]`):

1. **First pass** — your `make` reads the top-level `Makefile` and runs the
   wrapper rule:
   ```
   make -C $(KDIR) M=$(PWD) CC=$(CC) modules
   ```
2. **Second pass** — kbuild descends into `M=$(PWD)`, reads the **top-level
   `Kbuild`** (`obj-m += test_mod/`), then recurses into `test_mod/` and reads
   **`test_mod/Kbuild`** (`obj-m += test_mod.o`). The `Makefile`'s wrapper rules
   are never read in this pass — the two concerns stay isolated.

## Build

Requires a prebuilt kernel tree (headers + config + `Module.symvers`).

```bash
make                                    # build all modules recursively
make KDIR=/home/zed/code/linux-7.1.1    # against a local tree
```

Each module's `.ko` lands in its own subdirectory, e.g. `test_mod/test_mod.ko`.

## Load / unload

```bash
sudo insmod test_mod/test_mod.ko
sudo rmmod  test_mod
dmesg | tail
```

## Add a new module

1. Create a directory, e.g. `hello/`, with `hello.c` and a per-module `Kbuild`:
   ```makefile
   # hello/Kbuild
   obj-m += hello.o
   ```
2. Register it in the **top-level** `Kbuild`:
   ```makefile
   obj-m += test_mod/
   obj-m += hello/        # <-- add this line
   ```
3. `make` — now both `test_mod/test_mod.ko` and `hello/hello.ko` are built.

## Notes

- **Compiler**: the top-level `Makefile` sets `CC := x86_64-linux-gnu-gcc` to match
  the kernel's recorded compiler string and silence the
  "compiler differs from the one used to build the kernel" warning. Override with
  `make CC=gcc`. Uses `:=` (not `?=`) because GNU make ships a built-in `CC=cc`.
- **Taint**: unsigned out-of-tree modules **taint** the kernel
  (`dmesg`: "loading out-of-tree module taints kernel" +
  "module verification failed: signature ... missing"). This is a *warning*, not
  an error — the module still loads. To silence "verification failed", sign the
  module with a registered MOK (see `Documentation/kbuild/modules.rst`).
  "out-of-tree taints" itself cannot be removed.

## Clean

```bash
make clean
```
