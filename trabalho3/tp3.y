%{
#include <iostream>
#include <string>
#include <vector>
#include <map>

using namespace std;

int linha = 1, coluna = 1; 

struct Atributos {
  vector<string> c; // Código

  int linha = 0, coluna = 0;

  void clear() {
    c.clear();
    linha = 0;
    coluna = 0;
  }
};

#define YYSTYPE Atributos

extern "C" int yylex();
int yyparse();
void yyerror(const char *);

vector<string> concatena( vector<string> a, vector<string> b ) {
  a.insert( a.end(), b.begin(), b.end() );
  return a;
}

vector<string> operator+( vector<string> a, vector<string> b ) {
  return concatena( a, b );
}

vector<string> operator+( vector<string> a, string b ) {
  a.push_back( b );
  return a;
}

vector<string> operator+( string a, vector<string> b ) {
  return vector<string>{ a } + b;
}

vector<string> resolve_enderecos( vector<string> entrada ) {
  map<string,int> label;
  vector<string> saida;
  for( int i = 0; i < entrada.size(); i++ ) 
    if( entrada[i][0] == ':' ) 
        label[entrada[i].substr(1)] = saida.size();
    else
      saida.push_back( entrada[i] );
  
  for( int i = 0; i < saida.size(); i++ ) 
    if( label.count( saida[i] ) > 0 )
        saida[i] = to_string(label[saida[i]]);
    
  return saida;
}

string gera_label( string prefixo ) {
  static int n = 0;
  return prefixo + "_" + to_string( ++n ) + ":";
}

void print( vector<string> codigo ) {
  for( string s : codigo )
    cout << s << " ";
    
  cout << endl;  
}

void cmd_if_else(Atributos& ss, Atributos& s_cond, Atributos& s_false, Atributos& s_true){
    string lbl_true = gera_label( "lbl_true" );
    string lbl_fim_if = gera_label( "lbl_fim_if" );
    string definicao_lbl_true = ":" + lbl_true;
    string definicao_lbl_fim_if = ":" + lbl_fim_if;
    ss.c = s_cond.c +                          // Codigo da expressão
    lbl_true + "?" +                   // Código do IF
    s_false.c + lbl_fim_if + "#" +    // Código do False
    definicao_lbl_true + s_true.c +  // Código do True
    definicao_lbl_fim_if;           // Fim do IF
    
}



void cmd_for(Atributos& ss, Atributos& s_dec, Atributos& s_cond, Atributos& s_cmd, Atributos& s_it){

    string lbl_fim_for = gera_label( "fim_for" );
    string lbl_condicao_for = gera_label( "condicao_for" );
    string lbl_comando_for = gera_label( "comando_for" );
    string definicao_lbl_fim_for = ":" + lbl_fim_for;
    string definicao_lbl_condicao_for = ":" + lbl_condicao_for;
    string definicao_lbl_comando_for = ":" + lbl_comando_for;
    
   ss.c = s_dec.c + definicao_lbl_condicao_for +
                 s_cond.c + lbl_comando_for + "?" + lbl_fim_for + "#" +
                 definicao_lbl_comando_for + s_cmd.c + 
                 s_it.c + "^" + lbl_condicao_for + "#" +
                 definicao_lbl_fim_for;
}

%}

%token ID IF ELSE LET PRINT FOR MAIS_IGUAL
%token CDOUBLE CSTRING CINT
%token AND OR ME_IG MA_IG DIF IGUAL
%token MAIS_IGUAL MAIS_MAIS

%right '='
%nonassoc '<' '>'
%left '+' '-'
%left '*' '/' '%'
%left '['
%left '.'


%%

S : CMDs { print( resolve_enderecos( $1.c + "." ) ); }
  ;

CMDs : CMDs CMD  { $$.c = $1.c + $2.c; };
     | {$$.clear();}
     ;
     
CMD : CMD_LET ';'
    | CMD_IF
    | PRINT E ';' 
      { $$.c = $2.c + "println" + "#"; }
    | CMD_FOR
    | E ';'   { $$.c = $1.c + "^"; }
    | '{' CMDs '}' {$$.c = $2.c;}
    ;
 
CMD_FOR : FOR '(' PRIM_E ';' E ';' E ')' CMD 
        { 
            cmd_for($$, $3, $5, $9, $7);
        }
        ;

PRIM_E : CMD_LET 
       | E  { $$.c = $1.c + "^"; }
       ;

CMD_LET : LET VARs { $$.c = $2.c; }
        ;

VARs : VAR ',' VARs { $$.c = $1.c + $3.c; } 
     | VAR
     ;

VAR : ID                { $$.c = $1.c + "&"; }
    | ID '=' E          { $$.c = $1.c + "&" + $1.c + $3.c + "=" + "^"; }
    | ID '=' '{' '}'    { $$.c = $1.c + "&" +  $1.c +  vector<string>{"{}"} + "=" + "^";} 
    ;
     
CMD_IF : IF '(' E ')' CMD ELSE CMD
         {       
            cmd_if_else($$, $3, $7, $5);
         }
       ;
        
LVALUE : ID 
       ;
       
LVALUEPROP : E '[' E ']'
           | E '.' ID
           ;

// RVALUE : '{' '}' {$$.c = vector<string>{"{}"};}
//        | '[' ']' {$$.c = vector<string>{"[]"};}
//        ;



E : LVALUE '=' E        { $$.c = $1.c + $3.c + "="; }
  | LVALUE '=' '{' '}'  { $$.c = $1.c + vector<string>{"{}"} + "=";} 
  | LVALUEPROP '=' E 	
  | LVALUE MAIS_IGUAL E { $$.c = $1.c + $1.c +  "@" + $3.c +  "+" + "=";}
  | E '<' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '>' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '+' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '-' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '*' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '/' E             { $$.c = $1.c + $3.c + $2.c; }
  | E '%' E             { $$.c = $1.c + $3.c + $2.c; }
  | CDOUBLE
  | '[' ']'             {$$.c = vector<string>{"[]"};}
  | CINT
  | CSTRING
  | LVALUE 
    { $$.c = $1.c + "@"; } 
  | LVALUEPROP
  | '(' E ')'           { $$.c = $2.c; }
  ;
  
  
%%

#include "lex.yy.c"

void yyerror( const char* st ) {
   puts( st ); 
   printf( "Proximo a: %s\n", yytext );
   exit( 0 );
}

int main( int argc, char* argv[] ) {
  yyparse();
  
  return 0;
}