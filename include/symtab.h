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
    // 当前作用域
    int current_scope = -1;

    std::vector<std::map<Symbol, VarAttribute>> symbol_table;

public:
    int get_current_scope() { return current_scope; }

    // 在当前作用域插入一条局部变量信息
    void add_Symbol(Symbol C, VarAttribute val);

    // 返回离当前作用域最近的局部变量的类型(C为局部变量名对应的Symbol)
    Type::ty lookup_type(Symbol C);
    // 返回离当前作用域最近的局部变量的作用域
    int lookup_scope(Symbol C);
    // 返回离当前作用域最近的局部变量的相关信息
    VarAttribute lookup_val(Symbol C);
    // 进入新的作用域
    void enter_scope();
    // 退出当前作用域, 返回到上一层作用域
    void exit_scope();
};

/*
    对于一个局部变量a,假设我们的例子如下:
    int a = 5, b = 3;
    a = a + 5;
    return b + a;

    我们生成的llvmIR应当首先为a分配内存空间，在每次对a的使用前load，每次对a的赋值后store
    llvmIR示例(use pseudo code):
    %r1 = alloca i32 (local var a)
    %r2 = alloca i32 (local var b)
    store 5 -> ptr %r1
    store 3 -> ptr %r2
    %r3 = load ptr %r1
    %r4 = %r3 + 5
    store %r4 -> ptr %r1
    %r5 = load ptr %r2
    %r6 = load ptr %r1
    %r7 = %r5 + %r6
    ret i32 %r7
*/
class SymbolRegTable {
private:
    int current_scope = -1;
    std::vector<std::map<Symbol, int>> symbol_table;

public:
    void add_Symbol(Symbol C, int reg);

    // 返回离当前作用域最近的局部变量的alloca结果对应的寄存器编号(C为局部变量名对应的Symbol)
    int lookup(Symbol C);
    void enter_scope();
    void exit_scope();
};

#endif