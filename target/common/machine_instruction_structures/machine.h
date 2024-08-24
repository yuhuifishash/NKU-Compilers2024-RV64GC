#ifndef MACHINE
#define MACHINE

#include "../../../include/ir.h"
#include "../MachineBaseInstruction.h"
class MachineFunction;
class MachineBlock;
class MachineUnit;
class MachineCFG;
#include <list>

class MachineBlock {
private:
    int label_id;

protected:
    std::list<MachineBaseInstruction *> instructions;

private:
    MachineFunction *parent;

public:
    virtual std::vector<int> getAllBranch() = 0;    // [0]-false, [1]-true
    virtual void ReverseBranch() = 0;
    virtual std::list<MachineBaseInstruction *>::iterator getInsertBeforeBrIt() = 0;
    decltype(instructions) &GetInsList() { return instructions; }
    void clear() { instructions.clear(); }
    auto erase(decltype(instructions.begin()) it) { return instructions.erase(it); }
    auto insert(decltype(instructions.begin()) it, MachineBaseInstruction *ins) { return instructions.insert(it, ins); }
    auto getParent() { return parent; }
    void setParent(MachineFunction *parent) { this->parent = parent; }
    auto getLabelId() { return label_id; }
    auto ReverseBegin() { return instructions.rbegin(); }
    auto ReverseEnd() { return instructions.rend(); }
    auto begin() { return instructions.begin(); }
    auto end() { return instructions.end(); }
    auto size() { return instructions.size(); }
    void push_back(MachineBaseInstruction *ins) { instructions.push_back(ins); }
    void push_front(MachineBaseInstruction *ins) { instructions.push_front(ins); }
    void pop_back() { instructions.pop_back(); }
    void pop_front() { instructions.pop_front(); }
    int getBlockInNumber() {
        for (auto ins : instructions) {
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                return ins->getNumber() - 1;
            }
        }
        ERROR("Empty block");
        // return (*(instructions.begin()))->getNumber();
    }
    int getBlockOutNumber() {
        for (auto it = instructions.rbegin(); it != instructions.rend(); ++it) {
            auto ins = *it;
            if (ins->arch != MachineBaseInstruction::COMMENT) {
                return ins->getNumber();
            }
        }
        ERROR("Empty block");
        // return (*(instructions.rbegin()))->getNumber();
    }
    MachineBlock(int id) : label_id(id) {}
};

class MachineBlockFactory {
public:
    virtual MachineBlock *CreateBlock(int id) = 0;
};

class MachineFunction {
private:
    std::string func_name;
    MachineUnit *parent;
    std::vector<Register> parameters;
    int max_exist_label;
    MachineBlockFactory *block_factory;

protected:
    int stack_sz;
    int para_sz;
    bool has_inpara_instack;
    MachineCFG *mcfg;

public:
    bool HasInParaInStack() { return has_inpara_instack; }
    void SetHasInParaInStack(bool has) { has_inpara_instack = has; }
    void UpdateMaxLabel(int labelid) { max_exist_label = max_exist_label > labelid ? max_exist_label : labelid; }
    const decltype(parameters) &GetParameters() { return parameters; }
    void AddParameter(Register reg) { parameters.push_back(reg); }
    void SetStackSize(int sz) { stack_sz = sz; }
    void UpdateParaSize(int parasz) {
        if (parasz > para_sz) {
            para_sz = parasz;
        }
    }
    int GetParaSize() { return para_sz; }
    virtual void AddStackSize(int sz) { stack_sz += sz; }
    int GetStackSize() { return ((stack_sz + 15) / 16) * 16; }
    int GetRaOffsetToSp() { return stack_sz - 8; }
    int GetStackOffset() { return stack_sz; }
    MachineCFG *getMachineCFG() { return mcfg; }
    MachineUnit *getParentMachineUnit() { return parent; }
    std::string getFunctionName() { return func_name; }

    void SetParent(MachineUnit *parent) { this->parent = parent; }
    void SetMachineCFG(MachineCFG *mcfg) { this->mcfg = mcfg; }

    // private:
    // May be removed in future (?), or become private
    // You can also iterate blocks in MachineCFG
    std::vector<MachineBlock *> blocks{};

public:
    Register GetNewRegister(int regtype, int regwidth, bool save_across_call = false);
    Register GetNewReg(MachineDataType type);

protected:
    MachineBlock *InitNewBlock();

public:
    MachineFunction(std::string name, MachineBlockFactory *blockfactory)
        : func_name(name), stack_sz(0), para_sz(0), block_factory(blockfactory), max_exist_label(0),
          has_inpara_instack(false) {}
};

class MachineUnit {
public:
    std::vector<Instruction> global_def{};
    std::vector<MachineFunction *> functions;
};

class MachineCFG {
public:
    class MachineCFGNode {
    public:
        MachineBlock *Mblock;
    };

private:
    std::map<int, MachineCFGNode *> block_map{};
    std::vector<std::vector<MachineCFGNode *>> G{}, invG{};
    int max_label;

public:
    MachineCFG() : max_label(0){};
    void AssignEmptyNode(int id, MachineBlock *Mblk);

    // Just modify CFG edge, no change on branch instructions
    void MakeEdge(int edg_begin, int edg_end);
    void RemoveEdge(int edg_begin, int edg_end);

    MachineCFGNode *GetNodeByBlockId(int id) { return block_map[id]; }
    std::vector<MachineCFGNode *> GetSuccessorsByBlockId(int id) { return G[id]; }
    std::vector<MachineCFGNode *> GetPredecessorsByBlockId(int id) { return invG[id]; }

private:
    class Iterator {
    protected:
        MachineCFG *mcfg;

    public:
        Iterator(MachineCFG *mcfg) : mcfg(mcfg) {}
        MachineCFG *getMachineCFG() { return mcfg; }
        virtual void open() = 0;
        virtual MachineCFGNode *next() = 0;
        virtual bool hasNext() = 0;
        virtual void rewind() = 0;
        virtual void close() = 0;
    };
    class SeqScanIterator;
    class ReverseIterator;
    class DFSIterator;
    class BFSIterator;

public:
    DFSIterator *getDFSIterator();
    BFSIterator *getBFSIterator();
    SeqScanIterator *getSeqScanIterator();
    ReverseIterator *getReverseIterator(Iterator *);
};
#include "cfg_iterators/cfg_iterators.h"
#endif