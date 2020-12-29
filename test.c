int fibo(int xx) {
	if (xx == 1 || xx == 2) {
		return 1;
	}
	else {
		return fibo(xx - 1) + fibo(xx - 2);
	}
}

int main() {
	int num, ret;
	num = read();
	ret = fibo(num);
	write(ret);
	return 0;
}