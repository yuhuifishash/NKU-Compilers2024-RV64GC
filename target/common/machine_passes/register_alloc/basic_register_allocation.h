#ifndef BASIC_REGISTER_ALLOCATION_H
#define BASIC_REGISTER_ALLOCATION_H
#include "../../machine_instruction_structures/machine.h"
#include "../machine_pass.h"
#include "liveinterval.h"
#include "physical_register.h"
#include <vector>

// 记录单项分配结果
struct AllocResult {
    bool in_mem;
    union {
        int phy_reg_no;
        int stack_offset;    // 这里的offset表示偏移, 相对什么位置及单位可以自行决定
    };
    AllocResult() : in_mem(false) { phy_reg_no = 0; }
    AllocResult(const struct AllocResult &other) {
        in_mem = other.in_mem;
        phy_reg_no = other.phy_reg_no;
    }
};

class SpillCodeGen;

struct InstructionNumberEntry {
public:
    MachineBaseInstruction *ins;
    bool is_block_begin;
    InstructionNumberEntry() : ins(nullptr), is_block_begin(true) {}
    InstructionNumberEntry(MachineBaseInstruction *ins, bool isbegin) : ins(ins), is_block_begin(isbegin) {}
};

class RegisterAllocation : public MachinePass {
private:
    void UpdateIntervalsInCurrentFunc();
    std::queue<MachineFunction *> not_allocated_funcs;
    SpillCodeGen *spiller;

protected:
    std::map<int, InstructionNumberEntry> numbertoins;
    // 将分配结果写入alloc_result
    // SpillCodeGen/VregRewrite使用alloc_result的信息完成后续操作
    void AllocPhyReg(MachineFunction *mfun, Register vreg, int phyreg) {
        Assert(vreg.is_virtual);
        alloc_result[mfun][vreg].in_mem = false;
        alloc_result[mfun][vreg].phy_reg_no = phyreg;
    }
    void AllocStack(MachineFunction *mfun, Register vreg, int offset) {
        Assert(vreg.is_virtual);
        alloc_result[mfun][vreg].in_mem = true;
        alloc_result[mfun][vreg].stack_offset = offset;
    }
    void swapAllocResult(MachineFunction *mfun, Register v1, Register v2) {
        Assert(v1.is_virtual && v2.is_virtual);
        AllocResult tmp = alloc_result[mfun][v1];
        alloc_result[mfun][v1] = alloc_result[mfun][v2];
        alloc_result[mfun][v2] = tmp;
    }
    int getAllocResultInReg(MachineFunction *mfun, Register vreg) {
        Assert(alloc_result[mfun][vreg].in_mem == false);
        Assert(vreg.is_virtual);
        return alloc_result[mfun][vreg].phy_reg_no;
    }
    int getAllocResultInMem(MachineFunction *mfun, Register vreg) {
        Assert(alloc_result[mfun][vreg].in_mem == true);
        Assert(vreg.is_virtual);
        return alloc_result[mfun][vreg].stack_offset;
    }

protected:
    std::map<Register, LiveInterval> intervals;
    PhysicalRegistersAllocTools *phy_regs_tools;
    
    // 在当前函数中完成寄存器分配
    virtual bool DoAllocInCurrentFunc() = 0;
    std::map<MachineFunction *, std::map<Register, AllocResult>> alloc_result;

public:
    RegisterAllocation(MachineUnit *unit, PhysicalRegistersAllocTools *phy, SpillCodeGen *spiller)
        : MachinePass(unit), phy_regs_tools(phy), spiller(spiller) {}
    // 对所有函数进行寄存器分配
    void Execute();
};

class InstructionNumber : public MachinePass {
private:
    std::map<int, InstructionNumberEntry> &numbertoins;

public:
    // 给指令编号，并记录指令编号与指令的映射
    InstructionNumber(MachineUnit *unit, std::map<int, InstructionNumberEntry> &number2ins)
        : MachinePass(unit), numbertoins(number2ins) {}
    void Execute();
    void ExecuteInFunc(MachineFunction *func);
};

class VirtualRegisterRewrite : public MachinePass {
private:
    const std::map<MachineFunction *, std::map<Register, AllocResult>> &alloc_result;

public:
    // 根据分配结果重写指令中的虚拟寄存器
    VirtualRegisterRewrite(MachineUnit *unit,
                           const std::map<MachineFunction *, std::map<Register, AllocResult>> &alloc_result)
        : MachinePass(unit), alloc_result(alloc_result) {}
    void Execute();
    void ExecuteInFunc();
};

class SpillCodeGen {
private:
    std::map<Register, AllocResult> *alloc_result;
    // 生成从栈中读取溢出寄存器的指令
    virtual Register GenerateReadCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                      MachineDataType type) = 0;
    // 生成将溢出寄存器写入栈的指令
    virtual Register GenerateWriteCode(std::list<MachineBaseInstruction *>::iterator &it, int raw_stk_offset,
                                       MachineDataType type) = 0;

protected:
    MachineFunction *function;
    MachineBlock *cur_block;

public:
    // 在当前函数中执行溢出代码生成
    void ExecuteInFunc(MachineFunction *function, std::map<Register, AllocResult> *alloc_result);
};

#endif