#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

extern std::map<FuncDefInstruction, int> max_label_map;
extern std::map<FuncDefInstruction, int> max_reg_map;

void LLVMIR::EliminateUnreachedInstructionAndBlocks() {
    TODO("EliminateUNreachedInstructionAndBlocks");
}

void LLVMIR::CFGInit() {
    EliminateUnreachedInstructionAndBlocks();
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        TODO("init your members in class CFG if you need");
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::BuildCFG() {
    TODO("BuildCFG");
}

std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }

LLVMBlock CFG::GetBlock(int bbid) { return (*block_map)[bbid]; }

LLVMBlock CFG::NewBlock() {
    ++max_label;
    // std::cerr<<function_def->GetFunctionName()<<'\n';
    (*block_map)[max_label] = new BasicBlock(max_label);
    return GetBlock(max_label);
}
