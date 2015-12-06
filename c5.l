%{
#include <stdlib.h>
#include "calc3.h"
#include "y.tab.h"
void yyerror(char *);
%}

%%


['][a-z][']   {   
                    yylval.sIndex = yytext[1];
                    return CHAR;
                }

0           {
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

"\"\""                  {   
                            char * newString = "";
                            yylval.string = newString;
                            return STRING;
                        }

["][^"]*["]             {
                            char newString[(strlen(yytext) - 1)];
                            memcpy(newString, &yytext[1], strlen(yytext)-1);
                            newString[strlen(yytext) - 2] = '\0';
                            yylval.string = strdup(newString);
                            return STRING;
                        }

[1-9][0-9]* {   
                yylval.iValue = atoi(yytext);
                return INTEGER;
            }

[-()<>=+*/;{}.%\[\]] {
                return *yytext;
             }

">="            return GE;
"<="            return LE;
"=="            return EQ;
"!="            return NE;
"for"		    return FOR;
"do"            return DO;
"while"         return WHILE;
"if"            return IF;
"else"          return ELSE;
"puti"          return PUTI;
"puti_"         return PUTI_;
"putc"          return PUTC;
"puts"          return PUTS;
"puts_"         return PUTS_;
"geti"          return GETI;
"getc"          return GETC;
"gets"          return GETS;
"array"         return ARRAY;
"break"     return BREAK;
"continue"  return CONT;
"&&"		return AND;
"||"		return OR;

[a-zA-Z][a-zA-Z0-9]*        {   
                    yylval.string = strdup(yytext);
                    return VARIABLE;
                }

\/\/.*\n	;	/* skip comments */

[ \t\n]+        ;       /* ignore whitespace*/

.               yyerror("Unknown character");
%%
int yywrap(void) {
    return 1;
}

