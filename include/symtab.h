#ifndef SYMTAB_H
#define SYMTAB_H

#include "type.h"
#include <map>
#include <stack>
#include <string>
#include <vector>

/*词法分析阶段需要阅读的代码*/
class Entry {
private:
    std::string name;

public:
    Entry(std::string s) { name = s; }
    std::string get_string() { return name; }
};
typedef Entry *Symbol;

/* 相同的字符串均使用同一个Symbol指向，我们可以直接使用Symbol的 == 来判断标识符是否相等 
   必须使用add_id函数来添加新的Symbol
   作用域问题在词法分析和语法分析阶段不需要考虑，在semant阶段我们会进行处理
*/
class IdTable {
private:
    std::map<std::string, Symbol> id_table{};

public:
    Symbol add_id(std::string s);
};

/*语义分析和类型检查阶段需要阅读的代码*/
class SymbolTable {
private:
    int current_scope = -1;
    std::vector<std::map<Symbol, VarAttribute>> symbol_table;

public:
    int get_current_scope() { return current_scope; }
    void add_Symbol(Symbol C, VarAttribute val);

    // in semant   return type of C
    Type::ty lookup_type(Symbol C);
    int lookup_scope(Symbol C);
    VarAttribute lookup_val(Symbol C);
    void enter_scope();
    void exit_scope();
};


/*

*/
class SymbolRegTable {
private:
    int current_scope = -1;
    std::vector<std::map<Symbol, int>> symbol_table;

public:
    void add_Symbol(Symbol C, int reg);

    //return the regno of Symbol C
    int lookup(Symbol C);
    void enter_scope();
    void exit_scope();
};

#endif