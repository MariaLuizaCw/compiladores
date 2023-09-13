%{
// Código em c/c++

#include <iostream>

using namespace std;

string lexema;

%}

DIGITO  [0-9]
LETRA   [A-Za-z]
NUM     {DIGITO}+
UNDERLINE   "_"
CIFRAO   "$"
ID      ({LETRA}|{UNDERLINE}|{CIFRAO})({LETRA}|{DIGITO}|{UNDERLINE})*


%% 

[ \n\r\t]   { }
 
 
  /* Todas as palavras reservadas devem aparecer antes do padrão do ID */

[Ff][Oo][Rr]                       { lexema = yytext; return _FOR; }
[Ii][Ff]                           { lexema = yytext; return _IF; }

{ID}                               {lexema = yytext; return _ID; }

{NUM}                              {lexema = yytext; return _INT; }
 
{NUM}([.]{NUM})?([eE][-+]?{NUM})?  { lexema = yytext; return _FLOAT; }



">="                               { lexema = yytext; return _MAIG; }

"<="                               { lexema = yytext; return _MEIG; }

"=="                               { lexema = yytext; return _IG; }
 
"!="                                { lexema = yytext; return _DIF; }

  /* Trata todos os símbolos de um caracter usando o código ASCII */
.                                  { lexema = yytext; return yytext[0]; }
 
%%