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
    // TODO():添加更多你需要的成员变量和成员函数

    SymbolRegTable symbol_table;
    IRgenTable() {}
};

#endif