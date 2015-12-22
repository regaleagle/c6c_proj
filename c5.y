%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "calc3.h"


/* prototypes */
nodeType *opr(int oper, int nops, ...);
nodeType *id(char *i);
nodeType *fid(char *i);
nodeType *conInt(int value);
nodeType *conChar(char value);
nodeType *conStr(char *value);
void freeNode(nodeType *p);
int ex(nodeType *p);
void printProg();
//int ex(nodeType *p, int breakTo, int contTo);
int yylex(void);

void yyerror(char *s);

int sym[26];                    /* symbol table */
%}

%union {
    int iValue;                 /* integer value */
    char sIndex;                /* symbol table index */
    char *string;         /* regular String */

    nodeType *nPtr;             /* node pointer */
};

%token <iValue> INTEGER
%token <string> VARIABLE
%token <string> GLOBVARIABLE
%token <sIndex> CHAR
%token <string> STRING
%token FOR WHILE IF BREAK CONT DO GETI GETC GETS FUNCNOPAR FUNC ARGS CALLNOARG
%token PUTI PUTI_ PUTC PUTS PUTS_ ARRAY INITARRAY GETARRAY ASSIGNARRAY CALL ARG 
%nonassoc IFX
%nonassoc ELSE
// %nonassoc CA
// %nonassoc '{'

%left AND OR

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc UMINUS


%type <nPtr>  arglist stmt expr function stmt_list 

%%

program:
        function_list               { printProg(); exit(0); }
        ;

function_list:
        function_list function               { ex($2); freeNode($2); }
        |function_list stmt                   { ex($2); freeNode($2); }
        | /*NULL*/
        ;

function:
          VARIABLE '('')' '{'stmt_list'}'           { $$ = opr(FUNCNOPAR, 2, id($1), $5);}
        | VARIABLE '(' arglist ')''{' stmt_list'}'   { $$ = opr(FUNC, 3, id($1), $3, $6);}
        ;

stmt:
          ';'                                       { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                                  { $$ = $1; }
        | BREAK ';'                                 { $$ = opr(BREAK, 0);}
        | CONT ';'                                  { $$ = opr(CONT, 0);}
        | ARRAY VARIABLE '['INTEGER']' ';'          { $$ = opr(ARRAY, 2, id($2), conInt($4));}
        | ARRAY VARIABLE '['INTEGER']' '=' INTEGER ';'  { $$ = opr(INITARRAY, 3, id($2), conInt($4), conInt($7));}
        | VARIABLE '['expr']' '=' expr';'           { $$ = opr(ASSIGNARRAY, 3, id($1), $3, $6);}
        | PUTI '(' expr ')'';'                      { $$ = opr(PUTI, 1, $3); }
        | PUTI_ '(' expr ')'';'                     { $$ = opr(PUTI_, 1, $3); }
        | PUTC '(' expr ')' ';'                     { $$ = opr(PUTC, 1, $3); }
        | PUTS '(' expr ')' ';'                     { $$ = opr(PUTS, 1, $3); }
        | PUTS_ '(' expr ')' ';'                    { $$ = opr(PUTS_, 1, $3); }
        | GETI '(' VARIABLE ')' ';'         { $$ = opr(GETI, 1, id($3)); }
        | GETC '(' VARIABLE ')' ';'         { $$ = opr(GETC, 1, id($3)); }
        | GETS '(' VARIABLE ')' ';'         { $$ = opr(GETS, 1, id($3)); }
        | VARIABLE '=' expr ';'             { $$ = opr('=', 2, id($1), $3); }
    	| FOR '(' stmt stmt stmt ')' stmt   { $$ = opr(FOR, 4, $3, $4, $5, $7); }
        | WHILE '(' expr ')' stmt           { $$ = opr(WHILE, 2, $3, $5); }
        | DO stmt WHILE '(' expr ')'        { $$ = opr(DO, 2, $5, $2); }
        | IF '(' expr ')' stmt %prec IFX    { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt    { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'                 { $$ = $2; }
        ;

stmt_list:
          stmt                  { $$ = $1; }
        | stmt_list stmt        { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER                   {  $$ = conInt($1); }
        | CHAR                      {  $$ = conChar($1); }
        | STRING                    {  $$ = conStr($1); }
        | VARIABLE                  {  $$ = id($1); }
        | VARIABLE '('arglist')'   { $$ = opr(CALL, 2, fid($1), $3);}
        | VARIABLE '('')'           { $$ = opr(CALLNOARG, 1, fid($1));}
        | VARIABLE '['expr']'   { $$ = opr(GETARRAY, 2, id($1), $3);}
        | '-' expr %prec UMINUS { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr         { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr         { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr         { $$ = opr('*', 2, $1, $3); }
        | expr '%' expr         { $$ = opr('%', 2, $1, $3); }
        | expr '/' expr         { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr         { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr         { $$ = opr('>', 2, $1, $3); }
        | expr GE expr          { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr          { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr          { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr          { $$ = opr(EQ, 2, $1, $3); }
	    | expr AND expr		    { $$ = opr(AND, 2, $1, $3); }
    	| expr OR expr		    { $$ = opr(OR, 2, $1, $3); }
        | '(' expr ')'          { $$ = $2; }
        ;
arglist:
         expr                   { $$ = opr(ARG, 1, $1); }
        | arglist ','expr       { $$ = opr(ARGS, 2, $1, $3);}

%%

#define SIZEOF_NODETYPE ((char *)&p->conInt - (char *)p)

nodeType *conInt(int value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conIntNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeConInt;
    p->conInt.value = value;

    return p;
}

nodeType *conChar(char value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conCharNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeConChar;
    p->conChar.value = value;

    return p;
}

nodeType *conStr(char *value) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(conStrNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeConStr;
    p->conStr.value = value;

    return p;
}

nodeType *id(char *i) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(idNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;

    return p;
}

nodeType *fid(char *i) {
    nodeType *p;
    size_t nodeSize;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(funcIdNodeType);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeFuncId;
    p->fid.i = i;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    size_t nodeSize;
    int i;

    /* allocate node */
    nodeSize = SIZEOF_NODETYPE + sizeof(oprNodeType) +
        (nops - 1) * sizeof(nodeType*);
    if ((p = malloc(nodeSize)) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}
      

void yyerror(char *s) {
    fprintf(stdout, "%s\n", s);
}

int main(int argc, char **argv) {
    fflush(stdout);

extern FILE* yyin;
    yyin = fopen(argv[1], "r");
    yyparse();
    return 0;
}
