这是我在 2021 年的北航计算机学院操作系统课程中完成的作业代码，代码位于 `lab*` 分支中，由于时间关系，代码质量可能较差。

课程组提供的代码版权归原作者所有，作业代码版权归本人所有，保留所有权利。

代码仅用于课程学习交流，请勿直接抄袭，也不得用于课程以外的用途。

## 本地环境配置（Arch Linux）

1. 安装交叉编译器 `cross-mips-elf-gcc` 和仿真程序 `gxemul` （AUR 和 archlinuxcn 上都有）
2. 修改 `include.mk` 中的 `CROSS_COMPILE` 变量为 `mips-elf-`
3. 从 `include.mk` 中的 `CFLAGS` 变量中去除 `-fPIC` 选项（详见 [https://gcc.gnu.org/legacy-ml/gcc-patches/2010-04/msg00843.html](https://gcc.gnu.org/legacy-ml/gcc-patches/2010-04/msg00843.html)）
