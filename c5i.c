#include <stdio.h>
#include <stdbool.h>
#include "calc3.h"
#include "y.tab.h"

static bool cont = false; // = true if "continue" has been encountered
static bool brk = false; // = true if "break" has been encountered
static int scope = 0; // outermost scope - no continue/break allowed

int ex(nodeType *p) {
    int i;
    if (!p || cont || brk) return 0; //if cont or brk, do not execute
    switch(p->type) {
    case typeCon:       return p->con.value;
    case typeId:        return sym[p->id.i];
    case typeOpr:
        switch(p->opr.oper) {
        	case FOR:	    scope++;
                            ex(p->opr.op[0]);
        			        while (ex(p->opr.op[1])) {
            				    ex(p->opr.op[3]);
                                if(brk){ brk = false; break;}
                                cont = false; // cont is always false leaving the loop
            				    ex(p->opr.op[2]); 
        			        }
                            scope--;
        			        return 0;
            case WHILE:     scope++;
                            while(ex(p->opr.op[0])){
                                ex(p->opr.op[1]);
                                if(brk){ brk = false; break;} 
                                cont = false; // cont is always false leaving the loop
                            }
                            scope--;
                            return 0;
            case DO:        scope++;
                            do{
                                ex(p->opr.op[1]);
                                if(brk){ brk = false; break;} 
                                cont = false; // cont is always false leaving the loop
                            }
                            while(ex(p->opr.op[0]));
                            scope--;
                            return 0;
            case IF:        if (ex(p->opr.op[0]))
                                    ex(p->opr.op[1]);
                            else if (p->opr.nops > 2)
                                    ex(p->opr.op[2]);
                            return 0;
            case PRINT:     printf("%d\n", ex(p->opr.op[0])); return 0;
        	case READ:	    printf("? "); scanf("%d", &i);
        			        return sym[p->opr.op[0]->id.i] = i;
            case ';':       ex(p->opr.op[0]); return ex(p->opr.op[1]);
            case '=':       return sym[p->opr.op[0]->id.i] = ex(p->opr.op[1]);
            case UMINUS:    return -ex(p->opr.op[0]);
            case '+':       return ex(p->opr.op[0]) + ex(p->opr.op[1]);
            case '-':       return ex(p->opr.op[0]) - ex(p->opr.op[1]);
            case '*':       return ex(p->opr.op[0]) * ex(p->opr.op[1]);
            case '/':       return ex(p->opr.op[0]) / ex(p->opr.op[1]);
            case '%':       return ex(p->opr.op[0]) % ex(p->opr.op[1]);
            case '<':       return ex(p->opr.op[0]) < ex(p->opr.op[1]);
            case '>':       return ex(p->opr.op[0]) > ex(p->opr.op[1]);
            case GE:        return ex(p->opr.op[0]) >= ex(p->opr.op[1]);
            case LE:        return ex(p->opr.op[0]) <= ex(p->opr.op[1]);
            case NE:        return ex(p->opr.op[0]) != ex(p->opr.op[1]);
            case EQ:        return ex(p->opr.op[0]) == ex(p->opr.op[1]);
        	case AND:	    return ex(p->opr.op[0]) && ex(p->opr.op[1]);
        	case OR:	    return ex(p->opr.op[0]) || ex(p->opr.op[1]);
            case BREAK:     if (scope > 0) brk = true; return 0; //TODO
            case CONT:      if (scope > 0) cont = true; return 0; //TODO
        }
    }
    return 0;
}
