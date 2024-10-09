# NKU-Compilers2024(RISC-V64GC)

## 注意事项（学术诚信问题）
本框架由2024年编译系统实现赛RISC-V赛道作品改编而来，原代码的仓库为：

https://github.com/yuhuifishash/SysY

在基于本框架完成实验时，可以参考开源仓库中的代码，但必须在注释中明确注明自己哪部分代码是参考的其他仓库，并给出自己参考仓库的具体链接，文件以及哪一行代码。如果参考了公开源代码，但是没有在注释中明确注明，将按抄袭处理，当此作业记0分，下面是一个注释的例子。
```cpp
// Reference: https://github.com/llvm/llvm-project/blob/main/llvm/lib/Transforms/Scalar/LICM.cpp line1-line37
```

## 禁止事项

下面列举了一些禁止事项，一旦发现有这些行为，当次实验按0分处理

1. 篡改测试样例来通过测试

2. 特判文件名，函数名，变量名等方式直接根据文件名生成对应的目标代码

3. 特判测试样例输入和输出来通过测试

4. 对一整个函数进行规模较大的模式匹配后直接生成优化后的ir或目标代码(如当函数有11个基本块，且第一个基本块指令数为5条......时使用某种优化)

5. 其他试图获取不公平优势的行为

事实上，2024年编译系统设计赛中频繁有队伍进行第4点行为，包括一些性能分较高的队伍(~~决赛结束后复核被正义执行了~~)。在本学期的编译实验中，我们严禁此项行为，并且会对代码进行严格的审查，如果你不清楚什么级别的模式匹配是合法的，下面有一些例子

合理的模式匹配:

1. 模式匹配数组循环赋值，替换为memset

2. 模式匹配popcount等位运算操作，直接换为汇编指令或者库函数

禁止的模式匹配:

1. 通过判断一个函数有多少个参数，返回值，有多少个基本块，每个基本块从头到尾有什么指令的方式来模式匹配某一个函数，再将该函数替换为人工优化好的算法


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

## Run your Compiler
```bash
# 编译出的可执行文件在bin目录下，命名为SysYc
# 在项目的根目录下
./bin/SysYc -lexer -o "output_filename" "input_filename" [-O1] #词法分析
./bin/SysYc -parser -o "output_filename" "input_filename" [-O1] #语法分析
./bin/SysYc -semant -o "output_filename" "input_filename" [-O1] #语义分析
./bin/SysYc -llvm -o "output_filename" "input_filename" [-O1] #中间代码生成
./bin/SysYc -select -o "output_filename" "input_filename" [-O1] #查看目标代码生成指令选择阶段结果
./bin/SysYc -S -o "output_filename" "input_filename" [-O1] #目标代码生成

# 使用gdb调试编译器  -lexer可替换为其他上述出现的参数
gdb --args bin/SysYc -lexer -o "output_filenme" "input_filename" [-O1]

#[-O1]表示-O1是可选项
```

## Test your Compiler
```bash
# lab1和lab2不支持自动评测

python3 grade.py 3 0 #测试lab3基本要求
python3 grade.py 3 1 #测试lab3进阶要求

python3 grade.py 4 0 #测试lab4开启优化后的基本语法要求(你还需要通过optimize_test的测试)
python3 grade.py 4 1 #测试lab4开启优化后的进阶语法要求(你还需要通过optimize_test的测试)
python3 optimize_test.py #测试lab4基本优化要求, 结果不一定正确，以助教线下检查为准
# 进阶优化无测试脚本, 测试样例仅供参考, 助教线下会进行检查

# 如果你lab4获得了某个优化的分数，但是在lab5中选择了关闭该优化, 会扣除你从该优化中获得的25%分数
# 例如你在lab4中实现了完整的mem2reg并获得了2分, 但是在lab5中关闭了mem2reg优化, 会扣除2*0.25 = 0.5分
python3 grade.py 5 0 #测试lab5开启优化时的基本要求
python3 grade.py 5 1 #测试lab5开启优化时的进阶要求
```

## lab1:词法分析
需要阅读的代码：

**include/symtab.h**:定义了符号表相关类，本次实验你只需要根据注释提示阅读一小段即可

**utils/symtab.cc**:符号表相关类的具体实现

**utils/lexer_out.cc**:定义了词法分析结果的输出函数

**target/main.cc**:主函数，你需要了解框架的整体流程以及全局变量, 后续每次实验都需要进行阅读或者编写

**parser/SysY_parser.y**:只需要阅读开头%token的定义即可，你在词法分析中需要return的枚举类型均来自于该文件开头定义的%token

需要阅读并编写的代码:

**lexer/SysY_lexer.l**:编写你想实现的词法正则表达式及对应处理函数

完成代码后，以-lexer运行编译器，如果能正确输出测试用例中的每个token，即完成lab1。


## lab2:语法分析
需要阅读的代码：

**include/tree.h**:语法树节点父类

**include/SysY_tree.h**:SysY文法所有语法树节点类的定义

**utils/ast_out.cc**:语法树输出函数实现

**include/type.h**:语法树节点属性类的定义

需要阅读并编写的代码:

**parser/SysY_parser.y**:编写你想要实现的文法定义以及对应的处理函数, 本次实验中只需要构建出语法树即可，不需要其他的额外处理

完成代码后，以-parser运行编译器，如果能正确输出测试用例中对应的语法树，即完成lab2。

## lab3:类型检查和中间代码生成

### lab3-1:类型检查

需要阅读的代码：

**include/type.h**:语法树节点属性类的定义

需要阅读并编写的代码:

**include/tree.h**:如果需要的话，可以自行添加成员变量及函数

**include/SysY_tree.h**:如果需要的话，可以自行添加成员变量及函数

**ir_gen/semant.h**:类型检查辅助类的定义

**ir_gen/semant.cc**:类型检查的主要函数定义，根据你想实现的文法实现你需要函数中的TODO()

完成代码后，以-semant运行编译器，输出的语法树中需要有正确的类型, 作用域等信息, 对于一些语义错误的测试样例，要能输出错误信息

### lab3-2:中间代码生成

从lab3-2开始，每次都会有测试脚本对你的代码进行相对完善的测试

需要阅读的代码：

**include/Instruction.h**:LLVMIR指令类定义

**include/ir.h**:LLVMIR类的定义

**include/basic_block.h**:基本块的定义

**utils/Instruction.cc**:LLVMIR指令类辅助函数的实现

**utils/Instruction_out.cc**:LLVMIR输出函数的实现

需要阅读并编写的代码:

**include/tree.h**:如果需要的话，可以自行添加成员变量及函数

**include/SysY_tree.h**:如果需要的话，可以自行添加成员变量及函数

**ir_gen/semant.h**:在这一步骤中，我们还需要实现全局变量定义的中间代码生成，你需要根据需要自行添加成员变量及函数

**ir_gen/semant.cc**:在这一步骤中，我们还需要实现全局变量定义的中间代码生成，参考框架的实现是和类型检查一同完成，你也可以根据需要编写更多的函数

**ir_gen/IRgen.h**:中间代码生成辅助类的定义

**ir_gen/IRgen.cc**:中间代码生成的主要函数定义，根据你想实现的文法实现你需要函数中的TODO()



## lab4:代码优化

**基本要求**需要阅读并编写的代码:

**include/cfg.h**:控制流图的定义

**optimize/analysis/cfg.cc**:控制流图建立和cfg相关的辅助函数

**optimize/transform/simplify_cfg.h**:控制流图简化类的定义

**optimize/transform/simplify_cfg.cc**:控制流图简化的实现, 对于基本要求, 你只需要实现消除不可达基本块和指令即可

**optimize/transform/mem2reg.h**:Mem2Reg类的定义

**optimize/transform/mem2reg.cc**:Mem2Reg类的实现, 基本要求只需要实现部分函数即可

**进阶要求**需要阅读并编写的代码:

**optimize/analysis/dominator_tree.h**:支配树类的定义

**optimize/analysis/dominator_tree.cc**:支配树类的实现

**optimize/transform/mem2reg.cc**:进阶要求中，你需要实现完整的mem2reg

同时，如果你想在本次实验获得满分，你还需要根据大作业要求自行选择一些优化pass编写, 你需要自己从头开始编写你的优化pass



## lab5:目标代码生成

后端的代码框架设计上旨在易于扩展到多后端，所以代码上分为common文件夹和riscv64gc文件夹，其中common文件夹主要是架构无关的代码，例如寄存器分配算法，基本Machine指令的定义。
而riscv64gc文件夹中主要为体系结构相关代码。

**指令选择阶段**需要阅读的代码(你可能也需要在这些文件中加一些定义，来方便你代码的编写)：

**target/common/MachineBaseInstruction.h**:主要定义了基本的机器指令，RISC-V汇编指令继承了该文件中定义的类

**target/common/machine_instruction_structures/***:定义了与汇编代码相关的数据结构，例如控制流图，汇编代码上的函数，基本块等

**target/common/machine_passes/machine_pass.h**:MachineIR的Pass基类

**target/common/machine_passes/machine_selector.h**:指令选择基类

**target/common/machine_passes/machine_printer.h**:指令打印基类

**target/common/machine_passes/machine_phi_destruction.***:phi指令消除Pass，如果你没有实现完整的mem2reg，可以忽略该文件

**target/riscv64gc/instruction_print/***:RISC-V指令打印类和函数

**target/riscv64gc/riscv64.***:定义了RISC-V指令类和辅助函数

**指令选择阶段**需要阅读并编写的代码：

**target/riscv64gc/instruction_select/riscv64_instSelect.***:指令选择关键函数

**target/riscv64gc/instruction_select/riscv64_lowerframe.***:指令选择阶段只需要完成RiscV64LowerFrame类的实现，该类的作用为在函数入口地址处插入获取参数的指令

**寄存器分配阶段**需要阅读的代码:

**该阶段虽然要求编写的文件较多，但是总计代码量约300行左右，主要时间会花费在读懂已有代码上，如果你不想阅读我们提供的代码，可以选择自由发挥，写一个你自己的寄存器分配算法，即使是只在基本块内考虑或者将所有寄存器都溢出也是可以的(会酌情扣1-2分)**

**target/common/machine_passes/register_alloc/basic_register_allocation.***:寄存器分配基类

**target/common/machine_passes/register_alloc/fast_linear_scan/fast_linear_scan.h**: 定义了线性扫描算法的相关函数

**target/common/machine_passes/register_alloc/machine_liveness.cc**:活跃变量分析

**寄存器分配阶段**需要阅读并编写的代码:

**target/common/machine_passes/register_alloc/live_interval.h**:活跃区间计算，你只需要完成判断两个活跃区间是否存在重叠的代码即可。

**target/common/machine_passes/register_alloc/physical_register.***:维护物理寄存器以及溢出寄存器对内存的占用情况，这里定义了一些辅助函数，你可以根据需要实现它们或者根据自己的想法编写新的辅助函数

**target/common/machine_passes/register_alloc/fast_linear_scan/fast_linear_scan.cc**:你需要实现线性扫描算法的关键函数

**target/common/machine_passes/register_alloc/vreg_rewrite.h**:你需要实现根据寄存器分配情况生成溢出代码的函数，以及根据寄存器分配情况将虚拟寄存器重写为物理寄存器的函数

**target/riscv64gc/riscv64.cc**:实现插入Spill指令的函数，这些函数用于在寄存器分配阶段生成溢出代码，放在该文件的原因是溢出指令的生成是体系结构相关的

**target/riscv64gc/instruction_select/riscv64_lowerframe.***:完成寄存器分配后需要完成RiscV64LowerStack类的实现，在函数开始和结尾处插入保存和恢复 函数被调者 需要保存寄存器的代码


## Testcase reference
2023编译系统实现赛官方样例：https://gitlab.eduxiji.net/nscscc/compiler2023

北京大学编译原理实验课样例：https://github.com/pku-minic/awesome-sysy

洛谷P3380题解：https://www.luogu.com.cn/article/55088clb

