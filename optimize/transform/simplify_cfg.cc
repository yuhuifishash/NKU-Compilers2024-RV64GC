#include "simplify_cfg.h"

void SimplifyCFGPass::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}

// 删除从函数入口开始到达不了的基本块和指令
void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG *C) { TODO("EliminateUnreachedBlocksInsts"); }