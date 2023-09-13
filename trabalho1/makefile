all: saida entrada.txt
	./saida < entrada.txt

lex.yy.c: teste.l
	lex teste.l

saida: lex.yy.c main.cc 
	g++ -std=c++17 main.cc -o saida -lfl
	
clean:
	rm saida lex.yy.c
	