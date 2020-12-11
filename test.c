int func(int xx, float yy){
	xx = xx + yy;
	return 1;
}
int inc(){
	int a = 0;
	float b = 1.1;
	func(a, b);
	return 0;
}