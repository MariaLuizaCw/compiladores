

[] identificadores com mais de uma letra
[] func
[] números inteiros e reais
[] string
[x] comando print
[x] funções com zero ou mais parametros
[x] operadores - e + unários
[x] fatorial 


Gramática Normal


S ->  A; S
      print E print("print #"); S 
      vazio

A -> print(lexema) id = E print("= ^")


E -> E + T print("+")
     E - T print("-")
     T



T -> T * U print("*")
     T/U   print("/")
     U

U -> print("0")  -U  print("-")
     print("0")  +U  print("+")
     P


P -> FAT^P  print("^")
     FAT

P -> FAT^P print("^")
    | vazio  
 

FAT -> FAT!  print("fat #")
     | F

F -> print("lexema @") ID  
     print(lexema)  NUM 
     print(lexema) FLOAT 
     print(lexema) STRING 
     (E)
     func(ARG)   print("func #") 


obs: guardar func em uma variável local

ARGS -> E , ARGS
        vazio


Gramática Fatorada Simplificada

S ->  A; S 
      vazio

A -> print(lexema) id = E print("= ^")
     print E print("print #"); 

E -> T E'
E' ->  + T print("+") E'
     | - T print("-") E'
     | vazio

T -> UT'

T' -> * U print("*") T'
     | / U   print("/") T'
     | vazio


U -> print("0")  -U  print("-")
     print("0")  +U  print("+")
     P

P -> FAT RP

RP -> ^P  print("^")
     | vazio

 
FAT -> FAT!  print("fat #")
     | F

FAT -> F FAT'

FAT' ->  ! print("fat #") FAT'
     | vazio


F -> print("lexema @") ID  
     print(lexema)  NUM 
     print(lexema) FLOAT 
     print(lexema) STRING 
     (E)






------------------------------------------------------------



teste 10:
a = --( - 4 - 2 + 10 );
print +-+---a!;






teste 8:
a = -8!;
b = (2 * 2 + 1)!/3!;
print b! / -a ^ 2;


teste 9:

a = 2 ^ 9 ^ 0.5;
b = 3.1 * a ^ a ^ (1/3!) + 1;
print -b ^ 2!;


teste 11:
x = "maior: ";
a = - 2.1;
b = 3.1;
print x + dtos( max( a, b ) );


teste 12:
x = 1;
y = "1";
z = x + y;

teste 13:
x = 1;
y = 2;
a = mdc( x, y );

teste 14:
b = (-8)!;

teste 15:
a = 0;
print a^(-0!);