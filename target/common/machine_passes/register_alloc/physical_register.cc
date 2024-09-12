#include "physical_register.h"
bool PhysicalRegistersAllocTools::OccupyReg(int phy_id, LiveInterval interval) {
    // 你需要保证interval不与phy_id已有的冲突
    // 或者增加判断分配失败返回false的代码
    phy_occupied[phy_id].push_back(interval);
    return true;
}

bool PhysicalRegistersAllocTools::ReleaseReg(int phy_id, LiveInterval interval) { TODO("ReleaseReg"); }

bool PhysicalRegistersAllocTools::OccupyMem(int offset, int size, LiveInterval interval) {
    TODO("OccupyMem");
    return true;
}
bool PhysicalRegistersAllocTools::ReleaseMem(int offset, int size, LiveInterval interval) {
    TODO("ReleaseMem");
    return true;
}

int PhysicalRegistersAllocTools::getIdleReg(LiveInterval interval) {
    TODO("getIdleReg");
    return -1;
}
int PhysicalRegistersAllocTools::getIdleMem(LiveInterval interval) { TODO("getIdleMem"); }

int PhysicalRegistersAllocTools::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                              LiveInterval interval2) {

    TODO("swapRegspill");
    return 0;
}

std::vector<LiveInterval> PhysicalRegistersAllocTools::getConflictIntervals(LiveInterval interval) {
    std::vector<LiveInterval> result;
    for (auto phy_intervals : phy_occupied) {
        for (auto other_interval : phy_intervals) {
            if (interval.getReg().type == other_interval.getReg().type && (interval & other_interval)) {
                result.push_back(other_interval);
            }
        }
    }
    return result;
}
