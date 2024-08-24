#include "riscv64_lowerframe.h"

void RiscV64LowerFrame::Execute() {
    for (auto func : unit->functions) {
        current_func = func;
        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {
                Register para_basereg = current_func->GetNewReg(INT64);
                int i32_cnt = 0;
                int para_offset = 0;
                for (auto para : func->GetParameters()) {
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {
                            b->push_front(
                            rvconstructor->ConstructCopyReg(para, GetPhysicalReg(RISCV_a0 + i32_cnt), INT64));
                        }
                        if (i32_cnt >= 8) {
                            TODO("Implement this if necessary");
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {
                        TODO("Implement this if necessary");
                    } else {
                        ERROR("Unknown type");
                    }
                }
            }
        }
    }
}

void RiscV64LowerStack::Execute() {
    // TODO: 在函数开头保存s寄存器，并开辟栈空间
    // TODO: 在函数结尾恢复s寄存器，并收回栈空间
    Log("Save s reg and alloc stack space if necessary");
}