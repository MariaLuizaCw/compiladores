/* A Bison parser, made by GNU Bison 3.5.1.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Undocumented macros, especially those whose name start with YY_,
   are private implementation details.  Do not rely on them.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.5.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "tp3.y"

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


#line 296 "y.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif


/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ID = 258,
    IF = 259,
    ELSE = 260,
    LET = 261,
    CONST = 262,
    VAR = 263,
    PRINT = 264,
    FOR = 265,
    WHILE = 266,
    FUNCTION = 267,
    ASM = 268,
    RETURN = 269,
    CDOUBLE = 270,
    CSTRING = 271,
    CINT = 272,
    UNDEFINED = 273,
    BOOLEAN = 274,
    AND = 275,
    OR = 276,
    ME_IG = 277,
    MA_IG = 278,
    DIF = 279,
    IGUAL = 280,
    MAIS_IGUAL = 281,
    MAIS_MAIS = 282
  };
#endif
/* Tokens.  */
#define ID 258
#define IF 259
#define ELSE 260
#define LET 261
#define CONST 262
#define VAR 263
#define PRINT 264
#define FOR 265
#define WHILE 266
#define FUNCTION 267
#define ASM 268
#define RETURN 269
#define CDOUBLE 270
#define CSTRING 271
#define CINT 272
#define UNDEFINED 273
#define BOOLEAN 274
#define AND 275
#define OR 276
#define ME_IG 277
#define MA_IG 278
#define DIF 279
#define IGUAL 280
#define MAIS_IGUAL 281
#define MAIS_MAIS 282

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);





#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))

/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && ! defined __ICC && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                            \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   378

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  46
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  35
/* YYNRULES -- Number of rules.  */
#define YYNRULES  97
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  174

#define YYUNDEFTOK  2
#define YYMAXUTOK   282


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,    35,     2,     2,
      37,    42,    33,    31,    43,    32,    38,    34,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    44,    39,
      29,    28,    30,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    36,     2,    45,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    40,     2,    41,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   243,   243,   246,   247,   250,   251,   252,   253,   254,
     255,   256,   257,   258,   259,   260,   261,   262,   266,   269,
     269,   284,   285,   301,   322,   341,   347,   356,   357,   360,
     361,   364,   365,   368,   371,   372,   373,   374,   377,   380,
     381,   384,   385,   386,   389,   392,   393,   396,   397,   398,
     401,   404,   405,   408,   414,   415,   418,   421,   422,   423,
     424,   425,   426,   427,   430,   431,   432,   433,   434,   435,
     436,   437,   438,   442,   443,   447,   448,   451,   452,   453,
     454,   459,   460,   463,   466,   469,   475,   476,   479,   480,
     481,   482,   483,   484,   485,   488,   491,   492
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "IF", "ELSE", "LET", "CONST",
  "VAR", "PRINT", "FOR", "WHILE", "FUNCTION", "ASM", "RETURN", "CDOUBLE",
  "CSTRING", "CINT", "UNDEFINED", "BOOLEAN", "AND", "OR", "ME_IG", "MA_IG",
  "DIF", "IGUAL", "MAIS_IGUAL", "MAIS_MAIS", "'='", "'<'", "'>'", "'+'",
  "'-'", "'*'", "'/'", "'%'", "'['", "'('", "'.'", "';'", "'{'", "'}'",
  "')'", "','", "':'", "']'", "$accept", "S", "CMDs", "CMD", "EMPILHA_TS",
  "CMD_FUNC", "$@1", "LISTA_PARAMs", "PARAMs", "PARAM", "OBJ", "CAMPOS",
  "CAMPO", "CMD_FOR", "PRIM_E", "CMD_LET", "LET_VARs", "LET_VAR",
  "CMD_VAR", "VAR_VARs", "VAR_VAR", "CMD_CONST", "CONST_VARs", "CONST_VAR",
  "CMD_IF", "CMD_WHILE", "E", "LISTA_ELEMENTOS", "ELEMENTOS", "LISTA_ARGs",
  "ARGs", "U", "F", "LVALUE", "LVALUEPROP", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_int16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,    61,    60,
      62,    43,    45,    42,    47,    37,    91,    40,    46,    59,
     123,   125,    41,    44,    58,    93
};
# endif

#define YYPACT_NINF (-121)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  ((Yyn) == YYTABLE_NINF)

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -121,     4,   178,  -121,  -121,   -23,    14,    15,    19,   -10,
      -6,    33,   184,  -121,  -121,  -121,  -121,    77,   184,   216,
    -121,  -121,  -121,  -121,  -121,    -1,     3,     5,  -121,  -121,
     135,  -121,     1,    -2,   -15,   216,    12,  -121,     8,    18,
    -121,    28,    20,  -121,    30,    13,   216,  -121,     0,    36,
     299,     1,  -121,  -121,  -121,   321,    31,    34,   243,  -121,
    -121,  -121,  -121,    39,   216,   216,   216,   216,   216,   216,
     216,   216,   216,  -121,   216,    40,   216,  -121,   184,   216,
     184,   257,   184,    14,   216,    15,   184,    19,    42,  -121,
    -121,  -121,   321,   271,    45,    35,  -121,    43,    44,  -121,
    -121,  -121,   184,  -121,    94,  -121,   332,   341,   341,   -25,
     -25,    46,    46,    46,   321,    47,    48,   226,  -121,   332,
    -121,   321,   332,  -121,   321,   178,  -121,   321,  -121,   321,
    -121,  -121,   321,  -121,   216,   178,  -121,   184,  -121,    83,
    -121,   321,  -121,  -121,   210,  -121,    90,   310,  -121,    85,
    -121,   321,  -121,    58,   321,   178,   216,    75,    65,    72,
    -121,  -121,  -121,   285,   216,    76,    85,   178,   321,  -121,
    -121,  -121,   139,  -121
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       4,     0,     2,     1,    95,     0,     0,     0,     0,     0,
       0,     0,     0,    88,    91,    89,    90,     0,    76,     0,
      15,    18,     3,    12,    11,     0,     0,     0,     8,    10,
       0,    74,    87,    92,    93,     0,    41,    38,    40,     0,
      50,    52,    47,    44,    46,     0,     0,    19,     0,     0,
       0,    86,    92,    93,    80,    79,     0,    75,     0,     4,
       5,     6,     7,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    82,    13,     0,     0,     0,    63,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    34,
      35,    36,    37,     0,     0,     0,    27,     0,    30,    17,
      16,    73,     0,    94,     0,     9,    71,    64,    65,    66,
      67,    68,    69,    70,    85,     0,    81,     0,    97,    61,
      58,    57,    62,    60,    59,     0,    43,    42,    39,    53,
      51,    49,    48,    45,     0,     0,    18,     0,    28,     0,
      78,    77,    14,    72,     0,    96,    54,     0,    56,    22,
      32,    31,    29,     0,    83,     0,     0,    25,     0,    21,
      24,    84,    55,     0,     0,     0,     0,     0,    26,     4,
      23,    33,     0,    20
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -121,  -121,   -57,  -120,   -19,  -121,  -121,  -121,  -121,   -48,
     -17,   -20,  -121,  -121,  -121,    78,    38,  -121,    79,    41,
    -121,    82,    51,  -121,  -121,  -121,   -12,  -121,  -121,  -121,
    -121,  -121,   112,   120,   121
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    22,    59,    23,    94,   158,   159,   160,
      49,    97,    98,    24,    88,    25,    37,    38,    26,    43,
      44,    27,    40,    41,    28,    29,    30,    56,    57,   115,
     116,    31,    32,    33,    34
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      50,    54,   104,    95,     3,   146,    55,    58,    69,    70,
      71,    79,    72,    80,    35,   148,     4,    36,    39,     6,
       7,     8,    42,    81,    76,    77,    78,    45,    13,    14,
      15,    46,    16,    92,    93,   162,    47,    74,    60,    75,
      82,    96,    61,   118,    62,    17,    84,   171,    86,    18,
      19,    83,   106,   107,   108,   109,   110,   111,   112,   113,
     114,   120,   117,   123,   119,   126,   121,   122,   124,   131,
     127,    85,   129,    87,   132,    99,   101,   102,   105,   137,
       4,   134,   136,    72,   138,   140,    95,   139,   157,   143,
     141,   144,    13,    14,    15,   155,    16,     4,     5,   161,
       6,     7,     8,   164,     9,    10,    11,   165,    12,    13,
      14,    15,   172,    16,    19,   166,   169,   149,   170,   152,
     150,   128,   147,    89,    90,   151,    17,    91,   133,    51,
      18,    19,   154,    20,    21,   142,   130,    52,    53,     0,
       0,     0,     4,     5,   163,     6,     7,     8,    63,     9,
      10,    11,   168,    12,    13,    14,    15,     0,    16,     0,
      64,     0,     0,     0,    65,    66,    67,    68,    69,    70,
      71,    17,    72,     0,    73,    18,    19,     0,    20,    21,
     173,     4,     5,     0,     6,     7,     8,     4,     9,    10,
      11,     0,    12,    13,    14,    15,     0,    16,     0,    13,
      14,    15,     0,    16,     0,     0,     0,     0,     0,     0,
      17,     0,     0,     4,    18,    19,    17,    20,    21,     4,
      18,    19,     0,     0,    48,    13,    14,    15,     0,    16,
       0,    13,    14,    15,     0,    16,     0,     0,     0,     0,
       0,     0,    17,     0,     0,     0,    18,    19,    17,     0,
     153,    64,    18,    19,     0,    65,    66,    67,    68,    69,
      70,    71,     0,    72,     0,     0,     0,     0,    64,     0,
       0,   145,    65,    66,    67,    68,    69,    70,    71,     0,
      72,     0,    64,     0,     0,   103,    65,    66,    67,    68,
      69,    70,    71,     0,    72,     0,    64,     0,     0,   125,
      65,    66,    67,    68,    69,    70,    71,     0,    72,     0,
      64,     0,     0,   135,    65,    66,    67,    68,    69,    70,
      71,     0,    72,     0,    64,     0,     0,   167,    65,    66,
      67,    68,    69,    70,    71,    64,    72,     0,   100,    65,
      66,    67,    68,    69,    70,    71,    64,    72,     0,   156,
      65,    66,    67,    68,    69,    70,    71,    -1,    72,     0,
       0,    65,    66,    67,    68,    69,    70,    71,     0,    72,
      -1,    -1,    67,    68,    69,    70,    71,     0,    72
};

static const yytype_int16 yycheck[] =
{
      12,    18,    59,     3,     0,   125,    18,    19,    33,    34,
      35,    26,    37,    28,    37,   135,     3,     3,     3,     6,
       7,     8,     3,    35,    26,    27,    28,    37,    15,    16,
      17,    37,    19,    45,    46,   155,     3,    36,    39,    38,
      28,    41,    39,     3,    39,    32,    28,   167,    28,    36,
      37,    43,    64,    65,    66,    67,    68,    69,    70,    71,
      72,    78,    74,    80,    76,    82,    78,    79,    80,    86,
      82,    43,    84,    43,    86,    39,    45,    43,    39,    44,
       3,    39,    37,    37,    41,   102,     3,    43,     3,    42,
     102,    43,    15,    16,    17,     5,    19,     3,     4,    41,
       6,     7,     8,    28,    10,    11,    12,    42,    14,    15,
      16,    17,   169,    19,    37,    43,    40,   136,   166,   139,
     137,    83,   134,    45,    45,   137,    32,    45,    87,    17,
      36,    37,   144,    39,    40,    41,    85,    17,    17,    -1,
      -1,    -1,     3,     4,   156,     6,     7,     8,    13,    10,
      11,    12,   164,    14,    15,    16,    17,    -1,    19,    -1,
      25,    -1,    -1,    -1,    29,    30,    31,    32,    33,    34,
      35,    32,    37,    -1,    39,    36,    37,    -1,    39,    40,
      41,     3,     4,    -1,     6,     7,     8,     3,    10,    11,
      12,    -1,    14,    15,    16,    17,    -1,    19,    -1,    15,
      16,    17,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      32,    -1,    -1,     3,    36,    37,    32,    39,    40,     3,
      36,    37,    -1,    -1,    40,    15,    16,    17,    -1,    19,
      -1,    15,    16,    17,    -1,    19,    -1,    -1,    -1,    -1,
      -1,    -1,    32,    -1,    -1,    -1,    36,    37,    32,    -1,
      40,    25,    36,    37,    -1,    29,    30,    31,    32,    33,
      34,    35,    -1,    37,    -1,    -1,    -1,    -1,    25,    -1,
      -1,    45,    29,    30,    31,    32,    33,    34,    35,    -1,
      37,    -1,    25,    -1,    -1,    42,    29,    30,    31,    32,
      33,    34,    35,    -1,    37,    -1,    25,    -1,    -1,    42,
      29,    30,    31,    32,    33,    34,    35,    -1,    37,    -1,
      25,    -1,    -1,    42,    29,    30,    31,    32,    33,    34,
      35,    -1,    37,    -1,    25,    -1,    -1,    42,    29,    30,
      31,    32,    33,    34,    35,    25,    37,    -1,    39,    29,
      30,    31,    32,    33,    34,    35,    25,    37,    -1,    39,
      29,    30,    31,    32,    33,    34,    35,    25,    37,    -1,
      -1,    29,    30,    31,    32,    33,    34,    35,    -1,    37,
      29,    30,    31,    32,    33,    34,    35,    -1,    37
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,    47,    48,     0,     3,     4,     6,     7,     8,    10,
      11,    12,    14,    15,    16,    17,    19,    32,    36,    37,
      39,    40,    49,    51,    59,    61,    64,    67,    70,    71,
      72,    77,    78,    79,    80,    37,     3,    62,    63,     3,
      68,    69,     3,    65,    66,    37,    37,     3,    40,    56,
      72,    78,    79,    80,    56,    72,    73,    74,    72,    50,
      39,    39,    39,    13,    25,    29,    30,    31,    32,    33,
      34,    35,    37,    39,    36,    38,    26,    27,    28,    26,
      28,    72,    28,    43,    28,    43,    28,    43,    60,    61,
      64,    67,    72,    72,    52,     3,    41,    57,    58,    39,
      39,    45,    43,    42,    48,    39,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    75,    76,    72,     3,    72,
      56,    72,    72,    56,    72,    42,    56,    72,    62,    72,
      68,    56,    72,    65,    39,    42,    37,    44,    41,    43,
      56,    72,    41,    42,    43,    45,    49,    72,    49,    50,
      56,    72,    57,    40,    72,     5,    39,     3,    53,    54,
      55,    41,    49,    72,    28,    42,    43,    42,    72,    40,
      55,    49,    48,    41
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_int8 yyr1[] =
{
       0,    46,    47,    48,    48,    49,    49,    49,    49,    49,
      49,    49,    49,    49,    49,    49,    49,    49,    50,    52,
      51,    53,    53,    54,    54,    55,    55,    56,    56,    57,
      57,    58,    58,    59,    60,    60,    60,    60,    61,    62,
      62,    63,    63,    63,    64,    65,    65,    66,    66,    66,
      67,    68,    68,    69,    70,    70,    71,    72,    72,    72,
      72,    72,    72,    72,    72,    72,    72,    72,    72,    72,
      72,    72,    72,    72,    72,    73,    73,    74,    74,    74,
      74,    75,    75,    76,    76,    76,    77,    77,    78,    78,
      78,    78,    78,    78,    78,    79,    80,    80
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     0,     2,     2,     2,     1,     3,
       1,     1,     1,     2,     4,     1,     3,     3,     0,     0,
      10,     1,     0,     3,     1,     1,     3,     2,     3,     3,
       1,     3,     3,     9,     1,     1,     1,     1,     2,     3,
       1,     1,     3,     3,     2,     3,     1,     1,     3,     3,
       2,     3,     1,     3,     5,     7,     5,     3,     3,     3,
       3,     3,     3,     2,     3,     3,     3,     3,     3,     3,
       3,     3,     4,     3,     1,     1,     0,     3,     3,     1,
       1,     1,     0,     3,     4,     1,     2,     1,     1,     1,
       1,     1,     1,     1,     3,     1,     4,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YYUSE (yyoutput);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyo, yytoknum[yytype], *yyvaluep);
# endif
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyo, yytype, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[+yyssp[yyi + 1 - yynrhs]],
                       &yyvsp[(yyi + 1) - (yynrhs)]
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen(S) (YY_CAST (YYPTRDIFF_T, strlen (S)))
#  else
/* Return the length of YYSTR.  */
static YYPTRDIFF_T
yystrlen (const char *yystr)
{
  YYPTRDIFF_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYPTRDIFF_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYPTRDIFF_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            else
              goto append;

          append:
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (yyres)
    return yystpcpy (yyres, yystr) - yyres;
  else
    return yystrlen (yystr);
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYPTRDIFF_T *yymsg_alloc, char **yymsg,
                yy_state_t *yyssp, int yytoken)
{
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat: reported tokens (one for the "unexpected",
     one per "expected"). */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Actual size of YYARG. */
  int yycount = 0;
  /* Cumulated lengths of YYARG.  */
  YYPTRDIFF_T yysize = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[+*yyssp];
      YYPTRDIFF_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
      yysize = yysize0;
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYPTRDIFF_T yysize1
                    = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
                    yysize = yysize1;
                  else
                    return 2;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
    default: /* Avoid compiler warnings. */
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    /* Don't count the "%s"s in the final size, but reserve room for
       the terminator.  */
    YYPTRDIFF_T yysize1 = yysize + (yystrlen (yyformat) - 2 * yycount) + 1;
    if (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM)
      yysize = yysize1;
    else
      return 2;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          ++yyp;
          ++yyformat;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss;
    yy_state_t *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYPTRDIFF_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYPTRDIFF_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    goto yyexhaustedlab;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
# undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2:
#line 243 "tp3.y"
         { print( resolve_enderecos( yyvsp[0].c + "."  + funcoes) ); }
#line 1730 "y.tab.c"
    break;

  case 3:
#line 246 "tp3.y"
                 { yyval.c = yyvsp[-1].c + yyvsp[0].c; }
#line 1736 "y.tab.c"
    break;

  case 4:
#line 247 "tp3.y"
       {yyval.clear();}
#line 1742 "y.tab.c"
    break;

  case 9:
#line 254 "tp3.y"
                  { yyval.c = yyvsp[-2].c + yyvsp[-1].c + "^"; }
#line 1748 "y.tab.c"
    break;

  case 13:
#line 258 "tp3.y"
              { yyval.c = yyvsp[-1].c + "^"; }
#line 1754 "y.tab.c"
    break;

  case 14:
#line 259 "tp3.y"
                              {ts.pop_back(); yyval.c = "<{" + yyvsp[-1].c + "}>";}
#line 1760 "y.tab.c"
    break;

  case 15:
#line 260 "tp3.y"
          {yyval.clear();}
#line 1766 "y.tab.c"
    break;

  case 16:
#line 261 "tp3.y"
                   {yyval.c = yyvsp[-1].c + "'&retorno'" + "@" +  "~";}
#line 1772 "y.tab.c"
    break;

  case 17:
#line 262 "tp3.y"
                     {yyval.c = yyvsp[-1].c + "'&retorno'" + "@" +  "~";}
#line 1778 "y.tab.c"
    break;

  case 18:
#line 266 "tp3.y"
             { ts.push_back( map< string, Var >{} ); }
#line 1784 "y.tab.c"
    break;

  case 19:
#line 269 "tp3.y"
                       { insere_tabela_de_simbolos(DeclVar, yyvsp[0]);  in_function++;}
#line 1790 "y.tab.c"
    break;

  case 20:
#line 271 "tp3.y"
           { 
             string lbl_endereco_funcao = gera_label( "func_" + yyvsp[-8].c[0] );
             string definicao_lbl_endereco_funcao = ":" + lbl_endereco_funcao;
             
             yyval.c = yyvsp[-8].c + "&" + yyvsp[-8].c + "{}"  + "=" + "'&funcao'" +
                    lbl_endereco_funcao + "[=]" + "^";
             funcoes = funcoes + definicao_lbl_endereco_funcao + yyvsp[-4].c + yyvsp[-1].c +
                       "undefined" + "@" + "'&retorno'" + "@"+ "~";
             ts.pop_back(); 
             in_function--;
           }
#line 1806 "y.tab.c"
    break;

  case 22:
#line 285 "tp3.y"
             { yyval.clear(); }
#line 1812 "y.tab.c"
    break;

  case 23:
#line 302 "tp3.y"
       { // a & a arguments @ 0 [@] = ^ 
         yyval.c = yyvsp[-2].c + yyvsp[0].c + "&" + yyvsp[0].c + "arguments" + "@" + to_string( yyvsp[-2].contador )
                + "[@]" + "=" + "^"; 
                
         if( yyvsp[0].valor_default.size() > 0 ) {
           string lbl_true = gera_label( "lbl_true" );
           string lbl_fim_if = gera_label( "lbl_fim_if" );
           string definicao_lbl_true = ":" + lbl_true;
           string definicao_lbl_fim_if = ":" + lbl_fim_if;
          
           yyval.c = yyval.c + yyvsp[0].c + "@" +  "undefined" + "@" + "!=" +
                 lbl_true + "?" + yyvsp[0].c + yyvsp[0].valor_default + "=" + "^" +
                 lbl_fim_if + "#" +
                 definicao_lbl_true + 
                 definicao_lbl_fim_if;


         }
         yyval.contador = yyvsp[-2].contador + yyvsp[0].contador; 
       }
#line 1837 "y.tab.c"
    break;

  case 24:
#line 323 "tp3.y"
       { // a & a arguments @ 0 [@] = ^ 
         yyval.c = yyvsp[0].c + "&" + yyvsp[0].c + "arguments" + "@" + "0" + "[@]" + "=" + "^"; 
                
         if( yyvsp[0].valor_default.size() > 0 ) {
           string lbl_true = gera_label( "lbl_true" );
           string lbl_fim_if = gera_label( "lbl_fim_if" );
           string definicao_lbl_true = ":" + lbl_true;
           string definicao_lbl_fim_if = ":" + lbl_fim_if;
           yyval.c = yyval.c + yyvsp[0].c + "@" +  "undefined" + "@" + "!=" +
                 lbl_true + "?" + yyvsp[0].c + yyvsp[0].valor_default + "=" + "^" +
                 lbl_fim_if + "#" +
                 definicao_lbl_true + 
                 definicao_lbl_fim_if;
         }
         yyval.contador = yyvsp[0].contador; 
       }
#line 1858 "y.tab.c"
    break;

  case 25:
#line 342 "tp3.y"
      { yyval.c = yyvsp[0].c;      
        yyval.contador = 1;
        yyval.valor_default.clear();
        insere_tabela_de_simbolos(DeclLet, yyvsp[0]);
      }
#line 1868 "y.tab.c"
    break;

  case 26:
#line 348 "tp3.y"
      { // Código do IF
        yyval.c = yyvsp[-2].c;
        yyval.contador = 1;
        yyval.valor_default = yyvsp[0].c;         
        insere_tabela_de_simbolos(DeclLet, yyvsp[-2]);
      }
#line 1879 "y.tab.c"
    break;

  case 27:
#line 356 "tp3.y"
              {yyval.c = vector<string>{"{}"};}
#line 1885 "y.tab.c"
    break;

  case 28:
#line 357 "tp3.y"
                     {yyval.c = vector<string>{"{}"} + yyvsp[-1].c;}
#line 1891 "y.tab.c"
    break;

  case 29:
#line 360 "tp3.y"
                          {yyval.c = yyvsp[-2].c + "[<=]" + yyvsp[0].c;}
#line 1897 "y.tab.c"
    break;

  case 30:
#line 361 "tp3.y"
               {yyval.c = yyvsp[0].c + "[<=]";}
#line 1903 "y.tab.c"
    break;

  case 31:
#line 364 "tp3.y"
                 {yyval.c = yyvsp[-2].c + yyvsp[0].c;}
#line 1909 "y.tab.c"
    break;

  case 32:
#line 365 "tp3.y"
                   {yyval.c = yyvsp[-2].c + yyvsp[0].c;}
#line 1915 "y.tab.c"
    break;

  case 33:
#line 368 "tp3.y"
                                              { cmd_for(yyval, yyvsp[-6], yyvsp[-4], yyvsp[0], yyvsp[-2]); }
#line 1921 "y.tab.c"
    break;

  case 37:
#line 374 "tp3.y"
            { yyval.c = yyvsp[0].c + "^"; }
#line 1927 "y.tab.c"
    break;

  case 38:
#line 377 "tp3.y"
                       { yyval.c = yyvsp[0].c; }
#line 1933 "y.tab.c"
    break;

  case 39:
#line 380 "tp3.y"
                                { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1939 "y.tab.c"
    break;

  case 41:
#line 384 "tp3.y"
                                {yyval.c = insere_tabela_de_simbolos( DeclLet, yyvsp[0] ); }
#line 1945 "y.tab.c"
    break;

  case 42:
#line 385 "tp3.y"
                                {yyval.c = insere_tabela_de_simbolos( DeclLet, yyvsp[-2] ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1951 "y.tab.c"
    break;

  case 43:
#line 386 "tp3.y"
                            {yyval.c = insere_tabela_de_simbolos( DeclLet, yyvsp[-2] ) +  yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1957 "y.tab.c"
    break;

  case 44:
#line 389 "tp3.y"
                       { yyval.c = yyvsp[0].c; }
#line 1963 "y.tab.c"
    break;

  case 45:
#line 392 "tp3.y"
                                { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1969 "y.tab.c"
    break;

  case 47:
#line 396 "tp3.y"
                                {yyval.c = insere_tabela_de_simbolos( DeclVar, yyvsp[0] ); }
#line 1975 "y.tab.c"
    break;

  case 48:
#line 397 "tp3.y"
                                {yyval.c = insere_tabela_de_simbolos( DeclVar, yyvsp[-2] ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1981 "y.tab.c"
    break;

  case 49:
#line 398 "tp3.y"
                                {yyval.c = insere_tabela_de_simbolos( DeclVar, yyvsp[-2] ) + yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 1987 "y.tab.c"
    break;

  case 50:
#line 401 "tp3.y"
                            { yyval.c = yyvsp[0].c; }
#line 1993 "y.tab.c"
    break;

  case 51:
#line 404 "tp3.y"
                                      { yyval.c = yyvsp[-2].c + yyvsp[0].c; }
#line 1999 "y.tab.c"
    break;

  case 53:
#line 409 "tp3.y"
            { yyval.c = insere_tabela_de_simbolos( DeclConst,  yyvsp[-2] ) + 
                     yyvsp[-2].c + yyvsp[0].c + "=" + "^"; }
#line 2006 "y.tab.c"
    break;

  case 54:
#line 414 "tp3.y"
                                    { cmd_if_no_else(yyval, yyvsp[-2], yyvsp[0]); }
#line 2012 "y.tab.c"
    break;

  case 55:
#line 415 "tp3.y"
                                    { cmd_if_else(yyval, yyvsp[-4], yyvsp[0], yyvsp[-2]); }
#line 2018 "y.tab.c"
    break;

  case 56:
#line 418 "tp3.y"
                                  {  cmd_while(yyval, yyvsp[-2], yyvsp[0]);  }
#line 2024 "y.tab.c"
    break;

  case 57:
#line 421 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[0].c + "="; }
#line 2030 "y.tab.c"
    break;

  case 58:
#line 422 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[0].c + "="; }
#line 2036 "y.tab.c"
    break;

  case 59:
#line 423 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[0].c + "[=]"; }
#line 2042 "y.tab.c"
    break;

  case 60:
#line 424 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[0].c + "[=]"; }
#line 2048 "y.tab.c"
    break;

  case 61:
#line 425 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[-2].c +  "@" + yyvsp[0].c +  "+" + "=";}
#line 2054 "y.tab.c"
    break;

  case 62:
#line 426 "tp3.y"
                              { checa_declaracao(yyvsp[-2], true); yyval.c = yyvsp[-2].c + yyvsp[-2].c +  "[@]" + yyvsp[0].c +  "+" + "[=]";}
#line 2060 "y.tab.c"
    break;

  case 63:
#line 427 "tp3.y"
                              { 
                                 yyval.c = yyvsp[-1].c + "@" +  yyvsp[-1].c + yyvsp[-1].c + "@" + vector<string>{"1"} + vector<string>{"+"} + vector<string>{"="} + "^"; 
                              }
#line 2068 "y.tab.c"
    break;

  case 64:
#line 430 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2074 "y.tab.c"
    break;

  case 65:
#line 431 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2080 "y.tab.c"
    break;

  case 66:
#line 432 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2086 "y.tab.c"
    break;

  case 67:
#line 433 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2092 "y.tab.c"
    break;

  case 68:
#line 434 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2098 "y.tab.c"
    break;

  case 69:
#line 435 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2104 "y.tab.c"
    break;

  case 70:
#line 436 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2110 "y.tab.c"
    break;

  case 71:
#line 437 "tp3.y"
                              { yyval.c = yyvsp[-2].c + yyvsp[0].c + yyvsp[-1].c; }
#line 2116 "y.tab.c"
    break;

  case 72:
#line 439 "tp3.y"
    {
      yyval.c = yyvsp[-1].c + to_string( yyvsp[-1].contador ) + yyvsp[-3].c + "$";
    }
#line 2124 "y.tab.c"
    break;

  case 73:
#line 442 "tp3.y"
                            {yyval.c = vector<string>{"[]"} + yyvsp[-1].c;}
#line 2130 "y.tab.c"
    break;

  case 76:
#line 448 "tp3.y"
             { yyval.clear(); }
#line 2136 "y.tab.c"
    break;

  case 77:
#line 451 "tp3.y"
                            { yyval.contador++; yyval.c =  yyvsp[-2].c + to_string( yyval.contador ) + yyvsp[0].c + "[<=]";}
#line 2142 "y.tab.c"
    break;

  case 78:
#line 452 "tp3.y"
                              { yyval.contador++; yyval.c =  yyvsp[-2].c + to_string( yyval.contador ) + yyvsp[0].c + "[<=]";}
#line 2148 "y.tab.c"
    break;

  case 79:
#line 453 "tp3.y"
              {yyval.c = to_string( yyval.contador ) + yyvsp[0].c + "[<=]";}
#line 2154 "y.tab.c"
    break;

  case 80:
#line 454 "tp3.y"
                 {yyval.c = to_string( yyval.contador ) + yyvsp[0].c + "[<=]";}
#line 2160 "y.tab.c"
    break;

  case 82:
#line 460 "tp3.y"
             { yyval.clear(); }
#line 2166 "y.tab.c"
    break;

  case 83:
#line 464 "tp3.y"
       { yyval.c = yyvsp[-2].c + yyvsp[0].c;
          yyval.contador++; }
#line 2173 "y.tab.c"
    break;

  case 84:
#line 467 "tp3.y"
     { yyval.c = yyvsp[-3].c + vector<string>{"{}"};
          yyval.contador++; }
#line 2180 "y.tab.c"
    break;

  case 85:
#line 470 "tp3.y"
       { yyval.c = yyvsp[0].c;
         yyval.contador = 1; }
#line 2187 "y.tab.c"
    break;

  case 86:
#line 475 "tp3.y"
          {yyval.c = "0" + yyvsp[0].c + yyvsp[-1].c;}
#line 2193 "y.tab.c"
    break;

  case 92:
#line 483 "tp3.y"
                        { checa_declaracao(yyvsp[0], false ); yyval.c = yyvsp[0].c + "@"; }
#line 2199 "y.tab.c"
    break;

  case 93:
#line 484 "tp3.y"
                        {yyval.c = yyvsp[0].c + "[@]";}
#line 2205 "y.tab.c"
    break;

  case 94:
#line 485 "tp3.y"
                        { yyval.c = yyvsp[-1].c; }
#line 2211 "y.tab.c"
    break;

  case 96:
#line 491 "tp3.y"
                         { yyval.c = yyvsp[-3].c + yyvsp[-1].c;}
#line 2217 "y.tab.c"
    break;

  case 97:
#line 492 "tp3.y"
                         { yyval.c = yyvsp[-2].c + yyvsp[0].c;}
#line 2223 "y.tab.c"
    break;


#line 2227 "y.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = YY_CAST (char *, YYSTACK_ALLOC (YY_CAST (YYSIZE_T, yymsg_alloc)));
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;


#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif


/*-----------------------------------------------------.
| yyreturn -- parsing is finished, return the result.  |
`-----------------------------------------------------*/
yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[+*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 497 "tp3.y"


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
