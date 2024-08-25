#include "../include/ir.h"
#include "../ir_gen/semant.h"
#include "../parser/SysY_parser.tab.h"

#include "./common/machine_passes/register_alloc/fast_linear_scan/fast_linear_scan.h"
#include "./riscv64gc/instruction_print/riscv64_printer.h"
#include "./riscv64gc/instruction_select/riscv64_instSelect.h"
#include "./riscv64gc/instruction_select/riscv64_lowerframe.h"
#include "./riscv64gc/riscv64.h"

#include <assert.h>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>

#define ALIGNED_FORMAT_OUTPUT_HEAD(STR, CISU, PROP, STR3, STR4)                                                        \
    fout << std::fixed << std::setprecision(12) << std::setw(15) << std::left << STR << " " << std::setw(20)           \
         << std::left << CISU << " " << std::setw(32) << std::left << PROP << std::setw(15) << std::left << STR3       \
         << std::setw(15) << std::left << STR4 << "\n"

#define ALIGNED_FORMAT_OUTPUT(STR, CISU, PROP)                                                                         \
    s << std::fixed << std::setprecision(12) << std::setw(15) << std::left << STR << " " << std::setw(20) << std::left \
      << CISU << " " << std::setw(32) << std::left << PROP << " " << std::setw(15) << std::left << line_number         \
      << std::setw(15) << std::left << cur_col_number << "\n"

extern LLVMIR llvmIR;
extern Program ast_root;
extern FILE *yyin;
extern int error_num;
int line_number = 0;
int col_number = 0;
int cur_col_number = 0;
std::ofstream fout;
IdTable id_table;
extern int yylex();
extern YYSTYPE yylval;
extern char *yytext;
extern std::vector<std::string> error_msgs;
void PrintLexerResult(std::ostream &s, char *yytext, YYSTYPE yylval, int token, int line_number);

#define step_tag 2
#define o_tag 3
#define file_out 4
#define file_in 1
#define optimize_tag 5
/*
-lexer
-parser
-llvm
-S
SysYc *.sy -S -o *.s (-O1)
*/

enum Target { ARMV7 = 1, RV64GC = 2 } target;

bool optimize_flag = false;

int main(int argc, char **argv) {
    // target = ARMV7;
    target = RV64GC;

    FILE *fin = fopen(argv[file_in], "r");
    if (fin == NULL) {
        std::cerr << "Could not open input file " << argv[file_in] << std::endl;
        exit(1);
    }
    yyin = fin;
    fout.open(argv[file_out]);
    line_number = 1;

    if (strcmp(argv[step_tag], "-lexer") == 0) {
        int token;
        ALIGNED_FORMAT_OUTPUT_HEAD("Token", "Lexeme", "Property", "Line", "Column");
        while ((token = yylex()) != 0) {
            PrintLexerResult(fout, yytext, yylval, token, line_number);
        }
        fout.close();
        return 0;
    }
    yyparse();

    if (error_num > 0) {
        fout << "Parser error" << std::endl;
        fout.close();
        return 0;
    }

    if (strcmp(argv[step_tag], "-parser") == 0) {
        ast_root->printAST(fout, 0);
        fout.close();
        return 0;
    }

    ast_root->TypeCheck();
    if (error_msgs.size() > 0) {
        for (auto msg : error_msgs) {
            fout << msg << std::endl;
        }
        fout.close();
        return 0;
    }

    if (strcmp(argv[step_tag], "-semant") == 0) {
        ast_root->printAST(fout, 0);
        return 0;
    }

    ast_root->codeIR();

    // llvmIR.CFGInit();
    // llvmIR.BuildCFG();

    optimize_flag = (argc == 6 && (strcmp(argv[optimize_tag], "-O1") == 0 || strcmp(argv[optimize_tag], "-O2") == 0));

    if (optimize_flag) {
    }

    if (strcmp(argv[step_tag], "-llvm") == 0) {
        llvmIR.printIR(fout);
        fout.close();
        return 0;
    }
    if (strcmp(argv[step_tag], "-S") == 0) {
        MachineUnit *m_unit = new RiscV64Unit();
        RiscV64Register regs;
        RiscV64Spiller spiller;

        RiscV64Selector(m_unit, &llvmIR).SelectInstructionAndBuildCFG();
        RiscV64LowerFrame(m_unit).Execute();

        FastLinearScan(m_unit, &regs, &spiller).Execute();
        RiscV64LowerStack(m_unit).Execute();

        RiscV64Printer(fout, m_unit).emit();
    }
    if (strcmp(argv[step_tag], "-select") == 0) {
        MachineUnit *m_unit = new RiscV64Unit();
        RiscV64Register regs;
        RiscV64Spiller spiller;

        RiscV64Selector(m_unit, &llvmIR).SelectInstructionAndBuildCFG();
        RiscV64LowerFrame(m_unit).Execute();

        RiscV64Printer(fout, m_unit).emit();
    }
    fout.close();
    return 0;
}
