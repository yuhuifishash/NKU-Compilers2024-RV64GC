#include "simplify_cfg.h"
void SimplifyCFGPass::Execute()
{
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        EliminateUnreachedBlocksInsts(cfg);
    }
}

void SimplifyCFGPass::EliminateUnreachedBlocksInsts(CFG* C)
{
    TODO("EliminateUnreachedBlocksInsts");
}