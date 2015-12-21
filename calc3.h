typedef enum { typeConInt, typeConChar, typeConStr, typeId, typeFuncId, typeOpr } nodeEnum;

/* constants */
typedef struct {
    int value;                  /* value of constant */
} conIntNodeType;

typedef struct {
    char value;                  /* value of constant */
} conCharNodeType;

typedef struct {
    char * value;                  /* value of constant */
} conStrNodeType;

/* identifiers */
typedef struct {
    char * i;                      /* subscript to sym array */
} idNodeType;

typedef struct {
    char * i;                      /* subscript to sym array */
} funcIdNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];  /* operands (expandable) */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    /* union must be last entry in nodeType */
    /* because operNodeType may dynamically increase */
    union {
        conIntNodeType conInt;        /* constants */
        conCharNodeType conChar;        /* constants */
        conStrNodeType conStr;        /* constants */
        idNodeType id;          /* identifiers */
        funcIdNodeType fid;          /* function identifiers */
        oprNodeType opr;        /* operators */
    };
} nodeType;

struct variableList{
    int pos;
    // int type;  /*0 = none, 1 = int, 2 = char, 3 = String*/
    char * var;
    struct variableList * next;                  /* value of constant */
};
struct funcList{
    int pos;
    int params;
    // int type;  /*0 = none, 1 = int, 2 = char, 3 = String*/
    char * var;

    struct funcList * next;                  /* value of constant */
};

extern int sym[26];
