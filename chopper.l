/*----------------------------------------------------------------------------*/
%option yylineno
%option noyywrap

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

char        ('[A-Za-z0-9]')
int         [+-]?[0-9]+|0x[A-Fa-f0-9]{1,32}
float       [+-]?([0-9]*\.?[0-9]+|[0-9]+\.)
id          [A-Za-z_][A-Za-z0-9_]* 
/*----------------------------------------------------------------------------*/
%%

"//".*\n    { }  //行注释
"/*".*"*/"  { }  //块注释

{int}       { yylval.type_int = to_int(yytext); return INT; }  //int类型可匹配+-号及0x12a4的十六进制表示
{float}     { yylval.type_float = atof(yytext); return FLOAT; }  //float可匹配+-号及“.123”的表示
{char}		{ strcpy(yylval.type_char, yytext); return CHAR; }  //char类型

"int"       { strcpy(yylval.type_id, yytext); return TYPE; }
"float"     { strcpy(yylval.type_id, yytext); return TYPE; }
"char"      { strcpy(yylval.type_id, yytext); return TYPE; }
"return"    { return RETURN; }
"if"        { return IF; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"break"     { return BREAK; }
"continue"  { return CONTINUE; }
{id}        { strcpy(yylval.type_id, yytext); return ID; }  //由于优先级从上到下，因此该id不可放在关键字前，否则不会被识别为关键字而识别为标识符

";"			{ return SEMI; }
","			{ return COMMA; }
">"|"<"|">="|"<="|"=="|"!=" { strcpy(yylval.type_id, yytext); return RELOP; }
"+="        { return PLUSOP; }
"-="        { return MINUSOP; }
"*="        { return STAROP; }
"/="        { return DIVOP; }
"="			{ return ASSIGNOP; }
"++"        { return INC; }
"--"        { return DEC; }
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
[\n]        { yycolumn = 1; }  //记录行号以便输出错误信息，换行后将列信息变为1
[ \r\t]     { }  //空格、回车、制表符不做处理
.			{ printf("Error type A :Mysterious character \"%s\"\n\t at Line %d\n", yytext, yylineno); }
%%