#include "mem2reg.h"
#include <tuple>

// vset is the set of alloca regno that only store but not load
void Mem2RegPass::Mem2RegNoUseAlloca(CFG *C, std::set<int> &vset) {
    // this function are used in InsertPhi
    TODO("Mem2RegNoUseAlloca");
}

// vset is the set of alloca regno that load and store are all in the BB block_id
void Mem2RegPass::Mem2RegUseDefInSameBlock(CFG *C, std::set<int> &vset, int block_id) {
    // this function are used in InsertPhi
    TODO("Mem2RegUseDefInSameBlock");
}

// vset is the set of alloca regno that one store dominators all load instructions
void Mem2RegPass::Mem2RegOneDefDomAllUses(CFG *C, std::set<int> &vset) {
    // this function are used in InsertPhi
    TODO("Mem2RegOneDefDomAllUses");
}

void Mem2RegPass::InsertPhi(CFG *C) { TODO("InsertPhi"); }

void Mem2RegPass::VarRename(CFG *C) { TODO("VarRename"); }

void Mem2RegPass::Mem2Reg(CFG *C) {
    C->BuildDominatorTree();
    InsertPhi(C);
    VarRename(C);
}

void Mem2RegPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        Mem2Reg(cfg);
    }
}