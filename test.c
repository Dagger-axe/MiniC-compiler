int main() {
    int i, num = 1;
    for (i = 1; i <= 5; i++) {  //测试3
        if (num == 3) break;
        num++;
    }
    write(num);
    return 0;
}