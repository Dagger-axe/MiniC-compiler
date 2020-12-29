/*可以参考文献[2]Flex&Bison中第3章Bison内容58页*/
%error-verbose
%locations
%{
    #include <stdio.h>
    #include <math.h>
    #include <string.h>
    #include <stdarg.h>
    #include <unistd.h>
    #include "def.h"
    extern int yylineno;
    extern char *yytext;
    extern FILE *yyin;
    //用户定义输出文件
    char filename[64];
    //消除warning的声明
    void yyerror(const char* fmt, ...);
    void displayAST(struct node *, int);   
    int yylex();
%}
/*给出语法分析中非终结符和终结符的类型，供不同文法符号使用；对无语义子程序的规则，默认使用$$=$1，需要相应的类型匹配；默认类型是整型；样例中给出了整型、浮点型、字符串数组、抽象语法树节点4种，具体根据自己情况增减*/
%union {
	int type_int;
	float type_float;
	char type_id[32];
    char type_char[4];
	struct node *ptr;
};

/*用%type来指定非终结符的语义值类型，用<>选择union中某个类型，后面列出同类型的非终结符*/
%type <ptr> Program ExtDefList ExtDef Specifier ExtDecList FuncDec CompSt VarList VarDec ParamDec Stmt StmList DefList Def DecList Dec Exp Args

/* 用%token来指定终结符的语义值类型，与非终结符类似*/
%token <type_int> INT           //指定是type_int类型，用于AST树建立
%token <type_id> ID RELOP TYPE //指定是type_id 类型
%token <type_float> FLOAT       //指定是type_float类型
%token <type_char> CHAR         //指定是type_char类型
/*其他bison对该文件编译时，带参数-d，生成的exp.tab.h中给这些单词进行编码，可在lex.l中包含parser.tab.h使用这些单词种类码，见附件1。如下*/

%token LP RP LC RC SEMI COMMA
%token PLUSOP MINUSOP STAROP DIVOP INC DEC PLUS MINUS STAR DIV ASSIGNOP AND OR NOT
%token IF ELSE WHILE RETURN BREAK CONTINUE

%right ASSIGNOP PLUSOP MINUSOP STAROP DIVOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right UMINUS NOT INC DEC
%right LC LP
%left RC RP

%nonassoc LOWER_THEN_ELSE
%nonassoc ELSE
/*----------------------------------------------------------------------------*/
%%
/* 文法规约终点：语义分析入口 */
Program: ExtDefList { displayAST($1, 0);semantic_AnalysisInit($1, filename); }  //displayAST($1, 0);
    ; 
/* 外部变量定义列表：整个语法树 */
ExtDefList: { $$ = NULL; }
    | ExtDef ExtDefList { $$ = mknode(EXT_DEF_LIST, $1, $2, NULL, yylineno); }  //每一个EXTDEFLIST的结点，其第1棵子树对应一个外部变量声明或函数   
    ; 
/* 外部变量声明 或 函数定义 */
ExtDef: Specifier ExtDecList SEMI { $$ = mknode(EXT_VAR_DEF, $1, $2, NULL, yylineno); }   
    | Specifier FuncDec CompSt { $$ = mknode(FUNC_DEF, $1, $2, $3, yylineno); }         
    | error SEMI { $$ = NULL; }
    ;
Specifier: TYPE { $$ = mknode(TYPE, NULL, NULL, NULL, yylineno); strcpy($$->type_id, $1); if(!strcmp($1,"int"))$$->type = INT; if(!strcmp($1,"float"))$$->type = FLOAT; if(!strcmp($1,"char"))$$->type = CHAR; }   
    ;
/* 每一个EXT_DECLIST的结点，其第一棵子树，对应一个变量名(ID类型的结点)，第二棵子树，对应剩下的外部变量名 */      
ExtDecList: VarDec { $$ = $1; } 
    | VarDec COMMA ExtDecList { $$ = mknode(EXT_DEC_LIST, $1, $3, NULL, yylineno); }
    ;  
/* ID结点，标识符符号串存放结点的type_id */
VarDec: ID { $$ = mknode(ID, NULL, NULL, NULL, yylineno); strcpy($$->type_id, $1); }   
    ;
/* 注意函数名的存放 */
FuncDec: ID LP VarList RP { $$ = mknode(FUNC_DEC, $3, NULL, NULL, yylineno); strcpy($$->type_id, $1); }
    | ID LP RP { $$ = mknode(FUNC_DEC, NULL, NULL, NULL, yylineno); strcpy($$->type_id, $1); } 
    ;  
VarList: ParamDec { $$ = mknode(PARAM_LIST, $1, NULL, NULL, yylineno); }
    | ParamDec COMMA VarList { $$ = mknode(PARAM_LIST, $1, $3, NULL, yylineno); }
    ;
ParamDec: Specifier VarDec { $$ = mknode(PARAM_DEC, $1, $2, NULL, yylineno); }
    ;
/* 复合语句{定义 执行} */
CompSt: LC DefList StmList RC { $$ = mknode(COMP_STM, $2, $3, NULL, yylineno); }
    ;
/* 语句列表 */
StmList: { $$ = NULL; }  
    | Stmt StmList { $$ = mknode(STM_LIST, $1, $2, NULL, yylineno); }
    ;
Stmt: Exp SEMI { $$ = mknode(EXP_STMT, $1, NULL, NULL, yylineno); }
    | CompSt { $$ = $1; }  //复合语句结点直接最为语句结点，不再生成新的结点
    | RETURN Exp SEMI { $$ = mknode(RETURN, $2, NULL, NULL, yylineno); }
    | IF LP Exp RP Stmt %prec LOWER_THEN_ELSE { $$ = mknode(IF_THEN, $3, $5, NULL, yylineno); }
    | IF LP Exp RP Stmt ELSE Stmt { $$ = mknode(IF_THEN_ELSE, $3, $5, $7, yylineno); }
    | WHILE LP Exp RP Stmt { $$ = mknode(WHILE, $3, $5, NULL, yylineno); }
    | BREAK SEMI { $$ = mknode(BREAK, NULL, NULL, NULL, yylineno); }
    | CONTINUE SEMI { $$ = mknode(CONTINUE, NULL, NULL, NULL, yylineno); }
    ;  
DefList: { $$ = NULL; }
    | Def DefList { $$ = mknode(DEF_LIST, $1, $2, NULL, yylineno); }
    | error SEMI { $$ = NULL; }
    ;
Def: Specifier DecList SEMI { $$ = mknode(VAR_DEF, $1, $2, NULL, yylineno); }
    ;
DecList: Dec { $$ = mknode(DEC_LIST, $1, NULL, NULL, yylineno); }
    | Dec COMMA DecList { $$ = mknode(DEC_LIST, $1, $3, NULL, yylineno); }
    ;
Dec: VarDec { $$ = $1; }
    | VarDec ASSIGNOP Exp { $$ = mknode(ASSIGNOP, $1, $3, NULL, yylineno); strcpy($$->type_id, "ASSIGNOP"); }
    ;
Exp: Exp ASSIGNOP Exp   { $$ = mknode(ASSIGNOP, $1, $3, NULL, yylineno); strcpy($$->type_id, "ASSIGNOP"); }  //$$结点type_id空置未用，正好存放运算符
    | Exp AND Exp       { $$ = mknode(AND, $1, $3, NULL, yylineno); strcpy($$->type_id, "AND"); }
    | Exp OR Exp        { $$ = mknode(OR, $1, $3, NULL, yylineno); strcpy($$->type_id, "OR"); }
    | Exp RELOP Exp     { $$ = mknode(RELOP, $1, $3, NULL, yylineno); strcpy( $$->type_id, $2); }  //词法分析关系运算符号自身值保存在$2中
    | Exp PLUS Exp      { $$ = mknode(PLUS, $1, $3, NULL, yylineno); strcpy($$->type_id, "PLUS"); }
    | Exp MINUS Exp     { $$ = mknode(MINUS, $1, $3, NULL, yylineno); strcpy($$->type_id, "MINUS"); }
    | Exp STAR Exp      { $$ = mknode(STAR, $1, $3, NULL, yylineno); strcpy($$->type_id, "STAR"); }
    | Exp DIV Exp       { $$ = mknode(DIV, $1, $3, NULL, yylineno); strcpy($$->type_id, "DIV"); }
    | Exp PLUSOP Exp    { $$ = mknode(PLUSOP, $1, $3, NULL, yylineno); strcpy($$->type_id, "PLUSOP"); }     // +=
    | Exp MINUSOP Exp   { $$ = mknode(MINUSOP, $1, $3, NULL, yylineno); strcpy($$->type_id, "MINUSOP"); }   // -=
    | Exp STAROP Exp    { $$ = mknode(STAROP, $1, $3, NULL, yylineno); strcpy($$->type_id, "STAROP"); }     // *=
    | Exp DIVOP Exp     { $$ = mknode(DIVOP, $1, $3, NULL, yylineno); strcpy($$->type_id, "DIVOP"); }       // /=
    | INC Exp           { $$ = mknode(INC, $2, NULL, NULL, yylineno); strcpy($$->type_id, "INC"); }  // ++a
    | Exp INC           { $$ = mknode(INC, $1, NULL, NULL, yylineno); strcpy($$->type_id, "INC"); }  // a++
    | DEC Exp           { $$ = mknode(DEC, $2, NULL, NULL, yylineno); strcpy($$->type_id, "DEC"); }  // --a
    | Exp DEC           { $$ = mknode(DEC, $1, NULL, NULL, yylineno); strcpy($$->type_id, "DEC"); }  // a--
    | LP Exp RP         { $$ = $2; }
    | MINUS Exp %prec UMINUS { $$ = mknode(UMINUS, $2, NULL, NULL, yylineno); strcpy($$->type_id, "UMINUS"); }
    | NOT Exp           { $$ = mknode(NOT, $2, NULL, NULL, yylineno); strcpy($$->type_id, "NOT"); }
    | ID LP Args RP     { $$ = mknode(FUNC_CALL, $3, NULL, NULL, yylineno); strcpy($$->type_id, $1); }
    | ID LP RP          { $$ = mknode(FUNC_CALL, NULL, NULL, NULL, yylineno); strcpy($$->type_id, $1); }
    | ID                { $$ = mknode(ID, NULL, NULL, NULL, yylineno); strcpy($$->type_id, $1); }
    | INT               { $$ = mknode(INT, NULL, NULL, NULL, yylineno); $$->type_int = $1; $$->type = INT; }
    | FLOAT             { $$ = mknode(FLOAT, NULL, NULL, NULL, yylineno); $$->type_float = $1; $$->type = FLOAT; }
    | CHAR              { $$ = mknode(CHAR, NULL, NULL, NULL, yylineno); strcpy($$->type_char, $1); $$->type = CHAR; }
    ;
Args: Exp COMMA Args { $$ = mknode(ARGS, $1, $3, NULL, yylineno); }
    | Exp { $$ = mknode(ARGS, $1, NULL, NULL, yylineno); }
    ;

%%

void print_help() {
    printf("Usage:\t  ./chopper [-h] <file-name> [object-file-path]\n\n");
    printf("-h\t\t    Shows this usage instructions\n");
    printf("file-name\t    The name of the file to compile\n");
    printf("object-file-path    The file name and path of the target file\n");
}

int main(int argc, char *argv[]){
    if (argc != 2 && argc != 3) {
        printf("[ERROR]***Invalid arguments provided. Use -h for help.\n");
        return 0;
    }
    if (argc == 2 && !strcmp(argv[1], "-h")) {
        print_help();
        return 0;
    }
    /* 测试指定文件是否存在、是否可读 */
    int ret = access(argv[1], F_OK);
    if (ret == -1) {
        printf("[ERROR]***No such initial file.\n");
        return 0;
    } else {
        ret = access(argv[1], R_OK);
        if (ret == -1) {
            printf("[ERROR]***The initial file can\'t be read.\n");
            return 0;
        }
    }
    /* 当用户指定输出文件时，判断该路径是否存在 */
    if (argc == 3) {
        FILE *po = fopen(argv[2], "wb");
        if (!po) {
            printf("[ERROR]***The object path is wrong.\n");
            fclose(po);
            return 0;
        } else strcpy(filename, argv[2]);
        fclose(po);
    } else strcpy(filename, "objectfile.s");

	yyin = fopen(argv[1], "r");
	if(!yyin) return 0;
	yylineno = 1;
	yyparse();
	return 0;
}

void yyerror(const char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    fprintf(stderr, "Grammar Error Found at Line %d Column %d: ", yylloc.first_line, yylloc.first_column);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, ".\n");
}
