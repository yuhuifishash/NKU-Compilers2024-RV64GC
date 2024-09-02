#ifndef IRGEN_H
#define IRGEN_H

#include "../include/SysY_tree.h"
#include "../include/cfg.h"
#include "../include/symtab.h"
#include "../include/type.h"
#include <assert.h>
#include <map>
#include <vector>

class IRgenTable {
public:
    // 如果你无从下手,推荐先阅读LLVMIR类的printIR函数,了解我们是如何输出中间代码的
    // 然后你可以尝试往和输出相关联的变量中随便添加一些函数定义指令, 新建一些基本块或添加几条指令看看输出是怎么变化的
    // 弄懂LLVMIR类是如何存储中间代码后，剩余的就是理解中间代码生成算法了

    // TODO():添加更多你需要的成员变量和成员函数
    SymbolRegTable symbol_table;
    IRgenTable() {}
};

#endif