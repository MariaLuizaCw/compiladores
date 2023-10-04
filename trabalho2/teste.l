%{
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <map>

using namespace std;

int token;
string lexema;

void casa( int );

enum { ID = 256, NUM, PRINT, FLOAT, STRING, FUNC };

void E();
void F();
void T();
void E_Linha();
void P();
void RP();
void T_Linha();

map<int,string> nome_tokens = {
  { ID, "identificador" },
  { NUM, "número" }
};

%}

WS	[ \n\t]
DIGITO	[0-9]
LETRA	[A-Za-z_]

NUM	{DIGITO}+
ID	{LETRA}({LETRA}|{DIGITO})*

PAREN "("
FUNC	{LETRA}({LETRA}|{DIGITO})*

%%

{WS}  		{ }

"print" { lexema = yytext;  return PRINT;}
{NUM} 	{ lexema = yytext;  return NUM; }

{ID}		{ lexema = yytext;  return ID; }

{ID}/{PAREN} { lexema = yytext; return FUNC;}


\"([^"]|\\\"|\"\")+\"       { 
                              lexema = yytext;      
                              // lexema = remove_first_last_scapes('\"', lexema);
                              return STRING; 
                            }
  


{NUM}([.]{NUM})?([eE][-+]?{NUM})?  { lexema = yytext; return FLOAT; }

.		    { return yytext[0]; }

%%

int next_token() {
  return yylex();
}

string nome_token( int token ) {
  if( nome_tokens.find( token ) != nome_tokens.end() )
    return nome_tokens[token];
  else {
    string r;
    
    r = token;
    return r;
  }
}

void casa( int esperado ) {
  if( token == esperado )
    token = next_token();
  else {
    cout << "Esperado " << nome_token( esperado ) << "; encontrado " 
         << nome_token( token ) << endl;
         
    exit( 1 );
  }
}

void ARGS(){
  E();
  if(token == ','){
    casa(',');
    ARGS();
  }
}



void F() {
  string terminal  = lexema;
  switch( token ) {
    case ID: cout << lexema << " @ ";
             casa( ID );
             break;
           
    case NUM: cout << lexema << ' ';
             casa( NUM );
             break;
    case FLOAT: cout << lexema << ' ';
             casa( FLOAT );
             break;    

    case STRING: cout << lexema << ' ';
            casa( STRING );
            break;    
    case FUNC:
          casa(FUNC);
          casa('(');
          ARGS();
          casa(')');
          cout << terminal << " # ";
          break;
    case '(' : casa( '(' );
             E();
             casa( ')' );
             break;
  }
}


void FAT_LINHA(){
  if (token == '!'){
    casa('!');
    cout << "fat # ";
    FAT_LINHA();
  }
}

void FAT(){
  F();
  FAT_LINHA();
}


void RP(){
  if(token == '^'){
    casa('^');
    P();
    cout << "power # ";
  }
}

void P(){
  FAT();
  RP();
}


void U(){
   switch( token ) {
    case '-' : cout << 0 << ' ';
               casa('-');
               U();
               cout <<  "-"  << ' ';
               break;

    case '+' : cout << 0 << ' ';
               casa('+');
               U();
               cout <<  "+"  << ' ';
               break;
    default:
      P();
  }

}

void T_Linha() {
  switch( token ) {
    case '*' : casa( '*' );
               U();
               cout <<  "*"  << ' ';
               T_Linha();
               break;

    case '/' : casa( '/' );
               U();
               cout <<  "/"  << ' ';
               T_Linha();
               break;
  }
}


void T() {
  U();
  T_Linha();
}

void E_Linha(){
  switch( token ) {
    case '+' : casa( '+' );
               T();
               cout << '+' << ' ';
               E_Linha();
               break;

    case '-' : casa( '-' );
               T();
               cout <<  "-"  << ' ';
               E_Linha();
               break;

  }

}


void E(){
  T();
  E_Linha();
}




void A(){
  switch( token ) {
    case PRINT :  casa(PRINT);
                  E();
                  cout << "print #" << ' ';
                  break;

    case ID : cout << lexema << ' ';
              casa( ID );
              casa('=');
              E();
              cout << "= ^" << ' ';
  }
}



void S() {
  A();
  if(token == ';'){
    casa(';');
    S();
  }
}

int main() {
  token = next_token();
  S();
  
  cout << '\n';
  return 0;
}


