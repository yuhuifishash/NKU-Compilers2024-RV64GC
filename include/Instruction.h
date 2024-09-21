#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include "symtab.h"
#include <assert.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#ifndef ERROR
#define ERROR(...)                                                                                                     \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;31;1m";                                                                                    \
        std::cerr << "ERROR: ";                                                                                        \
        std::cerr << "\033[0;37;1m";                                                                                   \
        std::cerr << message << std::endl;                                                                             \
        std::cerr << "\033[0;33;1m";                                                                                   \
        std::cerr << "File: \033[4;37;1m" << __FILE__ << ":" << __LINE__ << std::endl;                                 \
        std::cerr << "\033[0m";                                                                                        \
        assert(false);                                                                                                 \
    } while (0)
#endif

#define ENABLE_TODO
#ifdef ENABLE_TODO
#ifndef TODO
#define TODO(...)                                                                                                      \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;34;1m";                                                                                    \
        std::cerr << "TODO: ";                                                                                         \
        std::cerr << "\033[0;37;1m";                                                                                   \
        std::cerr << message << std::endl;                                                                             \
        std::cerr << "\033[0;33;1m";                                                                                   \
        std::cerr << "File: \033[4;37;1m" << __FILE__ << ":" << __LINE__ << std::endl;                                 \
        std::cerr << "\033[0m";                                                                                        \
        assert(false);                                                                                                 \
    } while (0)
#endif
#else
#ifndef TODO
#define TODO(...)
#endif
#endif

#define ENABLE_LOG
#ifdef ENABLE_LOG
#ifndef Log
#define Log(...)                                                                                                       \
    do {                                                                                                               \
        char message[256];                                                                                             \
        sprintf(message, __VA_ARGS__);                                                                                 \
        std::cerr << "\033[;35;1m[\033[4;33;1m" << __FILE__ << ":" << __LINE__ << "\033[;35;1m "                       \
                  << __PRETTY_FUNCTION__ << "]";                                                                       \
        std::cerr << "\033[0;37;1m ";                                                                                  \
        std::cerr << message << std::endl;                                                                             \
        std::cerr << "\033[0m";                                                                                        \
    } while (0)
#endif
#else
#ifndef Log
#define Log(...)
#endif
#endif

#ifndef Assert
#define Assert(EXP)                                                                                                    \
    do {                                                                                                               \
        if (!(EXP)) {                                                                                                  \
            ERROR("Assertion failed: %s", #EXP);                                                                       \
        }                                                                                                              \
    } while (0)
#endif

// 你可能需要额外添加你需要的指令类

// TODO(): 在代码优化阶段，加入更多你需要的成员变量和函数
// TODO(): 在代码优化阶段，加入更多你需要的指令

// 我们规定，对于GlobalOperand, LabelOperand, RegOperand, 只要操作数相同, 地址也相同
// 所以这些Operand的构造函数是private, 使用GetNew***Operand函数来获取新的操作数变量
// 请注意代码中的typedef，为了方便书写，将一些类的指针进行了重命名, 如果不习惯该种风格，可以自行修改


class BasicOperand;
typedef BasicOperand *Operand;
// @operands in instruction
class BasicOperand {
public:
    enum operand_type { REG = 1, IMMI32 = 2, IMMF32 = 3, GLOBAL = 4, LABEL = 5, IMMI64 = 6 };

protected:
    operand_type operandType;

public:
    BasicOperand() {}
    operand_type GetOperandType() { return operandType; }
    virtual std::string GetFullName() = 0;
};

// @register operand;%r+register No
class RegOperand : public BasicOperand {
    int reg_no;
    RegOperand(int RegNo) {
        this->operandType = REG;
        this->reg_no = RegNo;
    }

public:
    int GetRegNo() { return reg_no; }

    friend RegOperand *GetNewRegOperand(int RegNo);
    virtual std::string GetFullName();
};
RegOperand *GetNewRegOperand(int RegNo);

// @integer32 immediate
class ImmI32Operand : public BasicOperand {
    int immVal;

public:
    int GetIntImmVal() { return immVal; }

    ImmI32Operand(int immVal) {
        this->operandType = IMMI32;
        this->immVal = immVal;
    }
    virtual std::string GetFullName();
};

// @integer64 immediate
class ImmI64Operand : public BasicOperand {
    long long immVal;

public:
    long long GetLlImmVal() { return immVal; }

    ImmI64Operand(long long immVal) {
        this->operandType = IMMI64;
        this->immVal = immVal;
    }
    virtual std::string GetFullName();
};

// @float32 immediate
class ImmF32Operand : public BasicOperand {
    float immVal;

public:
    float GetFloatVal() { return immVal; }

    long long GetFloatByteVal();

    ImmF32Operand(float immVal) {
        this->operandType = IMMF32;
        this->immVal = immVal;
    }
    virtual std::string GetFullName();
};

// @label %L+label No
class LabelOperand : public BasicOperand {
    int label_no;
    LabelOperand(int LabelNo) {
        this->operandType = LABEL;
        this->label_no = LabelNo;
    }

public:
    int GetLabelNo() { return label_no; }

    friend LabelOperand *GetNewLabelOperand(int LabelNo);
    virtual std::string GetFullName();
};

LabelOperand *GetNewLabelOperand(int RegNo);

// @global identifier @+name
class GlobalOperand : public BasicOperand {
    std::string name;
    GlobalOperand(std::string gloName) {
        this->operandType = GLOBAL;
        this->name = gloName;
    }

public:
    std::string GetName() { return name; }

    friend GlobalOperand *GetNewGlobalOperand(std::string name);
    virtual std::string GetFullName();
};

GlobalOperand *GetNewGlobalOperand(std::string name);

class BasicInstruction;

typedef BasicInstruction *Instruction;

// @instruction
class BasicInstruction {
public:
    // @Instriction types
    enum LLVMIROpcode {
        OTHER = 0,
        LOAD = 1,
        STORE = 2,
        ADD = 3,
        SUB = 4,
        ICMP = 5,
        PHI = 6,
        ALLOCA = 7,
        MUL = 8,
        DIV = 9,
        BR_COND = 10,
        BR_UNCOND = 11,
        FADD = 12,
        FSUB = 13,
        FMUL = 14,
        FDIV = 15,
        FCMP = 16,
        MOD = 17,
        BITXOR = 18,
        RET = 19,
        ZEXT = 20,
        SHL = 21,
        FPTOSI = 24,
        GETELEMENTPTR = 25,
        CALL = 26,
        SITOFP = 27,
        GLOBAL_VAR = 28,
        GLOBAL_STR = 29,
    };

    // @Operand datatypes
    enum LLVMType { I32 = 1, FLOAT32 = 2, PTR = 3, VOID = 4, I8 = 5, I1 = 6, I64 = 7, DOUBLE = 8 };

    // @ <cond> in icmp Instruction
    enum IcmpCond {
        eq = 1,     //: equal
        ne = 2,     //: not equal
        ugt = 3,    //: unsigned greater than
        uge = 4,    //: unsigned greater or equal
        ult = 5,    //: unsigned less than
        ule = 6,    //: unsigned less or equal
        sgt = 7,    //: signed greater than
        sge = 8,    //: signed greater or equal
        slt = 9,    //: signed less than
        sle = 10    //: signed less or equal
    };

    enum FcmpCond {
        FALSE = 1,    //: no comparison, always returns false
        OEQ = 2,      // ordered and equal
        OGT = 3,      //: ordered and greater than
        OGE = 4,      //: ordered and greater than or equal
        OLT = 5,      //: ordered and less than
        OLE = 6,      //: ordered and less than or equal
        ONE = 7,      //: ordered and not equal
        ORD = 8,      //: ordered (no nans)
        UEQ = 9,      //: unordered or equal
        UGT = 10,     //: unordered or greater than
        UGE = 11,     //: unordered or greater than or equal
        ULT = 12,     //: unordered or less than
        ULE = 13,     //: unordered or less than or equal
        UNE = 14,     //: unordered or not equal
        UNO = 15,     //: unordered (either nans)
        TRUE = 16     //: no comparison, always returns true
    };

private:
    int BlockID = 0;

protected:
    LLVMIROpcode opcode;
    int insNo;

public:
    int GetBlockID() { return BlockID; }
    void SetBlockID(int blockno) { BlockID = blockno; }
    int GetOpcode() { return opcode; }    // one solution: convert to pointer of subclasses

    virtual void PrintIR(std::ostream &s) = 0;
};

// load
// Syntax: <result>=load <ty>, ptr <pointer>
class LoadInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand pointer;
    Operand result;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetPointer() { return pointer; }
    void SetPointer(Operand op) { pointer = op; }
    Operand GetResult() { return result; }

    LoadInstruction(enum LLVMType type, Operand pointer, Operand result) {
        opcode = LLVMIROpcode::LOAD;
        this->type = type;
        this->result = result;
        this->pointer = pointer;
    }
    void PrintIR(std::ostream &s);
};

// store
// Syntax: store <ty> <value>, ptr<pointer>
class StoreInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand pointer;
    Operand value;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetPointer() { return pointer; }
    Operand GetValue() { return value; }
    void SetValue(Operand op) { value = op; }
    void SetPointer(Operand op) { pointer = op; }

    StoreInstruction(enum LLVMType type, Operand pointer, Operand value) {
        opcode = LLVMIROpcode::STORE;
        this->type = type;
        this->pointer = pointer;
        this->value = value;
    }

    virtual void PrintIR(std::ostream &s);
};

//<result>=add <ty> <op1>,<op2>
//<result>=sub <ty> <op1>,<op2>
//<result>=mul <ty> <op1>,<op2>
//<result>=div <ty> <op1>,<op2>
class ArithmeticInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand op1;
    Operand op2;
    Operand result;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetOperand1() { return op1; }
    Operand GetOperand2() { return op2; }
    Operand GetResult() { return result; }
    void SetOperand1(Operand op) { op1 = op; }
    void SetOperand2(Operand op) { op2 = op; }
    void SetResultReg(Operand op) { result = op; }
    void Setopcode(LLVMIROpcode id) { opcode = id; }
    ArithmeticInstruction(LLVMIROpcode opcode, enum LLVMType type, Operand op1, Operand op2, Operand result) {
        this->opcode = opcode;
        this->op1 = op1;
        this->op2 = op2;
        this->result = result;
        this->type = type;
    }

    virtual void PrintIR(std::ostream &s);
};

//<result>=icmp <cond> <ty> <op1>,<op2>
class IcmpInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand op1;
    Operand op2;
    IcmpCond cond;
    Operand result;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetOp1() { return op1; }
    Operand GetOp2() { return op2; }
    IcmpCond GetCond() { return cond; }
    Operand GetResult() { return result; }
    void SetOp1(Operand op) { op1 = op; }
    void SetOp2(Operand op) { op2 = op; }
    void SetCond(IcmpCond newcond) { cond = newcond; }

    IcmpInstruction(enum LLVMType type, Operand op1, Operand op2, IcmpCond cond, Operand result) {
        this->opcode = LLVMIROpcode::ICMP;
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
        this->cond = cond;
        this->result = result;
    }
    virtual void PrintIR(std::ostream &s);
};

//<result>=fcmp <ty> <op1>,<op2>
class FcmpInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand op1;
    Operand op2;
    FcmpCond cond;
    Operand result;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetOp1() { return op1; }
    Operand GetOp2() { return op2; }
    FcmpCond GetCond() { return cond; }
    Operand GetResult() { return result; }

    FcmpInstruction(enum LLVMType type, Operand op1, Operand op2, FcmpCond cond, Operand result) {
        this->opcode = LLVMIROpcode::FCMP;
        this->type = type;
        this->op1 = op1;
        this->op2 = op2;
        this->cond = cond;
        this->result = result;
    }
    virtual void PrintIR(std::ostream &s);
};

// phi syntax:
//<result>=phi <ty> [val1,label1],[val2,label2],……
class PhiInstruction : public BasicInstruction {
private:
    enum LLVMType type;
    Operand result;
    std::vector<std::pair<Operand, Operand>> phi_list;

public:
    PhiInstruction(enum LLVMType type, Operand result, decltype(phi_list) val_labels) {
        this->opcode = LLVMIROpcode::PHI;
        this->type = type;
        this->result = result;
        this->phi_list = val_labels;
    }
    PhiInstruction(enum LLVMType type, Operand result) {
        this->opcode = LLVMIROpcode::PHI;
        this->type = type;
        this->result = result;
    }
    virtual void PrintIR(std::ostream &s);
};

// alloca
// usage 1: <result>=alloca <type>
// usage 2: %3 = alloca [20 x [20 x i32]]
class AllocaInstruction : public BasicInstruction {
    enum LLVMType type;
    Operand result;
    std::vector<int> dims;

public:
    enum LLVMType GetDataType() { return type; }
    Operand GetResult() { return result; }
    std::vector<int> GetDims() { return dims; }
    AllocaInstruction(enum LLVMType dttype, Operand result) {
        this->opcode = LLVMIROpcode::ALLOCA;
        this->type = dttype;
        this->result = result;
    }
    AllocaInstruction(enum LLVMType dttype, std::vector<int> ArrDims, Operand result) {
        this->opcode = LLVMIROpcode::ALLOCA;
        this->type = dttype;
        this->result = result;
        dims = ArrDims;
    }

    virtual void PrintIR(std::ostream &s);
};

// Conditional branch
// Syntax:
// br i1 <cond>, label <iftrue>, label <iffalse>
class BrCondInstruction : public BasicInstruction {
    Operand cond;
    Operand trueLabel;
    Operand falseLabel;

public:
    Operand GetCond() { return cond; }
    Operand GetTrueLabel() { return trueLabel; }
    Operand GetFalseLabel() { return falseLabel; }
    BrCondInstruction(Operand cond, Operand trueLabel, Operand falseLabel) {
        this->opcode = BR_COND;
        this->cond = cond;
        this->trueLabel = trueLabel;
        this->falseLabel = falseLabel;
    }

    virtual void PrintIR(std::ostream &s);
};

// Unconditional branch
// Syntax:
// br label <dest>
class BrUncondInstruction : public BasicInstruction {
    Operand destLabel;

public:
    Operand GetDestLabel() { return destLabel; }
    BrUncondInstruction(Operand destLabel) {
        this->opcode = BR_UNCOND;
        this->destLabel = destLabel;
    }
    virtual void PrintIR(std::ostream &s);
};

/*
Global Id Define Instruction Syntax
Example 1:
    @p = global [105 x i32] zeroinitializer
Example 2:
    @.str = constant [4 x i8] c"%d \00", align 1
Example 3:
    @p = global [105 x [104 x i32]] [[104 x i32] [], [104 x i32] zeroinitializer, ...]
*/
class GlobalVarDefineInstruction : public BasicInstruction {
    // Datas About the Instruction
public:
    // Construction Function:Set All datas
    // Getters
    enum LLVMType type;
    std::string name;
    Operand init_val;
    VarAttribute arrayval;
    GlobalVarDefineInstruction(std::string nam, enum LLVMType typ, Operand i_val)
        : name(nam), type(typ), init_val(i_val) {
        this->opcode = LLVMIROpcode::GLOBAL_VAR;
    }
    GlobalVarDefineInstruction(std::string nam, enum LLVMType typ, VarAttribute v)
        : name(nam), type(typ), arrayval(v), init_val{nullptr} {
        this->opcode = LLVMIROpcode::GLOBAL_VAR;
    }
    virtual void PrintIR(std::ostream &s);
};

class GlobalStringConstInstruction : public BasicInstruction {
public:
    std::string str_val;
    std::string str_name;
    GlobalStringConstInstruction(std::string strval, std::string strname) : str_val(strval), str_name(strname) {
        this->opcode = LLVMIROpcode::GLOBAL_STR;
    }

    virtual void PrintIR(std::ostream &s);
};

/*
Call Instruction Syntax
Example 1:
    %12 = call i32 (ptr, ...)@printf(ptr @.str,i32 %11)
Example 2:
    call void @DFS(i32 0,i32 %4)
*/
class CallInstruction : public BasicInstruction {
    // Datas About the Instruction
private:
    enum LLVMType ret_type;
    Operand result;    // result can be null
    std::string name;
    std::vector<std::pair<enum LLVMType, Operand>> args;

public:
    // Construction Function:Set All datas
    CallInstruction(enum LLVMType retType, Operand res, std::string FuncName,
                    std::vector<std::pair<enum LLVMType, Operand>> arguments)
        : ret_type(retType), result(res), name(FuncName), args(arguments) {
        this->opcode = CALL;
        if (res != NULL && res->GetOperandType() == BasicOperand::REG) {
            if (((RegOperand *)res)->GetRegNo() == -1) {
                result = NULL;
            }
        }
    }
    CallInstruction(enum LLVMType retType, Operand res, std::string FuncName)
        : ret_type(retType), result(res), name(FuncName) {
        this->opcode = CALL;
        if (res != NULL && res->GetOperandType() == BasicOperand::REG) {
            if (((RegOperand *)res)->GetRegNo() == -1) {
                result = NULL;
            }
        }
    }

    std::string GetFunctionName() { return name; }
    void SetFunctionName(std::string new_name) { name = new_name; }
    std::vector<std::pair<enum LLVMType, Operand>> GetParameterList() { return args; }
    void push_back_Parameter(std::pair<enum LLVMType, Operand> newPara) { args.push_back(newPara); }
    void push_back_Parameter(enum LLVMType type, Operand val) { args.push_back(std::make_pair(type, val)); }
    virtual void PrintIR(std::ostream &s);
};

/*
Ret Instruction Syntax
Example 1:
    ret i32 0
Example 2:
    ret void
Example 3:
    ret i32 %r7
*/
class RetInstruction : public BasicInstruction {
    // Datas About the Instruction
private:
    enum LLVMType ret_type;
    Operand ret_val;

public:
    // Construction Function:Set All datas
    RetInstruction(enum LLVMType retType, Operand res) : ret_type(retType), ret_val(res) { this->opcode = RET; }
    // Getters
    enum LLVMType GetType() { return ret_type; }
    Operand GetRetVal() { return ret_val; }

    virtual void PrintIR(std::ostream &s);
};

/*
Syntax:
<result> = getelementptr <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
<result> = getelementptr inbounds <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
<result> = getelementptr <ty>, <N x ptr> <ptrval>, [inrange] <vector index type> <idx>
*/
class GetElementptrInstruction : public BasicInstruction {
private:
    enum LLVMType type;
    Operand result;
    Operand ptrval;

    std::vector<int> dims;
    std::vector<Operand> indexes;

public:
    GetElementptrInstruction(enum LLVMType typ, Operand res, Operand ptr) : type(typ), result(res), ptrval(ptr) {
        opcode = GETELEMENTPTR;
    }
    GetElementptrInstruction(enum LLVMType typ, Operand res, Operand ptr, std::vector<int> dim)
        : type(typ), result(res), ptrval(ptr), dims(dim) {
        opcode = GETELEMENTPTR;
    }
    GetElementptrInstruction(enum LLVMType typ, Operand res, Operand ptr, std::vector<int> dim,
                             std::vector<Operand> index)
        : type(typ), result(res), ptrval(ptr), dims(dim), indexes(index) {
        opcode = GETELEMENTPTR;
    }
    // get_elementptr_Instruction(enum llvm_type typ,operand res,operand
    // ptr,std::vector<int>dim,std::vector<int>idx):type(typ),result(res),ptrval(ptr),dims(dim),indexes(idx){}
    void push_dim(int d) { dims.push_back(d); }
    void push_idx_reg(int idx_reg_no) { indexes.push_back(GetNewRegOperand(idx_reg_no)); }
    void push_idx_imm32(int imm_idx) { indexes.push_back(new ImmI32Operand(imm_idx)); }
    void push_index(Operand idx) { indexes.push_back(idx); }
    void change_index(int i, Operand op) { indexes[i] = op; }

    enum LLVMType GetType() { return type; }
    Operand GetResult() { return result; }
    void SetResult(Operand op) { result = op; }
    Operand GetPtrVal() { return ptrval; }
    std::vector<int> GetDims() { return dims; }
    std::vector<Operand> GetIndexes() { return indexes; }

    void PrintIR(std::ostream &s);
};

class FunctionDefineInstruction : public BasicInstruction {
private:
    enum LLVMType return_type;
    std::string Func_name;

public:
    std::vector<enum LLVMType> formals;
    std::vector<Operand> formals_reg;
    FunctionDefineInstruction(enum LLVMType t, std::string n) {
        return_type = t;
        Func_name = n;
    }
    void InsertFormal(enum LLVMType t) {
        formals.push_back(t);
        formals_reg.push_back(GetNewRegOperand(formals_reg.size()));
    }
    enum LLVMType GetReturnType() { return return_type; }
    std::string GetFunctionName() { return Func_name; }

    void PrintIR(std::ostream &s);
};
typedef FunctionDefineInstruction *FuncDefInstruction;

class FunctionDeclareInstruction : public BasicInstruction {
private:
    enum LLVMType return_type;
    std::string Func_name;

public:
    std::vector<enum LLVMType> formals;
    FunctionDeclareInstruction(enum LLVMType t, std::string n) {
        return_type = t;
        Func_name = n;
    }
    void InsertFormal(enum LLVMType t) { formals.push_back(t); }
    enum LLVMType GetReturnType() { return return_type; }
    std::string GetFunctionName() { return Func_name; }

    void PrintIR(std::ostream &s);
};

class FptosiInstruction : public BasicInstruction {
private:
    Operand result;
    Operand value;

public:
    FptosiInstruction(Operand result_receiver, Operand value_for_cast)
        : result(result_receiver), value(value_for_cast) {
        this->opcode = FPTOSI;
    }
    Operand GetResult() { return result; }
    Operand GetSrc() { return value; }
    void PrintIR(std::ostream &s);
};

class SitofpInstruction : public BasicInstruction {
private:
    Operand result;
    Operand value;

public:
    SitofpInstruction(Operand result_receiver, Operand value_for_cast)
        : result(result_receiver), value(value_for_cast) {
        this->opcode = SITOFP;
    }

    Operand GetResult() { return result; }
    Operand GetSrc() { return value; }
    void PrintIR(std::ostream &s);
};

class ZextInstruction : public BasicInstruction {
private:
    LLVMType from_type;
    LLVMType to_type;
    Operand result;
    Operand value;

public:
    Operand GetResult() { return result; }
    Operand GetSrc() { return value; }
    ZextInstruction(LLVMType to_type, Operand result_receiver, LLVMType from_type, Operand value_for_cast)
        : to_type(to_type), result(result_receiver), from_type(from_type), value(value_for_cast) {
        this->opcode = ZEXT;
    }
    void PrintIR(std::ostream &s);
};

std::ostream &operator<<(std::ostream &s, BasicInstruction::LLVMType type);
std::ostream &operator<<(std::ostream &s, BasicInstruction::LLVMIROpcode type);
std::ostream &operator<<(std::ostream &s, BasicInstruction::IcmpCond type);
std::ostream &operator<<(std::ostream &s, BasicInstruction::FcmpCond type);
std::ostream &operator<<(std::ostream &s, Operand op);
#endif
