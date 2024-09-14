#include "basic_register_allocation.h"

void RegisterAllocation::Execute() {
    // 你需要保证此时不存在phi指令
    for (auto func : unit->functions) {
        not_allocated_funcs.push(func);
    }
    while (!not_allocated_funcs.empty()) {
        current_func = not_allocated_funcs.front();
        numbertoins.clear();
        // 对每条指令进行编号
        InstructionNumber(unit, numbertoins).ExecuteInFunc(current_func);

        // 需要清除之前分配的结果
        alloc_result[current_func].clear();
        not_allocated_funcs.pop();

        // 计算活跃区间
        UpdateIntervalsInCurrentFunc();

        if (DoAllocInCurrentFunc()) {    // 尝试进行分配
            // 如果发生溢出，插入spill指令后将所有物理寄存器退回到虚拟寄存器，重新分配
            spiller->ExecuteInFunc(current_func, &alloc_result[current_func]);    // 生成溢出代码
            current_func->AddStackSize(phy_regs_tools->getSpillSize());                 // 调整栈的大小
            not_allocated_funcs.push(current_func);                               // 重新分配直到不再spill
        }
    }
    // 重写虚拟寄存器，全部转换为物理寄存器
    VirtualRegisterRewrite(unit, alloc_result).Execute();
}

void InstructionNumber::Execute() {
    for (auto func : unit->functions) {
        ExecuteInFunc(func);
    }
}
void InstructionNumber::ExecuteInFunc(MachineFunction *func) {
    // 对每个指令进行编号(用于计算活跃区间)
    int count_begin = 0;
    current_func = func;
    // Note: If Change to DFS Iterator, RegisterAllocation::UpdateIntervalsInCurrentFunc() Also need to be
    // changed
    auto it = func->getMachineCFG()->getBFSIterator();
    it->open();
    while (it->hasNext()) {
        auto mcfg_node = it->next();
        auto mblock = mcfg_node->Mblock;
        // Update instruction number
        // 每个基本块开头会占据一个编号
        this->numbertoins[count_begin] = InstructionNumberEntry(nullptr, true);
        count_begin++;
        for (auto ins : *mblock) {
            this->numbertoins[count_begin] = InstructionNumberEntry(ins, false);
            ins->setNumber(count_begin++);
        }
    }
}

void RegisterAllocation::UpdateIntervalsInCurrentFunc() {
    intervals.clear();
    auto mfun = current_func;
    auto mcfg = mfun->getMachineCFG();

    Liveness liveness(mfun);

    // Note: If Change to DFS Iterator, InstructionNumber::Execute() Also need to be changed
    auto it = mcfg->getReverseIterator(mcfg->getBFSIterator());
    it->open();

    std::map<Register, int> last_def, last_use;

    while (it->hasNext()) {
        auto mcfg_node = it->next();
        auto mblock = mcfg_node->Mblock;
        auto cur_id = mcfg_node->Mblock->getLabelId();
        // For pseudo code see https://www.cnblogs.com/AANA/p/16311477.html
        for (auto reg : liveness.GetOUT(cur_id)) {
            if (intervals.find(reg) == intervals.end()) {
                intervals[reg] = LiveInterval(reg);
            }
            // Extend or add new Range
            if (last_use.find(reg) == last_use.end()) {
                // No previous Use, New Range
                intervals[reg].PushFront(mblock->getBlockInNumber(), mblock->getBlockOutNumber());
            } else {
                // Have previous Use, No Extend Range
                // intervals[reg].SetMostBegin(mblock->getBlockInNumber());
                intervals[reg].PushFront(mblock->getBlockInNumber(), mblock->getBlockOutNumber());
            }
            last_use[reg] = mblock->getBlockOutNumber();
        }
        for (auto reverse_it = mcfg_node->Mblock->ReverseBegin(); reverse_it != mcfg_node->Mblock->ReverseEnd();
             ++reverse_it) {
            auto ins = *reverse_it;
            for (auto reg : ins->GetWriteReg()) {
                // Update last_def of reg
                last_def[*reg] = ins->getNumber();

                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                // Have Last Use, Cut Range
                if (last_use.find(*reg) != last_use.end()) {
                    last_use.erase(*reg);
                    intervals[*reg].SetMostBegin(ins->getNumber());
                } else {
                    // No Last Use, New Range
                    intervals[*reg].PushFront(ins->getNumber(), ins->getNumber());
                }
                intervals[*reg].IncreaseReferenceCount(1);
            }
            for (auto reg : ins->GetReadReg()) {
                // Update last_use of reg
                if (intervals.find(*reg) == intervals.end()) {
                    intervals[*reg] = LiveInterval(*reg);
                }

                if (last_use.find(*reg) != last_use.end() /*|| (last_def[*reg] == last_use[*reg])*/) {
                } else {
                    // No Last Use, New Range
                    intervals[*reg].PushFront(mblock->getBlockInNumber(), ins->getNumber());
                }
                last_use[*reg] = ins->getNumber();

                intervals[*reg].IncreaseReferenceCount(1);
            }
        }
        last_use.clear();
        last_def.clear();
    }
    // 你可以在这里输出intervals的值来获得活跃变量分析的结果
    // 观察结果可能对你寄存器分配算法的编写有一定帮助
}