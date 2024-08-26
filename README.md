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
# 该仓库为Risc-V架构实验参考代码（实验代码框架由该仓库修改而来）。
cd SysY
mkdir obj
mkdir bin
make -j
# 如果在一些样例中Wrong Answer, 可以使用 clang-15重新测试。
# 你可以修改SysY_test.sh中的第31行和第32行，将clang改为clang-15。
./SysY_test.sh llvm O1 rv64gc # 测试中间代码生成
./SysY_test.sh S O1 rv64gc # 测试目标代码生成
# 如果所有样例都通过，则说明环境配置正确。
# 测试大概需要20分钟的时间。
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
需要阅读的代码：

**include/symtab.h**:定义了符号表相关类，本次实验你只需要根据注释提示阅读一小段即可

**utils/symtab.cc**:符号表相关类的具体实现

**utils/lexer_out.cc**:定义了词法分析结果的输出函数

**target/main.cc**:主函数，你需要了解框架的整体流程以及全局变量, 后续每次实验都需要进行阅读

需要阅读并编写的代码:

**lexer/SysY_lexer.l**:编写你想实现的词法正则表达式及对应处理函数


## lab2:语法分析
需要阅读的代码：

**include/tree.h**:语法树节点父类

**include/SysY_tree.h**:SysY文法所有语法树节点类的定义

**utils/ast_out.cc**:语法树输出函数实现

需要阅读并编写的代码:

**parser/SysY_parser.y**:编写你想要实现的文法定义以及对应的处理函数, 本次实验中只需要构建出语法树即可，不需要其他的额外处理

## lab3:类型检查和中间代码生成

### lab3-1:类型检查

需要阅读的代码：

**include/type.h**

需要阅读并编写的代码:

**include/tree.h**

**include/SysY_tree.h**

**ir_gen/semant.h**

**ir_gen/semant.cc**


### lab3-2:中间代码生成

需要阅读的代码：

**include/Instruction.h**

**include/ir.h**

**include/basic_block.h**

**utils/Instruction.cc**

**utils/Instruction_out.cc**

需要阅读并编写的代码:

**include/tree.h**

**include/SysY_tree.h**

**ir_gen/semant.h**

**ir_gen/semant.cc**

**ir_gen/IRgen.h**

**ir_gen/IRgen.cc**



## lab4:代码优化

## lab5:目标代码生成
