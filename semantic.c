#include "def.h"
int in_while = 0;  //标记是否在循环里
int loop_lev = 0;  //标记当前循环的层数

char *strcat0(char *s1, char *s2) {
    static char result[10];
    strcpy(result, s1);
    strcat(result, s2);
    return result;
}

char *newAlias() {
    static int no = 1;
    char s[10];
    // itoa(no++, s, 10);
    sprintf(s, "%d", no++);
    return strcat0("v", s);
}

char *newLabel() {
    static int no = 1;
    char s[10];
    // itoa(no++, s, 10);
    sprintf(s, "%d", no++);
    return strcat0("label_", s);
}

char *newTemp() {
    static int no = 1;
    char s[10];
    // itoa(no++, s, 10);
    sprintf(s, "%d", no++);
    return strcat0("temp_", s);
}

//生成一条TAC代码的结点组成的双向循环链表，返回头指针
struct codenode *genIR(int op, struct operandStruct opn1, struct operandStruct opn2, struct operandStruct result) {
    struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
    h->op = op;
    h->opn1 = opn1;
    h->opn2 = opn2;
    h->result = result;
    h->next = h->prior = h;
    return h;
}

//生成一条标号语句，返回头指针
struct codenode *genLabel(char *label) {
    struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
    h->op = LABEL;
    strcpy(h->result.id, label);
    h->next = h->prior = h;
    return h;
}

//生成GOTO语句，返回头指针
struct codenode *genGoto(char *label) {
    struct codenode *h = (struct codenode *)malloc(sizeof(struct codenode));
    h->op = GOTO;
    strcpy(h->result.id, label);
    h->next = h->prior = h;
    return h;
}

//合并多个中间代码的双向循环链表，首尾相连
struct codenode *merge(int num, ...) {
    struct codenode *h1, *h2, *p, *t1, *t2;
    va_list ap;
    va_start(ap, num);
    h1 = va_arg(ap, struct codenode *);
    while (--num > 0) {
        h2 = va_arg(ap, struct codenode *);
        if (h1 == NULL)
            h1 = h2;
        else if (h2) {
            t1 = h1->prior;
            t2 = h2->prior;
            t1->next = h2;
            t2->next = h1;
            h1->prior = t2;
            h2->prior = t1;
        }
    }
    va_end(ap);
    return h1;
}

//输出中间代码
void prnIR(struct codenode *head) {
    char opnstr1[32], opnstr2[32], resultstr[32];
    struct codenode *h = head;
    do {
        if (h->opn1.kind == INT)
            sprintf(opnstr1, "#%d", h->opn1.const_int);
        else if (h->opn1.kind == FLOAT)
            sprintf(opnstr1, "#%f", h->opn1.const_float);
        else if (h->opn1.kind == CHAR)
            sprintf(opnstr1, "#%c", h->opn1.const_char);
        else if (h->opn1.kind == ID)
            sprintf(opnstr1, "%s", h->opn1.id);

        if (h->opn2.kind == INT)
            sprintf(opnstr2, "#%d", h->opn2.const_int);
        else if (h->opn2.kind == FLOAT)
            sprintf(opnstr2, "#%f", h->opn2.const_float);
        else if (h->opn2.kind == CHAR)
            sprintf(opnstr2, "#%c", h->opn2.const_char);
        else if (h->opn2.kind == ID)
            sprintf(opnstr2, "%s", h->opn2.id);

        sprintf(resultstr, "%s", h->result.id);
        switch (h->op) {
            case ASSIGNOP: printf("  %s := %s\n", resultstr, opnstr1); break;
            case PLUS:
            case MINUS:
            case STAR:
            case DIV:
                printf("  %s := %s %c %s\n", resultstr, opnstr1, (h->op == PLUS) ? '+' : (h->op == MINUS) ? '-' : (h->op == STAR) ? '*' : '/', opnstr2);
                break;
            case INC:  // INC与DEC一致
            case DEC:
                printf("  %s := #1\n", opnstr2);
                printf("  %s := %s %c %s\n", resultstr, opnstr1, h->op == INC ? '+' : '-', opnstr2);
                printf("  %s := %s\n", opnstr1, resultstr);
                break;
            case FUNCTION: printf("\nFUNCTION %s :\n", h->result.id); break;
            case PARAM: printf("  PARAM %s\n", h->result.id); break;
            case LABEL: printf("LABEL %s :\n", h->result.id); break;
            case GOTO: printf("  GOTO %s\n", h->result.id); break;
            case JLE: printf("  IF %s <= %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case JLT: printf("  IF %s < %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case JGE: printf("  IF %s >= %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case JGT: printf("  IF %s > %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case EQ: printf("  IF %s == %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case NEQ: printf("  IF %s != %s GOTO %s\n", opnstr1, opnstr2, resultstr); break;
            case ARG: printf("  ARG %s\n", h->result.id); break;
            case CALL: printf("  %s := CALL %s\n", resultstr, opnstr1); break;
            case RETURN:
                if (h->result.kind)
                    printf("  RETURN %s\n", resultstr);
                else
                    printf("  RETURN\n");
                break;
        }
        h = h->next;
    } while (h != head);
}
void semantic_error(int line, char *msg1, char *msg2) {
    //这里可以只收集错误信息，最后一次显示
    printf("在%d行, %s %s\n", line, msg1, msg2);
}
void prn_symbol() {  //显示符号表
    int i = 0;
    char symbol_type[7];
    printf("%6s %6s %6s  %6s %4s %6s\n", "变量名", "别 名", "层 号", "类  型", "标记", "偏移量");
    for (i = 0; i < symbolTable.index; i++) {
        if (symbolTable.symbols[i].type == INT) {
            strcpy(symbol_type, "int");
        } else if (symbolTable.symbols[i].type == FLOAT) {
            strcpy(symbol_type, "float");
        } else if (symbolTable.symbols[i].type == CHAR) {
            strcpy(symbol_type, "char");
        }
        printf("%6s %6s %6d  %6s %4c %6d\n", symbolTable.symbols[i].name, symbolTable.symbols[i].alias, symbolTable.symbols[i].level, symbol_type,
               symbolTable.symbols[i].flag, symbolTable.symbols[i].offset);
    }
}

int searchSymbolTable(char *name) {
    int i, is_func = 0;
    for (i = symbolTable.index - 1; i >= 0; i--) {
        if (symbolTable.symbols[i].level == 0) is_func = 1;
        if (is_func && symbolTable.symbols[i].level == 1) continue;  //跳过参数
        if (!strcmp(symbolTable.symbols[i].name, name)) return i;
    }
    return -1;
}

int fillSymbolTable(char *name, char *alias, int level, int type, char flag, int offset) {
    //首先根据name查符号表，不能重复定义 重复定义返回-1
    int i;
    /*符号查重，考虑外部变量声明前有函数定义，
    其形参名还在符号表中，这时的外部变量与前函数的形参重名是允许的*/
    for (i = symbolTable.index - 1; i >= 0 && (symbolTable.symbols[i].level == level || level == 0); i--) {
        if (level == 0 && symbolTable.symbols[i].level == 1) continue;  //外部变量和形参不必比较重名
        if (!strcmp(symbolTable.symbols[i].name, name)) return -1;
    }
    //填写符号表内容
    strcpy(symbolTable.symbols[symbolTable.index].name, name);
    strcpy(symbolTable.symbols[symbolTable.index].alias, alias);
    symbolTable.symbols[symbolTable.index].level = level;
    symbolTable.symbols[symbolTable.index].type = type;
    symbolTable.symbols[symbolTable.index].flag = flag;
    symbolTable.symbols[symbolTable.index].offset = offset;
    return symbolTable.index++;  //返回的是符号在符号表中的位置序号，中间代码生成时可用序号取到符号别名
}

//填写临时变量到符号表，返回临时变量在符号表中的位置
int fill_Temp(char *name, int level, int type, char flag, int offset) {
    strcpy(symbolTable.symbols[symbolTable.index].name, "");
    strcpy(symbolTable.symbols[symbolTable.index].alias, name);
    symbolTable.symbols[symbolTable.index].level = level;
    symbolTable.symbols[symbolTable.index].type = type;
    symbolTable.symbols[symbolTable.index].flag = flag;
    symbolTable.symbols[symbolTable.index].offset = offset;
    return symbolTable.index++;  //返回的是临时变量在符号表中的位置序号
}

int LEV = 0;    //层号
int func_size;  // 1个函数的活动记录大小

void ext_var_list(struct node *T) {  //处理变量列表
    int rtn, num = 1;
    switch (T->kind) {
        case EXT_DEC_LIST:
            T->ptr[0]->type = T->type;                 //将类型属性向下传递变量结点
            T->ptr[0]->offset = T->offset;             //外部变量的偏移量向下传递
            T->ptr[1]->type = T->type;                 //将类型属性向下传递变量结点
            T->ptr[1]->offset = T->offset + T->width;  //外部变量的偏移量向下传递
            T->ptr[1]->width = T->width;
            ext_var_list(T->ptr[0]);
            ext_var_list(T->ptr[1]);
            T->num = T->ptr[1]->num + 1;
            break;
        case ID:
            rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'V', T->offset);  //最后一个变量名
            if (rtn == -1)
                semantic_error(T->position, T->type_id, "变量重复定义");
            else
                T->place = rtn;
            T->num = 1;
            break;
    }
}

int match_param(int i, struct node *T) {
    int j, num = symbolTable.symbols[i].paramnum;
    int type1, type2, position = T->position;
    T = T->ptr[0];
    if (num == 0 && T == NULL) return 1;
    for (j = 1; j <= num; j++) {
        if (!T) {
            semantic_error(position, "", "函数调用参数太少");
            return 0;
        }
        type1 = symbolTable.symbols[i + j].type;  //形参类型
        type2 = T->ptr[0]->type;
        if (type1 != type2) {
            semantic_error(position, "", "参数类型不匹配");
            return 0;
        }
        T = T->ptr[1];
    }
    if (T) {  // num个参数已经匹配完，还有实参表达式
        semantic_error(position, "", "函数调用参数太多");
        return 0;
    }
    return 1;
}

void boolExp(struct node *T) {  //布尔表达式，参考文献[1]p84的思想
    struct operandStruct opn1, opn2, result;
    int op;
    int rtn;
    if (T) {
        switch (T->kind) {
            case INT:
                if (T->type_int != 0)
                    T->code = genGoto(T->Etrue);
                else
                    T->code = genGoto(T->Efalse);
                T->width = 0;
                break;
            case FLOAT:
                if (T->type_float != 0.0)
                    T->code = genGoto(T->Etrue);
                else
                    T->code = genGoto(T->Efalse);
                T->width = 0;
                break;
            case CHAR:
                if (T->type_char[1] != -1 && T->type_char[1] != 0)
                    T->code = genGoto(T->Etrue);
                else
                    T->code = genGoto(T->Efalse);
                T->width = 0;
                break;
            case ID:  //查符号表，获得符号表中的位置，类型送type
                rtn = searchSymbolTable(T->type_id);
                if (rtn == -1) semantic_error(T->position, T->type_id, "变量未定义");
                if (symbolTable.symbols[rtn].flag == 'F')
                    semantic_error(T->position, T->type_id, "是函数名，类型不匹配");
                else {
                    int temprtn = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset);
                    opn1.kind = INT;
                    opn1.const_int = 0;
                    result.kind = ID;
                    strcpy(result.id, symbolTable.symbols[temprtn].alias);
                    result.offset = symbolTable.symbols[temprtn].offset + T->width;
                    T->code = genIR(ASSIGNOP, opn1, opn2, result);
                    T->width += 4;

                    opn1.kind = ID;
                    strcpy(opn1.id, symbolTable.symbols[rtn].alias);
                    opn1.offset = symbolTable.symbols[rtn].offset;
                    opn2.kind = ID;
                    strcpy(opn2.id, symbolTable.symbols[temprtn].alias);
                    opn2.offset = symbolTable.symbols[temprtn].offset;

                    result.kind = ID;
                    strcpy(result.id, T->Etrue);
                    op = NEQ;
                    T->code = merge(3, T->code, genIR(op, opn1, opn2, result), genGoto(T->Snext));
                    break;
                }
                T->width = 0;
                break;
            case RELOP:  //处理关系运算表达式, 2个操作数都按基本表达式处理
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                Exp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                Exp(T->ptr[1]);
                if (T->width < T->ptr[1]->width) T->width = T->ptr[1]->width;
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                opn2.kind = ID;
                strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                result.kind = ID;
                strcpy(result.id, T->Etrue);
                if (strcmp(T->type_id, "<") == 0)
                    op = JLT;
                else if (strcmp(T->type_id, "<=") == 0)
                    op = JLE;
                else if (strcmp(T->type_id, ">") == 0)
                    op = JGT;
                else if (strcmp(T->type_id, ">=") == 0)
                    op = JGE;
                else if (strcmp(T->type_id, "==") == 0)
                    op = EQ;
                else if (strcmp(T->type_id, "!=") == 0)
                    op = NEQ;
                T->code = genIR(op, opn1, opn2, result);
                T->code = merge(4, T->ptr[0]->code, T->ptr[1]->code, T->code, genGoto(T->Efalse));
                break;
            case AND:
            case OR:
                if (T->kind == AND) {
                    strcpy(T->ptr[0]->Etrue, newLabel());
                    strcpy(T->ptr[0]->Efalse, T->Efalse);
                } else {
                    strcpy(T->ptr[0]->Etrue, T->Etrue);
                    strcpy(T->ptr[0]->Efalse, newLabel());
                }
                strcpy(T->ptr[1]->Etrue, T->Etrue);
                strcpy(T->ptr[1]->Efalse, T->Efalse);
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                boolExp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                boolExp(T->ptr[1]);
                if (T->width < T->ptr[1]->width) T->width = T->ptr[1]->width;
                if (T->kind == AND)
                    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
                else
                    T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Efalse), T->ptr[1]->code);
                break;
            case NOT:
                strcpy(T->ptr[0]->Etrue, T->Efalse);
                strcpy(T->ptr[0]->Efalse, T->Etrue);
                boolExp(T->ptr[0]);
                T->code = T->ptr[0]->code;
                break;
        }
    }
}

void Exp(struct node *T) {  //处理基本表达式，参考文献[1]p82的思想
    int rtn, num, width, kind;
    struct node *T0;
    struct operandStruct opn1, opn2, result;
    if (T) {
        switch (T->kind) {
            case ID:
                rtn = searchSymbolTable(T->type_id);  //查符号表，获得符号表中的位置，类型送type
                if (rtn == -1) semantic_error(T->position, T->type_id, "变量未定义");
                if (symbolTable.symbols[rtn].flag == 'F')
                    semantic_error(T->position, T->type_id, "是函数名，类型不匹配");
                else {
                    T->place = rtn;  //结点保存变量在符号表中的位置
                    T->code = NULL;  //标识符不需要生成TAC
                    T->type = symbolTable.symbols[rtn].type;
                    T->offset = symbolTable.symbols[rtn].offset;
                    T->width = 0;  //未再使用新单元
                }
                break;
            case INT:
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset);  //为整常量生成一个临时变量
                T->type = INT;
                opn1.kind = INT;
                opn1.const_int = T->type_int;
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP, opn1, opn2, result);
                T->width = 4;
                break;
            case FLOAT:
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset);  //为浮点常量生成一个临时变量
                T->type = FLOAT;
                opn1.kind = FLOAT;
                opn1.const_float = T->type_float;
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP, opn1, opn2, result);
                T->width = 4;
                break;
            case CHAR:
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset);  //为字符变量生成一个临时变量
                T->type = CHAR;
                opn1.kind = CHAR;
                opn1.const_char = (int)T->type_char[1];
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = genIR(ASSIGNOP, opn1, opn2, result);
                T->width = 4;
                break;
            case ASSIGNOP:
                if (T->ptr[0]->kind != ID) {
                    semantic_error(T->position, "", "赋值语句需要左值");
                } else {
                    Exp(T->ptr[0]);  //处理左值，例中仅为变量
                    T->ptr[1]->offset = T->offset;
                    Exp(T->ptr[1]);
                    if (T->ptr[0]->type != T->ptr[1]->type) {
                        semantic_error(T->position, "", "赋值语句两边的类型不匹配");
                        break;
                    }
                    T->type = T->ptr[0]->type;
                    T->width = T->ptr[1]->width;
                    T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);
                    opn1.kind = ID;
                    strcpy(opn1.id,
                           symbolTable.symbols[T->ptr[1]->place].alias);  //右值一定是个变量或临时变量
                    opn1.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                    result.kind = ID;
                    strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
                    result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(2, T->code, genIR(ASSIGNOP, opn1, opn2, result));
                }
                break;
            case AND:  //以下均向下进入RELOP计算
            case OR:
            case RELOP:
                T->type = INT;
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                Exp(T->ptr[0]);
                Exp(T->ptr[1]);
                break;
            case PLUSOP:
                if (T->kind == PLUSOP) kind = PLUS;
            case MINUSOP:
                if (T->kind == MINUSOP) kind = MINUS;
            case STAROP:
                if (T->kind == STAROP) kind = STAR;
            case DIVOP:
                if (T->kind == DIVOP) kind = DIV;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);
                if (T->ptr[0]->type == CHAR || T->ptr[1]->type == CHAR) 
                    semantic_error(T->position, "", "字符串类型不能参与运算");
                if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT)
                    T->type = FLOAT, T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                else if (T->ptr[0]->type == INT && T->ptr[1]->type == INT)
                    T->type = INT, T->width = T->ptr[0]->width + T->ptr[1]->width + 2;
                else
                    semantic_error(T->position, "", "表达式两侧类型不匹配");

                opn1.kind = ID;
                opn1.type = T->ptr[0]->type;
                strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);  //此处与ASSIGNOP保持一致
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;

                opn2.kind = ID;
                opn2.type = T->ptr[1]->type;
                strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);  //另一个右值是自己
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;

                result.kind = ID;
                result.type = T->ptr[0]->type;
                strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
                result.offset = symbolTable.symbols[T->ptr[0]->place].offset;

                T->width = T->ptr[0]->width + T->ptr[1]->width + (T->type == INT ? 4 : 8);
                T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(kind, opn1, opn2, result));                
                break;
            case PLUS:
            case MINUS:
            case STAR:
            case DIV:
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                Exp(T->ptr[1]);

                if (T->ptr[0]->type == CHAR || T->ptr[1]->type == CHAR) semantic_error(T->position, "", "字符串类型不能参与运算");
                if (T->ptr[0]->type == FLOAT || T->ptr[1]->type == FLOAT)
                    T->type = FLOAT, T->width = T->ptr[0]->width + T->ptr[1]->width + 4;
                else if (T->ptr[0]->type == INT && T->ptr[1]->type == INT)
                    T->type = INT, T->width = T->ptr[0]->width + T->ptr[1]->width + 2;
                else
                    semantic_error(T->position, "", "表达式两侧类型不匹配");

                T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->ptr[0]->width + T->ptr[1]->width);
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                opn2.kind = ID;
                strcpy(opn2.id, symbolTable.symbols[T->ptr[1]->place].alias);
                opn2.type = T->ptr[1]->type;
                opn2.offset = symbolTable.symbols[T->ptr[1]->place].offset;
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(3, T->ptr[0]->code, T->ptr[1]->code, genIR(T->kind, opn1, opn2, result));
                T->width = T->ptr[0]->width + T->ptr[1]->width + (T->type == INT ? 4 : 8);
                break;
            case INC:  // a++ ++a a-- --a
            case DEC:
                Exp(T->ptr[0]);
                if (T->ptr[0]->kind != ID) {
                    semantic_error(T->position, "", "自增/减语句左值不符合要求");
                    break;
                }
                if (T->ptr[0]->type != INT) {
                    semantic_error(T->position, "", "自增/减语句左值不为int型变量，非法");
                    break;
                }
                T->type = T->ptr[0]->type;
                T->ptr[0]->offset = T->offset;  // a++ 等于 a = a + 1 ，第一个值即为当前a值
                //存a值
                opn1.kind = ID;
                strcpy(opn1.id, symbolTable.symbols[T->ptr[0]->place].alias);
                opn1.type = T->ptr[0]->type;
                opn1.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                //创建一个temp编号，用于显示参数"1"
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T',
                                     T->offset + T->ptr[0]->width);  //临时变量用来存自增/减的1
                opn2.kind = ID;
                strcpy(opn2.id, symbolTable.symbols[T->place].alias);
                opn2.type = T->type;
                opn2.offset = symbolTable.symbols[T->place].offset;
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T',
                                     T->offset + T->ptr[0]->width);  //申请一个新变量来存结果
                //在新申请的临时变量空间存结果
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.type = T->type;
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(2, T->ptr[0]->code, genIR(T->kind, opn1, opn2, result));
                T->width = T->ptr[0]->width + (T->type == INT ? 4 : 8);
                break;
            case NOT:
                T->type = INT;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                break;
            case UMINUS:
                T->type = T->ptr[0]->type;
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                break;
            case FUNC_CALL:  //根据T->type_id查出函数的定义
                rtn = searchSymbolTable(T->type_id);
                if (rtn == -1) {
                    semantic_error(T->position, T->type_id, "函数未定义");
                    break;
                }
                if (symbolTable.symbols[rtn].flag != 'F') {
                    semantic_error(T->position, T->type_id, "不是一个函数");
                    break;
                }
                T->type = symbolTable.symbols[rtn].type;
                width = T->type == INT ? 4 : 8;  //存放函数返回值的单数字节数
                if (T->ptr[0]) {
                    T->ptr[0]->offset = T->offset;
                    Exp(T->ptr[0]);                       //处理所有实参表达式求值，及类型
                    T->width = T->ptr[0]->width + width;  //累加上计算实参使用临时变量的单元数
                    T->code = T->ptr[0]->code;
                } else {
                    T->width = width;
                    T->code = NULL;
                }
                match_param(rtn, T);  //处理所以参数的匹配
                //处理参数列表的中间代码
                T0 = T->ptr[0];
                while (T0) {
                    result.kind = ID;
                    strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
                    result.offset = symbolTable.symbols[T0->ptr[0]->place].offset;
                    T->code = merge(2, T->code, genIR(ARG, opn1, opn2, result));
                    T0 = T0->ptr[1];
                }
                T->place = fill_Temp(newTemp(), LEV, T->type, 'T', T->offset + T->width - width);
                opn1.kind = ID;
                strcpy(opn1.id, T->type_id);  //保存函数名
                opn1.offset = rtn;            //这里offset用以保存函数定义入口, 在目标代码生成时，能获取相应信息
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[T->place].alias);
                result.offset = symbolTable.symbols[T->place].offset;
                T->code = merge(2, T->code, genIR(CALL, opn1, opn2, result));  //生成函数调用中间代码
                break;
            case ARGS:  //此处仅处理各实参表达式的求值的代码序列，不生成ARG的实参系列
                T->ptr[0]->offset = T->offset;
                Exp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                T->code = T->ptr[0]->code;
                if (T->ptr[1]) {
                    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    Exp(T->ptr[1]);
                    T->width += T->ptr[1]->width;
                    T->code = merge(2, T->code, T->ptr[1]->code);
                }
                break;
        }
    }
}

void semantic_Analysis(struct node *T) {
    //对抽象语法树的先根遍历, 按displayAST的控制结构修改完成符号表管理和语义检查、TAC生成（语句部分）
    int rtn, num, width, paranum;
    struct node *T0;
    struct operandStruct opn1, opn2, result;
    if (T) {
        switch (T->kind) {
            case EXT_DEF_LIST:
                if (!T->ptr[0]) break;
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);  //访问外部定义列表中的第一个
                T->code = T->ptr[0]->code;
                if (T->ptr[1]) {
                    T->ptr[1]->offset = T->ptr[0]->offset + T->ptr[0]->width;
                    semantic_Analysis(T->ptr[1]);  //访问该外部定义列表中的其它外部定义
                    T->code = merge(2, T->code, T->ptr[1]->code);
                }
                break;
            case EXT_VAR_DEF:  //处理外部说明, 将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
                if (!strcmp(T->ptr[0]->type_id, "int")) {
                    T->type = T->ptr[1]->type = INT;
                    T->ptr[1]->width = 4;  //宽度传递
                } else if (!strcmp(T->ptr[0]->type_id, "float")) {
                    T->type = T->ptr[1]->type = FLOAT;
                    T->ptr[1]->width = 8;
                } else if (!strcmp(T->ptr[0]->type_id, "char")) {
                    T->type = T->ptr[1]->type = CHAR;
                    T->ptr[1]->width = 4;
                }
                T->ptr[1]->offset = T->offset;                   //这个外部变量的偏移量向下传递
                ext_var_list(T->ptr[1]);                         //处理外部变量说明中的标识符序列
                T->width = (T->ptr[1]->width) * T->ptr[1]->num;  //计算这个外部变量说明的宽度
                T->code = NULL;                                  //这里假定外部变量不支持初始化
                break;
            case FUNC_DEF:  //填写函数定义信息到符号表
                if (!strcmp(T->ptr[0]->type_id, "int"))
                    T->ptr[1]->type = INT;
                else if (!strcmp(T->ptr[0]->type_id, "float"))
                    T->ptr[1]->type = FLOAT;
                else
                    T->ptr[1]->type = CHAR;
                T->width = 0;                   //函数的宽度设置为0，不会对外部变量的地址分配产生影响
                T->offset = DX;                 //设置局部变量在活动记录中的偏移量初值
                semantic_Analysis(T->ptr[1]);   //处理函数名和参数结点部分，这里不考虑用寄存器传递参数
                T->offset += T->ptr[1]->width;  //用形参单元宽度修改函数局部变量的起始偏移量
                T->ptr[2]->offset = T->offset;
                strcpy(T->ptr[2]->Snext, newLabel());  //函数体语句执行结束后的位置属性
                semantic_Analysis(T->ptr[2]);          //处理函数体结点
                //计算活动记录大小, 这里offset属性存放的是活动记录大小，不是偏移
                symbolTable.symbols[T->ptr[1]->place].offset = T->offset + T->ptr[2]->width;
                T->code = merge(3, T->ptr[1]->code, T->ptr[2]->code,
                                genLabel(T->ptr[2]->Snext));  //函数体的代码作为函数的代码
                break;
            case FUNC_DEC:  //根据返回类型，函数名填写符号表
                rtn = fillSymbolTable(T->type_id, newAlias(), LEV, T->type, 'F',
                                      0);  //函数不在数据区中分配单元，偏移量为0
                if (rtn == -1) {
                    semantic_error(T->position, T->type_id, "函数重复定义");
                    break;
                } else
                    T->place = rtn;
                result.kind = ID;
                strcpy(result.id, T->type_id);
                result.offset = rtn;
                T->code = genIR(FUNCTION, opn1, opn2, result);  //生成中间代码：FUNCTION 函数名
                T->offset = DX;                                 //设置形式参数在活动记录中的偏移量初值
                if (T->ptr[0]) {                                //判断是否有参数
                    T->ptr[0]->offset = T->offset;
                    semantic_Analysis(T->ptr[0]);  //处理函数参数列表
                    T->width = T->ptr[0]->width;
                    symbolTable.symbols[rtn].paramnum = T->ptr[0]->num;
                    T->code = merge(2, T->code, T->ptr[0]->code);  //连接函数名和参数代码序列
                } else
                    symbolTable.symbols[rtn].paramnum = 0, T->width = 0;
                break;
            case PARAM_LIST:  //处理函数形式参数列表
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);
                if (T->ptr[1]) {
                    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    semantic_Analysis(T->ptr[1]);
                    T->num = T->ptr[0]->num + T->ptr[1]->num;              //统计参数个数
                    T->width = T->ptr[0]->width + T->ptr[1]->width;        //累加参数单元宽度
                    T->code = merge(2, T->ptr[0]->code, T->ptr[1]->code);  //连接参数代码
                } else {
                    T->num = T->ptr[0]->num;
                    T->width = T->ptr[0]->width;
                    T->code = T->ptr[0]->code;
                }
                break;
            case PARAM_DEC:
                rtn = fillSymbolTable(T->ptr[1]->type_id, newAlias(), 1, T->ptr[0]->type, 'P', T->offset);
                if (rtn == -1)
                    semantic_error(T->ptr[1]->position, T->ptr[1]->type_id, "参数名重复定义");
                else
                    T->ptr[1]->place = rtn;
                T->num = 1;                                 //参数个数计算的初始值
                T->width = T->ptr[0]->type == INT ? 4 : 8;  //参数宽度
                result.kind = ID;
                strcpy(result.id, symbolTable.symbols[rtn].alias);
                result.offset = T->offset;
                T->code = genIR(PARAM, opn1, opn2, result);  //生成：FUNCTION 函数名
                break;
            case COMP_STM:
                LEV++;
                //设置层号加1，并且保存该层局部变量在符号表中的起始位置在symbol_scope_Stack
                symbol_scope_Stack.ScopeArray[symbol_scope_Stack.top++] = symbolTable.index;
                T->width = 0;
                T->code = NULL;
                if (T->ptr[0]) {
                    T->ptr[0]->offset = T->offset;
                    semantic_Analysis(T->ptr[0]);  //处理该层的局部变量DEF_LIST
                    T->width += T->ptr[0]->width;
                    T->code = T->ptr[0]->code;
                }
                if (T->ptr[1]) {
                    T->ptr[1]->offset = T->offset + T->width;
                    strcpy(T->ptr[1]->Snext, T->Snext);  // S.next属性向下传递
                    semantic_Analysis(T->ptr[1]);        //处理复合语句的语句序列
                    T->width += T->ptr[1]->width;
                    T->code = merge(2, T->code, T->ptr[1]->code);
                }
//                prn_symbol();  //c在退出一个符合语句前显示的符号表
                LEV--;                                                                        //出复合语句，层号减1
                symbolTable.index = symbol_scope_Stack.ScopeArray[--symbol_scope_Stack.top];  //删除该作用域中的符号
                break;
            case DEF_LIST:
                T->code = NULL;
                if (T->ptr[0]) {
                    T->ptr[0]->offset = T->offset;
                    semantic_Analysis(T->ptr[0]);  //处理一个局部变量定义
                    T->code = T->ptr[0]->code;
                    T->width = T->ptr[0]->width;
                }
                if (T->ptr[1]) {
                    T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    semantic_Analysis(T->ptr[1]);  //处理剩下的局部变量定义
                    T->code = merge(2, T->code, T->ptr[1]->code);
                    T->width += T->ptr[1]->width;
                }
                break;
            case VAR_DEF:  //处理一个局部变量定义, 将第一个孩子(TYPE结点)中的类型送到第二个孩子的类型域
                //类似于上面的外部变量EXT_VAR_DEF，换了一种处理方法
                T->code = NULL;
                //确定变量序列各变量类型
                if (!strcmp(T->ptr[0]->type_id, "int")) {
                    T->ptr[1]->type = INT;
                    width = 4;
                } else if (!strcmp(T->ptr[0]->type_id, "float")) {
                    T->ptr[1]->type = FLOAT;
                    width = 8;
                } else if (!strcmp(T->ptr[0]->type_id, "char")) {
                    T->ptr[1]->type = CHAR;
                    width = 4;
                }
                T0 = T->ptr[1];  // T0为变量名列表子树根指针，对ID、ASSIGNOP类结点在登记到符号表，作为局部变量
                num = 0;
                T0->offset = T->offset;
                T->width = 0;
                while (T0) {  //处理所有DEC_LIST结点
                    num++;
                    T0->ptr[0]->type = T0->type;  //类型属性向下传递
                    if (T0->ptr[1]) T0->ptr[1]->type = T0->type;
                    T0->ptr[0]->offset = T0->offset;  // offset属性向下传递
                    if (T0->ptr[1]) T0->ptr[1]->offset = T0->offset + width;
                    if (T0->ptr[0]->kind == ID) {
                        rtn = fillSymbolTable(T0->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V',
                                              T->offset + T->width);  //此处偏移量未计算，暂时为0
                        if (rtn == -1)
                            semantic_error(T0->ptr[0]->position, T0->ptr[0]->type_id, "变量重复定义");
                        else
                            T0->ptr[0]->place = rtn;
                        T->width += width;
                    } else if (T0->ptr[0]->kind == ASSIGNOP) {  // int a, b = 1; 此时分析 b = 1的过程
                        rtn = fillSymbolTable(T0->ptr[0]->ptr[0]->type_id, newAlias(), LEV, T0->ptr[0]->type, 'V',
                                              T->offset + T->width);  //此处偏移量未计算，暂时为0
                        if (rtn == -1)
                            semantic_error(T0->ptr[0]->ptr[0]->position, T0->ptr[0]->ptr[0]->type_id, "变量重复定义");
                        else {  //支持int a, b = 1, c = read();的形式
                            T0->ptr[0]->place = rtn;
                            T0->ptr[0]->ptr[1]->offset = T->offset + T->width + width;
                            Exp(T0->ptr[0]->ptr[1]);
                            opn1.kind = ID;
                            strcpy(opn1.id, symbolTable.symbols[T0->ptr[0]->ptr[1]->place].alias);
                            //指定 a = 1 中临时变量 1 在栈中位置，以完成赋值
                            opn1.offset = symbolTable.symbols[T0->ptr[0]->ptr[1]->place].offset;
                            result.kind = ID;
                            strcpy(result.id, symbolTable.symbols[T0->ptr[0]->place].alias);
                            //指定 a = 1 中变量 a 在栈中位置，以完成赋值
                            result.offset = symbolTable.symbols[rtn].offset;
                            T->code = merge(3, T->code, T0->ptr[0]->ptr[1]->code, genIR(ASSIGNOP, opn1, opn2, result));
                        }
                        T->width += width + T0->ptr[0]->ptr[1]->width;
                    }
                    T0 = T0->ptr[1];
                }
                break;
            case STM_LIST:
                if (!T->ptr[0]) {
                    T->code = NULL;
                    T->width = 0;
                    break;
                }               //空语句序列
                if (T->ptr[1])  // 2条以上语句连接，生成新标号作为第一条语句结束后到达的位置
                    strcpy(T->ptr[0]->Snext, newLabel());
                else  //语句序列仅有一条语句，S.next属性向下传递
                    strcpy(T->ptr[0]->Snext, T->Snext);
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);
                T->code = T->ptr[0]->code;
                T->width = T->ptr[0]->width;
                if (T->ptr[1]) {  // 2条以上语句连接, S.next属性向下传递
                    strcpy(T->ptr[1]->Snext, T->Snext);
                    T->ptr[1]->offset = T->offset;  //顺序结构共享单元方式
                    //                  T->ptr[1]->offset = T->offset + T->ptr[0]->width;
                    //                  //顺序结构顺序分配单元方式
                    semantic_Analysis(T->ptr[1]);
                    //序列中第1条为表达式语句，返回语句，复合语句时，第2条前不需要标号
                    if (T->ptr[0]->kind == RETURN || T->ptr[0]->kind == EXP_STMT || T->ptr[0]->kind == COMP_STM)
                        T->code = merge(2, T->code, T->ptr[1]->code);
                    else
                        T->code = merge(3, T->code, genLabel(T->ptr[0]->Snext), T->ptr[1]->code);
                    if (T->ptr[1]->width > T->width) T->width = T->ptr[1]->width;  //顺序结构共享单元方式
                    //                        T->width += T->ptr[1]->width;  //顺序结构顺序分配单元方式
                }
                break;
            case IF_THEN:
                strcpy(T->ptr[0]->Etrue, newLabel());  //设置条件语句真假转移位置
                strcpy(T->ptr[0]->Efalse, T->Snext);
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                boolExp(T->ptr[0]);
                T->width = T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext, T->Snext);
                semantic_Analysis(T->ptr[1]);  // if子句
                if (T->width < T->ptr[1]->width) T->width = T->ptr[1]->width;
                T->code = merge(3, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code);
                break;  //控制语句都还没有处理offset和width属性
            case IF_THEN_ELSE:
                strcpy(T->ptr[0]->Etrue, newLabel());  //设置条件语句真假转移位置
                strcpy(T->ptr[0]->Efalse, newLabel());
                T->ptr[0]->offset = T->ptr[1]->offset = T->ptr[2]->offset = T->offset;
                boolExp(T->ptr[0]);  //条件，要单独按短路代码处理
                T->width = T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext, T->Snext);
                semantic_Analysis(T->ptr[1]);  // if子句
                if (T->width < T->ptr[1]->width) T->width = T->ptr[1]->width;
                strcpy(T->ptr[2]->Snext, T->Snext);
                semantic_Analysis(T->ptr[2]);  // else子句
                if (T->width < T->ptr[2]->width) T->width = T->ptr[2]->width;
                T->code =
                    merge(6, T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, genGoto(T->Snext), genLabel(T->ptr[0]->Efalse), T->ptr[2]->code);
                break;
            case WHILE:
                breakcode = NULL;
                continuecode = NULL;
                strcpy(T->ptr[0]->Etrue, newLabel());  // ptr[0]存循环条件，子结点继承属性的计算
                strcpy(T->ptr[0]->Efalse, T->Snext);
                T->ptr[0]->offset = T->ptr[1]->offset = T->offset;
                boolExp(T->ptr[0]);  //循环条件，要单独按短路代码处理
                T->width = T->ptr[0]->width;
                strcpy(T->ptr[1]->Snext, newLabel());
                in_while = 1;
                loop_lev++;
                breakcode = genGoto(T->Snext);             //记录出口，即整个while的出口
                continuecode = genGoto(T->ptr[1]->Snext);  //记录入口，即循环体执行结束后下一次执行的入口
                semantic_Analysis(T->ptr[1]);              //循环体
                if (T->width < T->ptr[1]->width) T->width = T->ptr[1]->width;
                T->code = merge(5, genLabel(T->ptr[1]->Snext), T->ptr[0]->code, genLabel(T->ptr[0]->Etrue), T->ptr[1]->code, genGoto(T->ptr[1]->Snext));
                in_while = 0;
                loop_lev--;
                breakcode = NULL;
                continuecode = NULL;
                break;
            case BREAK:
                if (in_while == 0) semantic_error(T->position, T->type_id, "\"break\"不在循环中");
                T->code = breakcode;
                break;
            case CONTINUE:
                if (loop_lev <= 0) semantic_error(T->position, T->type_id, "\"coninue\"不在循环中");
                T->code = continuecode;
                break;
            case EXP_STMT:
                T->ptr[0]->offset = T->offset;
                semantic_Analysis(T->ptr[0]);
                T->code = T->ptr[0]->code;
                T->width = T->ptr[0]->width;
                break;
            case RETURN:
                if (T->ptr[0]) {
                    T->ptr[0]->offset = T->offset;
                    Exp(T->ptr[0]);
                    num = symbolTable.index;
                    do
                        num--;
                    while (symbolTable.symbols[num].flag != 'F');
                    if (T->ptr[0]->type != symbolTable.symbols[num].type) {
                        semantic_error(T->position, "返回值类型错误", "");
                        T->width = 0;
                        T->code = NULL;
                        break;
                    }
                    T->width = T->ptr[0]->width;
                    result.kind = ID;
                    strcpy(result.id, symbolTable.symbols[T->ptr[0]->place].alias);
                    result.offset = symbolTable.symbols[T->ptr[0]->place].offset;
                    T->code = merge(2, T->ptr[0]->code, genIR(RETURN, opn1, opn2, result));
                } else {
                    T->width = 0;
                    result.kind = 0;
                    T->code = genIR(RETURN, opn1, opn2, result);
                }
                break;
            case ID:  //形式一致，向下进入func_call执行
            case INT:
            case FLOAT:
            case ASSIGNOP:
            case AND:
            case OR:
            case RELOP:
            case PLUSOP:
            case MINUSOP:
            case STAROP:
            case DIVOP:
            case INC:
            case DEC:
            case PLUS:
            case MINUS:
            case STAR:
            case DIV:
            case NOT:
            case UMINUS:
            case FUNC_CALL:
                Exp(T);  //处理基本表达式
                break;
        }
    }
}

void objectCode(struct codenode *head, char *filename) {  //目标代码生成
    FILE *pfile = fopen(filename, "wb");
    if (!pfile) {
        printf("[ERROR]***Fail to open object file.\n");
        return;
    }
    char opnstr1[32], opnstr2[32], resultstr[32];
    struct codenode *h = head, *pt;
    int i;

    // .data
    fprintf(pfile, ".data\n");
    fprintf(pfile,
            "input_info:.asciiz \"Enter an integer num:\"\n");  //.asciiz str 储存str于内存中，以null结尾
    fprintf(pfile, "ret_info:.asciiz\"\\n\"\n");
    fprintf(pfile, ".globl main\n");  // global main
    // .text
    fprintf(pfile, ".text\n");
    fprintf(pfile, "read:\n");
    fprintf(pfile, "    li $v0, 4\n");
    fprintf(pfile, "    la $a0, input_info\n");
    fprintf(pfile, "    syscall\n");
    fprintf(pfile, "    li $v0, 5\n");
    fprintf(pfile, "    syscall\n");
    fprintf(pfile, "    jr $ra\n");
    fprintf(pfile, "write:\n");
    fprintf(pfile, "    li $v0, 1\n");
    fprintf(pfile, "    syscall\n");
    fprintf(pfile, "    li $v0, 4\n");
    fprintf(pfile, "    la $a0, ret_info\n");
    fprintf(pfile, "    syscall\n");
    fprintf(pfile, "    move $v0, $0\n");
    fprintf(pfile, "    jr $ra\n");
    // op: $t1, $t2   res: $t3
    do {
        switch (h->op) {
            case ASSIGNOP:
                if (h->opn1.kind == INT)                                    // x:=#k  li指令将立即加载到该寄存器中
                    fprintf(pfile, "    li $t3, %d\n", h->opn1.const_int);  //常量到$t3
                else if (h->opn1.kind == FLOAT)
                    fprintf(pfile, "    li $t3, %f\n", h->opn1.const_float);
                else if (h->opn1.kind == CHAR)
                    fprintf(pfile, "    li $t3, %d\n", h->opn1.const_char);
                else {  //  x := y  lw $1,10($2) --> $1 = memory[$2 + 10] 将内存的值取出来
                    fprintf(pfile, "    lw $t1, %d($sp)\n", h->opn1.offset);
                    fprintf(pfile, "    move $t3, $t1\n");
                }
                //	sw $1, 10($2)  -->  memory[$2 + 10]= $1  结果存入寄存器中 将result的值存在result的偏移地址
                fprintf(pfile, "    sw $t3, %d($sp)\n", h->result.offset);
                break;
            case INC:
            case DEC:
                fprintf(pfile, "    lw $t1, %d($sp)\n", h->opn1.offset);
                fprintf(pfile, "    li $t2, 1\n");
                if (h->op == INC)
                    fprintf(pfile, "    add $t3, $t1, $t2\n");
                else
                    fprintf(pfile, "    sub $t3, $t1, $t2\n");
                fprintf(pfile, "    sw $t3, %d($sp)\n", h->opn1.offset);
            case PLUS:   // x := y + z
            case MINUS:  // x := y - z
            case STAR:   // x := y * z
            case DIV:    // x := y / z
                // opn1 -> $t1, opn2 -> $t2
                fprintf(pfile, "    lw $t1, %d($sp)\n", h->opn1.offset);
                fprintf(pfile, "    lw $t2, %d($sp)\n", h->opn2.offset);
                if (h->op == PLUS)
                    fprintf(pfile, "    add $t3,$t1,$t2\n");
                else if (h->op == MINUS)
                    fprintf(pfile, "    sub $t3,$t1,$t2\n");
                else if (h->op == STAR)
                    fprintf(pfile, "    mul $t3,$t1,$t2\n");
                else {
                    fprintf(pfile, "    div $t1, $t2\n");
                    fprintf(pfile, "    mflo $t3\n");
                }
                // 计算结果保存到临时变量的地址中
                fprintf(pfile, "    sw $t3, %d($sp)\n", h->result.offset);
                break;
            case FUNCTION:
                fprintf(pfile, "\n%s:\n", h->result.id);
                // 对main函数单独开辟栈帧
                if (!strcmp(h->result.id, "main")) {
                    if (symbolTable.symbols[h->result.offset].offset < 0) 
                        fprintf(pfile, "    addi $sp, $sp, %d\n", symbolTable.symbols[h->result.offset].offset);
                    else
                        fprintf(pfile, "    addi $sp, $sp, -%d\n", symbolTable.symbols[h->result.offset].offset);
                }
                break;
            case PARAM: break;
            case LABEL: fprintf(pfile, "%s:\n", h->result.id); break;
            case GOTO:  // j target
                fprintf(pfile, "    j %s\n", h->result.id);
                break;
            case JLE:
            case JLT:
            case JGE:
            case JGT:
            case EQ:
            case NEQ:  // lw $1,10($2)  -->  $1 = memory[$2 + 10]  将内存的值取出来
                fprintf(pfile, "    lw $t1, %d($sp)\n", h->opn1.offset);
                fprintf(pfile, "    lw $t2, %d($sp)\n", h->opn2.offset);
                // if rs relop rt then branch(转到label)
                if (h->op == JLE)
                    fprintf(pfile, "    ble $t1,$t2,%s\n", h->result.id);
                else if (h->op == JLT)
                    fprintf(pfile, "    blt $t1,$t2,%s\n", h->result.id);
                else if (h->op == JGE)
                    fprintf(pfile, "    bge $t1,$t2,%s\n", h->result.id);
                else if (h->op == JGT)
                    fprintf(pfile, "    bgt $t1,$t2,%s\n", h->result.id);
                else if (h->op == EQ)
                    fprintf(pfile, "    beq $t1,$t2,%s\n", h->result.id);
                else
                    fprintf(pfile, "    bne $t1,$t2,%s\n", h->result.id);
                break;
            case ARG: break;
            case CALL:
                if (!strcmp(h->opn1.id, "read")) {
                    fprintf(pfile, "    addi $sp, $sp, -4\n");  // rt <-- rs + imm立即数加
                    fprintf(pfile, "    sw $ra,0($sp)\n");
                    fprintf(pfile, "    jal read\n");
                    fprintf(pfile, "    lw $ra,0($sp)\n");
                    fprintf(pfile, "    addi $sp, $sp, 4\n");
                    fprintf(pfile, "    sw $v0, %d($sp)\n", h->result.offset);
                    break;
                }
                if (!strcmp(h->opn1.id, "write")) {
                    fprintf(pfile, "    lw $a0, %d($sp)\n", h->prior->result.offset);
                    fprintf(pfile, "    addi $sp, $sp, -4\n");
                    fprintf(pfile, "    sw $ra,0($sp)\n");
                    fprintf(pfile, "    jal write\n");
                    fprintf(pfile, "    lw $ra,0($sp)\n");
                    fprintf(pfile, "    addi $sp, $sp, 4\n");
                    break;
                }
                //读取参数个数
                for (pt = h, i = 0; i < symbolTable.symbols[h->opn1.offset].paramnum; i++) pt = pt->prior;
                fprintf(pfile, "    move $t0,$sp\n");
                fprintf(pfile, "    addi $sp,$sp,-%d\n", symbolTable.symbols[h->opn1.offset].offset);
                // $ra 返回地址
                fprintf(pfile, "    sw $ra,0($sp)\n");
                i = h->opn1.offset + 1;
                while (symbolTable.symbols[i].flag == 'P') {
                    fprintf(pfile, "    lw $t1,%d($t0)\n", pt->result.offset);
                    fprintf(pfile, "    move $t3,$t1\n");
                    fprintf(pfile, "    sw $t3,%d($sp)\n", symbolTable.symbols[i].offset);
                    pt = pt->next;
                    i++;
                }
                fprintf(pfile, "    jal %s\n", h->opn1.id);
                fprintf(pfile, "    lw $ra,0($sp)\n");
                fprintf(pfile, "    addi $sp,$sp,%d\n", symbolTable.symbols[h->opn1.offset].offset);
                fprintf(pfile, "    sw $v0,%d($sp)\n", h->result.offset);
                break;
            case RETURN:
                fprintf(pfile, "    lw $v0,%d($sp)\n", h->result.offset);
                fprintf(pfile, "    jr $ra\n");
                break;
        }
        h = h->next;
    } while (h != head);
    fclose(pfile);
}

void semantic_AnalysisInit(struct node *T, char *filename) {
    symbolTable.index = 0;
    fillSymbolTable("read", "", 0, INT, 'F', 4);
    symbolTable.symbols[0].paramnum = 0;  // read的形参个数
    fillSymbolTable("write", "", 0, INT, 'F', 4);
    symbolTable.symbols[1].paramnum = 1;
    fillSymbolTable("lyg", "", 1, INT, 'P', 12);
    symbol_scope_Stack.ScopeArray[0] = 0;  //外部变量在符号表中的起始序号为0
    symbol_scope_Stack.top = 1;
    T->offset = 0;  //外部变量在数据区的偏移量
    semantic_Analysis(T);
    //prnIR(T->code);  //在终端显示中间代码
    objectCode(T->code, filename);  //生成目标代码
}
