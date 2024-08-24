#ifndef RISCV64_INSTSELECT_H
#define RISCV64_INSTSELECT_H
#include "../../common/machine_passes/machine_selector.h"
#include "../riscv64.h"
class RiscV64Selector : public MachineSelector {
private:
    int cur_offset;    // 当前的局部变量在栈中的偏移

public:
    RiscV64Selector(MachineUnit *dest, LLVMIR *IR) : MachineSelector(dest, IR) {}
    void SelectInstructionAndBuildCFG();
    void ClearFunctionSelectState();
    template <class INSPTR> void ConvertAndAppend(INSPTR);
};
#endif