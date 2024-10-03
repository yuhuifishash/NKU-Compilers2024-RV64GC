#ifndef DEF_USE_H
#define DEF_USE_H
#include "../../include/ir.h"
#include "../pass.h"
#include <set>
#include <vector>

class DefUseAnalysis : public IRPass {
private:
    void GetDefUseInSingleCFG(CFG* C);

    //<key1:CFG*, value1: 
    //      <key2: 寄存器编号, value2: 定义该寄存器的指令 > >
    std::map<CFG*, std::map<int, Instruction> > IRDefMaps;

    // 添加更多的成员变量和函数来完成def-use分析
public:
    DefUseAnalysis(LLVMIR *IR) : IRPass(IR) {}
    
    const std::map<int, Instruction>& GetDefMap(CFG* C) {return IRDefMaps[C];}
    void Execute();
    // TODO(): add more functions and members if you need
};
#endif