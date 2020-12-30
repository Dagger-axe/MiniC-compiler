#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "chopper.tab.h"

enum node_kind {
    EXT_DEF_LIST,
    EXT_VAR_DEF,
    FUNC_DEF,
    FUNC_DEC,
    EXT_DEC_LIST,
    PARAM_LIST,
    PARAM_DEC,
    VAR_DEF,
    DEC_LIST,
    DEF_LIST,
    COMP_STM,
    STM_LIST,
    EXP_STMT,
    IF_THEN,
    IF_THEN_ELSE,
    FUNC_CALL,
    ARGS,
    FUNCTION,
    PARAM,
    ARG,
    CALL,
    LABEL,
    GOTO,
    JLT,
    JLE,
    JGT,
    JGE,
    EQ,
    NEQ
};
#define SYMBOLTABLESIZE 1000  //定义符号表的大小
#define DX 3 * sizeof(int)    //活动记录控制信息需要的单元数
/*以下包括语法树、中间代码存储需要的类型等定义，仅供参考，请根据自己的需要修改定义*/
struct operandStruct { /*中间代码操作数信息*/
    int kind;          //操作的类型标识
    int type;          //操作数的类型标识
    union {
        int const_int;      //整常数值，立即数
        float const_float;  //浮点常数值，立即数
        int const_char;    //字符常数值，立即数
        char id[33];        //变量、临时变量的别名、标号字符串
    };
    int level;   //变量的层号，0表示是全局变量，数据保存在静态数据区
    int offset;  //变量单元偏移量，或函数在符号表的位置
};

struct codenode {                             //中间代码TAC结点,采用双向循环链表存放
    int op;                                   // TAC代码的运算符种类
    struct operandStruct opn1, opn2, result;  // 2个操作数和运算结果
    struct codenode *next, *prior;
};
struct codenode *breakcode;     // break所指位置指针
struct codenode *continuecode;  // continue所指位置指针

struct node {  //以下对结点属性定义没有考虑存储效率，只是简单地列出要用到的一些属性
    enum node_kind kind;  //结点类型
    union {
        char type_id[33];   //由标识符生成的叶结点
        int type_int;       //由整常数生成的叶结点
        float type_float;   //由浮点常数生成的叶结点
        char type_char[4];  // 'c'
        struct Arr * type_array;  //由数组生成的叶节点
    };
    struct node *ptr[3];  //子树指针，由kind确定有多少棵子树
    int level;            //层号
    int place;            //表示结点对应的变量或运算结果临时变量在符号表的位置序号
    char Etrue[15], Efalse[15];  //对布尔表达式的翻译时，真假转移目标的标号
    char Snext[15];              //该结点对应语句执行后的下一条语句位置标号
    struct codenode *code;       //该结点中间代码链表头指针
    char op[10];
    int type;      //结点对应值的类型
    int position;  //语法单位所在位置行号
    int offset;    //偏移量
    int width;     //各种数据占用的字节数
    int num;       //参数个数
};

/* 数组的结构体定义，包括数组第i维大小及下一维指针 */
struct arrnode {
    int size;  //该维度大小
    struct arrnode *next;
};

/*符号表中元素结构*/
struct symbol {     //这里只列出了一个符号表项的部分属性，没考虑属性间的互斥
    char name[33];  //变量或函数名
    int level;      //层号，外部变量名或函数名，层号为0；形参为1；进入复合语句加1，退出减1
    int type;        //变量类型或函数返回值类型, 1表示int，2表示float
    int paramnum;    //形式参数个数
    char alias[10];  //别名，为解决嵌套层次使用，可以使每个数据名称唯一
    char flag;       //符号标记缩写，函数：'F'  变量：'V'   参数：'P'  临时变量：'T'
    char offset;     //外部变量和局部变量，在其静态数据区或活动记录中的偏移量；
    //数组部分
    struct arrnode *arrlist;
    int dimension;                    
};
/*符号表，是一个顺序栈，index初值为0*/
struct symboltable {
    struct symbol symbols[SYMBOLTABLESIZE];  //符号栈中每个变量的信息
    int index;  //每进入一层，index自增1；每退出一层，index自减1；始终记录当前所在层数
} symbolTable;

struct symbol_scope_array {  //用于查询栈顶位置以及在退出一层时指导该层在栈中的起始
    int ScopeArray[30];      //第i层的起始序号
    int top;                 //当前的栈顶元素
} symbol_scope_Stack;

struct node *mknode(int kind, struct node *first, struct node *second, struct node *third, int position);
void semantic_AnalysisInit(struct node *T, char *filename);
void semantic_Analysis(struct node *T);
void boolExp(struct node *T);
void Exp(struct node *T);
void objectCode(struct codenode *head, char *filename);

/*---将int型的输入转换为int值，可匹配十六进制---*/
int to_int(char *str);