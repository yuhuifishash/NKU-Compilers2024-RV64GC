#include "dominator_tree.h"
#include "../../include/ir.h"

void LLVMIR::BuildDominatorTree() {
    for (auto [defI, cfg] : llvm_cfg) {
        cfg->BuildDominatorTree();
    }
}

void DominatorTree::BuildDominatorTree(bool reverse) { TODO("BuildDominatorTree"); }

std::set<int> DominatorTree::GetDF(std::set<int> S) { TODO("GetDF"); }

std::set<int> DominatorTree::GetDF(int id) { TODO("GetDF"); }

bool DominatorTree::IsDominate(int id1, int id2) { TODO("IsDominate"); }
