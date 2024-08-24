#ifndef CFG_H
#define CFG_H

#include "../optimize/cfg/dominator_tree.h"
#include "SysY_tree.h"
#include "basic_block.h"
#include <bitset>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <vector>

class CFG {
public:
    int max_label = 0;
    int max_reg = 0;
    FuncDefInstruction function_def;
    LLVMBlock ret_block;

    /*this is the pointer to the value of LLVMIR.function_block_map
      you can see it in the LLVMIR::CFGInit()*/
    std::map<int, LLVMBlock> *block_map;

    std::vector<std::vector<LLVMBlock>> G{};       // control flow graph
    std::vector<std::vector<LLVMBlock>> invG{};    // inverse control flow graph
    DominatorTree DomTree;

    void BuildCFG();
    void BuildDominatorTree() {
        DomTree.C = this;
        DomTree.BuildDominatorTree();
    }

    // if blockid1 dominate blockid2, return true, else return false
    bool IsDominate(int id1, int id2) { return DomTree.IsDominate(id1, id2); }

    std::vector<LLVMBlock> GetPredecessor(LLVMBlock B);
    std::vector<LLVMBlock> GetPredecessor(int bbid);
    std::vector<LLVMBlock> GetSuccessor(LLVMBlock B);
    std::vector<LLVMBlock> GetSuccessor(int bbid);
    LLVMBlock GetBlock(int bbid);
    LLVMBlock NewBlock();
};

#endif