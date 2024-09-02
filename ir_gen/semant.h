#ifndef SEMANT_H
#define SEMANT_H

#include "../include/SysY_tree.h"
#include "../include/symtab.h"
#include "../include/type.h"
#include <map>
#include <set>
#include <vector>
extern IdTable id_table;
/*
semant
*/
class SemantTable {
public:
    // 如果你对已有的成员变量不满意，可以删除它们并添加自己想要的

    // key的含义是函数名, FuncDef为该函数名对应的语法树
    // 可以用于函数的错误检查(例如函数调用实参与形参是否匹配)
    std::map<Symbol, FuncDef> FunctionTable;

    // 存储局部变量名与该局部变量的信息(初始值，类型等)
    SymbolTable symbol_table;

    // key的含义是全局变量名，value的含义是该全局变量的信息(初始值，类型等)
    // 我们可以在semant阶段就在llvmIR中生成全局变量定义指令
    std::map<Symbol, VarAttribute> GlobalTable;

    SemantTable() {
        // 添加库函数到函数表中, 我们不要求实现putf这个库函数
        // 可以非常方便地检查对库函数的调用是否符合定义
        Symbol getint = id_table.add_id("getint");
        FunctionTable[getint] = new __FuncDef(Type::INT, getint, new std::vector<FuncFParam>{}, nullptr);

        Symbol getch = id_table.add_id("getch");
        FunctionTable[getch] = new __FuncDef(Type::INT, getch, new std::vector<FuncFParam>{}, nullptr);

        Symbol getfloat = id_table.add_id("getfloat");
        FunctionTable[getfloat] = new __FuncDef(Type::FLOAT, getfloat, new std::vector<FuncFParam>{}, nullptr);

        Symbol getarray = id_table.add_id("getarray");
        FunctionTable[getarray] = new __FuncDef(
        Type::INT, getarray,
        new std::vector<FuncFParam>{new __FuncFParam(Type::INT, new std::vector<Expression>(1, nullptr))}, nullptr);

        Symbol getfarray = id_table.add_id("getfarray");
        FunctionTable[getfarray] = new __FuncDef(
        Type::INT, getfarray,
        new std::vector<FuncFParam>{new __FuncFParam(Type::FLOAT, new std::vector<Expression>(1, nullptr))}, nullptr);

        Symbol putint = id_table.add_id("putint");
        FunctionTable[putint] =
        new __FuncDef(Type::VOID, putint, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);

        Symbol putch = id_table.add_id("putch");
        FunctionTable[putch] =
        new __FuncDef(Type::VOID, putch, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);

        Symbol putfloat = id_table.add_id("putfloat");
        FunctionTable[putfloat] =
        new __FuncDef(Type::VOID, putfloat, new std::vector<FuncFParam>{new __FuncFParam(Type::FLOAT)}, nullptr);

        Symbol putarray = id_table.add_id("putarray");
        FunctionTable[putarray] =
        new __FuncDef(Type::VOID, putarray,
                      new std::vector<FuncFParam>{new __FuncFParam(Type::INT),
                                                  new __FuncFParam(Type::INT, new std::vector<Expression>(1, nullptr))},
                      nullptr);

        Symbol putfarray = id_table.add_id("putfarray");
        FunctionTable[putfarray] = new __FuncDef(
        Type::VOID, putfarray,
        new std::vector<FuncFParam>{new __FuncFParam(Type::INT),
                                    new __FuncFParam(Type::FLOAT, new std::vector<Expression>(1, nullptr))},
        nullptr);

        Symbol starttime = id_table.add_id("_sysy_starttime");
        FunctionTable[starttime] =
        new __FuncDef(Type::VOID, starttime, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);

        Symbol stoptime = id_table.add_id("_sysy_stoptime");
        FunctionTable[stoptime] =
        new __FuncDef(Type::VOID, stoptime, new std::vector<FuncFParam>{new __FuncFParam(Type::INT)}, nullptr);
    }
};

#endif