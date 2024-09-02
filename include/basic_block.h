#ifndef BASIC_BLOCK_H
#define BASIC_BLOCK_H

#include "Instruction.h"
#include <deque>
#include <iostream>
#include <set>
#include <vector>

// 请注意代码中的typedef，为了方便书写，将一些类的指针进行了重命名, 如果不习惯该种风格，可以自行修改
class BasicBlock {
public:
    std::string comment;    // used for debug
    int block_id = 0;
    std::deque<Instruction> Instruction_list{};

    /*
        pos = 1 -> end   pos = 0 -> begin
        inst1  <- front
        ...
        inst5  <- back
    */
    void InsertInstruction(int pos, Instruction Ins);

    void printIR(std::ostream &s);
    BasicBlock(int id) : block_id(id) {}
};
typedef BasicBlock *LLVMBlock;

#endif