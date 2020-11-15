/*----------------------------------------------------------------------------*/
%option yylineno
%{
    #include "chopper.tab.h"
    #include <string.h>
    #include "def.h"
    int yycolumn = 1;
    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
        yylloc.first_column = yycolumn; \
        yylloc.last_column = yycolumn + yyleng - 1; \
        yycolumn += yyleng;
    typedef union {
        int type_int;
        float type_float;
        char type_id[32];
        struct node *ptr;
    } YYLVAL;
    #define YYSTYPE YYLVAL
%}
int         [0-9]+
float       ([0-9]*\.[0-9]+)|([0-9]+\.)
id          [A-Za-z_][A-Za-z0-9_]* 
/*----------------------------------------------------------------------------*/
%%
{int}       { yylval.type_int = atoi(yytext); return INT; }
{float}     { yylval.type_float = atoi(yytext); return FLOAT; }
"int"       { strcpy(yylval.type_id, yytext); return TYPE; }
"float"     { strcpy(yylval.type_id, yytext); return TYPE; }
"return"    { return RETURN; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
{id}        { strcpy(yylval.type_id, yytext); return ID; }

";"			{ return SEMI; }
","			{ return COMMA; }
">"|"<"|">="|"<="|"=="|"!=" { strcpy(yylval.type_id, yytext); return RELOP; }
"="			{ return ASSIGNOP; }
"+"			{ return PLUS; }
"-"			{ return MINUS; }
"*"			{ return STAR; }
"/"			{ return DIV; }
"&&"        { return AND; }
"||"        { return OR; }
"!"			{ return NOT; }
"("			{ return LP; }
")"			{ return RP; }
"{"			{ return LC; }
"}"			{ return RC; }
[\n]        { yycolumn = 1; }  //记录行号以便输出错误信息
[ \r\t]     { }  //空格、回车、制表符不做处理 
"//".*\n    { }
"/*"[\s\S]*"*/"     { }
.			{/*输出词法分析错误编号、异常符号串、错误行列号*/}
%%
/*----------------------------------------------------------------------------*/ 