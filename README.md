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

(事实上，2024年编译系统设计赛中频繁有队伍进行第4点行为，包括来自北航的risc-v和arm赛道特等奖队伍, 并且组委会对于申诉采取了冷处理的态度, 但是在本学期的编译实验中，我们严禁此项行为，并且会对代码进行严格的审查, 如果你不清楚什么级别的模式匹配是合法的，下面有一些例子)

合理的模式匹配:

1. 模式匹配数组循环赋值，替换为memset

2. 模式匹配popcount等位运算操作，直接换为汇编指令或者库函数

禁止的模式匹配:

1. 通过判断一个函数有多少个参数，返回值，有多少个基本块，每个基本块从头到尾有什么指令的方式来模式匹配某一个函数，再将该函数替换为人工优化好的算法

这里有一个禁止的模式匹配代码例子(https://gitlab.eduxiji.net/educg-group-26173-2487151/T202410006203109-3846/-/blob/main/src/midend/Transform/Multiply.java):
```java
private static boolean detectMultiply(Function func) {
    if (func.getFuncRArguments().size() != 2) return false;
    if (!func.getRetType().isInt32Ty()) return false;
    FuncInfo funcInfo = AnalysisManager.getFuncInfo(func);
    if (!funcInfo.isRecursive) return false;
    //四条返回值 一条0 三条rem
    ArrayList<Instruction.Return> returns = new ArrayList<>();
    for (BasicBlock bb : func.getBlocks()) {
        Instruction term = bb.getTerminator();
        if (term instanceof Instruction.Return ret) {
            returns.add(ret);
        }
    }
    if (returns.size() != 4) return false;
    int zeroCnt = 0;
    int remCnt = 0;
    ArrayList<Instruction.Rem> rems = new ArrayList<>();
    for (Instruction.Return ret : returns) {
        if (ret.getRetValue().equals(Constant.ConstantInt.get(0))) {
            zeroCnt++;
        }
        else if (ret.getRetValue() instanceof Instruction.Rem rem) {
            rems.add(rem);
            remCnt++;
        }
        else {
            return false;
        }
    }
    if (zeroCnt != 1 || remCnt != 3) return false;
    for (Instruction.Rem rem : rems) {
        if (rem.getOperand_2() instanceof Constant.ConstantInt c) {
            if (Rem_Value == null) {
                Rem_Value = c;
            }
            else if (!Rem_Value.equals(c)) {
                return false;
            }
        }
        else {
            return false;
        }
    }
//        System.out.println("Detect multiply function: " + func.getName());
    return true;
}


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

**target/main.cc**:主函数，你需要了解框架的整体流程以及全局变量, 后续每次实验都需要进行阅读或者编写

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

**include/type.h**:语法树节点属性类的定义

需要阅读并编写的代码:

**include/tree.h**:如果需要的话，可以自行添加成员变量及函数

**include/SysY_tree.h**:如果需要的话，可以自行添加成员变量及函数

**ir_gen/semant.h**:类型检查辅助类的定义

**ir_gen/semant.cc**:类型检查的主要函数定义，根据你想实现的文法实现你需要函数中的TODO()


### lab3-2:中间代码生成

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

**optimize/cfg/cfg.cc**:基本的cfg优化函数及辅助函数定义

**optimize/ssa/mem2reg.h**:Mem2Reg类的定义

**optimize/ssa/mem2reg.cc**:Mem2Reg类的实现, 基本要求只需要实现部分函数即可

**进阶要求**需要阅读并编写的代码:

**optimize/cfg/dominator_tree.h**:支配树类的定义

**optimize/cfg/dominator_tree.cc**:支配树类的实现

**optimize/ssa/mem2reg.cc**:进阶要求中，你需要实现完整的mem2reg

同时，如果你想在本次实验获得满分，你还需要根据大作业要求自行选择一些优化pass编写, 你需要自己从头开始编写你的优化pass



## lab5:目标代码生成
