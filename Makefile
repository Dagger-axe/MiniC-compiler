chopper: chopper.l chopper.y ast.c
	bison -d chopper.y
	flex chopper.l
	cc -o chopper chopper.tab.c lex.yy.c ast.c -lfl
clean:
	rm -rf chopper