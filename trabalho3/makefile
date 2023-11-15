all: tp3 codigo
	echo "=== Execução ==="
	./new_interpreter/interpretador < codigo
	
codigo: tp3 entrada.txt
	./tp3 < entrada.txt > codigo
	echo "=== Codigo ==="
	cat codigo

tp3: lex.yy.c y.tab.c
	g++ y.tab.c -o tp3 -lfl	
	
lex.yy.c: tp3.l
	lex tp3.l
	
y.tab.c: tp3.y
	yacc tp3.y
	
clean: 
	rm -f lex.yy.c y.tab.c tp3