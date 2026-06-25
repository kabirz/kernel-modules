# AGENTS.md

## 构建命令

```bash
make                  # 构建所有模块和测试程序
make clean            # 清理构建产物
make KDIR=/path/to/kernel  # 针对指定内核树构建
make cdb              # 重新生成 compile_commands.json (用于 clangd)
make defconfig        # 生成默认配置（所有模块启用）
make menuconfig       # 查看当前配置
```

## 代码检查

```bash
# 内核模块通过 kbuild 编译，没有单独的 lint 步骤
# 编译时警告视为错误
# 测试程序：使用 gcc -Wall -Wextra -Werror 检查
```

## 代码风格

- 内核模块遵循 Linux 内核编码风格（参见 `Documentation/process/coding-style.rst`）
- 使用 `pr_info()` / `pr_err()` 进行内核日志输出
- 模块模板：`MODULE_AUTHOR`、`MODULE_DESCRIPTION`、`MODULE_LICENSE("GPL")`、`MODULE_VERSION`
- 每个文件顶部添加 SPDX 许可证标识
- 测试程序使用标准 C 和 POSIX API

## 项目结构

- 每个模块在自己的子目录中，包含一个 `Kbuild` 文件
- 顶层 `Kbuild` 通过 `obj-m += <name>/` 列出所有模块
- 顶层 `Makefile` 是构建包装器（仅在第一遍读取）
- `test/` 目录下的测试程序通过 `hostprogs-always-y` 构建
- `Kconfig` 定义配置选项，`.config` 存储当前配置

## 添加新模块

1. 创建目录：`mkdir new_module/`
2. 创建源码：`new_module/new_module.c`
3. 创建 `new_module/Kbuild`，内容为 `obj-m += new_module.o`
4. 在顶层 `Kbuild` 中注册：`obj-m += new_module/`
5. 构建：`make`

## 添加新测试程序

1. 创建 `test/test_new.c`
2. 在 `test/Kbuild` 中添加：`hostprogs-always-y += ... test_new`
3. 构建：`make`
4. 运行：`sudo insmod <module>.ko && ./test/test_new`
