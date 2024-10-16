#include "semant.h"
#include "../include/SysY_tree.h"
#include "../include/ir.h"
#include "../include/type.h"
/*
    语义分析阶段需要对语法树节点上的类型和常量等信息进行标注, 即NodeAttribute类
    同时还需要标注每个变量的作用域信息，即部分语法树节点中的scope变量
    你可以在utils/ast_out.cc的输出函数中找到你需要关注哪些语法树节点中的NodeAttribute类及其他变量
    以及对语义错误的代码输出报错信息
*/

/*
    错误检查的基本要求:
    • 检查 main 函数是否存在 (根据SysY定义，如果不存在main函数应当报错)；
    • 检查未声明变量，及在同一作用域下重复声明的变量；
    • 条件判断和运算表达式：int 和 bool 隐式类型转换（例如int a=5，return a+!a）；
    • 数值运算表达式：运算数类型是否正确 (如返回值为 void 的函数调用结果是否参与了其他表达式的计算)；
    • 检查未声明函数，及函数形参是否与实参类型及数目匹配；
    • 检查是否存在整型变量除以整型常量0的情况 (如对于表达式a/(5-4-1)，编译器应当给出警告或者直接报错)；

    错误检查的进阶要求:
    • 对数组维度进行相应的类型检查 (例如维度是否有浮点数，定义维度时是否为常量等)；
    • 对float进行隐式类型转换以及其他float相关的检查 (例如模运算中是否有浮点类型变量等)；
*/
extern LLVMIR llvmIR;

SemantTable semant_table;
std::vector<std::string> error_msgs{}; // 将语义错误信息保存到该变量中

void __Program::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->TypeCheck();
    }
}

void Exp::TypeCheck() {
    addexp->TypeCheck();

    attribute = addexp->attribute;
}

void AddExp_plus::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void AddExp_sub::TypeCheck() {
    addexp->TypeCheck();
    mulexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void MulExp_mul::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    TODO("BinaryExp Semant");
}

void MulExp_div::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    TODO("BinaryExp Semant");
}

void MulExp_mod::TypeCheck() {
    mulexp->TypeCheck();
    unary_exp->TypeCheck();

    TODO("BinaryExp Semant");
}

void RelExp_leq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void RelExp_lt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void RelExp_geq::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void RelExp_gt::TypeCheck() {
    relexp->TypeCheck();
    addexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void EqExp_eq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void EqExp_neq::TypeCheck() {
    eqexp->TypeCheck();
    relexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void LAndExp_and::TypeCheck() {
    landexp->TypeCheck();
    eqexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void LOrExp_or::TypeCheck() {
    lorexp->TypeCheck();
    landexp->TypeCheck();

    TODO("BinaryExp Semant");
}

void ConstExp::TypeCheck() {
    addexp->TypeCheck();
    attribute = addexp->attribute;
    if (!attribute.V.ConstTag) {    // addexp is not const
        error_msgs.push_back("Expression is not const " + std::to_string(line_number) + "\n");
    }
}

void Lval::TypeCheck() { TODO("Lval Semant"); }

void FuncRParams::TypeCheck() { TODO("FuncRParams Semant"); }

void Func_call::TypeCheck() { TODO("FunctionCall Semant"); }

void UnaryExp_plus::TypeCheck() { TODO("UnaryExp Semant"); }

void UnaryExp_neg::TypeCheck() { TODO("UnaryExp Semant"); }

void UnaryExp_not::TypeCheck() { TODO("UnaryExp Semant"); }

void IntConst::TypeCheck() {
    attribute.T.type = Type::INT;
    attribute.V.ConstTag = true;
    attribute.V.val.IntVal = val;
}

void FloatConst::TypeCheck() {
    attribute.T.type = Type::FLOAT;
    attribute.V.ConstTag = true;
    attribute.V.val.FloatVal = val;
}

void StringConst::TypeCheck() { TODO("StringConst Semant"); }

void PrimaryExp_branch::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void assign_stmt::TypeCheck() { TODO("AssignStmt Semant"); }

void expr_stmt::TypeCheck() {
    exp->TypeCheck();
    attribute = exp->attribute;
}

void block_stmt::TypeCheck() { b->TypeCheck(); }

void ifelse_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
    elsestmt->TypeCheck();
}

void if_stmt::TypeCheck() {
    Cond->TypeCheck();
    if (Cond->attribute.T.type == Type::VOID) {
        error_msgs.push_back("if cond type is invalid " + std::to_string(line_number) + "\n");
    }
    ifstmt->TypeCheck();
}

void while_stmt::TypeCheck() { TODO("WhileStmt Semant"); }

void continue_stmt::TypeCheck() { TODO("ContinueStmt Semant"); }

void break_stmt::TypeCheck() { TODO("BreakStmt Semant"); }

void return_stmt::TypeCheck() { return_exp->TypeCheck(); }

void return_stmt_void::TypeCheck() {}

void ConstInitVal::TypeCheck() { TODO("ConstInitVal Semant"); }

void ConstInitVal_exp::TypeCheck() { TODO("ConstInitValExp Semant"); }

void VarInitVal::TypeCheck() { TODO("VarInitVal Semant"); }

void VarInitVal_exp::TypeCheck() { TODO("VarInitValExp Semant"); }

void VarDef_no_init::TypeCheck() { TODO("VarDefNoInit Semant"); }

void VarDef::TypeCheck() { TODO("VarDef Semant"); }

void ConstDef::TypeCheck() { TODO("ConstDef Semant"); }

void VarDecl::TypeCheck() { TODO("VarDecl Semant"); }

void ConstDecl::TypeCheck() { TODO("ConstDecl Semant"); }

void BlockItem_Decl::TypeCheck() { decl->TypeCheck(); }

void BlockItem_Stmt::TypeCheck() { stmt->TypeCheck(); }

void __Block::TypeCheck() {
    semant_table.symbol_table.enter_scope();
    auto item_vector = *item_list;
    for (auto item : item_vector) {
        item->TypeCheck();
    }
    semant_table.symbol_table.exit_scope();
}

void __FuncFParam::TypeCheck() {
    VarAttribute val;
    val.ConstTag = false;
    val.type = type_decl;
    scope = 1;

    // 如果dims为nullptr, 表示该变量不含数组下标, 如果你在语法分析中采用了其他方式处理，这里也需要更改
    if (dims != nullptr) {    
        auto dim_vector = *dims;

        // the fisrt dim of FuncFParam is empty
        // eg. int f(int A[][30][40])
        val.dims.push_back(-1);    // 这里用-1表示empty，你也可以使用其他方式
        for (int i = 1; i < dim_vector.size(); ++i) {
            auto d = dim_vector[i];
            d->TypeCheck();
            if (d->attribute.V.ConstTag == false) {
                error_msgs.push_back("Array Dim must be const expression in line " + std::to_string(line_number) +
                                     "\n");
            }
            if (d->attribute.T.type == Type::FLOAT) {
                error_msgs.push_back("Array Dim can not be float in line " + std::to_string(line_number) + "\n");
            }
            val.dims.push_back(d->attribute.V.val.IntVal);
        }
        attribute.T.type = Type::PTR;
    } else {
        attribute.T.type = type_decl;
    }

    if (name != nullptr) {
        if (semant_table.symbol_table.lookup_scope(name) != -1) {
            error_msgs.push_back("multiple difinitions of formals in function " + name->get_string() + " in line " +
                                 std::to_string(line_number) + "\n");
        }
        semant_table.symbol_table.add_Symbol(name, val);
    }
}

void __FuncDef::TypeCheck() {
    semant_table.symbol_table.enter_scope();

    semant_table.FunctionTable[name] = this;

    auto formal_vector = *formals;
    for (auto formal : formal_vector) {
        formal->TypeCheck();
    }

    // block TypeCheck
    if (block != nullptr) {
        auto item_vector = *(block->item_list);
        for (auto item : item_vector) {
            item->TypeCheck();
        }
    }

    semant_table.symbol_table.exit_scope();
}

void CompUnit_Decl::TypeCheck() { TODO("CompUnitDecl Semant"); }

void CompUnit_FuncDef::TypeCheck() { func_def->TypeCheck(); }