#include "../../include/cfg.h"
#include <tuple>

// vset is the set of alloca regno that only store but not load
void Mem2RegNoUseAlloca(CFG *C, std::set<int> &vset) {
    // this function are used in InsertPhi
    TODO("Mem2RegNoUseAlloca");
}

// vset is the set of alloca regno that load and store are all in the BB block_id
void Mem2RegUseDefInSameBlock(CFG *C, std::set<int> &vset, int block_id) {
    // this function are used in InsertPhi
    TODO("Mem2RegUseDefInSameBlock");
}

// vset is the set of alloca regno that one store dominators all load instructions
void Mem2RegOneDefDomAllUses(CFG *C, std::set<int> &vset) {
    // this function are used in InsertPhi
    TODO("Mem2RegOneDefDomAllUses");
}

void InsertPhi(CFG *C) { TODO("InsertPhi"); }

void VarRename(CFG *C) { TODO("VarRename"); }

void Mem2Reg(CFG *C) {
    C->BuildDominatorTree();
    InsertPhi(C);
    VarRename(C);
}
