#include "../../include/Instruction.h"
#include "../../include/ir.h"
#include <assert.h>

void LLVMIR::CFGInit() {
    for (auto &[defI, bb_map] : function_block_map) {
        CFG *cfg = new CFG();
        cfg->block_map = &bb_map;
        cfg->function_def = defI;
        cfg->BuildCFG();
        TODO("init your members in class CFG if you need");
        llvm_cfg[defI] = cfg;
    }
}

void LLVMIR::BuildCFG() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildCFG();
    }
}

void CFG::BuildCFG() { TODO("BuildCFG"); }

std::vector<LLVMBlock> CFG::GetPredecessor(LLVMBlock B) { return invG[B->block_id]; }

std::vector<LLVMBlock> CFG::GetPredecessor(int bbid) { return invG[bbid]; }

std::vector<LLVMBlock> CFG::GetSuccessor(LLVMBlock B) { return G[B->block_id]; }

std::vector<LLVMBlock> CFG::GetSuccessor(int bbid) { return G[bbid]; }