#ifndef PHYSICAL_REGISTER_INFO
#define PHYSICAL_REGISTER_INFO
#include "liveinterval.h"
#include <vector>

// 维护物理寄存器以及溢出寄存器对内存的占用情况
class PhysicalRegisters {
private:
protected:
    // 物理寄存器占用情况
    // phy_occupied[phy_id] = {interval1, interval2, ...} 表示物理寄存器phy_id在interval1, interval2...中被占用
    std::vector<std::vector<LiveInterval>> phy_occupied;

    // 内存占用情况
    // mem_occupied[offset] = {interval1, interval2, ...} 表示内存offset在interval1, interval2...中被占用
    std::vector<std::vector<LiveInterval>> mem_occupied;

    // 由区间获取所有合法的物理寄存器
    virtual std::vector<int> getValidRegs(LiveInterval interval) = 0;

    // 由物理寄存器获取所有的别名物理寄存器
    // 例子1：RISCV中，a0是a0的别名；RISCV中，只有寄存器和寄存器自己构成别名
    // 例子2：x86中，eax的别名有：eax, ax, al, ah
    // 例子3：ARM中，q0的别名有：q0,d0,d1,s0,s1,s2,s3
    virtual std::vector<int> getAliasRegs(int phy_id);

public:
    // 清空占用状态
    virtual void clear() {
        phy_occupied.clear();
        mem_occupied.clear();
    }
    // 占领物理寄存器,返回是否成功
    virtual bool OccupyReg(int phy_id, LiveInterval interval);
    // 释放物理寄存器,返回是否成功
    virtual bool ReleaseReg(int phy_id, LiveInterval interval);

    // 占领内存,返回是否成功
    virtual bool OccupyMem(int offset, int size, LiveInterval interval);
    // 释放内存,返回是否成功
    virtual bool ReleaseMem(int offset, int size, LiveInterval interval);

    // 获取空闲的（活跃区间不冲突的）物理寄存器, 找不到则返回-1
    virtual int getIdleReg(LiveInterval interval);
    // 获取空闲的（活跃区间不冲突的）内存
    virtual int getIdleMem(LiveInterval interval);

    // 交换分配结果（必须是一个溢出，一个不溢出), 用于在线性扫描的过程中选择溢出寄存器
    virtual int swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size, LiveInterval interval2);

    // 获得所有活跃区间与interval冲突的、已经分配在同数据类型的物理寄存器中的活跃区间
    // 用于在寄存器分配过程中选择溢出区间
    virtual std::vector<LiveInterval> getConflictIntervals(LiveInterval interval);

    // 获取溢出寄存器占用内存大小
    int getSpillSize() {
        // 也许需要添加新的成员变量进行维护
        TODO("GetSpillSize");
        return -1; 
    }
};

#endif