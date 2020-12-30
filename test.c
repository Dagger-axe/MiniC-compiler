int fibo(int ret) {
/*  1   2   3   4   5   6   7   8   9   10
 *  1   1   2   3   5   8   13  21  34  55
 */
    if (ret == 1 || ret == 2)
        return 1;
    else
        return fibo(ret - 1) + fibo(ret - 2);
}

int main() {    
    char ch = 'a';
    int ret = 1, num = read();
    double aa = 1.3;
    while (ret <= 4) {   //增加5次
        if (ret == 3) {
            ch = 'b';
            ret += 2;
            aa += 2.1;
            continue;
        }
        if (ret == 4) break;  //if 限制仅增加3次
        ret++;
    }
    write(ret);
    return 0;
}