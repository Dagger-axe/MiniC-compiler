.PHONY:chopper chopper.o
chopper: chopper.l chopper.y
	bison -d chopper.y
	flex chopper.l
	cc -o chopper chopper.tab.c lex.yy.c semantic.c -lfl
clean:
	rm -rf chopper