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
    // 寄存器分配, 返回是否溢出
    bool DoAllocInCurrentFunc();

public:
    FastLinearScan(MachineUnit *unit, PhysicalRegistersAllocTools *phy, SpillCodeGen *spiller);
};

#endif