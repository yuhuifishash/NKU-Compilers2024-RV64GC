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
std::vector<char> tmp_str;
%}

%%
    /* TODO():增加处理注释的代码*/
"//".* {}


"/*"[^*]*\*+([^/*][^*]*\*+)*"/" {
    for(int i=0;yytext[i];++i){
        if (yytext[i] == '\n'){
            ++line_number;
            col_number=0;
        } else {
            ++col_number;
        }
    }
}



"/*" {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = "comment error at /*";
    return ERROR;
}

"*/" {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = "comment error at */";
    return ERROR;
}

    /* TODO():增加处理列号的代码(cur_col_number表示当前token开始位置, col_number表示当前token结束位置) */
"<=" {cur_col_number = col_number; col_number += strlen(yytext); return LEQ;}         
">=" {cur_col_number = col_number; col_number += strlen(yytext); return GEQ;}         
"==" {cur_col_number = col_number; col_number += strlen(yytext); return EQ;}        
"!=" {cur_col_number = col_number; col_number += strlen(yytext); return NE;}
"&&" {cur_col_number = col_number; col_number += strlen(yytext); return AND;}         
"||" {cur_col_number = col_number; col_number += strlen(yytext); return OR;}               
"const" {cur_col_number = col_number; col_number += strlen(yytext); return CONST;}     
"if" {cur_col_number = col_number; col_number += strlen(yytext); return IF;}       
"else" {cur_col_number = col_number; col_number += strlen(yytext); return ELSE;}  
"while" {cur_col_number = col_number; col_number += strlen(yytext); return WHILE;}
"void" {cur_col_number = col_number; col_number += strlen(yytext); return NONE_TYPE;}    
"int" {cur_col_number = col_number; col_number += strlen(yytext); return INT;}    
"float" {cur_col_number = col_number; col_number += strlen(yytext); return FLOAT;}      
"return" {cur_col_number = col_number; col_number += strlen(yytext); return RETURN;}    
"break" {cur_col_number = col_number; col_number += strlen(yytext); return BREAK;}     
"continue" {cur_col_number = col_number; col_number += strlen(yytext); return CONTINUE;}

\n {++line_number;col_number = 0;}

[ \t\f\r\v] {col_number += strlen(yytext);}

[\.\+\-\*\/\=\<\!\%\>] {cur_col_number = col_number; col_number += strlen(yytext); return yytext[0];}

[\{\}\;\(\)\,\[\]] {cur_col_number = col_number; col_number += strlen(yytext); return yytext[0];}

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
    /*TODO():参考SysY2022定义, 处理更多的词法, 如浮点数，十六进制数等*/

[0-9]+\.[0-9]+([eE][-+]?[0-9]+)? {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.float_token = 0;
    int i = 0;
    while(yytext[i] && yytext[i] != '.'){
        yylval.float_token *= 10;
        yylval.float_token += yytext[i] - '0';
        i++;
    }
    int dotlen = 0;
    i++;
    while(yytext[i] && yytext[i] != 'e'){
        yylval.float_token *= 10;
        yylval.float_token += yytext[i] - '0';
        dotlen++;
        i++;
    }
    if(yytext[i] == 'e'){
        i++;
        int newlen = 0;
        if(yytext[i] == '-'){
            i++;
            while(yytext[i]){
                newlen *= 10;
                newlen += yytext[i] - '0';
                i++;
            }
        }else{
            while(yytext[i]){
                newlen *= 10;
                newlen -= yytext[i] - '0';
                i++;
            }
            
        }
        dotlen += newlen;
    }
    while(dotlen > 0){
        yylval.float_token /= 10;
        dotlen--;
    }
    while(dotlen < 0){
        yylval.float_token *= 10;
        dotlen++;
    }
    return FLOAT_CONST;
}

[0-9]+[eE][-+]?[0-9]+ {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.float_token = 0;
    int i = 0;
    while(yytext[i] && yytext[i] != 'e'){
        yylval.float_token *= 10;
        yylval.float_token += yytext[i] - '0';
        i++;
    }
    int dotlen = 0;
    if(yytext[i] == 'e'){
        i++;
        int newlen = 0;
        if(yytext[i] == '-'){
            i++;
            while(yytext[i]){
                newlen *= 10;
                newlen += yytext[i] - '0';
                i++;
            }
        }else{
            while(yytext[i]){
                newlen *= 10;
                newlen -= yytext[i] - '0';
                i++;
            }
            
        }
        dotlen += newlen;
    }
    while(dotlen > 0){
        yylval.float_token /= 10;
        dotlen--;
    }
    while(dotlen < 0){
        yylval.float_token *= 10;
        dotlen++;
    }
    return FLOAT_CONST;
}
    /*unknown tokens, return ERROR*/
. {
    cur_col_number = col_number;
    col_number += strlen(yytext);
    yylval.error_msg = yytext;
    return ERROR;
}
%%



