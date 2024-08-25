#ifndef IR_H
#define IR_H

#include "cfg.h"

class LLVMIR {
public:
    std::vector<Instruction> global_def{};
    std::vector<Instruction> function_declare{};

    std::map<FuncDefInstruction, CFG *> llvm_cfg{};                               //<function,cfg>
    std::map<FuncDefInstruction, std::map<int, LLVMBlock>> function_block_map;    //<function,<id,block> >

    void NewFunction(FuncDefInstruction I) { function_block_map[I] = {}; }
    LLVMBlock GetBlock(FuncDefInstruction I, int now_label) { return function_block_map[I][now_label]; }
    LLVMBlock NewBlock(FuncDefInstruction I, int &max_label) {
        ++max_label;
        function_block_map[I][max_label] = new BasicBlock(max_label);
        return GetBlock(I, max_label);
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