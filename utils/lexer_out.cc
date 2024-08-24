#include "../parser/SysY_parser.tab.h"
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

extern int line_number;
extern int col_number;
extern int cur_col_number;

extern int yylex();
extern YYSTYPE yylval;
extern char *yytext;

void PrintLexerResult(std::ostream &s, char *yytext, YYSTYPE yylval, int token, int line_number) {
    std::setfill(' ');
    switch (token) {
    case INT:
        ALIGNED_FORMAT_OUTPUT("INT", yytext, "");
        break;
    case INT_CONST:
        ALIGNED_FORMAT_OUTPUT("INT_CONST", yytext, yylval.int_token);
        break;
    case FLOAT:
        ALIGNED_FORMAT_OUTPUT("FLOAT", yytext, "");
        break;
    // case YYerror:
    //     ALIGNED_FORMAT_OUTPUT("ERROR",yylval.error_msg,line_number);
    //     break;
    // case YYEOF:
    //     ALIGNED_FORMAT_OUTPUT("EOF",yytext,line_number);
    //     break;
    // case YYUNDEF:
    //     ALIGNED_FORMAT_OUTPUT("UNDEF",yytext,line_number);
    //     break;
    case STR_CONST:
        ALIGNED_FORMAT_OUTPUT("STR_CONST", yytext, yylval.symbol_token->get_string());
        break;
    case IDENT:
        ALIGNED_FORMAT_OUTPUT("IDENT", yytext, yylval.symbol_token->get_string());
        break;
    case FLOAT_CONST:
        ALIGNED_FORMAT_OUTPUT("FLOAT_CONST", yytext, yylval.float_token);
        break;
    case LEQ:
        ALIGNED_FORMAT_OUTPUT("LEQ", yytext, "");
        break;
    case GEQ:
        ALIGNED_FORMAT_OUTPUT("GEQ", yytext, "");
        break;
    case EQ:
        ALIGNED_FORMAT_OUTPUT("EQ", yytext, "");
        break;
    case NE:
        ALIGNED_FORMAT_OUTPUT("NE", yytext, "");
        break;
    case AND:
        ALIGNED_FORMAT_OUTPUT("AND", yytext, "");
        break;
    case OR:
        ALIGNED_FORMAT_OUTPUT("OR", yytext, "");
        break;
    case CONST:
        ALIGNED_FORMAT_OUTPUT("CONST", yytext, "");
        break;
    case IF:
        ALIGNED_FORMAT_OUTPUT("IF", yytext, "");
        break;
    case ELSE:
        ALIGNED_FORMAT_OUTPUT("ELSE", yytext, "");
        break;
    case WHILE:
        ALIGNED_FORMAT_OUTPUT("WHILE", yytext, "");
        break;
    case NONE_TYPE:
        ALIGNED_FORMAT_OUTPUT("VOID", yytext, "");
        break;
    case RETURN:
        ALIGNED_FORMAT_OUTPUT("RETURN", yytext, "");
        break;
    case BREAK:
        ALIGNED_FORMAT_OUTPUT("BREAK", yytext, "");
        break;
    case CONTINUE:
        ALIGNED_FORMAT_OUTPUT("CONTINUE", yytext, "");
        break;
    case ERROR:
        ALIGNED_FORMAT_OUTPUT("ERROR", yytext, yylval.error_msg);
        break;
    case '[':
        ALIGNED_FORMAT_OUTPUT("LSQUARE", yytext, "");
        break;
    case ']':
        ALIGNED_FORMAT_OUTPUT("RSQUARE", yytext, "");
        break;
    case '(':
        ALIGNED_FORMAT_OUTPUT("LPAREN", yytext, "");
        break;
    case ')':
        ALIGNED_FORMAT_OUTPUT("RPAREN", yytext, "");
        break;
    case '{':
        ALIGNED_FORMAT_OUTPUT("LBRACE", yytext, "");
        break;
    case '}':
        ALIGNED_FORMAT_OUTPUT("RBRACE", yytext, "");
        break;
    case '+':
        ALIGNED_FORMAT_OUTPUT("ADD", yytext, "");
        break;
    case '-':
        ALIGNED_FORMAT_OUTPUT("SUB", yytext, "");
        break;
    case '*':
        ALIGNED_FORMAT_OUTPUT("MUL", yytext, "");
        break;
    case '/':
        ALIGNED_FORMAT_OUTPUT("DIV", yytext, "");
        break;
    case '%':
        ALIGNED_FORMAT_OUTPUT("MOD", yytext, "");
        break;
    case '=':
        ALIGNED_FORMAT_OUTPUT("ASSIGN", yytext, "");
        break;
    case '!':
        ALIGNED_FORMAT_OUTPUT("NOT", yytext, "");
        break;
    case ',':
        ALIGNED_FORMAT_OUTPUT("COMMA", yytext, "");
        break;
    case ';':
        ALIGNED_FORMAT_OUTPUT("SEMICOLON", yytext, "");
        break;
    case '>':
        ALIGNED_FORMAT_OUTPUT("LESS", yytext, "");
        break;
    case '<':
        ALIGNED_FORMAT_OUTPUT("GREATER", yytext, "");
        break;
    default:
        ALIGNED_FORMAT_OUTPUT((char)token, yytext, "");
    }
}