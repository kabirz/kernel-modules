# kernel-modules

一个**多模块**的树外 Linux 内核模块项目。每个模块在自己的子目录中，通过 kbuild **目录递归**构建，添加新模块只需"创建目录 + 注册一行"。

## 目录结构

```
kernel-modules/
├── Makefile          # 顶层构建包装器
├── Kbuild            # 顶层：obj-m += <module>/ 列出所有模块
├── AGENTS.md         # AI 代理的构建/代码风格指南
├── CLAUDE.md -> AGENTS.md
├── README.md
│
├── hello/            # Hello World 基础模块
├── test_mod/         # 带参数的简单模块
├── cdev/             # 字符设备驱动
├── kobject/          # 内核对象 (kobject/kset) 示例
├── sample/           # 示例模块
├── multi/            # 多文件模块示例
├── proc/             # /proc 文件系统示例
├── random/           # 随机数生成
├── schedule/         # 定时器、高精度定时器、工作队列
├── signal/           # 信号/异步通知
├── misc/             # 杂项设备、completion、globalmem
├── block/            # 块设备驱动 (blk-mq)
├── net/              # 网络设备驱动
├── usb/              # USB 设备驱动
├── input/            # 输入设备 (evdev)
├── gpio/             # GPIO 驱动 (gpiod API)
├── irq/              # 中断处理 + tasklet
├── thread/           # 内核线程 (kthread)
├── lock/             # 自旋锁、互斥锁、信号量示例
├── mmap/             # 内存映射 (mmap)
├── netlink/          # Netlink 通信
├── sysfs/            # Sysfs 属性接口
├── platform/         # 平台设备驱动
│
└── test/             # 用户空间测试程序 (hostprogs-always-y)
    ├── test_block
    ├── test_mmap
    ├── test_sysfs
    ├── test_netlink
    ├── test_net
    └── test_input
```

## 模块示例分类

| 分类 | 模块 | 说明 |
|------|------|------|
| **基础** | hello | 模块加载/卸载、module_param |
| | test_mod | 最小模块模板 |
| | multi | 多文件编译 |
| **设备** | cdev/buf | 字符设备 (read/write/ioctl) |
| | cdev/mem_char | 内存字符设备 |
| | block | 块设备 (blk-mq) |
| | net | 网络设备 (net_device) |
| | usb | USB 驱动框架 |
| | misc | 杂项设备、completion |
| **文件系统** | proc/readonly | 只读 /proc 文件 |
| | proc/readwrite | 读写 /proc 文件 |
| | proc/seq | Seq_file 大量输出 |
| | sysfs | Sysfs 属性 |
| | mmap | 映射到用户空间 |
| **调度** | schedule/timer | 周期定时器 |
| | schedule/hrtimer | 高精度定时器 |
| | schedule/workqueue | 工作队列 (work_struct) |
| **并发** | lock/spinlock_demo | 自旋锁保护 |
| | lock/mutex_demo | 互斥锁保护 |
| | lock/semaphore_demo | 信号量 (限制并发) |
| | irq | 中断处理 + tasklet |
| | thread | 内核线程 (kthread) |
| **IPC** | signal | 异步通知 (SIGIO) |
| | netlink | 内核-用户空间 netlink |
| **硬件** | gpio | GPIO (gpiod API) |
| | input | 输入子系统 (evdev) |
| | platform | 平台设备/驱动 |
| **对象** | kobject | Kobject、kset、sysfs 集成 |

## 构建

需要预编译的内核树（头文件 + 配置 + `Module.symvers`）。

```bash
make                                    # 递归构建所有模块
make KDIR=/home/zed/code/linux-7.1.1    # 针对本地内核树构建
```

每个模块的 `.ko` 文件在其子目录中，例如 `hello/hello.ko`。

## 测试程序

`test/` 目录下的测试程序通过 `hostprogs-always-y` 使用内核构建系统编译：

```bash
# 构建所有内容（模块 + 测试程序）
make

# 运行测试（示例：sysfs）
sudo insmod sysfs/sysfs_demo.ko
./test/test_sysfs
sudo rmmod sysfs_demo
```

可用测试：
- `test_block` — 块设备读写
- `test_mmap` — 内存映射
- `test_sysfs` — Sysfs 属性读写
- `test_netlink` — Netlink 通信
- `test_net` — 网络设备接口
- `test_input` — 输入事件读取

## 加载/卸载

```bash
sudo insmod hello/hello.ko
sudo rmmod  hello
dmesg | tail
```

## 添加新模块

1. 创建目录，例如 `hello/`，包含 `hello.c` 和模块的 `Kbuild`：
   ```makefile
   # hello/Kbuild
   obj-m += hello.o
   ```
2. 在**顶层** `Kbuild` 中注册：
   ```makefile
   obj-m += test_mod/
   obj-m += hello/        # <-- 添加这一行
   ```
3. `make` — 现在 `test_mod/test_mod.ko` 和 `hello/hello.ko` 都会被构建。

## 注意事项

- **编译器**：顶层 `Makefile` 设置 `CC := x86_64-linux-gnu-gcc` 以匹配内核的编译器字符串，
  避免 "compiler differs from the one used to build the kernel" 警告。
  可用 `make CC=gcc` 覆盖。使用 `:=`（非 `?=`）是因为 GNU make 内置 `CC=cc`。
- **Taint**：未签名的树外模块会**污染**内核
  （`dmesg`："loading out-of-tree module taints kernel" +
  "module verification failed: signature ... missing"）。
  这是*警告*，不是错误 — 模块仍然可以加载。
  要消除 "verification failed"，需要用注册的 MOK 签名模块
  （参见 `Documentation/kbuild/modules.rst`）。
  "out-of-tree taints" 本身无法移除。

## 清理

```bash
make clean
```
