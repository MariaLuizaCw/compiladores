%{
#include 
#include 
#include 
#include 

using namespace std;

int linha = 1, coluna = 0; 

struct Atributos {
  vector c; // Código

  int linha = 0, coluna = 0;

  // Só para argumentos e parâmetros
  int contador = 0;     
  
  // Só para valor default de argumento        
  vector valor_default; 

  void clear() {
    c.clear();
    valor_default.clear();
    linha = 0;
    coluna = 0;
    contador = 0;
  }
};

enum TipoDecl { Let = 1, Const, Var };
map nomeTipoDecl = { 
  { Let, "let" }, 
  { Const, "const" }, 
  { Var, "var" }
};

struct Simbolo {
  TipoDecl tipo;
  int linha;
  int coluna;
};

int in_func = 0;

// Tabela de símbolos - agora é uma pilha
vector< map< string, Simbolo > > ts = { map< string, Simbolo >{} }; 
vector funcoes;

vector declara_var( TipoDecl tipo, string nome, int linha, int coluna );
void checa_simbolo( string nome, bool modificavel );

#define YYSTYPE Atributos

extern "C" int yylex();
int yyparse();
void yyerror(const char *);

vector concatena( vector a, vector b ) {
  a.insert( a.end(), b.begin(), b.end() );
  return a;
}

vector operator+( vector a, vector b ) {
  return concatena( a, b );
}

vector operator+( vector a, string b ) {
  a.push_back( b );
  return a;
}

vector operator+( string a, vector b ) {
  return vector{ a } + b;
}

vector resolve_enderecos( vector entrada ) {
  map label;
  vector saida;
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

void print( vector codigo ) {
  for( string s : codigo )
    cout << s << " ";
    
  cout << endl;  
}
%}

%token ID IF ELSE LET CONST VAR PRINT FOR FUNCTION
%token CDOUBLE CSTRING CINT
%token AND OR ME_IG MA_IG DIF IGUAL
%token MAIS_IGUAL MAIS_MAIS

%right '='
%nonassoc '<' '>'
%left '+' '-'
%left '*' '/' '%'

%right '[' '('
%left '.'


%%

S : CMDs { print( resolve_enderecos( $1.c + "." + funcoes ) ); }
  ;

CMDs : CMDs CMD  { $_$.c = $1.c + $2.c; };
     |           { $_$.clear(); }
     ;
            
    
CMD : CMD_LET ';'
    | CMD_VAR ';'
    | CMD_CONST ';'
    | CMD_IF
    | CMD_FUNC
    | PRINT E ';' 
      { $_$.c = $2.c + "println" + "#"; }
    | CMD_FOR
    | E ';'
      { $_$.c = $1.c + "^"; };
    | '{' EMPILHA_TS CMDs '}'
      { ts.pop_back();
        $_$.c = "<{" + $3.c + "}>"; }
    ;
    
EMPILHA_TS : { ts.push_back( map< string, Simbolo >{} ); } 
           ;
    
CMD_FUNC : FUNCTION ID { declara_var( Var, $2.c[0], $2.linha, $2.coluna ); } 
             '(' EMPILHA_TS LISTA_PARAMs ')' '{' CMDs '}'
           { 
             string lbl_endereco_funcao = gera_label( "func_" + $2.c[0] );
             string definicao_lbl_endereco_funcao = ":" + lbl_endereco_funcao;
             
             $_$.c = $2.c + "&" + $2.c + "{}"  + "=" + "'&funcao'" +
                    lbl_endereco_funcao + "[=]" + "^";
             funcoes = funcoes + definicao_lbl_endereco_funcao + $6.c + $9.c +
                       "undefined" + "@" + "'&retorno'" + "@"+ "~";
             ts.pop_back(); 
           }
         ;
         
LISTA_PARAMs : PARAMs
           | { $_$.clear(); }
           ;
           
PARAMs : PARAMs ',' PARAM  
       { // a & a arguments @ 0 [@] = ^ 
         $_$.c = $1.c + $3.c + "&" + $3.c + "arguments" + "@" + to_string( $1.contador )
                + "[@]" + "=" + "^"; 
                
         if( $3.valor_default.size() > 0 ) {
           // Gerar código para testar valor default.
         }
         $_$.contador = $1.contador + $3.contador; 
       }
     | PARAM 
       { // a & a arguments @ 0 [@] = ^ 
         $_$.c = $1.c + "&" + $1.c + "arguments" + "@" + "0" + "[@]" + "=" + "^"; 
                
         if( $1.valor_default.size() > 0 ) {
           // Gerar código para testar valor default.
         }
         $_$.contador = $1.contador; 
       }
     ;
     
PARAM : ID 
      { $_$.c = $1.c;      
        $_$.contador = 1;
        $_$.valor_default.clear();
        declara_var( Let, $1.c[0], $1.linha, $1.coluna ); 
      }
    | ID '=' E
      { // Código do IF
        $_$.c = $1.c;
        $_$.contador = 1;
        $_$.valor_default = $3.c;         
        declara_var( Let, $1.c[0], $1.linha, $1.coluna ); 
      }
    ;
 
CMD_FOR : FOR '(' PRIM_E ';' E ';' E ')' CMD 
        { string lbl_fim_for = gera_label( "fim_for" );
          string lbl_condicao_for = gera_label( "condicao_for" );
          string lbl_comando_for = gera_label( "comando_for" );
          string definicao_lbl_fim_for = ":" + lbl_fim_for;
          string definicao_lbl_condicao_for = ":" + lbl_condicao_for;
          string definicao_lbl_comando_for = ":" + lbl_comando_for;
          
          $_$.c = $3.c + definicao_lbl_condicao_for +
                 $5.c + lbl_comando_for + "?" + lbl_fim_for + "#" +
                 definicao_lbl_comando_for + $9.c + 
                 $7.c + "^" + lbl_condicao_for + "#" +
                 definicao_lbl_fim_for;
        }
        ;

PRIM_E : CMD_LET 
       | CMD_VAR
       | CMD_CONST
       | E  
         { $_$.c = $1.c + "^"; }
       ;

CMD_LET : LET LET_VARs { $_$.c = $2.c; }
        ;

LET_VARs : LET_VAR ',' LET_VARs { $_$.c = $1.c + $3.c; } 
         | LET_VAR
         ;

LET_VAR : ID  
          { $_$.c = declara_var( Let, $1.c[0], $1.linha, $1.coluna ); }
        | ID '=' E
          { 
            $_$.c = declara_var( Let, $1.c[0], $1.linha, $1.coluna ) + 
                   $1.c + $3.c + "=" + "^"; }
        ;
  
CMD_VAR : VAR VAR_VARs { $_$.c = $2.c; }
        ;
        
VAR_VARs : VAR_VAR ',' VAR_VARs { $_$.c = $1.c + $3.c; } 
         | VAR_VAR
         ;

VAR_VAR : ID  
          { $_$.c = declara_var( Var, $1.c[0], $1.linha, $1.coluna ); }
        | ID '=' E
          {  $_$.c = declara_var( Var, $1.c[0], $1.linha, $1.coluna ) + 
                    $1.c + $3.c + "=" + "^"; }
        ;
  
CMD_CONST: CONST CONST_VARs { $_$.c = $2.c; }
         ;
  
CONST_VARs : CONST_VAR ',' CONST_VARs { $_$.c = $1.c + $3.c; } 
           | CONST_VAR
           ;

CONST_VAR : ID '=' E
            { $_$.c = declara_var( Const, $1.c[0], $1.linha, $1.coluna ) + 
                     $1.c + $3.c + "=" + "^"; }
          ;
  
CMD_IF : IF '(' E ')' CMD ELSE CMD
         { string lbl_true = gera_label( "lbl_true" );
           string lbl_fim_if = gera_label( "lbl_fim_if" );
           string definicao_lbl_true = ":" + lbl_true;
           string definicao_lbl_fim_if = ":" + lbl_fim_if;
                    
            $_$.c = $3.c +                       // Codigo da expressão
                   lbl_true + "?" +             // Código do IF
                   $7.c + lbl_fim_if + "#" +    // Código do False
                   definicao_lbl_true + $5.c +  // Código do True
                   definicao_lbl_fim_if         // Fim do IF
                   ;
         }
       ;
        
LVALUE : ID 
       ;
       
LVALUEPROP : E '[' E ']'
           | E '.' ID  
           ;

E : LVALUE '=' '{' '}'
    { checa_simbolo( $1.c[0], true ); $_$.c = $1.c + "{}" + "="; }
  | LVALUE '=' E 
    { checa_simbolo( $1.c[0], true ); $_$.c = $1.c + $3.c + "="; }
  | LVALUEPROP '=' E 	
  | E '<' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '>' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '+' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '-' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '*' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '/' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '%' E
    { $_$.c = $1.c + $3.c + $2.c; }
  | E '(' LISTA_ARGs ')'
    {
      $_$.c = $3.c + to_string( $3.contador ) + $1.c + "$";
    }
  | CDOUBLE
  | CINT
  | LVALUE 
    { checa_simbolo( $1.c[0], false );
      $_$.c = $1.c + "@"; } 
  | LVALUEPROP  
  | '(' E ')'
    { $_$.c = $2.c; }
   
  | '(' '{' '}' ')'
    { $_$.c = vector{"{}"}; }
  ;
  
LISTA_ARGs : ARGs
           | { $_$.clear(); }
           ;
             
ARGs : ARGs ',' E
       { $_$.c = $1.c + $3.c;
         $_$.contador = $1.contador + $3.contador; }
     | E
       { $_$.c = $1.c;
         $_$.contador = 1; }
     ;
             
             
  
%%

#include "lex.yy.c"

vector declara_var( TipoDecl tipo, string nome, int linha, int coluna ) {
//  cerr << "insere_simbolo( " << tipo << ", " << nome 
//       << ", " << linha << ", " << coluna << ")" << endl;
       
  auto& topo = ts.back();    
       
  if( topo.count( nome ) == 0 ) {
    topo[nome] = Simbolo{ tipo, linha, coluna };
    return vector{ nome, "&" };
  }
  else if( tipo == Var && topo[nome].tipo == Var ) {
    topo[nome] = Simbolo{ tipo, linha, coluna };
    return vector{};
  } 
  else {
    cerr << "Redeclaração de '" << nomeTipoDecl[topo[nome].tipo] << " " << nome 
         << "' na linha: " << topo[nome].linha 
         << ", coluna: " << topo[nome].coluna << endl;
    exit( 1 );     
  }
}

void checa_simbolo( string nome, bool modificavel ) {
  for( int i = ts.size() - 1; i >= 0; i-- ) {  
    auto& atual = ts[i];
    
    if( atual.count( nome ) > 0 ) {
      if( modificavel && atual[nome].tipo == Const ) {
        cerr << "Variavel '" << nome << "' não pode ser modificada." << endl;
        exit( 1 );     
      }
      else 
        return;
    }
  }

  cerr << "Variavel '" << nome << "' não declarada." << endl;
  exit( 1 );     
}

void yyerror( const char* st ) {
   cerr << st << endl; 
   cerr << "Proximo a: " << yytext << endl;
   exit( 1 );
}

int main( int argc, char* argv[] ) {
  yyparse();
  
  return 0;
}