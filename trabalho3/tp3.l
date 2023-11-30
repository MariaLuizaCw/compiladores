%{
int token( int tk );
%}

D  	    [0-9]
L   	[A-Za-z_]
INT	    {D}+
DOUBLE  {D}+"."{D}+([Ee][+\-]?{D}+)?
ID      {L}({L}|{D})*
BOOL    "true"|"false"

%%

"\t"       { coluna += 4; }
" "        { coluna++; }
"\n"	   { linha++; coluna = 1; }

{DOUBLE}   { return token( CDOUBLE ); }
{INT}	   { return token( CINT ); } 


\"([^"]|\\\"|\"\")+\"       { return token( CSTRING ); }
\'([^']|\\\'|\'\')+\'       { return token( CSTRING ); }
  

"&&"	   { return token( AND ); }
"||"	   { return token( OR ); }
"<="	   { return token( ME_IG ); }
">="	   { return token( MA_IG ); }
"!="	   { return token( DIF ); }
"=="	   { return token( IGUAL ); }
"asm{".*"}"  {  string lexema = extrair_asm(yytext); //trim( yytext + 3, "{}" ); 
                yylval.c = tokeniza( lexema );
                coluna += strlen( yytext ); 
                return ASM; 
              }


"+="       { return token( MAIS_IGUAL ); }
"++"       { return token( MAIS_MAIS ); }

"let"      { return token( LET ); }
"function" {return token (FUNCTION);}
"var"      { return token( VAR ); }
"const"    { return token( CONST ); }
"if"       { return token( IF ); }
"else"     { return token( ELSE ); }
"for"      { return token( FOR ); }
"while"    { return token( WHILE ); }
"return"   { return token( RETURN ); }
")"/.*"=>" {return token(PARENTESES_FUNCAO);}
"=>"[ \n\s\t]*"{"  {return token(SETA_CHAVE);}
"=>"        {return token(SETA);}
{BOOL}     {return token(BOOLEAN);}
{ID}       { return token( ID ); }

.          { return token( *yytext ); }

%%

int token( int tk ) {  
  yylval.c = vector<string>{ yytext };
  coluna += strlen( yytext ); 
  
  yylval.linha = linha;
  yylval.coluna = coluna;

  return tk;
}