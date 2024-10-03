#include "def_use.h"
#include "../../include/ir.h"

/*
伪代码:
for BasicBlock bb in C
    for Instruction I in bb
        if I 有结果寄存器
            int regno = I->GetResultRegNo()
            IRDefMaps[C][regno] = I
*/
void DefUseAnalysis::GetDefUseInSingleCFG(CFG* C) {
    TODO("GetDefUseInSingleCFG");
}

void DefUseAnalysis::Execute() {
    for (auto [defI, cfg] : llvmIR->llvm_cfg) {
        GetDefUseInSingleCFG(cfg);
    }
}