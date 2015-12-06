#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "calc3.h"
#include "y.tab.h"

static int lbl;
static bool noVar = true;
void addVar(char *s);
int findVar(char *s);
void freeVar(struct variableList *v);
void appendString(char *newString);
void storeUserVar(char * newString);

struct variableList *first;
struct variableList *iter;
int varCounter;
char * printString;
char inlineTemp[500];

int ex(nodeType *p, int breakTo, int contTo) {
    int lblx, lbly, lbl1, lbl2, lblCont;
    int breakArg, contArg;

    if (!p) {
        // freeVar(first);
        
        return 0;
    }
    if (breakTo >= 0 || contTo >=0){
        breakArg = breakTo;
        contArg = contTo;
    }else{
        breakArg = -1;
        contArg = -1;
    }
    if (noVar){
        asprintf(&printString, "");
        first = (struct variableList *)malloc(sizeof(struct variableList));
        first->pos = 0;
        first->var = "first";
        first->next = NULL;
        iter = first;
        noVar = false;
    }
    switch(p->type) {
        case typeConInt:
            sprintf(inlineTemp, "\tpush\t%d\n", p->conInt.value);
            appendString(inlineTemp);
            break;
        case typeConChar:       
            sprintf(inlineTemp, "\tpush\t\'%c\'\n", p->conChar.value);
            appendString(inlineTemp);
            break;
        case typeConStr:       
            sprintf(inlineTemp, "\tpush\t\"%s\"\n", p->conStr.value); 
            appendString(inlineTemp);
            break;
        case typeId:
            //MUST ADAPT FOR GLOBAL VS LOCAL        
            {int index1 = findVar(p->id.i);
            sprintf(inlineTemp, "\tpush\tsb[%d]\n", index1);
            appendString(inlineTemp);} 
            break;
        case typeOpr:
            switch(p->opr.oper) {
                case ARRAY:
                    {int index2 = varCounter;
                    // printf("%d\n", varCounter);
                    addVar(p->opr.op[0]->id.i);
                    // sprintf(inlineTemp, "\tpop\tsb[%d]\n", index2);
                    // appendString(inlineTemp);
                    varCounter += p->opr.op[1]->conInt.value;}
                    // printf("%d\n", varCounter);
                    break;
                case GETARRAY:
                    ex(p->opr.op[1], breakArg, contArg);
                    int arrAdd = findVar(p->opr.op[0]->id.i);
                    sprintf(inlineTemp, "\tpush\t%d\n\tadd\n\tpop\tin\n\tpush\tsb[in]\n", arrAdd);
                    appendString(inlineTemp);
                    break;
                case ASSIGNARRAY:
                    {int arrAdd = findVar(p->opr.op[0]->id.i);
                    ex(p->opr.op[1], breakArg, contArg);
                    sprintf(inlineTemp, "\tpush\t%d\n\tadd\n\tpop\tin\n", arrAdd);
                    appendString(inlineTemp);
                    ex(p->opr.op[2], breakArg, contArg);   
                    sprintf(inlineTemp, "\tpop\tsb[in]\n");
                    appendString(inlineTemp);}            
                    break;
            	case FOR:
            		ex(p->opr.op[0], breakArg, contArg);
            		sprintf(inlineTemp, "L%03d:\n", lblx = lbl++);
                    appendString(inlineTemp);
            		ex(p->opr.op[1], breakArg, contArg);
            		sprintf(inlineTemp, "\tj0\tL%03d\n", lbly = lbl++);
                    appendString(inlineTemp);
                    lblCont = lbl++;
            		ex(p->opr.op[3], lbly, lblCont);
                    sprintf(inlineTemp, "L%03d:\n", lblCont);
                    appendString(inlineTemp);
            		ex(p->opr.op[2], breakArg, contArg);
            		sprintf(inlineTemp, "\tjmp\tL%03d\n", lblx);
                    appendString(inlineTemp);
            		sprintf(inlineTemp, "L%03d:\n", lbly);
                    appendString(inlineTemp);
            		break;
                case WHILE:
                    sprintf(inlineTemp, "L%03d:\n", lbl1 = lbl++);
                    appendString(inlineTemp);
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tj0\tL%03d\n", lbl2 = lbl++);
                    appendString(inlineTemp);
                    ex(p->opr.op[1],lbl2, lbl1);
                    sprintf(inlineTemp, "\tjmp\tL%03d\n", lbl1);
                    appendString(inlineTemp);
                    sprintf(inlineTemp, "L%03d:\n", lbl2);
                    appendString(inlineTemp);
                    break;
                case DO:
                    lbl1 = lbl++;
                    lblCont = lbl++;
                    lbl2 = lbl++;
                    sprintf(inlineTemp, "L%03d:\n", lbl1);
                    appendString(inlineTemp);
                    ex(p->opr.op[1],lbl2, lblCont);
                    sprintf(inlineTemp, "L%03d:\n", lblCont);
                    appendString(inlineTemp);
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tj0\tL%03d\n", lbl2);
                    appendString(inlineTemp);
                    sprintf(inlineTemp, "\tjmp\tL%03d\n", lbl1);
                    appendString(inlineTemp);
                    sprintf(inlineTemp, "L%03d:\n", lbl2);
                    appendString(inlineTemp);
                    break;
                case IF:
                    ex(p->opr.op[0], breakArg, contArg);
                    if (p->opr.nops > 2) {
                        /* if else */
                        sprintf(inlineTemp, "\tj0\tL%03d\n", lbl1 = lbl++);
                        appendString(inlineTemp);
                        ex(p->opr.op[1], breakArg, contArg);
                        sprintf(inlineTemp, "\tjmp\tL%03d\n", lbl2 = lbl++);
                        appendString(inlineTemp);
                        sprintf(inlineTemp, "L%03d:\n", lbl1);
                        appendString(inlineTemp);
                        ex(p->opr.op[2], breakArg, contArg);
                        sprintf(inlineTemp, "L%03d:\n", lbl2);
                        appendString(inlineTemp);
                    } else {
                        /* if */
                        sprintf(inlineTemp, "\tj0\tL%03d\n", lbl1 = lbl++);
                        appendString(inlineTemp);
                        ex(p->opr.op[1], breakArg, contArg);
                        sprintf(inlineTemp, "L%03d:\n", lbl1);
                        appendString(inlineTemp);
                    }
                    break;
            	case GETI:
            	    sprintf(inlineTemp, "\tgeti\n");
                    appendString(inlineTemp);
                    storeUserVar(p->opr.op[0]->id.i);
            	    break;
                case GETC:
                    sprintf(inlineTemp, "\tgetc\n");
                    appendString(inlineTemp);
                    storeUserVar(p->opr.op[0]->id.i);
                    break;
                case GETS:
                    sprintf(inlineTemp, "\tgets\n");
                    appendString(inlineTemp);
                    storeUserVar(p->opr.op[0]->id.i);
                    break;
                case PUTI:
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tputi\n");
                    appendString(inlineTemp);
                    break;
                case PUTI_:
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tputi_\n");
                    appendString(inlineTemp);
                    break;
                case PUTC:
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tputc\n");
                    appendString(inlineTemp);
                    break;
                case PUTS:
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tputs\n");
                    appendString(inlineTemp);
                    break;
                case PUTS_:
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tputs_\n");
                    appendString(inlineTemp);
                    break;
                case '=': 
                    //Fix Variable to string      
                    ex(p->opr.op[1], breakArg, contArg);
                    int index3 = varCounter;
                    // printf("the var counter: %d\n", varCounter);
                    // printf("looking for: %s\n", p->opr.op[0]->id.i);
                    int temp = findVar(p->opr.op[0]->id.i);
                    // printf("temp: %d\n", temp);
                    if(temp == -1){
                        addVar(p->opr.op[0]->id.i);
                        varCounter++;
                    }else{
                        index3 = temp;
                    }
                    sprintf(inlineTemp, "\tpop\tsb[%d]\n", index3);
                    appendString(inlineTemp);
                    
                    break;
                case UMINUS:    
                    ex(p->opr.op[0], breakArg, contArg);
                    sprintf(inlineTemp, "\tneg\n");
                    appendString(inlineTemp);
                    break;
                case BREAK:
                    sprintf(inlineTemp, "\tjmp\tL%03d\n", breakTo);
                    appendString(inlineTemp);
                    break;
                case CONT:
                    sprintf(inlineTemp, "\tjmp\tL%03d\n", contTo);
                    appendString(inlineTemp);
                    break;
                default:
                    ex(p->opr.op[0], breakArg, contArg);
                    ex(p->opr.op[1], breakArg, contArg);
                    switch(p->opr.oper) {
                        case '+':   sprintf(inlineTemp, "\tadd\n"); appendString(inlineTemp); break;
                        case '-':   sprintf(inlineTemp, "\tsub\n"); appendString(inlineTemp); break; 
                        case '*':   sprintf(inlineTemp, "\tmul\n"); appendString(inlineTemp); break;
                        case '/':   sprintf(inlineTemp, "\tdiv\n"); appendString(inlineTemp); break;
                        case '%':   sprintf(inlineTemp, "\tmod\n"); appendString(inlineTemp); break;
                        case '<':   sprintf(inlineTemp, "\tcomplt\n"); appendString(inlineTemp); break;
                        case '>':   sprintf(inlineTemp, "\tcompgt\n"); appendString(inlineTemp); break;
                        case GE:    sprintf(inlineTemp, "\tcompge\n"); appendString(inlineTemp); break;
                        case LE:    sprintf(inlineTemp, "\tcomple\n"); appendString(inlineTemp); break;
                        case NE:    sprintf(inlineTemp, "\tcompne\n"); appendString(inlineTemp); break;
                        case EQ:    sprintf(inlineTemp, "\tcompeq\n"); appendString(inlineTemp); break;
                	    case AND:   sprintf(inlineTemp, "\tand\n"); appendString(inlineTemp); break;
                	    case OR:    sprintf(inlineTemp, "\tor\n"); appendString(inlineTemp); break;
                    }
            }
    }
    return 0;
}

void addVar(char *s) {
    struct variableList *temp = (struct variableList *)malloc(sizeof(struct variableList));
    temp->pos = varCounter;
    // printf("newPos = %d\n", temp->pos);
    temp->var = s;
    iter->next = temp;
    iter = temp;
    iter->next = NULL; 
}

int findVar(char *s) {
    if(first->next != NULL){
        struct variableList *temp = first->next; 
        do{
            if(!strcmp (temp->var,s)){
                return temp->pos;
            }
            // }else if (temp->next != NULL){
            //     printf("looping temp\n");
            //     temp = temp->next;
            // }
        } while((temp = temp->next) != NULL);
    }
    return -1;

}
void freeVar(struct variableList *v) {
    while(v->next != NULL){
        freeVar(v->next);
    }
    free(v);
}

void appendString(char * newString){
    char * temp;
    asprintf(&temp, "%s%s", printString, newString);
    free(printString);
    printString = temp; 
}
void storeUserVar(char * newString){
    int index2 = varCounter;
    int tempVar = findVar(newString);
    if(tempVar == -1){
        addVar(newString);
        varCounter++;
    }else{
        index2 = tempVar;
    }
    sprintf(inlineTemp, "\tpop\tsb[%d]\n", index2);
    appendString(inlineTemp);
}


void printProg(){
    printf("\tpush\t%d\n",varCounter);
    printf("\tpop\tsp\n");
    printf("%s", printString);    
}
