#ifndef FAST_LINEAR_SCAN_H
#define FAST_LINEAR_SCAN_H
#include "../basic_register_allocation.h"

bool IntervalsPrioCmp(LiveInterval a, LiveInterval b);
class FastLinearScan : public RegisterAllocation {
private:
    std::priority_queue<LiveInterval, std::vector<LiveInterval>, decltype(IntervalsPrioCmp) *> unalloc_queue;
    // 计算溢出权重
    double CalculateSpillWeight(LiveInterval);

protected:
    // 分配寄存器
    bool DoAllocInCurrentFunc();

public:
    FastLinearScan(MachineUnit *unit, PhysicalRegisters *phy, SpillCodeGen *spiller);
};

#endif