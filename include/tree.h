#ifndef TREE_H
#define TREE_H

// tree definition (used for AST)
#include "type.h"
#include <iostream>

class tree_node {
protected:
    int line_number;

public:
    NodeAttribute attribute;    // 在类型检查阶段需要阅读该类的代码，语法分析阶段可以先忽略该变量
    int GetLineNumber() { return line_number; }
    void SetLineNumber(int t) { line_number = t; }

    virtual void codeIR() = 0;                              // 中间代码生成
    virtual void printAST(std::ostream &s, int pad) = 0;    // 打印语法树
    virtual void TypeCheck() = 0;                           // 类型检查
};

#endif