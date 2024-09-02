%option noyywrap
%{
#include <cmath>
#include <vector>
#include <cstring>
#include "../include/symtab.h"
#include "../parser/SysY_parser.tab.h"

extern int line_number;
extern int col_number;
extern int cur_col_number;
extern IdTable id_table;
void yyerror(char* s, ...);
%}

%%
    /* TODO():增加处理注释的代码*/

    /* TODO():增加处理列号的代码(cur_col_number表示当前token开始位置, col_number表示当前token结束位置) */
"<=" {return LEQ;}         
">=" {return GEQ;}         
"==" {return EQ;}        
"!=" {return NE;}
"&&" {return AND;}         
"||" {return OR;}               
"const" {return CONST;}     
"if" {return IF;}       
"else" {return ELSE;}  
"while" {return WHILE;}
"void" {return NONE_TYPE;}    
"int" {return INT;}    
"float" {return FLOAT;}      
"return" {return RETURN;}    
"break" {return BREAK;}     
"continue" {return CONTINUE;}

\n {++line_number;col_number = 0;}

[ \t\f\r\v] {col_number += strlen(yytext);}

[\.\+\-\*\/\=\<\!\%\>] {return yytext[0];}

[\{\}\;\(\)\,\[\]] {return yytext[0];}

[_a-zA-Z][_a-zA-Z0-9]* {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.symbol_token = id_table.add_id(std::string(yytext));
    return IDENT;
}
([1-9][0-9]*)|0 {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    // yylval.int_token = stoi(std::string(yytext));
    yylval.int_token = 0;
    for(int i = 0;yytext[i];i++){
        yylval.int_token *= 10;
        yylval.int_token += yytext[i] - '0';
    }
    return INT_CONST;
}
    /*你不用过多地深究yylval究竟是在哪里定义的这个变量，你只需要将yylval正确赋值即可
      在词法分析中，你只需要考虑yylval的三种类型，分别为int_token, float_token, symbol_token
      之所以要定义这三种类型，是因为我们只返回这个token的类型是不够的，我们还需要知道这个token的属性
      例如我们需要知道INT_CONST具体的值是多少
    */
    /*TODO():参考SysY2022定义, 处理更多的词法, 如浮点数，十六进制数等*/
    

    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%



