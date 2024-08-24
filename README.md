# NKU-Compilers2024(RISC-V64GC)

## 注意事项（学术诚信问题）
本框架由2024年编译系统实现赛RISC-V赛道作品改编而来，原代码的仓库为：

https://github.com/yuhuifishash/SysY

在基于本框架完成实验时，可以参考开源仓库中的代码，但必须在注释中明确注明自己哪部分代码是参考的其他仓库，并给出自己参考仓库的具体链接，文件以及哪一行代码。如果参考了公开源代码，但是没有在注释中明确注明，将按抄袭处理，当此作业记0分，下面是一个注释的例子。
```cpp
// Reference: https://github.com/llvm/llvm-project/blob/main/llvm/lib/Transforms/Scalar/LICM.cpp line1-line37
```

## Prerequisites

Clang 15.0+

riscv64-unknown-linux-gnu-gcc 12.2+

qemu-riscv64 7.0+

flex 2.6+

bison 3.8+

```bash
# test your environments
git clone https://github.com/yuhuifishash/SysY
cd SysY
mkdir obj
mkdir bin
make -j
# if you get WA in step below, try to use clang-15 in SysY_test.sh(line 31 and line 32)
./SysY_test.sh llvm O1 rv64gc 
./SysY_test.sh S O1 rv64gc
# if all the testcases are AC, your environment is correct
# the script will run about 20 minutes
```

## Build Steps
```bash
mkdir obj
mkdir bin
make -j
```

if you update parser(*.y), use the command below
```bash
make parser
make -j
```

if you update lexer(*.l), use the command below
```bash
make lexer
make -j
```

## lab1:词法分析

## lab2:语法分析

## lab3:类型检查和中间代码生成

## lab4:代码优化

## lab5:目标代码生成
