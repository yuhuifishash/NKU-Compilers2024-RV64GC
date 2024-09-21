#include "IRgen.h"
#include "../include/ir.h"
#include "semant.h"

extern SemantTable semant_table;    // 也许你会需要一些语义分析的信息

IRgenTable irgen_table;    // 中间代码生成的辅助变量
LLVMIR llvmIR;             // 我们需要在这个变量中生成中间代码

void AddLibFunctionDeclare();

// 在基本块B末尾生成一条新指令
void IRgenArithmeticI32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticF32(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int reg1, int reg2, int result_reg);
void IRgenArithmeticI32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int reg2, int result_reg);
void IRgenArithmeticF32ImmLeft(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, int reg2,
                               int result_reg);
void IRgenArithmeticI32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, int val1, int val2, int result_reg);
void IRgenArithmeticF32ImmAll(LLVMBlock B, BasicInstruction::LLVMIROpcode opcode, float val1, float val2,
                              int result_reg);

void IRgenIcmp(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenFcmp(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, int reg2, int result_reg);
void IRgenIcmpImmRight(LLVMBlock B, BasicInstruction::IcmpCond cmp_op, int reg1, int val2, int result_reg);
void IRgenFcmpImmRight(LLVMBlock B, BasicInstruction::FcmpCond cmp_op, int reg1, float val2, int result_reg);

void IRgenFptosi(LLVMBlock B, int src, int dst);
void IRgenSitofp(LLVMBlock B, int src, int dst);
void IRgenZextI1toI32(LLVMBlock B, int src, int dst);

void IRgenGetElementptrIndexI32(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenGetElementptrIndexI64(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr,
                        std::vector<int> dims, std::vector<Operand> indexs);

void IRgenLoad(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, int value_reg, Operand ptr);
void IRgenStore(LLVMBlock B, BasicInstruction::LLVMType type, Operand value, Operand ptr);

void IRgenCall(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg,
               std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);
void IRgenCallVoid(LLVMBlock B, BasicInstruction::LLVMType type,
                   std::vector<std::pair<enum BasicInstruction::LLVMType, Operand>> args, std::string name);

void IRgenCallNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, int result_reg, std::string name);
void IRgenCallVoidNoArgs(LLVMBlock B, BasicInstruction::LLVMType type, std::string name);

void IRgenRetReg(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenRetImmInt(LLVMBlock B, BasicInstruction::LLVMType type, int val);
void IRgenRetImmFloat(LLVMBlock B, BasicInstruction::LLVMType type, float val);
void IRgenRetVoid(LLVMBlock B);

void IRgenBRUnCond(LLVMBlock B, int dst_label);
void IRgenBrCond(LLVMBlock B, int cond_reg, int true_label, int false_label);

void IRgenAlloca(LLVMBlock B, BasicInstruction::LLVMType type, int reg);
void IRgenAllocaArray(LLVMBlock B, BasicInstruction::LLVMType type, int reg, std::vector<int> dims);

void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src);
RegOperand *GetNewRegOperand(int RegNo);

// generate TypeConverse Instructions from type_src to type_dst
// eg. you can use fptosi instruction to converse float to int
// eg. you can use zext instruction to converse bool to int
void IRgenTypeConverse(LLVMBlock B, Type::ty type_src, Type::ty type_dst, int src) {
    TODO("IRgenTypeConverse. Implement it if you need it");
}

void BasicBlock::InsertInstruction(int pos, Instruction Ins) {
    assert(pos == 0 || pos == 1);
    if (pos == 0) {
        Instruction_list.push_front(Ins);
    } else if (pos == 1) {
        Instruction_list.push_back(Ins);
    }
}

/*
二元运算指令生成的伪代码：
假设现在的语法树节点是：AddExp_plus
该语法树表示 addexp + mulexp

addexp->codeIR()
mulexp->codeIR()
假设mulexp生成完后，我们应该在基本块B0继续插入指令。
addexp的结果存储在r0寄存器中，mulexp的结果存储在r1寄存器中
生成一条指令r2 = r0 + r1，并将该指令插入基本块B0末尾。
标注后续应该在基本块B0插入指令，当前节点的结果寄存器为r2。
(如果考虑支持浮点数，需要查看语法树节点的类型来判断此时是否需要隐式类型转换)
*/

/*
while语句指令生成的伪代码：
while的语法树节点为while(cond)stmt

假设当前我们应该在B0基本块开始插入指令
新建三个基本块Bcond，Bbody，Bend
在B0基本块末尾插入一条无条件跳转指令，跳转到Bcond

设置当前我们应该在Bcond开始插入指令
cond->codeIR()
假设cond生成完后，我们应该在B1基本块继续插入指令，Bcond的结果为r0
如果r0的类型不为bool，在B1末尾生成一条比较语句，比较r0是否为真。
在B1末尾生成一条条件跳转语句，如果为真，跳转到Bbody，如果为假，跳转到Bend

设置当前我们应该在Bbody开始插入指令
stmt->codeIR()
假设当stmt生成完后，我们应该在B2基本块继续插入指令
在B2末尾生成一条无条件跳转语句，跳转到Bcond

设置当前我们应该在Bend开始插入指令
*/

void __Program::codeIR() {
    AddLibFunctionDeclare();
    auto comp_vector = *comp_list;
    for (auto comp : comp_vector) {
        comp->codeIR();
    }
}

void Exp::codeIR() { addexp->codeIR(); }

void AddExp_plus::codeIR() { TODO("BinaryExp CodeIR"); }

void AddExp_sub::codeIR() { TODO("BinaryExp CodeIR"); }

void MulExp_mul::codeIR() { TODO("BinaryExp CodeIR"); }

void MulExp_div::codeIR() { TODO("BinaryExp CodeIR"); }

void MulExp_mod::codeIR() { TODO("BinaryExp CodeIR"); }

void RelExp_leq::codeIR() { TODO("BinaryExp CodeIR"); }

void RelExp_lt::codeIR() { TODO("BinaryExp CodeIR"); }

void RelExp_geq::codeIR() { TODO("BinaryExp CodeIR"); }

void RelExp_gt::codeIR() { TODO("BinaryExp CodeIR"); }

void EqExp_eq::codeIR() { TODO("BinaryExp CodeIR"); }

void EqExp_neq::codeIR() { TODO("BinaryExp CodeIR"); }

// short circuit &&
void LAndExp_and::codeIR() { TODO("LAndExpAnd CodeIR"); }

// short circuit ||
void LOrExp_or::codeIR() { TODO("LOrExpOr CodeIR"); }

void ConstExp::codeIR() { addexp->codeIR(); }

void Lval::codeIR() { TODO("Lval CodeIR"); }

void FuncRParams::codeIR() { TODO("FuncRParams CodeIR"); }

void Func_call::codeIR() { TODO("FunctionCall CodeIR"); }

void UnaryExp_plus::codeIR() { TODO("UnaryExpPlus CodeIR"); }

void UnaryExp_neg::codeIR() { TODO("UnaryExpNeg CodeIR"); }

void UnaryExp_not::codeIR() { TODO("UnaryExpNot CodeIR"); }

void IntConst::codeIR() { TODO("IntConst CodeIR"); }

void FloatConst::codeIR() { TODO("FloatConst CodeIR"); }

void StringConst::codeIR() { TODO("StringConst CodeIR"); }

void PrimaryExp_branch::codeIR() { exp->codeIR(); }

void assign_stmt::codeIR() { TODO("AssignStmt CodeIR"); }

void expr_stmt::codeIR() { exp->codeIR(); }

void block_stmt::codeIR() { TODO("BlockStmt CodeIR"); }

void ifelse_stmt::codeIR() { TODO("IfElseStmt CodeIR"); }

void if_stmt::codeIR() { TODO("IfStmt CodeIR"); }

void while_stmt::codeIR() { TODO("WhileStmt CodeIR"); }

void continue_stmt::codeIR() { TODO("ContinueStmt CodeIR"); }

void break_stmt::codeIR() { TODO("BreakStmt CodeIR"); }

void return_stmt::codeIR() { TODO("ReturnStmt CodeIR"); }

void return_stmt_void::codeIR() { TODO("ReturnStmtVoid CodeIR"); }

void ConstInitVal::codeIR() { TODO("ConstInitVal CodeIR"); }

void ConstInitVal_exp::codeIR() { TODO("ConstInitValWithExp CodeIR"); }

void VarInitVal::codeIR() { TODO("VarInitVal CodeIR"); }

void VarInitVal_exp::codeIR() { TODO("VarInitValWithExp CodeIR"); }

void VarDef_no_init::codeIR() { TODO("VarDefNoInit CodeIR"); }

void VarDef::codeIR() { TODO("VarDef CodeIR"); }

void ConstDef::codeIR() { TODO("ConstDef CodeIR"); }

void VarDecl::codeIR() { TODO("VarDecl CodeIR"); }

void ConstDecl::codeIR() { TODO("ConstDecl CodeIR"); }

void BlockItem_Decl::codeIR() { TODO("BlockItemDecl CodeIR"); }

void BlockItem_Stmt::codeIR() { TODO("BlockItemStmt CodeIR"); }

void __Block::codeIR() { TODO("Block CodeIR"); }

void __FuncFParam::codeIR() { TODO("FunctionFParam CodeIR"); }

void __FuncDef::codeIR() { TODO("FunctionDef CodeIR"); }

void CompUnit_Decl::codeIR() { TODO("CompUnitDecl CodeIR"); }

void CompUnit_FuncDef::codeIR() { func_def->codeIR(); }

void AddLibFunctionDeclare() {
    FunctionDeclareInstruction *getint = new FunctionDeclareInstruction(BasicInstruction::I32, "getint");
    llvmIR.function_declare.push_back(getint);

    FunctionDeclareInstruction *getchar = new FunctionDeclareInstruction(BasicInstruction::I32, "getch");
    llvmIR.function_declare.push_back(getchar);

    FunctionDeclareInstruction *getfloat = new FunctionDeclareInstruction(BasicInstruction::FLOAT32, "getfloat");
    llvmIR.function_declare.push_back(getfloat);

    FunctionDeclareInstruction *getarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getarray");
    getarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getarray);

    FunctionDeclareInstruction *getfloatarray = new FunctionDeclareInstruction(BasicInstruction::I32, "getfarray");
    getfloatarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(getfloatarray);

    FunctionDeclareInstruction *putint = new FunctionDeclareInstruction(BasicInstruction::VOID, "putint");
    putint->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putint);

    FunctionDeclareInstruction *putch = new FunctionDeclareInstruction(BasicInstruction::VOID, "putch");
    putch->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(putch);

    FunctionDeclareInstruction *putfloat = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfloat");
    putfloat->InsertFormal(BasicInstruction::FLOAT32);
    llvmIR.function_declare.push_back(putfloat);

    FunctionDeclareInstruction *putarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putarray");
    putarray->InsertFormal(BasicInstruction::I32);
    putarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putarray);

    FunctionDeclareInstruction *putfarray = new FunctionDeclareInstruction(BasicInstruction::VOID, "putfarray");
    putfarray->InsertFormal(BasicInstruction::I32);
    putfarray->InsertFormal(BasicInstruction::PTR);
    llvmIR.function_declare.push_back(putfarray);

    FunctionDeclareInstruction *starttime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_starttime");
    starttime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(starttime);

    FunctionDeclareInstruction *stoptime = new FunctionDeclareInstruction(BasicInstruction::VOID, "_sysy_stoptime");
    stoptime->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(stoptime);

    // 一些llvm自带的函数，也许会为你的优化提供帮助
    FunctionDeclareInstruction *llvm_memset =
    new FunctionDeclareInstruction(BasicInstruction::VOID, "llvm.memset.p0.i32");
    llvm_memset->InsertFormal(BasicInstruction::PTR);
    llvm_memset->InsertFormal(BasicInstruction::I8);
    llvm_memset->InsertFormal(BasicInstruction::I32);
    llvm_memset->InsertFormal(BasicInstruction::I1);
    llvmIR.function_declare.push_back(llvm_memset);

    FunctionDeclareInstruction *llvm_umax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umax.i32");
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvm_umax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umax);

    FunctionDeclareInstruction *llvm_umin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.umin.i32");
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvm_umin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_umin);

    FunctionDeclareInstruction *llvm_smax = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smax.i32");
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvm_smax->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smax);

    FunctionDeclareInstruction *llvm_smin = new FunctionDeclareInstruction(BasicInstruction::I32, "llvm.smin.i32");
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvm_smin->InsertFormal(BasicInstruction::I32);
    llvmIR.function_declare.push_back(llvm_smin);
}
