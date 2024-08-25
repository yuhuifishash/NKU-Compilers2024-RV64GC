#include "physical_register.h"
bool PhysicalRegisters::OccupyReg(int phy_id, LiveInterval interval) {
    phy_occupied[phy_id].push_back(interval);
    return true;
}

bool PhysicalRegisters::ReleaseReg(int phy_id, LiveInterval interval) {
    auto it = phy_occupied[phy_id].begin();
    for (; it != phy_occupied[phy_id].end(); ++it) {
        if (*it == interval) {
            phy_occupied[phy_id].erase(it);
            return true;
        }
    }
    return false;
}

bool PhysicalRegisters::OccupyMem(int offset, int size, LiveInterval interval) {
    TODO("OccupyMem");
    return true;
}
bool PhysicalRegisters::ReleaseMem(int offset, int size, LiveInterval interval) {
    TODO("ReleaseMem");
    return true;
}

int PhysicalRegisters::getIdleReg(LiveInterval interval) {
    for (auto i : getValidRegs(interval)) {
        int ok = true;
        for (auto conflict_j : getAliasRegs(i)) {
            for (auto other_interval : phy_occupied[conflict_j]) {
                if (interval & other_interval) {
                    ok = false;
                    break;
                }
            }
        }
        if (ok) {
            return i;
        }
    }
    return -1;
}
int PhysicalRegisters::getIdleMem(LiveInterval interval) {
    TODO("getIdleMem");
}

int PhysicalRegisters::swapRegspill(int p_reg1, LiveInterval interval1, int offset_spill2, int size,
                                    LiveInterval interval2) {

    TODO("swapRegspill");
    return 0;
}

std::vector<LiveInterval> PhysicalRegisters::getConflictIntervals(LiveInterval interval) {
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
std::vector<int> PhysicalRegisters::getAliasRegs(int phy_id) { return std::vector<int>({phy_id}); }
