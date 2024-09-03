#include "riscv64_printer.h"
#include <assert.h>

const bool print_comment = true;
// const bool print_comment = false;

bool isMemFormatOp(int opcode) {
    return opcode == RISCV_LB || opcode == RISCV_LBU || opcode == RISCV_LH || opcode == RISCV_LHU ||
           opcode == RISCV_LW || opcode == RISCV_LWU || opcode == RISCV_LD || opcode == RISCV_FLW ||
           opcode == RISCV_FLD || opcode == RISCV_FSW || opcode == RISCV_FSD;
}

template <> void RiscV64Printer::printRVfield<Register *>(Register *printee) {
    if (!printee->is_virtual) {
        s << RiscV64Registers[printee->reg_no].name;
    } else {
        s << "%" << printee->reg_no;
    }
}

template <> void RiscV64Printer::printRVfield<Register>(Register printee) {
    if (!printee.is_virtual) {
        s << RiscV64Registers[printee.reg_no].name;
    } else {
        s << "%" << printee.reg_no;
    }
}

template <> void RiscV64Printer::printRVfield<RiscVLabel *>(RiscVLabel *ins) {
    if (ins->is_data_address) {
        // Just an example
        // May change in future
        // s << ".LPIC" << ins->mem_label_id;
        if (ins->is_hi) {
            s << "%hi(" << ins->name << ")";
        } else {
            s << "%lo(" << ins->name << ")";
        }
    } else {
        s << "." << current_func->getFunctionName() << "_" << ins->jmp_label_id;
    }
}

template <> void RiscV64Printer::printRVfield<RiscVLabel>(RiscVLabel ins) {
    if (ins.is_data_address) {
        // Just an example
        // May change in future
        // s << ".LPIC" << ins->mem_label_id;
        if (ins.is_hi) {
            s << "%hi(" << ins.name << ")";
        } else {
            s << "%lo(" << ins.name << ")";
        }
    } else {
        s << "." << current_func->getFunctionName() << "_" << ins.jmp_label_id;
    }
}

template <> void RiscV64Printer::printRVfield<MachineBaseOperand *>(MachineBaseOperand *op) {
    if (op->op_type == MachineBaseOperand::REG) {
        auto reg_op = (MachineRegister *)op;
        printRVfield(reg_op->reg);
    } else if (op->op_type == MachineBaseOperand::IMMI) {
        auto immi_op = (MachineImmediateInt *)op;
        s << immi_op->imm32;
    } else if (op->op_type == MachineBaseOperand::IMMF) {
        auto immf_op = (MachineImmediateFloat *)op;
        s << immf_op->fimm32;
    }
}

template <> void RiscV64Printer::printAsm<MachineBaseInstruction *>(MachineBaseInstruction *ins);
template <> void RiscV64Printer::printAsm<RiscV64Instruction *>(RiscV64Instruction *ins) {
    s << OpTable[ins->getOpcode()].name << "\t\t";
    if (strlen(OpTable[ins->getOpcode()].name) <= 3) {
        s << "\t";
    }
    switch (OpTable[ins->getOpcode()].ins_formattype) {
    case RvOpInfo::R_type:
        printRVfield(ins->getRd());
        s << ",";
        printRVfield(ins->getRs1());
        s << ",";
        printRVfield(ins->getRs2());
        break;
    case RvOpInfo::R2_type:
        printRVfield(ins->getRd());
        s << ",";
        printRVfield(ins->getRs1());
        if (ins->getOpcode() == RISCV_FCVT_W_S || ins->getOpcode() == RISCV_FCVT_WU_S) {
            s << ",rtz";
        }
        break;
    case RvOpInfo::R4_type:
        printRVfield(ins->getRd());
        s << ",";
        printRVfield(ins->getRs1());
        s << ",";
        printRVfield(ins->getRs2());
        s << ",";
        printRVfield(ins->getRs3());
        break;
    case RvOpInfo::I_type:
        printRVfield(ins->getRd());
        s << ",";
        if (!isMemFormatOp(ins->getOpcode())) {
            printRVfield(ins->getRs1());
            s << ",";
            if (ins->getUseLabel()) {
                printRVfield(ins->getLabel());
            } else {
                s << ins->getImm();
            }
        } else {
            if (ins->getUseLabel()) {
                printRVfield(ins->getLabel());
            } else {
                s << ins->getImm();
            }
            s << "(";
            printRVfield(ins->getRs1());
            s << ")";
        }
        break;
    case RvOpInfo::S_type:
        printRVfield(ins->getRs1());
        s << ",";
        if (ins->getUseLabel()) {
            printRVfield(ins->getLabel());
        } else {
            s << ins->getImm();
        }
        s << "(";
        printRVfield(ins->getRs2());
        s << ")";
        break;
    case RvOpInfo::B_type:
        printRVfield(ins->getRs1());
        s << ",";
        printRVfield(ins->getRs2());
        s << ",";
        if (ins->getUseLabel()) {
            printRVfield(ins->getLabel());
        } else {
            s << ins->getImm();
        }
        break;
    case RvOpInfo::U_type:
        printRVfield(ins->getRd());
        s << ",";
        if (ins->getUseLabel()) {
            printRVfield(ins->getLabel());
        } else {
            s << ins->getImm();
        }
        break;
    case RvOpInfo::J_type:
        printRVfield(ins->getRd());
        s << ",";
        if (ins->getUseLabel()) {
            printRVfield(ins->getLabel());
        } else {
            s << ins->getImm();
        }
        break;
    case RvOpInfo::CALL_type:
        s << ins->getLabel().name;
        break;
    case RvOpInfo::BCC_type:
        printRVfield(ins->getRs1());
        s << ",";
        printRVfield(ins->getRs2());
        s << ",1f; ";
        printAsm(ins->GetSubInstruction());
        s << "; 1: ";
        break;
    default:
        ERROR("Unexpected instruction format");
    }
}

template <> void RiscV64Printer::printAsm<MachineCopyInstruction *>(MachineCopyInstruction *ins) {
    printRVfield(ins->GetDst());
    s << " = COPY ";
    printRVfield(ins->GetSrc());
    s << ", " << ins->GetCopyType().toString();
}

template <> void RiscV64Printer::printAsm<MachinePhiInstruction *>(MachinePhiInstruction *ins) {
    // Lazy("Phi Output");
    // s << "# ";
    printRVfield(ins->GetResult());
    s << " = " << ins->GetResult().type.toString() << " PHI ";
    for (auto [label, op] : ins->GetPhiList()) {
        s << "[";
        printRVfield(op);
        s << ",%L" << label;
        s << "] ";
    }
}

template <> void RiscV64Printer::printAsm<MachineBaseInstruction *>(MachineBaseInstruction *ins) {
    if (ins->arch == MachineBaseInstruction::RiscV) {
        printAsm<RiscV64Instruction *>((RiscV64Instruction *)ins);
        return;
    } else if (ins->arch == MachineBaseInstruction::COPY) {
        printAsm<MachineCopyInstruction *>((MachineCopyInstruction *)ins);
        return;
    } else if (ins->arch == MachineBaseInstruction::PHI) {
        printAsm<MachinePhiInstruction *>((MachinePhiInstruction *)ins);
        return;
    } else {
        s << "# Unknown Instruction, probably COMMENT\n";
        return;
    }
}

void RiscV64Printer::SyncFunction(MachineFunction *func) { current_func = func; }

void RiscV64Printer::SyncBlock(MachineBlock *block) { cur_block = block; }

void RiscV64Printer::emit() {
    s << "\t.text\n\t.globl main\n";
    s << "\t.attribute arch, \"rv64i2p1_m2p0_a2p1_f2p2_d2p2_c2p0_zicsr2p0_zifencei2p0_zba1p0_zbb1p0\"\n";
    for (auto func : printee->functions) {
        current_func = func;
        // s << "\t.globl\t" << func->getFunctionName() << "\n";
        s << func->getFunctionName() << ":\n";
        // May use iterator instead of directly accessing vector<blocks> in future
        // auto block = func->blocks[0];
        std::map<int, int> vsd;
        std::stack<int> stack;
        stack.push(0);    // 采用dfs的方式输出汇编代码，尽量减少无条件跳转指令数量
        while (!stack.empty()) {
            int block_id = stack.top();
            vsd[block_id] = 1;
            stack.pop();
            auto block = func->getMachineCFG()->GetNodeByBlockId(block_id)->Mblock;
            s << "." << func->getFunctionName() << "_" << block_id << ":\n";
            cur_block = block;
            for (auto ins : *block) {
                if (ins->arch == MachineBaseInstruction::RiscV) {
                    auto cur_rvins = (RiscV64Instruction *)ins;
                    if (OpTable[cur_rvins->getOpcode()].ins_formattype == RvOpInfo::B_type) {
                        auto dest_label = cur_rvins->getLabel().jmp_label_id;
                        if (vsd.find(dest_label) == vsd.end()) {
                            vsd[dest_label] = 1;
                            stack.push(dest_label);
                        }
                    }
                    if (cur_rvins->getOpcode() == RISCV_JAL && cur_rvins->getUseLabel() == true &&
                        cur_rvins->getRd() == GetPhysicalReg(RISCV_x0)) {
                        auto dest_label = cur_rvins->getLabel().jmp_label_id;
                        if (vsd.find(dest_label) == vsd.end()) {
                            vsd[dest_label] = 1;
                            stack.push(dest_label);
                            continue;
                        }
                    }
                    s << "\t";
                    printAsm((RiscV64Instruction *)ins);
                    s << "\n";
                } else if (ins->arch == MachineBaseInstruction::PHI) {
                    if (::print_comment) {
                        s << "\t";
                        printAsm((MachinePhiInstruction *)ins);
                        s << "\n";
                    }
                } else if (ins->arch == MachineBaseInstruction::COPY) {
                    s << "\t";
                    printAsm((MachineCopyInstruction *)ins);
                    s << "\n";
                } else if (ins->arch == MachineBaseInstruction::COMMENT) {
                    if (::print_comment) {
                        s << "\t";
                        s << "# " << ((MachineComment *)ins)->GetComment();
                        s << "\n";
                    }
                } else if (ins->arch == MachineBaseInstruction::SELECT) {
                    if (::print_comment) {
                        s << "\t";
                        auto sel_ins = (MachineSelectInstruction *)ins;
                        printAsm(sel_ins->GetCond());
                        s << "|";
                        printRVfield(sel_ins->GetDst());
                        s << ",";
                        printRVfield(sel_ins->GetSrcTrue());
                        s << ",";
                        printRVfield(sel_ins->GetSrcFalse());
                        s << "\n";
                    }
                } else if (ins->arch == MachineBaseInstruction::NOP) {
                } else {
                    ERROR("Unexpected arch");
                }
            }
        }
    }
    s << "\t.data\n";    // 输出全局变量定义指令
    for (auto global : printee->global_def) {
        if (global->GetOpcode() == BasicInstruction::GLOBAL_VAR) {
            auto global_ins = (GlobalVarDefineInstruction *)global;
            s << global_ins->name << ":\n";
            if (global_ins->type == BasicInstruction::I32) {
                if (global_ins->arrayval.dims.empty()) {
                    if (global_ins->init_val != nullptr) {
                        Assert(global_ins->init_val->GetOperandType() == BasicOperand::IMMI32);
                        auto imm_op = (ImmI32Operand *)global_ins->init_val;
                        s << "\t.word\t" << imm_op->GetIntImmVal() << "\n";
                    } else {
                        s << "\t.word\t0\n";
                    }
                } else {
                    int zero_cum = 0;
                    for (auto val : global_ins->arrayval.IntInitVals) {
                        if (val == 0) {
                            zero_cum += 4;
                        } else {
                            if (zero_cum != 0) {
                                s << "\t.zero\t" << zero_cum << "\n";
                                zero_cum = 0;
                            }
                            s << "\t.word\t" << val << "\n";
                        }
                    }
                    if (global_ins->arrayval.IntInitVals.empty()) {
                        int prod = 1;
                        for (auto dim : global_ins->arrayval.dims) {
                            prod *= dim;
                        }
                        s << "\t.zero\t" << prod * 4 << "\n";
                    }
                    if (zero_cum != 0) {
                        s << "\t.zero\t" << zero_cum << "\n";
                        zero_cum = 0;
                    }
                }
            } else if (global_ins->type == BasicInstruction::FLOAT32) {
                if (global_ins->arrayval.dims.empty()) {
                    if (global_ins->init_val != nullptr) {
                        Assert(global_ins->init_val->GetOperandType() == BasicOperand::IMMF32);
                        auto imm_op = (ImmF32Operand *)global_ins->init_val;
                        auto immf = imm_op->GetFloatVal();
                        s << "\t.word\t" << *(int *)&immf << "\n";
                    } else {
                        s << "\t.word\t0\n";
                    }
                } else {
                    int zero_cum = 0;
                    for (auto val : global_ins->arrayval.FloatInitVals) {
                        if (val == 0) {
                            zero_cum += 4;
                        } else {
                            if (zero_cum != 0) {
                                s << "\t.zero\t" << zero_cum << "\n";
                                zero_cum = 0;
                            }
                            s << "\t.word\t" << *(int *)&val << "\n";
                        }
                    }
                    if (global_ins->arrayval.FloatInitVals.empty()) {
                        int prod = 1;
                        for (auto dim : global_ins->arrayval.dims) {
                            prod *= dim;
                        }
                        s << "\t.zero\t" << prod * 4 << "\n";
                    }
                    if (zero_cum != 0) {
                        s << "\t.zero\t" << zero_cum << "\n";
                        zero_cum = 0;
                    }
                }
            } else if (global_ins->type == BasicInstruction::I64) {
                Assert(global_ins->arrayval.dims.empty());
                if (global_ins->init_val != nullptr) {
                    Assert(global_ins->init_val->GetOperandType() == BasicOperand::IMMI64);
                    auto imm_op = (ImmI64Operand *)global_ins->init_val;
                    auto imm_ll = imm_op->GetLlImmVal();
                    s << "\t.quad\t" << imm_ll << "\n";
                } else {
                    s << "\t.quad\t0\n";
                }
            }
        } else if (global->GetOpcode() == BasicInstruction::GLOBAL_STR) {
            auto str_ins = (GlobalStringConstInstruction *)global;
            // Log("Here");
            s << str_ins->str_name << ":\n";
            s << "\t.asciz\t"
              << "\"" << str_ins->str_val << "\""
              << "\n";
        } else {
            ERROR("Unexpected global define instruction");
        }
    }
}