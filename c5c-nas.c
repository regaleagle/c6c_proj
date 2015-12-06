#include <stdio.h>
#include <stdbool.h>
#include "calc3.h"
#include "y.tab.h"

static int lbl;
static bool noVar = true;

int ex(nodeType *p) {
    int lblx, lbly, lbl1, lbl2;

    if (!p) return 0;
    if (noVar){
        printf("\tpush\t26\n");
        printf("\tpop\tsp\n");
        noVar = false;
    }
    switch(p->type) {
        case typeCon:       
            printf("\tpush\t%d\n", p->con.value); 
            break;
        case typeId:       
            //TODO VARIALBLES 
            printf("\tpush\tsb[%d]\n", p->id.i); 
            break;
        case typeOpr:
            switch(p->opr.oper) {
            	case FOR:
            		ex(p->opr.op[0]);
            		printf("L%03d:\n", lblx = lbl++);
            		ex(p->opr.op[1]);
            		printf("\tj0\tL%03d\n", lbly = lbl++);
            		ex(p->opr.op[3]);
            		ex(p->opr.op[2]);
            		printf("\tjmp\tL%03d\n", lblx);
            		printf("L%03d:\n", lbly);
            		break;
                case WHILE:
                    printf("L%03d:\n", lbl1 = lbl++);
                    ex(p->opr.op[0]);
                    printf("\tj0\tL%03d\n", lbl2 = lbl++);
                    ex(p->opr.op[1]);
                    printf("\tjmp\tL%03d\n", lbl1);
                    printf("L%03d:\n", lbl2);
                    break;
                case IF:
                    ex(p->opr.op[0]);
                    if (p->opr.nops > 2) {
                        /* if else */
                        printf("\tj0\tL%03d\n", lbl1 = lbl++);
                        ex(p->opr.op[1]);
                        printf("\tjmp\tL%03d\n", lbl2 = lbl++);
                        printf("L%03d:\n", lbl1);
                        ex(p->opr.op[2]);
                        printf("L%03d:\n", lbl2);
                    } else {
                        /* if */
                        printf("\tj0\tL%03d\n", lbl1 = lbl++);
                        ex(p->opr.op[1]);
                        printf("L%03d:\n", lbl1);
                    }
                    break;
            	case READ:
                //TODO - variables
            	    printf("\tgeti\n");
                    printf("\tpop\tsb[%d]\n", p->opr.op[0]->id.i);
            	    break;
                case PRINT:
                    //TODO? PRINT newline     
                    ex(p->opr.op[0]);
                    printf("\tputi\n");
                    break;
                case '=':       
                    ex(p->opr.op[1]);
                    printf("\tpop\tsb[%d]\n", p->opr.op[0]->id.i);
                    break;
                case UMINUS:    
                    ex(p->opr.op[0]);
                    printf("\tneg\n");
                    break;
                default:
                    ex(p->opr.op[0]);
                    ex(p->opr.op[1]);
                    switch(p->opr.oper) {
                        case '+':   printf("\tadd\n"); break;
                        case '-':   printf("\tsub\n"); break; 
                        case '*':   printf("\tmul\n"); break;
                        case '/':   printf("\tdiv\n"); break;
                        case '%':   printf("\tmod\n"); break;
                        case '<':   printf("\tcomplt\n"); break;
                        case '>':   printf("\tcompgt\n"); break;
                        case GE:    printf("\tcompge\n"); break;
                        case LE:    printf("\tcomple\n"); break;
                        case NE:    printf("\tcompne\n"); break;
                        case EQ:    printf("\tcompeq\n"); break;
                	    case AND:   printf("\tand\n"); break;
                	    case OR:    printf("\tor\n"); break;
                    }
            }
    }
    return 0;
}
