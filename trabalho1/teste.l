%{
// Código em c/c++

#include <iostream>

using namespace std;

string lexema;

string remove_first_last_scapes(char aspas, string lexema){
    string new_lexema = "";
     for(int i = 1; i < lexema.length()-1; i++){
        if (lexema[i] == '\\' & lexema[i+1] == aspas)
            continue;
        if (lexema[i] == aspas & lexema[i+1] == aspas)
            continue;
        new_lexema += lexema[i];
    }
    return new_lexema;
};

string remove_first_last(string lexema, int n_first, int n_last){
    int s = lexema.length();
    return lexema.substr(n_first, (s-1)-n_last);
};

%}

DIGITO  [0-9]
LETRA   [A-Za-z]
NUM     {DIGITO}+
UNDERLINE   "_"
CIFRAO   "$"
ID      ({LETRA}|{UNDERLINE}|{CIFRAO})({LETRA}|{DIGITO}|{UNDERLINE})*
FALSOID ({LETRA}|{UNDERLINE}|{CIFRAO})({LETRA}|{CIFRAO}|{DIGITO}|{UNDERLINE})*

BEGING_FORMAT "${"
END_FORMAT "}"
HALFFORMAT  ({BEGING_FORMAT}{ID})



BEGIN_COMENT1 "/*"
END_COMENT1 "*/"
BREAKERS [ \n\t\r]
ANYTHING_NOT_STAR_AND_BAR ([^*/])
STAR_BUT_NOT_BAR (\*[^\/])
BAR_BUT_NOT_STAR ([^\*]\/)
MIDLE ({ANYTHING_NOT_STAR_AND_BAR}|{STAR_BUT_NOT_BAR}|{BAR_BUT_NOT_STAR})*
COMENT1 ({BEGIN_COMENT1})({MIDLE}|{BREAKERS})({END_COMENT1})



BEGIN_COMENT2 "//"
COMENT2 ({BEGIN_COMENT2})(.)*

%% 

{COMENT2}                               {
                                            lexema = yytext;  
                                            return _COMENTARIO;
                                        }

{COMENT1}                               {
                                            lexema = yytext;  
                                            return _COMENTARIO;
                                        }

[`|}]([^$])+/{HALFFORMAT}               {
                                            lexema = yytext;      
                                            lexema = remove_first_last(lexema, 1, 0);
                                            return _STRING2; 
                                        }




{HALFFORMAT}                            {
                                            lexema = yytext;      
                                            lexema = remove_first_last(lexema, 2, 0);
                                            return 268; 
                                        }

\}([^$])+\`                             {
                                            lexema = yytext;      
                                            lexema = remove_first_last(lexema, 1, 1);
                                            return _STRING2; 
                                        }

\`([^{]|\n])+\`                           {
                                            lexema = yytext;      
                                            lexema = remove_first_last(lexema, 1, 1);
                                            return _STRING2; 
                                        }


\'([^']|\\\'|\'\')+\'                   { 
                                            lexema = yytext;      
                                            lexema = remove_first_last_scapes('\'', lexema);
                                            return _STRING; 
                                        }
 

\"([^"]|\\\"|\"\")+\"                   { 
                                            lexema = yytext;      
                                            lexema = remove_first_last_scapes('\"', lexema);
                                            return _STRING; 
                                        }
  
[ \n\r\t"'{}`]   { }

[Ff][Oo][Rr]                       { lexema = yytext; return _FOR; }
[Ii][Ff]                           { lexema = yytext; return _IF; }

{ID}                               {lexema = yytext; return _ID; }

{FALSOID}                          {lexema = yytext; cout << "Erro: Identificador invalido: " << lexema << endl; }

{NUM}                              {lexema = yytext; return _INT; }
 
{NUM}([.]{NUM})?([eE][-+]?{NUM})?  { lexema = yytext; return _FLOAT; }



">="                               { lexema = yytext; return _MAIG; }

"<="                               { lexema = yytext; return _MEIG; }

"=="                               { lexema = yytext; return _IG; }
 
"!="                                { lexema = yytext; return _DIF; }

.                                  { lexema = yytext; return yytext[0]; }
 
%%