%{
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <exception>

using namespace std;

int linha = 1, coluna = 1; 

enum TipoDecl { DeclVar = 1, DeclConst, DeclLet };

struct Var {
  int linha, coluna;
  TipoDecl tipo;
};

vector< map< string, Var > > ts = { map< string, Var >{} }; 
vector<string> funcoes;
int in_function = 0;
#define YYSTYPE Atributos



struct Atributos {
  vector<string> c; // Código

  int linha = 0, coluna = 0;

  int contador = 0;     
  
  // Só para valor default de argumento        
  vector<string> valor_default; 

  void clear() {
    c.clear();
    valor_default.clear();
    linha = 0;
    coluna = 0;
    contador = 0;
  }
};


string extrair_asm(string s) {
    size_t inicio = s.find('{');
    size_t fim = s.find('}', inicio);
    return s.substr(inicio + 1, fim - inicio - 1);
}


vector<string> tokeniza(string line){
	vector<string> c;
	string intr = "";

  for(auto cr : line){

    if(cr == ' '){
      c.push_back(intr);
      intr = "";
    } else {
      intr += cr;
    }
  }
  c.push_back(intr);
	return c;
}


vector<string>  insere_tabela_de_simbolos( TipoDecl decl, Atributos id){
    Var variable;
    variable.linha = id.linha;
    variable.coluna = id.coluna;
    variable.tipo = decl;
    string name = id.c[0];

    auto& top_ts = ts.back();   
    auto it = top_ts.find(name);
    if (it == top_ts.end()) {
      top_ts[name] = variable;
      return  vector<string>{ name, "&" };
    } else if (decl == DeclVar && top_ts[name].tipo == DeclVar) {
      top_ts[name] = variable;
      return  vector<string>{};
    } else {
        cout << "Erro: a variável '" << name << "' já foi declarada na linha " + to_string(top_ts[name].linha) +"." << '\n';
        exit(1);
    }
}
  
void checa_declaracao(Atributos id, bool modificavel){
  string name = id.c[0];
  for( int i = ts.size() - 1; i >= 0; i-- ) {  
    auto& top_ts = ts[i];
    if( top_ts.count( name ) > 0 ) {
      if( modificavel && top_ts[name].tipo == DeclConst ) {
        cerr << "Variavel '" << name << "' não pode ser modificada." << endl;
        exit( 1 );     
      }
      else 
        return;
    }
  }

  if (!in_function & name != "undefined"){
    cerr << "Variavel '" << name << "' não declarada." << endl;
    exit( 1 ); 
  }
   
}


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

void cmd_if_no_else(Atributos& ss, Atributos& s_cond, Atributos& s_true){

    string lbl_true = gera_label( "lbl_true" );
    string lbl_fim_if = gera_label( "lbl_fim_if" );
    string definicao_lbl_true = ":" + lbl_true;
    string definicao_lbl_fim_if = ":" + lbl_fim_if;
    ss.c = s_cond.c +                          // Codigo da expressão
    lbl_true + "?" +                   // Código do IF
    lbl_fim_if + "#" +    // Código do False
    definicao_lbl_true + s_true.c +  // Código do True
    definicao_lbl_fim_if;           // Fim do IF

}


void cmd_while (Atributos& ss,  Atributos& s_cond, Atributos& s_cmd){

    string lbl_fim_while = gera_label( "fim_while" );
    string lbl_condicao_while = gera_label( "condicao_while" );
    string lbl_comando_while = gera_label( "comando_while" );
    string definicao_lbl_fim_while = ":" + lbl_fim_while;
    string definicao_lbl_condicao_while = ":" + lbl_condicao_while;
    string definicao_lbl_comando_while = ":" + lbl_comando_while;
    
    ss.c =  definicao_lbl_condicao_while +
            s_cond.c + lbl_comando_while + "?" + lbl_fim_while + "#" +
            definicao_lbl_comando_while + s_cmd.c + lbl_condicao_while + "#" +
            definicao_lbl_fim_while;
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

%token ID IF ELSE LET CONST VAR  PRINT FOR WHILE FUNCTION ASM  RETURN
%token CDOUBLE CSTRING CINT UNDEFINED BOOLEAN
%token AND OR ME_IG MA_IG DIF IGUAL
%token MAIS_IGUAL MAIS_MAIS SETA PARENTESES_FUNCAO

%right '=' SETA
%nonassoc IGUAL MAIS_IGUAL  MAIS_MAIS
%nonassoc '<' '>'
%left '+' '-'
%left '*' '/' '%'
%right '[' '('
%left '.'


%%

S : CMDs { print( resolve_enderecos( $1.c + "."  + funcoes) ); }
  ;

CMDs : CMDs CMD  { $$.c = $1.c + $2.c; };
     | {$$.clear();}
     ;

CMD : CMD_LET ';'
    | CMD_VAR ';'
    | CMD_CONST ';'
    | CMD_IF
    | E ASM ';'   { $$.c = $1.c + $2.c + "^"; }
    | CMD_WHILE
    | CMD_FOR
    | CMD_FUNC
    | E ';'   { $$.c = $1.c + "^"; }
    | '{' EMPILHA_TS CMDs '}' {ts.pop_back(); $$.c = "<{" + $3.c + "}>";}
    | ';' {$$.clear();}
    | RETURN E ';' {$$.c = $2.c + "'&retorno'" + "@" +  "~";};
    | RETURN OBJ ';' {$$.c = $2.c + "'&retorno'" + "@" +  "~";};
    // | PRINT E ';'  { $$.c = $2.c + "println" + "#"; }
    ;

EMPILHA_TS : { ts.push_back( map< string, Var >{} ); } 
           ;

CMD_FUNC : FUNCTION ID { insere_tabela_de_simbolos(DeclVar, $2);  in_function++;} 
             '('  EMPILHA_TS LISTA_PARAMs ')' '{'  CMDs '}'
           { 
             string lbl_endereco_funcao = gera_label( "func_" + $2.c[0] );
             string definicao_lbl_endereco_funcao = ":" + lbl_endereco_funcao;
             
             $$.c = $2.c + "&" + $2.c + "{}"  + "=" + "'&funcao'" +
                    lbl_endereco_funcao + "[=]" + "^";
             funcoes = funcoes + definicao_lbl_endereco_funcao + $6.c + $9.c +
                       "undefined" + "@" + "'&retorno'" + "@"+ "~";
             ts.pop_back(); 
             in_function--;
           }
         ;

FUNC_ANON : FUNCTION { in_function++;} 
             '('  EMPILHA_TS LISTA_PARAMs ')' '{'  CMDs '}'
           { 
             string lbl_endereco_funcao = gera_label( "funcanon" );
             string definicao_lbl_endereco_funcao = ":" + lbl_endereco_funcao;
             
             $$.c =  vector<string>{"{}"} + "'&funcao'" +
                    lbl_endereco_funcao + "[<=]";
             funcoes = funcoes + definicao_lbl_endereco_funcao + $5.c + $8.c +
                       "undefined" + "@" + "'&retorno'" + "@"+ "~";
             ts.pop_back(); 
             in_function--;
           }
         ;




LISTA_PARAMs : PARAMs
           | { $$.clear(); }
           ;

// void cmd_if_else(Atributos& ss, Atributos& s_cond, Atributos& s_false, Atributos& s_true){
//     string lbl_true = gera_label( "lbl_true" );
//     string lbl_fim_if = gera_label( "lbl_fim_if" );
//     string definicao_lbl_true = ":" + lbl_true;
//     string definicao_lbl_fim_if = ":" + lbl_fim_if;
//     ss.c = s_cond.c +                          // Codigo da expressão
//     lbl_true + "?" +                   // Código do IF
//     s_false.c + lbl_fim_if + "#" +    // Código do False
//     definicao_lbl_true + s_true.c +  // Código do True
//     definicao_lbl_fim_if;           // Fim do IF
    
// }

PARAMs : PARAMs ',' PARAM  
       { // a & a arguments @ 0 [@] = ^ 
         $$.c = $1.c + $3.c + "&" + $3.c + "arguments" + "@" + to_string( $1.contador )
                + "[@]" + "=" + "^"; 
                
         if( $3.valor_default.size() > 0 ) {
           string lbl_true = gera_label( "lbl_true" );
           string lbl_fim_if = gera_label( "lbl_fim_if" );
           string definicao_lbl_true = ":" + lbl_true;
           string definicao_lbl_fim_if = ":" + lbl_fim_if;
          
           $$.c = $$.c + $3.c + "@" +  "undefined" + "@" + "!=" +
                 lbl_true + "?" + $3.c + $3.valor_default + "=" + "^" +
                 lbl_fim_if + "#" +
                 definicao_lbl_true + 
                 definicao_lbl_fim_if;


         }
         $$.contador = $1.contador + $3.contador; 
       }
     | PARAM 
       { // a & a arguments @ 0 [@] = ^ 
         $$.c = $1.c + "&" + $1.c + "arguments" + "@" + "0" + "[@]" + "=" + "^"; 
                
         if( $1.valor_default.size() > 0 ) {
           string lbl_true = gera_label( "lbl_true" );
           string lbl_fim_if = gera_label( "lbl_fim_if" );
           string definicao_lbl_true = ":" + lbl_true;
           string definicao_lbl_fim_if = ":" + lbl_fim_if;
           $$.c = $$.c + $1.c + "@" +  "undefined" + "@" + "!=" +
                 lbl_true + "?" + $1.c + $1.valor_default + "=" + "^" +
                 lbl_fim_if + "#" +
                 definicao_lbl_true + 
                 definicao_lbl_fim_if;
         }
         $$.contador = $1.contador; 
       }
     ;
     
PARAM : ID 
      { $$.c = $1.c;      
        $$.contador = 1;
        $$.valor_default.clear();
        insere_tabela_de_simbolos(DeclLet, $1);
      }
    | ID '=' E
      { // Código do IF
        $$.c = $1.c;
        $$.contador = 1;
        $$.valor_default = $3.c;         
        insere_tabela_de_simbolos(DeclLet, $1);
      }
    ;

OBJ : '{' '}' {$$.c = vector<string>{"{}"};}
    | '{' CAMPOS '}' {$$.c = vector<string>{"{}"} + $2.c;}
    ;

CAMPOS : CAMPO ',' CAMPOS {$$.c = $1.c + "[<=]" + $3.c;}
       | CAMPO {$$.c = $1.c + "[<=]";}
       ;

CAMPO : ID ':' E {$$.c = $1.c + $3.c;}
      | ID ':' OBJ {$$.c = $1.c + $3.c;}
      ;    

CMD_FOR : FOR '(' PRIM_E ';' E ';' E ')' CMD  { cmd_for($$, $3, $5, $9, $7); }
        ;

PRIM_E : CMD_LET 
       | CMD_VAR
       | CMD_CONST
       | E  { $$.c = $1.c + "^"; }
       ;

CMD_LET : LET LET_VARs { $$.c = $2.c; }
        ;

LET_VARs : LET_VAR ',' LET_VARs { $$.c = $1.c + $3.c; } 
          | LET_VAR
          ;

LET_VAR : ID                    {$$.c = insere_tabela_de_simbolos( DeclLet, $1 ); }
        | ID '=' E              {$$.c = insere_tabela_de_simbolos( DeclLet, $1 ) + $1.c + $3.c + "=" + "^"; }
        | ID '=' OBJ        {$$.c = insere_tabela_de_simbolos( DeclLet, $1 ) +  $1.c + $3.c + "=" + "^"; } 
        ;
    
CMD_VAR : VAR VAR_VARs { $$.c = $2.c; }
        ;
        
VAR_VARs : VAR_VAR ',' VAR_VARs { $$.c = $1.c + $3.c; } 
         | VAR_VAR
         ;

VAR_VAR : ID                    {$$.c = insere_tabela_de_simbolos( DeclVar, $1 ); }
        | ID '=' E              {$$.c = insere_tabela_de_simbolos( DeclVar, $1 ) + $1.c + $3.c + "=" + "^"; }
        | ID '=' OBJ            {$$.c = insere_tabela_de_simbolos( DeclVar, $1 ) + $1.c + $3.c + "=" + "^"; } 
        ;

CMD_CONST: CONST CONST_VARs { $$.c = $2.c; }
         ;
  
CONST_VARs : CONST_VAR ',' CONST_VARs { $$.c = $1.c + $3.c; } 
           | CONST_VAR
           ;

CONST_VAR : ID '=' E
            { $$.c = insere_tabela_de_simbolos( DeclConst,  $1 ) + 
                     $1.c + $3.c + "=" + "^"; }
          ;


CMD_IF : IF '(' E ')' CMD           { cmd_if_no_else($$, $3, $5); }
       | IF '(' E ')' CMD ELSE CMD  { cmd_if_else($$, $3, $7, $5); }
       ;
      
CMD_WHILE : WHILE '(' E ')' CMD   {  cmd_while($$, $3, $5);  }
          ;

E : ID '=' E              { checa_declaracao($1, true); $$.c = $1.c + $3.c + "="; }
  | ID '=' OBJ            { checa_declaracao($1, true); $$.c = $1.c + $3.c + "="; } 
  | LVALUEPROP '=' E          { checa_declaracao($1, true); $$.c = $1.c + $3.c + "[=]"; }
  | LVALUEPROP '=' OBJ        { checa_declaracao($1, true); $$.c = $1.c + $3.c + "[=]"; }
  | ID MAIS_IGUAL E       { checa_declaracao($1, true); $$.c = $1.c + $1.c +  "@" + $3.c +  "+" + "=";}
  | LVALUEPROP MAIS_IGUAL E   { checa_declaracao($1, true); $$.c = $1.c + $1.c +  "[@]" + $3.c +  "+" + "[=]";}
  | ID MAIS_MAIS          { 
                                 $$.c = $1.c + "@" +  $1.c + $1.c + "@" + vector<string>{"1"} + vector<string>{"+"} + vector<string>{"="} + "^"; 
                              }
  | E '<' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '>' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '+' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '-' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '*' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '/' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E '%' E                   { $$.c = $1.c + $3.c + $2.c; }
  | E IGUAL E                 { $$.c = $1.c + $3.c + $2.c; }
  | FUNC_ANON
  | '(' E ')'                 { $$.c = $2.c; }
  | E '(' LISTA_ARGs ')'
    {
      $$.c = $3.c + to_string( $3.contador ) + $1.c + "$";
    }
  | '[' LISTA_ELEMENTOS ']' {$$.c = vector<string>{"[]"} + $2.c;}
  | ID {ts.push_back( map< string, Var >{});  insere_tabela_de_simbolos(DeclLet, $1); in_function++;} SETA E 
    { 
      string lbl_endereco_funcao = gera_label( "funcanon" );
      string definicao_lbl_endereco_funcao = ":" + lbl_endereco_funcao;
      vector<string>arg =  $1.c + "&" + $1.c + "arguments" + "@" + "0" + "[@]" + "=" + "^";
      $$.c = vector<string>{"{}"} + "'&funcao'" + lbl_endereco_funcao + "[<=]";
      funcoes = funcoes + definicao_lbl_endereco_funcao + arg + $4.c + "'&retorno'" + "@" + "~";
      ts.pop_back(); 
      in_function--;
    }
  | '(' LISTA_PARAMs EMPILHA_TS PARENTESES_FUNCAO SETA E 
    { ts.pop_back(); }
  | U
  ;


LISTA_ELEMENTOS : ELEMENTOS
           | { $$.clear(); }
           ;
             
ELEMENTOS : ELEMENTOS ',' E { $$.contador++; $$.c =  $1.c + to_string( $$.contador ) + $3.c + "[<=]";}
          | ELEMENTOS ',' OBJ { $$.contador++; $$.c =  $1.c + to_string( $$.contador ) + $3.c + "[<=]";}
          | E {$$.c = to_string( $$.contador ) + $1.c + "[<=]";}
          | OBJ  {$$.c = to_string( $$.contador ) + $1.c + "[<=]";}
          ;
          
       

LISTA_ARGs : ARGs
           | { $$.clear(); }
           ;
             
ARGs : ARGs ',' E
       { $$.c = $1.c + $3.c;
          $$.contador++; }
     | ARGs ',' '{' '}'
     { $$.c = $1.c + vector<string>{"{}"};
          $$.contador++; }
     | E
       { $$.c = $1.c;
         $$.contador = 1; }
     ;
           

U : '-' F {$$.c = "0" + $2.c + $1.c;}
  | F
  ;

F :  CDOUBLE
  | CINT
  | BOOLEAN
  | CSTRING
  | ID              { checa_declaracao($1, false ); $$.c = $1.c + "@"; } 
  | LVALUEPROP          {$$.c = $1.c + "[@]";}
  ;

       
LVALUEPROP : F '[' E ']' { $$.c = $1.c + $3.c;}
           | F '.' ID    { $$.c = $1.c + $3.c;}
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