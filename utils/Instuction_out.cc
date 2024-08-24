#include "../include/Instruction.h"
#include "../include/basic_block.h"
#include "../include/ir.h"
#include <sstream>

CFG *current_CFG;
extern int output_Physical_reg;

std::ostream &operator<<(std::ostream &s, BasicInstruction::LLVMType type) {
    switch (type) {
    case BasicInstruction::I32:
        s << "i32";
        break;
    case BasicInstruction::FLOAT32:
        s << "float";
        break;
    case BasicInstruction::PTR:
        s << "ptr";
        break;
    case BasicInstruction::VOID:
        s << "void";
        break;
    case BasicInstruction::I8:
        s << "i8";
        break;
    case BasicInstruction::I1:
        s << "i1";
        break;
    case BasicInstruction::I64:
        s << "i64";
        break;
    case BasicInstruction::DOUBLE:
        s << "double";
        break;
    }
    return s;
}
std::ostream &operator<<(std::ostream &s, BasicInstruction::LLVMIROpcode type) {
    switch (type) {
    case BasicInstruction::LOAD:
        s << "load";
        break;
    case BasicInstruction::STORE:
        s << "store";
        break;
    case BasicInstruction::ADD:
        s << "add";
        break;
    case BasicInstruction::SUB:
        s << "sub";
        break;
    case BasicInstruction::ICMP:
        s << "icmp";
        break;
    case BasicInstruction::PHI:
        s << "phi";
        break;
    case BasicInstruction::ALLOCA:
        s << "alloca";
        break;
    case BasicInstruction::MUL:
        s << "mul";
        break;
    case BasicInstruction::DIV:
        s << "sdiv";
        break;
    case BasicInstruction::BR_COND:
        s << "br";
        break;
    case BasicInstruction::BR_UNCOND:
        s << "br";
        break;
    case BasicInstruction::FADD:
        s << "fadd";
        break;
    case BasicInstruction::FSUB:
        s << "fsub";
        break;
    case BasicInstruction::FMUL:
        s << "fmul";
        break;
    case BasicInstruction::FDIV:
        s << "fdiv";
        break;
    case BasicInstruction::FCMP:
        s << "fcmp";
        break;
    case BasicInstruction::MOD:
        s << "srem";
        break;
    case BasicInstruction::BITXOR:
        s << "xor";
        break;
    case BasicInstruction::BITAND:
        s << "and";
        break;
    case BasicInstruction::SHL:
        s << "shl";
        break;
    default:
        ERROR("Unexpected LLVMIROpcode when printing instructions");
    }
    return s;
}
std::ostream &operator<<(std::ostream &s, BasicInstruction::IcmpCond type) {
    switch (type) {
    case BasicInstruction::eq:
        s << "eq";
        break;
    case BasicInstruction::ne:
        s << "ne";
        break;
    case BasicInstruction::ugt:
        s << "ugt";
        break;
    case BasicInstruction::uge:
        s << "uge";
        break;
    case BasicInstruction::ult:
        s << "ult";
        break;
    case BasicInstruction::ule:
        s << "ule";
        break;
    case BasicInstruction::sgt:
        s << "sgt";
        break;
    case BasicInstruction::sge:
        s << "sge";
        break;
    case BasicInstruction::slt:
        s << "slt";
        break;
    case BasicInstruction::sle:
        s << "sle";
        break;
    default:
        ERROR("Unexpected IcmpCond when printing instructions");
    }
    return s;
}
std::ostream &operator<<(std::ostream &s, BasicInstruction::FcmpCond type) {
    switch (type) {
    case BasicInstruction::FALSE:
        s << "false";
        break;
    case BasicInstruction::OEQ:
        s << "oeq";
        break;
    case BasicInstruction::OGT:
        s << "ogt";
        break;
    case BasicInstruction::OGE:
        s << "oge";
        break;
    case BasicInstruction::OLT:
        s << "olt";
        break;
    case BasicInstruction::OLE:
        s << "ole";
        break;
    case BasicInstruction::ONE:
        s << "one";
        break;
    case BasicInstruction::ORD:
        s << "ord";
        break;
    case BasicInstruction::UEQ:
        s << "ueq";
        break;
    case BasicInstruction::UGT:
        s << "ugt";
        break;
    case BasicInstruction::UGE:
        s << "uge";
        break;
    case BasicInstruction::ULT:
        s << "ult";
        break;
    case BasicInstruction::ULE:
        s << "ule";
        break;
    case BasicInstruction::UNE:
        s << "une";
        break;
    case BasicInstruction::UNO:
        s << "uno";
        break;
    case BasicInstruction::TRUE:
        s << "true";
        break;
    default:
        ERROR("Unexpected FcmpCond when printing instructions");
    }
    return s;
}

std::string RegOperand::GetFullName() {
    if (reg_no >= 0) {
        return "%r" + std::to_string(reg_no);
    }
}

std::string ImmI32Operand::GetFullName() { return std::to_string(immVal); }

std::string ImmI64Operand::GetFullName() { return std::to_string(immVal); }

std::string ImmF32Operand::GetFullName() {
    std::stringstream ss;
    ss << "0x" << std::hex << GetFloatByteVal();
    ss << std::dec;
    return ss.str();
}

std::string LabelOperand::GetFullName() { return "%L" + std::to_string(label_no); }

std::string GlobalOperand::GetFullName() { return "@" + name; }

// @Output a operand
std::ostream &operator<<(std::ostream &s, Operand op) {
    s << op->GetFullName();
    return s;
}

void LoadInstruction::PrintIR(std::ostream &s) { s << result << " = load " << type << ", ptr " << pointer << "\n"; }
void StoreInstruction::PrintIR(std::ostream &s) {
    s << "store " << type << " " << value << ", ptr " << pointer << "\n";
}
void ArithmeticInstruction::PrintIR(std::ostream &s) {
    if (opcode == LL_ADDMOD) {
        s << result << " = call i32 @___llvm_ll_add_mod(i32 " << op1 << ",i32 " << op2 << ",i32 " << op3 << ")\n";
        return;
    } else if (opcode == UMIN_I32) {
        s << result << " = call i32 @llvm.umin.i32(i32 " << op1 << ",i32 " << op2 << ")\n";
        return;
    } else if (opcode == UMAX_I32) {
        s << result << " = call i32 @llvm.umax.i32(i32 " << op1 << ",i32 " << op2 << ")\n";
        return;
    } else if (opcode == SMIN_I32) {
        s << result << " = call i32 @llvm.smin.i32(i32 " << op1 << ",i32 " << op2 << ")\n";
        return;
    } else if (opcode == SMAX_I32) {
        s << result << " = call i32 @llvm.smax.i32(i32 " << op1 << ",i32 " << op2 << ")\n";
        return;
    } else if (opcode == FMIN_F32) {
        s << result << " = call float @___llvm_fmin_f32(float " << op1 << ",float " << op2 << ")\n";
        return;
    } else if (opcode == FMAX_F32) {
        s << result << " = call float @___llvm_fmax_f32(float " << op1 << ",float " << op2 << ")\n";
        return;
    }
    s << result << " = " << opcode << " " << type << " " << op1 << "," << op2 << "\n";
}
void IcmpInstruction::PrintIR(std::ostream &s) {
    s << result << " = icmp " << cond << " " << type << " " << op1 << "," << op2 << "\n";
}
void FcmpInstruction::PrintIR(std::ostream &s) {
    s << result << " = fcmp " << cond << " " << type << " " << op1 << "," << op2 << "\n";
}
void PhiInstruction::PrintIR(std::ostream &s) {
    s << result << " = phi " << type << " ";
    for (auto it = phi_list.begin(); it != phi_list.end(); ++it) {
        s << "[" << it->second << "," << it->first << "]";
        auto jt = it;
        if ((++jt) != phi_list.end())
            s << ",";
    }
    s << '\n';
}
void AllocaInstruction::PrintIR(std::ostream &s) {
    s << result << " = alloca ";
    if (dims.empty())
        s << type << "\n";    // 单个变量
    else {
        for (std::vector<int>::iterator it = dims.begin(); it != dims.end(); ++it)
            s << "[" << *it << " x ";    // 高维数组
        s << type << std::string(dims.size(), ']') << "\n";
    }
}
void BrCondInstruction::PrintIR(std::ostream &s) {
    // br i1 <cond>, label <iftrue>, label <iffalse>
    s << "br i1 " << cond << ", label " << trueLabel << ", label " << falseLabel << "\n";
}
void BrUncondInstruction::PrintIR(std::ostream &s) {
    // br label <dest>
    s << "br label " << destLabel << "\n";
}

// Function Definition Syntax:
// define void @DFS(i32 %0,i32 %1){
//   Function Body
// }
void FunctionDefineInstruction::PrintIR(std::ostream &s) {
    // define void @FunctionName
    s << "define " << return_type << " @" << Func_name;

    // Print Parameter List
    // example:(i32 %0,f32 %1)
    s << "(";
    for (uint32_t i = 0; i < formals.size(); ++i) {
        s << formals[i] << " " << formals_reg[i];
        if (i + 1 < formals.size()) {
            s << ",";
        }
    }
    s << ")\n";
}

void FunctionDeclareInstruction::PrintIR(std::ostream &s) {
    // declare void @FunctionName(i32,f32)
    s << "declare " << return_type << " @" << Func_name << "(";
    for (uint32_t i = 0; i < formals.size(); ++i) {
        s << formals[i];
        if (i + 1 < formals.size()) {
            s << ",";
        }
    }
    if (is_more_args) {
        s << ", ...";
    }
    s << ")\n";
}

void BasicBlock::printIR(std::ostream &s) {
    s << "L" << block_id << ":  ;" << comment << "\n";

    for (Instruction ins : Instruction_list) {
        s << "    ";
        ins->PrintIR(s);    // Auto "\n" In Instruction::printIR()
    }
}

void LLVMIR::printIR(std::ostream &s) {
    // output lib func decl
    for (Instruction lib_func_decl : function_declare) {
        lib_func_decl->PrintIR(s);
    }

    // output global
    for (Instruction global_decl_ins : global_def) {
        global_decl_ins->PrintIR(s);
    }

    // output Functions
    for (auto Func_Block_item : function_block_map) {    //<function,<id,block> >
        FuncDefInstruction f = Func_Block_item.first;
        current_CFG = llvm_cfg[f];
        // output function Syntax
        f->PrintIR(s);

        // output Blocks in functions
        s << "{\n";
        for (auto block : Func_Block_item.second) {
            block.second->printIR(s);
        }
        s << "}\n";
    }
}

long long Float_to_Byte(float f) {
    float rawFloat = f;
    unsigned long long rawFloatByte = *((int *)&rawFloat);
    unsigned long long signBit = rawFloatByte >> 31;
    unsigned long long expBits = (rawFloatByte >> 23) & ((1 << 8) - 1);
    unsigned long long part1 = rawFloatByte & ((1 << 23) - 1);

    unsigned long long out_signBit = signBit << 63;
    unsigned long long out_sigBits = part1 << 29;
    unsigned long long expBits_highestBit = (expBits & (1 << 7)) << 3;
    unsigned long long expBits_lowerBit = (expBits & (1 << 7) - 1);
    unsigned long long expBits_lowerBit_highestBit = expBits_lowerBit & (1 << 6);
    unsigned long long expBits_lowerBit_ext = (expBits_lowerBit_highestBit) | (expBits_lowerBit_highestBit << 1) |
                                              (expBits_lowerBit_highestBit << 2) | (expBits_lowerBit_highestBit << 3);
    unsigned long long expBits_full = expBits_highestBit | expBits_lowerBit | expBits_lowerBit_ext;
    unsigned long long out_expBits = expBits_full << 52;
    unsigned long long out_rawFloatByte = out_signBit | out_expBits | out_sigBits;
    /*
        Example: Float Value 114.514

        llvm Double:
            0                               ---1 bit    (sign bit)
            1000 0000 101                   ---11 bits  (exp bits)
            1100 1010 0000 1110 0101 011    ---23 bits  (part 1)
            00000000000000000000000000000   ---29 bits  (part 2 All zero)

        IEEE Float:
            0                               ---1 bit    (sign bit)
            1    0000 101                   ---8 bits   (exp bits)
            1100 1010 0000 1110 0101 011    ---23 bits  (part 1)

    */

    return out_rawFloatByte;
}

long long ImmF32Operand::GetFloatByteVal() { return Float_to_Byte(immVal); }

void recursive_print(std::ostream &s, BasicInstruction::LLVMType type, VarAttribute &v, int dimDph, int beginPos,
                     int endPos) {
    if (dimDph == 0) {
        int allzero = 1;
        if (v.type == 1) {
            for (auto x : v.IntInitVals) {
                if (x != 0) {
                    allzero = 0;
                    break;
                }
            }
        } else {
            for (auto x : v.FloatInitVals) {
                if (x != 0) {
                    allzero = 0;
                    break;
                }
            }
        }
        if (allzero) {
            for (int dim : v.dims) {
                s << "[" << dim << "x ";
            }
            s << type << std::string(v.dims.size(), ']') << " "
              << "zeroinitializer";
            return;
        }
    }
    if (beginPos == endPos) {
        if (type == BasicInstruction::I32) {
            s << type << " " << v.IntInitVals[beginPos];
        } else if (type == BasicInstruction::FLOAT32) {
            s << type << " "
              << "0x" << std::hex << Float_to_Byte(v.FloatInitVals[beginPos]);
            s << std::dec;
        }
        return;
    }
    for (int i = dimDph; i < v.dims.size(); i++) {
        s << "[" << v.dims[i] << " x ";
    }
    s << type << std::string(v.dims.size() - dimDph, ']') << " ";
    s << "[";
    int step = 1;
    for (int i = dimDph + 1; i < v.dims.size(); i++) {
        step *= v.dims[i];
    }
    for (int i = 0; i < v.dims[dimDph]; i++) {
        recursive_print(s, type, v, dimDph + 1, beginPos + i * step, beginPos + (i + 1) * step - 1);
        if (i != v.dims[dimDph] - 1)
            s << ",";    // Not the last element
    }
    s << "]";
}

// Remember "\n"
void GlobalVarDefineInstruction::PrintIR(std::ostream &s) {
    if (arval.dims.empty()) {
        if (init_val != nullptr)
            s << "@" << name << " = global " << type << " " << init_val << "\n";
        else
            s << "@" << name << " = global " << type << " "
              << "zeroinitializer\n";
        return;
    }
    s << "@" << name << " = global ";
    // print type
    // print init_val
    int step = 1;
    for (int i = 0; i < arval.dims.size(); i++) {
        step *= arval.dims[i];
    }
    recursive_print(s, type, arval, 0, 0, step - 1);
    s << "\n";
}

// Remember "\n"
/*
Call Instruction Syntax
Example 1:
    %12 = call i32 (ptr, ...)@printf(ptr @.str,i32 %11)
Example 2:
    call void @DFS(i32 0,i32 %4)
*/
void CallInstruction::PrintIR(std::ostream &s) {
    if (ret_type != LLVMType::VOID) {
        s << result << " = ";
    }
    s << "call " << ret_type << " @" << name;

    // print Parameter List
    s << "(";
    for (std::vector<std::pair<LLVMType, Operand>>::iterator it = args.begin(); it != args.end(); ++it) {
        s << it->first << " " << it->second;
        if (it + 1 != args.end())
            s << ",";
    }
    s << ")\n";
}

// Remember "\n"
void RetInstruction::PrintIR(std::ostream &s) {
    s << "ret " << ret_type;
    if (ret_val != nullptr) {
        s << " " << ret_val;
    }
    s << "\n";
}

void SelectInstruction::PrintIR(std::ostream &s) {
    s << result << " = ";
    s << "select i1 " << cond;
    s << ", " << type << " " << op1;
    s << ", " << type << " " << op2;
    s << "\n";
}

/*
Syntax:
<result> = getelementptr <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
<result> = getelementptr inbounds <ty>, ptr <ptrval>{, [inrange] <ty> <idx>}*
<result> = getelementptr <ty>, <N x ptr> <ptrval>, [inrange] <vector index type> <idx>
*/
void GetElementptrInstruction::PrintIR(std::ostream &s) {
    s << result << " = getelementptr ";
    // print type
    if (dims.empty())
        s << type;
    else {
        for (int dim : dims) {
            s << "[" << dim << " x ";
        }
        s << type;
        s << std::string(dims.size(), ']');
    }

    // print ptrval
    s << ", ptr " << ptrval;
    // print indexes
    for (Operand idx : indexes)
        s << ", i32 " << idx;
    s << "\n";
}

void FptosiInstruction::PrintIR(std::ostream &s) {
    s << result << " = fptosi float"
      << " " << value << " to "
      << "i32"
      << "\n";
}

void FpextInstruction::PrintIR(std::ostream &s) {
    s << result << " = fpext float"
      << " " << value << " to "
      << "double"
      << "\n";
}

void SitofpInstruction::PrintIR(std::ostream &s) {
    s << result << " = sitofp i32"
      << " " << value << " to "
      << "float"
      << "\n";
}

void GlobalStringConstInstruction::PrintIR(std::ostream &s) {
    int str_len = str_val.size() + 1;
    for (char c : str_val) {
        if (c == '\\')
            str_len--;
    }
    s << "@" << str_name << " = private unnamed_addr constant [" << str_len << " x i8] c\"";
    for (int i = 0; i < str_val.size(); i++) {
        char c = str_val[i];
        if (c == '\\') {
            i++;
            c = str_val[i];
            if (c == 'n')
                s << "\\0A";
            else if (c == 't')
                s << "\\09";
            else if (c == '\"')
                s << "\\22";
            else if (c == 'r')
                s << "\\0D";
            else if (c == 'b')
                s << "\\08";
            else if (c == 'f')
                s << "\\0C";
            else if (c == 'v')
                s << "\\0B";
            else if (c == 'a')
                s << "\\07";
            else if (c == '?')
                s << "\?";
            else if (c == '0')
                s << "\\00";
            else if (c == '\'')
                s << "\'";
            else if (c == '\\')
                s << "\\\\";
            else
                s << c;
        } else
            s << c;
    }
    s << "\\00"
      << "\"\n";
}

void ZextInstruction::PrintIR(std::ostream &s) {
    s << result << " = zext " << from_type << " " << value << " to " << to_type << "\n";
}

void BitCastInstruction::PrintIR(std::ostream &s) {
    s << dst << " = bitcast " << src_type << " " << src << " to " << dst_type << "\n";
}