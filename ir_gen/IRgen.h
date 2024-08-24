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
    std::map<int, VarAttribute> RegTable;
    std::map<int, int> FormalArrayTable;
    SymbolRegTable symbol_table;
    IRgenTable() {}
};

#endif