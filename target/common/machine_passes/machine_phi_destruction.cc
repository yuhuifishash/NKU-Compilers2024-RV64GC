#include "machine_phi_destruction.h"
extern bool optimize_flag;
void MachinePhiDestruction::Execute() {
    for (auto func : unit->functions) {
        current_func = func;
        PhiDestructionInCurrentFunction();
    }
}

void MachinePhiDestruction::PhiDestructionInCurrentFunction() { TODO("Implement this if necessary"); }