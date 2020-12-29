int fibo(int ret) {
    if (ret == 1 || ret == 2)
        return 1;
    else
        return fibo(ret - 1) + fibo(ret - 2);
}

int main() {
    int ret, num = read();
    ret = fibo(num);
    write(ret);
    return 0;
}