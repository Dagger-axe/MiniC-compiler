### 概述

&emsp;&emsp;一个目标代码为`MIPS`的`C--`语言的编译器，该语言是C语言的子集，可以实现的功能有：

+ 变量声明：`int`、`float`、`double`、`char`类型的数据，支持十六进制的输入格式，同时支持数组；支持连续的变量声明和声明中的赋值运算，即支持`int a, b = 1, c = read();`的声明形式；声明必须在其他操作前完成，不能与其他操作混杂在一起；支持不同作用域中使用相同变量名。
+ 函数定义及返回值使用：仅允许函数定义一次，即不允许使用函数声明；支持对返回值的直接使用，即支持`Fibonacci数列`的递归实现。
+ 控制语句：支持`if-else`、`while`、`for`、`break`、`continue`，均支持嵌套；其中可以使用的条件包括对所有类型的各类比较操作。
+ 运算操作：支持`+=`、`-=`、`*=`、`/=`、`++`、`--`操作，不支持位运算。

### 编译

&emsp;&emsp;如果没有安装`flex`和`bison`，则先执行如下命令：

```bash
sudo apt-get install flex bison
```

&emsp;&emsp;若已安装则在工程文件夹中执行`make`命令即可：

```bash
lyg@DESKTOP-C3PNG35:/mnt/e/编译原理/实验/compiler$ make
bison -d chopper.y
flex chopper.l
cc -o chopper chopper.tab.c lex.yy.c ast.c semantic.c -lfl
lyg@DESKTOP-C3PNG35:/mnt/e/编译原理/实验/compiler$ ./chopper test.c
```

### 使用

```
./chopper [-h] <file-name> [object-file-path]
```

&emsp;&emsp;`Usage`信息如下：

```
Usage:    ./chopper [-h] <file-name> [object-file-path]

-h                  Shows this usage instructions
file-name           The name of the file to compile
object-file-path    The file name and path of the target file
```

### 整体架构及实现

+ **chopper.l：**用于定义基本词法。

+ **chopper.y：**用于定义基本语法（文法）。

+ **def.h：**用于定义语法树、数组、中间代码等结构，并用于声明语义分析、中间代码生成和目标代码生成中的函数。

+ **ast.c：**用于创建语法树并遍历语法树查看该语法树是否构建正确。（在最终编译的过程中仅用到了该文件中的十六进制转换函数`to_int()`和语法树节点构造函数`mknode()`）

+ **semantic.c：**用于完成语义分析、中间代码生成和目标代码生成。

### 测试

&emsp;&emsp;测试样例如下：

```c
int fibo(int num) {
    if (num == 1 || num == 2) return 1;
    else return fibo(num - 1) + fibo(num - 2);
}

int main() {
    int i = 1, j = 2, arr[2][3];  //测试1声明
    int num = 1;  //测试2声明
    char ch = 'a';

    arr[1][2] = read();  //测试1
    write(arr[1][2]);

    while (num <= 4) {  //测试2
        if (ch == 'a') {
            num += 1;
            ch = 'B';
            continue;
        } else if (num == 3) break;
        num++;
    }
    write(num);
    
    return 0;
}
```

#### 环境

&emsp;&emsp;在`Windows10`环境下使用`QtSpim`进行测试。

#### 结果

&emsp;&emsp;编译无错误信息输出：

```
lyg@DESKTOP-C3PNG35:/mnt/e/编译原理/实验/compiler$ ./chopper test.c obj.s
lyg@DESKTOP-C3PNG35:/mnt/e/编译原理/实验/compiler$ 
```

&emsp;&emsp;运行与预期相符：

```
Enter an integer:369
369
3
```

![image-20201230222507327](README.assets/image-20201230222507327.png)
