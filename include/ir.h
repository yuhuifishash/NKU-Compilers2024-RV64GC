#ifndef IR_H
#define IR_H

#include "cfg.h"

class LLVMIR {
public:
    // 全局变量定义指令
    std::vector<Instruction> global_def{};
    // 函数声明指令
    std::vector<Instruction> function_declare{};

    // key为函数定义指令(需要保证函数定义指令与函数一一对应), value为函数对应的cfg
    // 在中间代码生成中, 你可以暂时忽略该成员变量, 你可以在代码优化时再考虑该变量
    std::map<FuncDefInstruction, CFG *> llvm_cfg{};

    // key为函数定义指令, value为函数对应的所有基本块, 该成员变量会在输出中使用，所以你必须对该变量正确赋值
    std::map<FuncDefInstruction, std::map<int, LLVMBlock>> function_block_map;

    void NewFunction(FuncDefInstruction I) { function_block_map[I] = {}; }
    LLVMBlock GetBlock(FuncDefInstruction I, int now_label) { return function_block_map[I][now_label]; }

    // 获取一个新的编号为x的基本块, 该编号不能与已经有的重复
    LLVMBlock NewBlock(FuncDefInstruction I, int x) {
        function_block_map[I][x] = new BasicBlock(x);
        return GetBlock(I, x);
    }
    void printIR(std::ostream &s);

    /*this function elimate unreached instructions and blocks. it is called after IRgen
      it will elimate instructions after ret and unreached blocks*/
    void EliminateUnreachedInstructionAndBlocks();
    void CFGInit();
    void BuildCFG();
    void BuildDominatorTree();
};

#endif