#include "../include/ir.h"
#include "../ir_gen/semant.h"
#include "../parser/SysY_parser.tab.h"

#include "../optimize/ssa/mem2reg.h"
#include "../optimize/cfg/simplify_cfg.h"

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


/* 框架并没有对内存泄漏问题进行处理(即没有编写析构函数)
   如果你只是想完成本学期的编译实验作业，可以忽略内存泄漏的问题，统一在编译结束后由操作系统帮忙回收
   如果你对自己有更高的要求或者对此感兴趣，可以尝试自己编写相关函数来解决内存泄漏问题
   将裸指针换为智能指针也许是个简单快捷的解决方案(虽然会对程序性能造成较大的影响)
*/

/*
   本学期的编译作业还对你的编译器编译速度有一定要求，编译单个用例的时间不能超过10s，一些大型的测试用例不得超过300s
   测试用例中有一些较大的用例，例如lone_line.sy, long_code.sy, long_func.sy
   你需要在实现时注意数据结构的选择以及算法的实现细节
*/


// 为了方便起见，直接根据OJ要求将命令行参数位置固定了
#define step_tag 1
#define o_tag 2
#define file_out 3
#define file_in 4
#define optimize_tag 5

/*
-lexer
-parser
-llvm
-S
SysYc -S -o *.s *.sy (-O1)
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

    // 当你完成代码优化的基础要求后，将下面两行注释取消
    llvmIR.CFGInit();
    SimplifyCFGPass(&llvmIR).Execute();

    optimize_flag = (argc == 6 && (strcmp(argv[optimize_tag], "-O1") == 0));

    if (optimize_flag) {
        // 当你完成Mem2Reg后，将该行注释取消
        // (Mem2RegPass(&llvmIR)).Execute(); 

        //TODO: add more passes
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
