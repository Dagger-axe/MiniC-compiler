#include "def.h"

void displayAST(struct node *T, int indent) {  //对抽象语法树的先根遍历
    int i = 1;
    struct node *T0;
    //遍历数组所需
    int arrdimt;  //数组维度的判断标准
    int *arrdimt_info;  //数组维度信息
    char arrname[32];  //数组名
    struct node **array;  //数组指针
    if (T) {
        switch (T->kind) {
            case EXT_DEF_LIST: {
                displayAST(T->ptr[0], indent);  //显示该外部定义列表中的第一个
                displayAST(T->ptr[1], indent);  //显示外部定义列表中的其它外部定义
                break;
            }
            case EXT_VAR_DEF: {
                printf("%*c外部变量定义：\n", indent, ' ');
                displayAST(T->ptr[0], indent + 3);  //显示外部变量类型
                printf("%*c变量名：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6);  //显示变量列表
                break;
            }
            case TYPE: {
                printf("%*c类型： %s\n", indent, ' ', T->type_id);
                break;
            }
            case ARR_DEC:
                printf("%*c数组声明：\n", indent, ' ');
                T0 = T;
                arrdimt = 1;
                while (T0->ptr[0]->kind == ARR_DEC) {
                    T0 = T0->ptr[0];
                    arrdimt += 1;  //确定多维数组的维度
                }
                T0 = T;
                arrdimt_info = (int*)malloc(sizeof(int) * arrdimt);  //储存数组维度信息
                i = arrdimt;
                while (T0->ptr[0]) {
                    i--;
                    arrdimt_info[i] = T0->type_int;   //获取数组维度
                    if(!i) strcpy(arrname, T0 -> ptr[0] -> type_id);
                    T0 = T0->ptr[0];
                }
                printf("%*c维度：", indent + 3, ' ');
                while (i < arrdimt - 1) {
                    printf("[%d]", arrdimt_info[i]);
                    i++;
                }
                printf("\n");
                printf("%*c数组名：%s\n", indent + 3, ' ', arrname);
                free(arrdimt_info);
                break;
            case ARR_EXP:
                printf("%*c数组内部表达式：\n", indent, ' ');
                T0 = T;
                arrdimt = 1;
                while (T0->ptr[0]->kind != ID) {
                    T0 = T0->ptr[0];
                    arrdimt++;
                }
                T0 = T;
                array = (struct node **)malloc(sizeof(struct node *) * arrdimt);
                i = arrdimt;
                while (T0->ptr[0]) {
                    i--;
                    array[i] = T0;
                    T0 = T0->ptr[0];
                }
                printf("%*c数组名：%s\n", indent + 3, ' ', array[0]->ptr[0]->type_id);
                while (i < arrdimt)
                {
                    printf("%*c数组第%d维度表达式：\n", indent + 3, ' ', i + 1);
                    displayAST(array[i]->ptr[1], indent + 6);
                    i++;
                }
                free(array);
                break;
            case EXT_DEC_LIST: {
                displayAST(T->ptr[0], indent);  //依次显示外部变量名，
                displayAST(T->ptr[1], indent);  //后续还有相同的，仅显示语法树此处理代码可以和类似代码合并
                break;
            }
            case FUNC_DEF: {
                printf("%*c函数定义：\n", indent, ' ');
                displayAST(T->ptr[0], indent + 3);  //显示函数返回类型
                displayAST(T->ptr[1], indent + 3);  //显示函数名和参数
                displayAST(T->ptr[2], indent + 3);  //显示函数体
                break;
            }
            case FUNC_DEC: {
                printf("%*c函数名：%s\n", indent, ' ', T->type_id);
                if (T->ptr[0]) {
                    printf("%*c函数形参：\n", indent, ' ');
                    displayAST(T->ptr[0], indent + 3);  //显示函数参数列表
                } else
                    printf("%*c无参函数\n", indent + 3, ' ');
                break;
            }
            case PARAM_LIST: {
                displayAST(T->ptr[0], indent);  //依次显示全部参数类型和名称，
                displayAST(T->ptr[1], indent);
                break;
            }
            case PARAM_DEC: {
                printf("%*c类型：%s, 参数名：%s\n", indent, ' ', T->ptr[0]->type == INT ? "int" : "float", T->ptr[1]->type_id);
                break;
            }
            case EXP_STMT: {
                printf("%*c表达式语句：\n", indent, ' ');
                displayAST(T->ptr[0], indent + 3);
                break;
            }
            case RETURN: {
                printf("%*c返回语句：\n", indent, ' ');
                displayAST(T->ptr[0], indent + 3);
                break;
            }
            case COMP_STM: {
                printf("%*c复合语句：\n", indent, ' ');
                printf("%*c复合语句的变量定义：\n", indent + 3, ' ');
                displayAST(T->ptr[0], indent + 6);  //显示定义部分
                printf("%*c复合语句的语句部分：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6);  //显示语句部分
                break;
            }
            case STM_LIST: {
                displayAST(T->ptr[0], indent);  //显示第一条语句
                displayAST(T->ptr[1], indent);  //显示剩下语句
                break;
            }
            case WHILE: {
                printf("%*c循环语句：\n", indent, ' ');
                printf("%*c循环条件：\n", indent + 3, ' ');
                displayAST(T->ptr[0], indent + 6);  //显示循环条件
                printf("%*c循环体：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6);  //显示循环体
                break;
            }
            case FOR:			
                printf("%*c循环语句：\n", indent, ' ');
                printf("%*c循环条件：\n", indent + 3, ' ');
                displayAST(T->ptr[0], indent + 6); //显示循环条件
                printf("%*c循环体：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6); //显示循环体
                break;
            case FOR_DEC:
                displayAST(T->ptr[0], indent + 6);
                displayAST(T->ptr[1], indent + 6);
                displayAST(T->ptr[2], indent + 6);
                break;
            case BREAK: {
                printf("%*c循环终止：BREAK\n", indent, ' ');
                break;
            }
            case CONTINUE: {
                printf("%*c循环继续：CONTINUE\n", indent, ' ');
                break;
            }
            case IF_THEN: {
                printf("%*c条件语句(IF_THEN)：\n", indent, ' ');
                printf("%*c条件：\n", indent + 3, ' ');
                displayAST(T->ptr[0], indent + 6);  //显示条件
                printf("%*cIF子句：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6);  //显示if子句
                break;
            }
            case IF_THEN_ELSE: {
                printf("%*c条件语句(IF_THEN_ELSE)：\n", indent, ' ');
                printf("%*c条件：\n", indent + 3, ' ');
                displayAST(T->ptr[0], indent + 6);  //显示条件
                printf("%*cIF子句：\n", indent + 3, ' ');
                displayAST(T->ptr[1], indent + 6);  //显示if子句
                printf("%*cELSE子句：\n", indent + 3, ' ');
                displayAST(T->ptr[2], indent + 6);  //显示else子句
                break;
            }
            case DEF_LIST: {
                displayAST(T->ptr[0], indent);  //显示该局部变量定义列表中的第一个
                displayAST(T->ptr[1], indent);  //显示其它局部变量定义
                break;
            }
            case VAR_DEF: {
                printf("%*cLOCAL VAR_NAME：\n", indent, ' ');
                displayAST(T->ptr[0], indent + 3);  //显示变量类型
                displayAST(T->ptr[1], indent + 3);  //显示该定义的全部变量名
                break;
            }
            case DEC_LIST: {
                printf("%*cVAR_NAME：\n", indent, ' ');
                T0 = T;
                while (T0) {
                    if (T0->ptr[0]->kind == ID)
                        printf("%*c %s\n", indent + 3, ' ', T0->ptr[0]->type_id);
                    else if (T0->ptr[0]->kind == ASSIGNOP) {
                        printf("%*c %s ASSIGNOP\n ", indent + 3, ' ',
                               T0->ptr[0]->ptr[0]->type_id);  //输出num = 1;的num
                        //显示初始化表达式
                        displayAST(T0->ptr[0]->ptr[1], indent + strlen(T0->ptr[0]->ptr[0]->type_id) + 4);
                    } else if (T0->ptr[0]->kind == ARR_DEC) {
                        printf("%*c数组声明：\n", indent+3, ' ');
                        T0 = T;
                        arrdimt = 1;
                        while (T0->ptr[0]->kind != ID) {
                            T0 = T0->ptr[0];
                            arrdimt++;
                        }
                        T0 = T;
                        array = (struct node **)malloc(sizeof(struct node *) * arrdimt);
                        i = arrdimt;
                        while (T0->ptr[0]) {
                            i--;
                            array[i] = T0;
                            T0 = T0->ptr[0];
                        }
                        printf("%*c维度：", indent + 6, ' ');
                        while (i < arrdimt - 1) {
                            printf("[%d]", (array[i])->type_int);
                            i++;
                        }
                        printf("\n");
                        printf("%*c数组名：%s\n", indent + 6, ' ', array[0]->ptr[0]->type_id);
                        free(array);
                        break;  //已深度遍历，提前break
					}
                    T0 = T0->ptr[1];
                }
                break;
            }
            case ID: {
                printf("%*cID： %s\n", indent, ' ', T->type_id);
                break;
            }
            case INT: {
                printf("%*cINT：%d\n", indent, ' ', T->type_int);
                break;
            }
            case FLOAT: {
                printf("%*cFLAOT：%f\n", indent, ' ', T->type_float);
                break;
            }
            case CHAR: {
                printf("%*cCHAR：%s\n", indent, ' ', T->type_char);
                break;
            }
            case ASSIGNOP:  //以下均会向下执行DIV处，因为执行过程一致
            case AND:
            case OR:
            case RELOP:
            case PLUSOP:
            case MINUSOP:
            case STAROP:
            case DIVOP:
            case PLUS:
            case MINUS:
            case INC:
            case DEC:
            case STAR:
            case DIV: {
                printf("%*c%s\n", indent, ' ', T->type_id);
                displayAST(T->ptr[0], indent + 3);
                displayAST(T->ptr[1], indent + 3);
                break;
            }
            case NOT:
            case UMINUS: {
                printf("%*c%s\n", indent, ' ', T->type_id);
                displayAST(T->ptr[0], indent + 3);
                break;
            }
            case FUNC_CALL: {
                printf("%*c函数调用：\n", indent, ' ');
                printf("%*c函数名：%s\n", indent + 3, ' ', T->type_id);
                displayAST(T->ptr[0], indent + 3);
                break;
            }
            case ARGS: {
                i = 1;
                while (T) { /* ARGS表示实际参数表达式序列结点，其第一棵子树为头部实际参数表达式，第二棵子树为剩下的实际参数
                             */
                    struct node *T0 = T->ptr[0];
                    printf("%*c第%d个实际参数表达式：\n", indent, ' ', i++);
                    displayAST(T0, indent + 3);
                    T = T->ptr[1];
                }
                break;
            }
        }
    }
}