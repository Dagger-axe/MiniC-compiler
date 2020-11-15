cc=g++
chopper: chopper.l chopper.y
	bison -d chopper.y
	flex chopper.l
	cc -o $@ chopper.tab.c lex.yy.c ast.c -lfl