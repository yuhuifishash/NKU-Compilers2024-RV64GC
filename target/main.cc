#include "../include/ir.h"
#include "../ir_gen/semant.h"
#include "../parser/SysY_parser.tab.h"

#include "../optimize/transform/mem2reg.h"
#include "../optimize/transform/simplify_cfg.h"

#include "../optimize/analysis/dominator_tree.h"

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
void PrintLexerResult(std::ostream &s, char *yytext, YYSTYPE yylval, int token);

/* 框架目前并没有对内存泄漏问题进行处理(例如没有编写语法树的析构函数)
   如果你只是想完成本学期的编译实验作业，可以忽略内存泄漏的问题，统一在编译结束后由操作系统帮忙回收
   如果你对自己有更高的要求或者对此感兴趣，可以尝试自己编写相关函数来解决内存泄漏问题
   将裸指针换为智能指针也许是个简单快捷的解决方案(虽然会对程序性能造成较大的影响)
*/

/* 如果你简化了框架的实现, 或者解决了框架现存的问题
   或者是用现代C++特性对框架进行了重构, 并且有效地简化了代码或者提高了代码的复用性
   我们鼓励你与助教进行联系或发起pull request, 也许你的改进会被用到下一年的实验框架中
*/

/*
   本学期的编译作业还对你的编译器编译速度有一定要求，编译大部分用例的时间不能超过5s，一些大型的测试用例不得超过300s
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
            /*
                yytext为当前识别到的token对应的字符串

                yylval的类型为YYSTYPE, YYSTYPE在SysY_parser.y中用%union定义,
                该变量的作用为在词法分析和语法分析中传递信息， 由于lab1中只有词法分析, 我们输出yylval对应的信息即可

                token为词法分析函数的返回值, 对应SysY_lexer.l中的return, 表示token的类型
                token实际为一个枚举类型, 定义位于SysY_parser.y中的%token
            */
            PrintLexerResult(fout, yytext, yylval, token);
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
        /*
            如果你的语法分析实现不符合预期, 可能会导致语法树打印出现SegmentFault, 大概率是nullptr导致的
            一种办法是找到在哪个地方RE了, 然后看为啥这里有个空指针, 之后修改SysY_parser.y
            如果你认为这里就应该是空指针, 但是语法树的输出没考虑到, 你也可以自行修改相应语法树的输出函数
        */
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
    // llvmIR.CFGInit();
    // SimplifyCFGPass(&llvmIR).Execute();

    optimize_flag = (argc == 6 && (strcmp(argv[optimize_tag], "-O1") == 0));

    if (optimize_flag) {
        DomAnalysis dom(&llvmIR);
        dom.Execute();    // 对于AnalysisPass后续应该由TransformPass更新信息, 维护Analysis的正确性
        (Mem2RegPass(&llvmIR, &dom)).Execute();

        // TODO: add more passes
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
