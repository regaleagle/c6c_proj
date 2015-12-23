#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "calc3.h"
#include "y.tab.h"

static int lbl;
static bool noVar = true;
static bool declare = false;
void addVar(char *s);
int findVar(char *s);
void freeVar(struct variableList *v);
void storeUserVar(char * newString);
void appendString(char * newString);
struct funcList* findFunc(char *s);
void addFunc(char *s, int label);
void appendFuncString(char * newString);


struct variableList *first;
struct variableList *iter;
int varCounter = 0;
int localVarCounter = 0;
char * printString;
char inlineTemp[500];
int scope = 0;
int tempScope = 0;


char * funcString;
char * startFuncString;
char * finalFuncString;
char functionTemp[500];
struct variableList *localFirst;
struct variableList *localIter;

struct funcList *funcFirst;
struct funcList *funcIter;

int params = 0;
int args = 0;

int ex(nodeType *p, int breakTo, int contTo) {
    int lblx, lbly, lbl1, lbl2, lblCont;
    int breakArg, contArg;

    if (!p) {     
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
        asprintf(&funcString, "");
        asprintf(&startFuncString, "");
        asprintf(&finalFuncString, "");
        first = (struct variableList *)malloc(sizeof(struct variableList));
        first->pos = 0;
        first->var = "first";
        first->next = NULL;
        iter = first;
        localFirst = (struct variableList *)malloc(sizeof(struct variableList));
        localFirst->pos = 0;
        localFirst->var = "first";
        localFirst->next = NULL;
        localIter = localFirst;
        funcFirst = (struct funcList *)malloc(sizeof(struct funcList));
        funcFirst->pos = 0;
        funcFirst->var = "first";
        funcFirst->next = NULL;
        funcIter = funcFirst;
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
            // printf("here\n");
            //memmove(yytext, yytext+1, strlen(yytext));
            //MUST ADAPT FOR GLOBAL VS LOCAL                

            if (declare){
                params++;     
                if(findVar(p->id.i) == -1){
                    addVar(p->id.i);
                    localVarCounter--;
                }   
            }else{
                tempScope = scope;
                if(p->id.i[0] == '@') {scope = 0;memmove(p->id.i, p->id.i+1, strlen(p->id.i));}
                int index1 = findVar(p->id.i);
                if(scope == 0) sprintf(inlineTemp, "\tpush\tsb[%d]\n", index1);
                else sprintf(inlineTemp, "\tpush\tfp[%d]\n", index1);
                scope = tempScope;
                appendString(inlineTemp); 
                
            }
            break;
        case typeFuncId:        
            {struct funcList* funct = findFunc(p->fid.i);
            if(args == funct->params){
                sprintf(inlineTemp, "\tcall\tL%03d, %d\n", funct->pos, funct->params);
                appendString(inlineTemp);} 
            }
            args = 0;
            break;
        case typeOpr:
            switch(p->opr.oper) {
                case CALL:
                    args = 0;
                    ex(p->opr.op[1], breakArg, contArg);
                    ex(p->opr.op[0], breakArg, contArg);

                    break;
                case CALLNOARG:
                    args = 0;
                    ex(p->opr.op[0], breakArg, contArg);

                    break;
                case FUNCNOPAR:
                    if(scope == 0){
                        sprintf(startFuncString, "L%03d:\n", lblx = lbl++);
                        if(findVar(p->opr.op[0]->id.i) == -1 && findFunc(p->opr.op[0]->id.i) == NULL){
                            scope++;
                            addFunc(p->opr.op[0]->id.i,lblx);
                            localVarCounter = 0;
                            ex(p->opr.op[1], breakArg, contArg);
                            sprintf(inlineTemp, "\tret\n");
                            appendString(inlineTemp);
                            appendFuncString(startFuncString);
                            sprintf(inlineTemp, "\tpush\t%d\n\tpush\tsp\n\tadd\n\tpop\tsp\n",localVarCounter);
                            appendFuncString(inlineTemp);
                            appendFuncString(funcString);
                            free(funcString);
                            asprintf(&funcString, "");
                            freeVar(localFirst);
                            localFirst = (struct variableList *)malloc(sizeof(struct variableList));
                            localFirst->pos = 0;
                            localFirst->var = "first";
                            localFirst->next = NULL;
                            localIter = localFirst;
                            scope--;
                            params = 0;
                        }
                    }
                    break;
                case FUNC:
                    if(scope == 0){
                        localVarCounter = -4;
                        sprintf(startFuncString, "L%03d:\n", lblx = lbl++);
                        if(findVar(p->opr.op[0]->id.i) == -1 && findFunc(p->opr.op[0]->id.i) == NULL){
                            scope++;
                            declare = true;
                            ex(p->opr.op[1], breakArg, contArg);
                            declare = false;
                            addFunc(p->opr.op[0]->id.i,lblx);
                            localVarCounter = 0;
                            ex(p->opr.op[2], breakArg, contArg);
                            sprintf(inlineTemp, "\tret\n");
                            appendString(inlineTemp);
                            appendFuncString(startFuncString);
                            sprintf(inlineTemp, "\tpush\t%d\n\tpush\tsp\n\tadd\n\tpop\tsp\n",localVarCounter);
                            appendFuncString(inlineTemp);
                            appendFuncString(funcString);
                            free(funcString);
                            asprintf(&funcString, "");
                            freeVar(localFirst);
                            localFirst = (struct variableList *)malloc(sizeof(struct variableList));
                            localFirst->pos = 0;
                            localFirst->var = "first";
                            localFirst->next = NULL;
                            localIter = localFirst;
                            scope--;
                            params = 0;
                        }
                    }
                    break;
                case ARG:
                    args++;
                    ex(p->opr.op[0], breakArg, contArg);
                    break;
                case ARGS:
                    ex(p->opr.op[0], breakArg, contArg);
                    args++;
                    ex(p->opr.op[1], breakArg, contArg);
                    
                    break;
                case ARRAYS:
                    ex(p->opr.op[0], breakArg, contArg);
                    if(scope == 0){
                        int index2 = varCounter;
                        addVar(p->opr.op[1]->id.i);
                        varCounter += p->opr.op[2]->conInt.value;
                    }else{
                        int index2 = localVarCounter;
                        addVar(p->opr.op[1]->id.i);
                        localVarCounter += p->opr.op[2]->conInt.value;
                    }
                    break;
                case ARRAY:

                    if(scope == 0){
                        int index2 = varCounter;
                        addVar(p->opr.op[0]->id.i);
                        varCounter += p->opr.op[1]->conInt.value;
                    }else{
                        int index2 = localVarCounter;
                        addVar(p->opr.op[0]->id.i);
                        localVarCounter += p->opr.op[1]->conInt.value;
                    }
                    break;
                 case INITSTRING:
                    {int index2;
                    int valueInt;
                    int incr;
                    if(scope == 0){
                        index2 = varCounter;
                        addVar(p->opr.op[0]->id.i);
                        varCounter += p->opr.op[1]->conInt.value;
                        valueInt = p->opr.op[2]->conInt.value;
                        incr = varCounter;

                    }else{
                        index2 = localVarCounter;
                        addVar(p->opr.op[0]->id.i);
                        localVarCounter += p->opr.op[1]->conInt.value;
                        valueInt = p->opr.op[2]->conInt.value;
                        incr = localVarCounter;
                    }
                    sprintf(inlineTemp, "\tpush\t%d\n\tpop\tin\nL%03d:\n\tpush\tin\n\tpush\t1\n\tsub\n\tpop\tin\n\tpush\t%d\n", incr, lbl1 = lbl++, valueInt);
                    appendString(inlineTemp);
                    if(scope == 0){
                        sprintf(inlineTemp,  "\tpop\tsb[in]\n\tpush\tin\n\tpush\t%d\n", index2);
                        appendString(inlineTemp);
                    }else{
                        sprintf(inlineTemp,  "\tpop\tfp[in]\n\tpush\tin\n\tpush\t%d\n", index2);
                        appendString(inlineTemp);
                    }
                    sprintf(inlineTemp, "\tcompgt\n\tj1\tL%03d\n", lbl1);
                    appendString(inlineTemp);
                    }
                    
                    break;

                case INITARRAY:
                    {int index2;
                    int valueInt;
                    int incr;
                    if(scope == 0){
                        index2 = varCounter;
                        addVar(p->opr.op[0]->id.i);
                        varCounter += p->opr.op[1]->conInt.value;
                        valueInt = p->opr.op[2]->conInt.value;
                        incr = varCounter;

                    }else{
                        index2 = localVarCounter;
                        addVar(p->opr.op[0]->id.i);
                        localVarCounter += p->opr.op[1]->conInt.value;
                        valueInt = p->opr.op[2]->conInt.value;
                        incr = localVarCounter;
                    }
                    sprintf(inlineTemp, "\tpush\t%d\n\tpop\tin\nL%03d:\n\tpush\tin\n\tpush\t1\n\tsub\n\tpop\tin\n\tpush\t%d\n", incr, lbl1 = lbl++, valueInt);
                    appendString(inlineTemp);
                    if(scope == 0){
                        sprintf(inlineTemp,  "\tpop\tsb[in]\n\tpush\tin\n\tpush\t%d\n", index2);
                        appendString(inlineTemp);
                    }else{
                        sprintf(inlineTemp,  "\tpop\tfp[in]\n\tpush\tin\n\tpush\t%d\n", index2);
                        appendString(inlineTemp);
                    }
                    sprintf(inlineTemp, "\tcompgt\n\tj1\tL%03d\n", lbl1);
                    appendString(inlineTemp);
                    }
                    
                    break;
                case GETARRAY:
                    ex(p->opr.op[1], breakArg, contArg);
                    tempScope = scope;
                    if(p->opr.op[0]->id.i[0] == '@') {scope = 0;memmove(p->opr.op[0]->id.i, p->opr.op[0]->id.i+1, strlen(p->opr.op[0]->id.i));}
                    int arrAdd = findVar(p->opr.op[0]->id.i);
                    if(scope == 0)sprintf(inlineTemp, "\tpush\t%d\n\tadd\n\tpop\tin\n\tpush\tsb[in]\n", arrAdd);
                    else sprintf(inlineTemp, "\tpush\t%d\n\tadd\n\tpop\tin\n\tpush\tfp[in]\n", arrAdd);
                    scope = tempScope;
                    appendString(inlineTemp);
                    
                    break;
                case ASSIGNARRAY:
                    {
                    ex(p->opr.op[1], breakArg, contArg);
                    tempScope = scope;
                    int tempScope2 = tempScope;
                    if(p->opr.op[0]->id.i[0] == '@') {scope = 0; tempScope2 =0; memmove(p->opr.op[0]->id.i, p->opr.op[0]->id.i+1, strlen(p->opr.op[0]->id.i));}
                    int arrAdd = findVar(p->opr.op[0]->id.i);
                    sprintf(inlineTemp, "\tpush\t%d\n\tadd\n\tpop\tin\n", arrAdd);
                    if(scope != tempScope) scope = tempScope;
                    appendString(inlineTemp);
                    ex(p->opr.op[2], breakArg, contArg);
                    scope = tempScope2;   
                    if(scope == 0)sprintf(inlineTemp, "\tpop\tsb[in]\n");
                    else sprintf(inlineTemp, "\tpop\tfp[in]\n");
                    scope = tempScope;
                    appendString(inlineTemp); 
                    }

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
                    tempScope = scope;
                    if(p->opr.op[0]->id.i[0] == '@') {scope = 0;memmove(p->opr.op[0]->id.i, p->opr.op[0]->id.i+1, strlen(p->opr.op[0]->id.i));}
                    int index3 = (scope == 0 ? varCounter : localVarCounter);
                    int temp = findVar(p->opr.op[0]->id.i);
                    if(temp == -1 && tempScope == scope){
                        addVar(p->opr.op[0]->id.i);
                        if(scope == 0) varCounter++;
                        else localVarCounter++;
                    }else{
                        index3 = temp;
                    }
                    if(scope == 0) sprintf(inlineTemp, "\tpop\tsb[%d]\n", index3);
                    else sprintf(inlineTemp, "\tpop\tfp[%d]\n", index3);
                    scope = tempScope;
                    appendString(inlineTemp);
                    
                    break;
                // case ';':
                //     ex(p->opr.op[0], breakArg, contArg);
                //     ex(p->opr.op[1], breakArg, contArg);
                //     break;
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
    temp->var = s;
    if(scope == 0){
        temp->pos = varCounter;
        iter->next = temp;
        iter = temp;
        iter->next = NULL;  
    }else{
        temp->pos = localVarCounter;
        localIter->next = temp;
        localIter = temp;
        localIter->next = NULL; 
    }
    
}

int findVar(char *s) {
    struct variableList *temp;
    if(first->next != NULL && scope == 0){
        
        temp = first->next; 
        do{
            if(!strcmp (temp->var,s)){
                return temp->pos;
            }
        } while((temp = temp->next) != NULL);
    }else if (localFirst->next != NULL){
        temp = localFirst->next; 
        do{
            if(!strcmp (temp->var,s)){
                return temp->pos;
            }
        } while((temp = temp->next) != NULL);
    }
    return -1;

}

void addFunc(char *s, int label) {    
    struct funcList *funcTemp = (struct funcList *)malloc(sizeof(struct funcList));
    funcTemp->pos = label;
    funcTemp->var = s;
    funcTemp->params = params;
    funcIter->next = funcTemp;
    funcIter = funcTemp;
    funcIter->next = NULL;
}

struct funcList* findFunc(char *s) {
    if(funcFirst->next != NULL){
        struct funcList *temp = funcFirst->next; 
        do{
            if(!strcmp (temp->var,s)){
                return temp;
            }

        } while((temp = temp->next) != NULL);
    }
    return NULL;

}

void freeVar(struct variableList *v) {
    if(v->next != NULL){
        freeVar(v->next);
    }
    free(v);
}
void appendFuncString(char * newString){
    char * temp;
    asprintf(&temp, "%s%s", finalFuncString, newString);
    free(finalFuncString);
    finalFuncString = temp;   
}
void appendString(char * newString){
    char * temp;
    if(scope == 0){
        asprintf(&temp, "%s%s", printString, newString);
        free(printString);
        printString = temp;
    }else{
        asprintf(&temp, "%s%s", funcString, newString);
        free(funcString);
        funcString = temp;
    }    
}

void storeUserVar(char * newString){
    tempScope = scope;
    if(newString[0] == '@') {scope = 0;memmove(newString, newString+1, strlen(newString));}
    int index2 = (scope == 0 ? varCounter : localVarCounter);
    int tempVar = findVar(newString);
    if(tempVar == -1 && tempScope == scope){
        addVar(newString);
        if(scope == 0) varCounter++;
        else localVarCounter++;
    }else{
        index2 = tempVar;
    }
    if(scope == 0) sprintf(inlineTemp, "\tpop\tsb[%d]\n", index2);
    else sprintf(inlineTemp, "\tpop\tfp[%d]\n", index2);
    scope = tempScope;
    appendString(inlineTemp);
    
}


void printProg(){
    printf("\tpush\t%d\n",varCounter);
    printf("\tpop\tsp\n");
    printf("%s", printString); 
    printf("\tend\n"); 
    printf("%s", finalFuncString);

}
