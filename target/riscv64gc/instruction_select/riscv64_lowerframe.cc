#include "riscv64_lowerframe.h"

/*
    假设IR中的函数定义为f(i32 %r0, i32 %r1)
    则parameters应该存放两个虚拟寄存器%0,%1

    在LowerFrame后应当为
    add %0,a0,x0  (%0 <- a0)
    add %1,a1,x0  (%1 <- a1)

    对于浮点寄存器按照类似的方法处理即可
*/
void RiscV64LowerFrame::Execute() {
    // 在每个函数的开头处插入获取参数的指令
    for (auto func : unit->functions) {
        current_func = func;
        for (auto &b : func->blocks) {
            cur_block = b;
            if (b->getLabelId() == 0) {    // 函数入口，需要插入获取参数的指令
                int i32_cnt = 0;
                for (auto para : func->GetParameters()) {    // 你需要在指令选择阶段正确设置parameters的值
                    if (para.type.data_type == INT64.data_type) {
                        if (i32_cnt < 8) {    // 插入使用寄存器传参的指令
                            b->push_front(rvconstructor->ConstructR(RISCV_ADD, para, GetPhysicalReg(RISCV_a0 + i32_cnt),
                                                                    GetPhysicalReg(RISCV_x0)));
                        }
                        if (i32_cnt >= 8) {    // 插入使用内存传参的指令
                            TODO("Implement this if you need");
                        }
                        i32_cnt++;
                    } else if (para.type.data_type == FLOAT64.data_type) {    // 处理浮点数
                        TODO("Implement this if you need");
                    } else {
                        ERROR("Unknown type");
                    }
                }
            }
        }
    }
}

void RiscV64LowerStack::Execute() {
    // 在函数在寄存器分配后执行
    // TODO: 在函数开头保存 函数被调者需要保存的寄存器，并开辟栈空间
    // TODO: 在函数结尾恢复 函数被调者需要保存的寄存器，并收回栈空间
    // TODO: 开辟和回收栈空间
    // 具体需要保存/恢复哪些可以查阅RISC-V函数调用约定
    Log("RiscV64LowerStack");

    // 到此我们就完成目标代码生成的所有工作了
}
