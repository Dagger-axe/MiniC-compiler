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