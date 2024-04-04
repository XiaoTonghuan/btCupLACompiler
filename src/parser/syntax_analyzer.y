%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "syntax_tree.h"

// external functions from lex
extern int yylex();
extern int yyparse();
extern int yyrestart();
extern FILE * yyin;

// external variables from lexical_analyzer module
extern int lines;
extern char * yytext;
extern int pos_end;
extern int pos_start;

// Global syntax tree
syntax_tree *gt;
int depth =0;

// Error reporting
void yyerror(const char *s);

// Helper functions written for you with love
syntax_tree_node *node(const char *node_name, int children_num, ...);
%}

/* TODO: Complete this definition.
   Hint: See pass_node(), node(), and syntax_tree.h.
         Use forward declaring. */
%union {
   struct syntax_tree_node* node;
   char op;
}

/* TODO: Your tokens here. */
//终结符
%token <node> CONST INT FLOAT VOID IF WHILE BREAK CONTINUE RETURN ELSE 
%token <node> ADD SUB MUL DIV LT LTE GT GTE EQ NEQ ASSIN SEMICOLON COMMA
%token <node> NOT MOD AND OR
%token <node> LPARENTHESE RPARENTHESE LBRACKET RBRACKET LBRACE RBRACE
%token <node> IDENTIFIER INTCONST  FLOATCONST ERROR
//非终结符
%type <node> CompUnit Decl ConstDecl BType ConstDef ConstInitVal VarDecl
%type <node> VarDef InitVal FuncDef  FuncFParams FuncFParam Block 
%type <node> BlockItem Stmt Exp Cond LVal PrimaryExp Number Integer Float 
%type <node> UnaryExp UnaryOp FuncRParams 
%type <node> MulExp AddExp RelExp EqExp LAndExp LOrExp ConstExp 
%type <node> ConstExpGroup ConstInitValGroup VarDefGroup ArrayDef InitValGroup FuncFParamArray ArrayList BlockGroup ConstDeclGroup

%start CompUnit

%%
/* TODO: Your rules here. */

/* Example:
program: declaration-list {$$ = node( "program", 1, $1); gt->root = $$;}
       ;
*/


CompUnit: CompUnit FuncDef
        {
            //printf("%d\n",depth++);
            $$ = node( "CompUnit", 2, $1,$2);
            gt->root=$$;
        }
        |CompUnit Decl
        {
            //printf("%d\n",depth++);
            $$ = node( "CompUnit", 2, $1,$2);
            gt->root=$$;
        }
        | Decl
        {
            //printf("%d\n",depth++);
            $$ = node( "CompUnit", 1, $1);
            gt->root=$$;
            
        } 
        |FuncDef 
        {
            //printf("%d\n",depth++);
            $$ = node( "CompUnit", 1, $1);
            gt->root=$$;
        } ;

Decl 
    : ConstDecl 
    { 
        $$ = node("Decl",1,$1);
    }
    | VarDecl
    {
        $$ = node("Decl",1,$1);
    };

ConstDecl
    : CONST BType ConstDef SEMICOLON
    {
        $$ = node("ConstDecl",4,$1,$2,$3,$4);
    }
    |CONST BType ConstDef ConstDeclGroup SEMICOLON 
    {
        $$ = node("ConstDecl",5,$1,$2,$3,$4,$5);
    } ;

ConstDeclGroup
    : COMMA ConstDef
    {
        $$ = node("ConstDeclGroup",2,$1,$2);
    }
    | ConstDeclGroup COMMA ConstDef
    {
        $$ = node("ConstDeclGroup",3,$1,$2,$3);
    }

BType
    : INT
    {
        $$ = node("BType",1,$1);
    } 
    | FLOAT
    {
        $$ = node("BType",1,$1);
    };

ConstDef
    : IDENTIFIER ConstExpGroup ASSIN ConstInitVal 
    {
        $$ = node("ConstDef",4,$1,$2,$3,$4);
    }
    | IDENTIFIER ASSIN ConstInitVal
    {
        $$ = node("ConstDef",3,$1,$2,$3);
    };

ConstExpGroup
    : LBRACKET ConstExp RBRACKET 
    {
        $$ = node("ConstExpGroup",3,$1,$2,$3);
    }
    | ConstExpGroup LBRACKET ConstExp RBRACKET
    {
        $$ = node("ConstExpGroup",4,$1,$2,$3,$4);
    }; 

ConstInitVal 
    : ConstExp
    {
        $$ = node("ConstInitVal",1,$1);
    } 
    | LBRACE RBRACE
    {
        $$ = node("ConstInitVal",2,$1,$2);
    }
    | LBRACE ConstInitValGroup RBRACE
    {
        $$ = node("ConstInitVal",3,$1,$2,$3);
    };

ConstInitValGroup
    : ConstInitVal 
    {
        $$ = node("ConstInitValGroup",1,$1);
    }
    | ConstInitValGroup COMMA ConstInitVal 
    {
        $$ = node("ConstInitValGroup",3,$1,$2,$3);
    };

VarDecl 
    : INT VarDefGroup SEMICOLON 
    {
        $$ = node("VarDecl",3,$1,$2,$3);
    } 
    |FLOAT VarDefGroup SEMICOLON 
    {
        $$ = node("VarDecl",3,$1,$2,$3);
    };

VarDefGroup
    : VarDef 
    {
        $$ = node("VarDefGroup",1,$1);
    }
    | VarDefGroup COMMA VarDef
    {
        $$ = node("VarDefGroup",3,$1,$2,$3);
    };

VarDef 
    : IDENTIFIER 
    {
        $$ = node("VarDef",1,$1);
    }
    | IDENTIFIER ASSIN InitVal 
    {
        $$ = node("VarDef",3,$1,$2,$3);
    }
    | IDENTIFIER ArrayDef 
    {
        $$ = node("VarDef",2,$1,$2);
    }
    | IDENTIFIER ArrayDef ASSIN InitVal 
    {
        $$ = node("VarDef",4,$1,$2,$3,$4);
    };

ArrayDef
    : LBRACKET ConstExp RBRACKET
    {
        $$ = node("ArrayDef",3,$1,$2,$3);
    }
    | ArrayDef LBRACKET ConstExp RBRACKET
    {
        $$ = node("ArrayDef",4,$1,$2,$3,$4);
    };

InitVal 
    : Exp
    {
        $$ = node("InitVal",1,$1);
    } 
    | LBRACE RBRACE 
    {
        $$ = node("InitVal",2,$1,$2);
    }
    | LBRACE InitValGroup RBRACE
    {
        $$ = node("InitVal",3,$1,$2,$3);
    };

InitValGroup
    : InitVal
    {
        $$ = node("InitValGroup",1,$1);
    } 
    | InitValGroup COMMA InitVal
    {
        $$ = node("InitValGroup",3,$1,$2,$3);
    };

FuncDef 
    : INT IDENTIFIER LPARENTHESE RPARENTHESE Block 
    {
        $$ = node("FuncDef",5,$1,$2,$3,$4,$5);
    }
    |FLOAT IDENTIFIER LPARENTHESE RPARENTHESE Block 
    {
         $$ = node("FuncDef",5,$1,$2,$3,$4,$5);
    }
    |VOID IDENTIFIER LPARENTHESE RPARENTHESE Block 
    {
         $$ = node("FuncDef",5,$1,$2,$3,$4,$5);
    }
    | INT IDENTIFIER LPARENTHESE FuncFParams RPARENTHESE Block
    {
         $$ = node("FuncDef",6,$1,$2,$3,$4,$5,$6);
    }
    | FLOAT IDENTIFIER LPARENTHESE FuncFParams RPARENTHESE Block
    {
         $$ = node("FuncDef",6,$1,$2,$3,$4,$5,$6);
    }
    | VOID IDENTIFIER LPARENTHESE FuncFParams RPARENTHESE Block
    {
         $$ = node("FuncDef",6,$1,$2,$3,$4,$5,$6);
    };


FuncFParams 
    : FuncFParam 
    {
        $$ = node("FuncFParams",1,$1);
    }
    | FuncFParams COMMA FuncFParam 
    {
        $$ = node("FuncFParams",3,$1,$2,$3);
    };

FuncFParam
    : BType IDENTIFIER
    {
        $$ = node("FuncFParam",2,$1,$2);
    } 
    | BType IDENTIFIER LBRACKET RBRACKET
    {
        $$ = node("FuncFParam",4,$1,$2,$3,$4);
    } 
    | BType IDENTIFIER LBRACKET RBRACKET FuncFParamArray
    {
        $$ = node("FuncFParam",5,$1,$2,$3,$4,$5);
    };

FuncFParamArray
    : LBRACKET Exp RBRACKET 
    {
        $$ = node("FuncFParamArray",3,$1,$2,$3);
    }
    | FuncFParamArray LBRACKET Exp RBRACKET
    {
        $$ = node("FuncFParamArray",4,$1,$2,$3,$4);
    };

Block 
    : LBRACE RBRACE 
    {
        $$ = node("Block",2,$1,$2);
    }
    | LBRACE BlockGroup RBRACE 
    {
        $$ = node("Block",3,$1,$2,$3);
    };

BlockGroup
    : BlockItem 
    {
        $$ = node("BlockGroup",1,$1);        
    }
    | BlockGroup BlockItem
    {
        $$ = node("BlockGroup",2,$1,$2);
    };

BlockItem 
    : Decl
    {
        $$ = node("BlockItem",1,$1);
    } 
    | Stmt
    {
        $$ = node("BlockItem",1,$1);
    };

Stmt 
    :  LVal ASSIN Exp SEMICOLON
    {
        $$ = node("Stmt",4,$1,$2,$3,$4);
    }
	| SEMICOLON  
    {
        $$ = node("Stmt",1,$1);
    }
	| Exp SEMICOLON 
    {
        $$ = node("Stmt",2,$1,$2);
    }
	| Block 
    {
        $$ = node("Stmt",1,$1);
    }
	| IF LPARENTHESE Cond RPARENTHESE Stmt 
    {
        $$ = node("Stmt",5,$1,$2,$3,$4,$5);
    }
	| IF LPARENTHESE Cond RPARENTHESE Stmt ELSE Stmt
    {
        $$ = node("Stmt",7,$1,$2,$3,$4,$5,$6,$7);
    }
	| WHILE LPARENTHESE Cond RPARENTHESE Stmt
    {
        $$ = node("Stmt",5,$1,$2,$3,$4,$5);
    }
	| BREAK SEMICOLON
    {
        $$ = node("Stmt",2,$1,$2);
    }
	| CONTINUE SEMICOLON
    {
        $$ = node("Stmt",2,$1,$2);
    }
	| RETURN SEMICOLON
    {
        $$ = node("Stmt",2,$1,$2);
    } 
	| RETURN Exp SEMICOLON 
    {
        $$ = node("Stmt",3,$1,$2,$3);
    };

Exp 
    : AddExp 
    {
        $$ = node("Exp",1,$1);
    };

Cond 
    : LOrExp 
    {
        $$ = node("Cond",1,$1);
    };

LVal 
    : IDENTIFIER
    {
        $$ = node("LVal",1,$1);
    } 
    | IDENTIFIER ArrayList
    {
        $$ = node("LVal",2,$1,$2);
    };

ArrayList
    :LBRACKET Exp RBRACKET
    {
        $$ = node("ArrayList",3,$1,$2,$3);
    }
    | ArrayList LBRACKET Exp RBRACKET
    {
        $$ = node("ArrayList",4,$1,$2,$3,$4);
    };

Number 
    : Integer
    {
        $$ = node("Number",1,$1);
    }
    | Float
    {
        $$ = node("Number",1,$1);
    };

Integer
    :INTCONST
    {
        $$ = node("Integer",1,$1);
    }

Float
    :FLOATCONST
    {
        $$ = node("Float",1,$1);
    }

PrimaryExp 
    : LPARENTHESE Exp RPARENTHESE 
    {
        $$ = node("PrimaryExp",3,$1,$2,$3);
    }
    | LVal 	 
    {
        $$ = node("PrimaryExp",1,$1);
    }
    | Number	 
    {
        $$ = node("PrimaryExp",1,$1);
    };

UnaryExp 
    : PrimaryExp 
    {
        $$ = node("UnaryExp",1,$1);
    }
    | IDENTIFIER LPARENTHESE RPARENTHESE 
    {
        $$ = node("UnaryExp",3,$1,$2,$3);
    }
    | IDENTIFIER LPARENTHESE FuncRParams RPARENTHESE
    {
        $$ = node("UnaryExp",4,$1,$2,$3,$4);
    } 
    | UnaryOp UnaryExp
    {
        $$ = node("UnaryExp",2,$1,$2);
    };

UnaryOp 
    : ADD
    {
        $$ = node("UnaryOp",1,$1);
    }
    | SUB
    {
        $$ = node("UnaryOp",1,$1);
    } 
    | NOT
    {
        $$ = node("UnaryOp",1,$1);
    } ;

FuncRParams
    : Exp
    {
        $$ = node("FuncRParams",1,$1);
    } 
    | FuncRParams COMMA Exp
    {
        $$ = node("FuncRParams",3,$1,$2,$3);
    }; 

MulExp 
    : UnaryExp 
    {
        $$ = node("MulExp",1,$1);
    }
    | MulExp MUL UnaryExp 
    {
        $$ = node("MulExp",3,$1,$2,$3);
    }
    | MulExp DIV UnaryExp
    {
        $$ = node("MulExp",3,$1,$2,$3);
    } 
    | MulExp MOD UnaryExp
    {
        $$ = node("MulExp",3,$1,$2,$3);
    } ;

AddExp 
    : MulExp 
    {
        $$ = node("AddExp",1,$1);
    }
    | AddExp ADD MulExp 
    {
        $$ = node("AddExp",3,$1,$2,$3);
    }
    | AddExp SUB MulExp
    {
        $$ = node("AddExp",3,$1,$2,$3);
    } ;

RelExp 
    : AddExp 
    {
        $$ = node("RelExp",1,$1);
    }
    | RelExp LT AddExp
    {
        $$ = node("RelExp",3,$1,$2,$3);
    } 
    |RelExp GT AddExp
    {
        $$ = node("RelExp",3,$1,$2,$3);
    }
    |RelExp LTE AddExp
    {
        $$ = node("RelExp",3,$1,$2,$3);
    } 
    |RelExp GTE AddExp 
    {
        $$ = node("RelExp",3,$1,$2,$3);
    };

EqExp 
    : RelExp 
    {
        $$ = node("EqExp",1,$1);
    }
    | EqExp EQ RelExp
    {
        $$ = node("EqExp",3,$1,$2,$3);
    } 
    | EqExp NEQ RelExp
    {
        $$ = node("EqExp",3,$1,$2,$3);
    } ;

LAndExp 
    : EqExp 
    {
        $$ = node("LAndExp",1,$1);
    }
    | LAndExp AND EqExp
    {
        $$ = node("LAndExp",3,$1,$2,$3);
    } ;

LOrExp 
    : LAndExp 
    {
        $$ = node("LOrExp",1,$1);
    }
    | LOrExp OR LAndExp
    {
        $$ = node("LOrExp",3,$1,$2,$3);
    } ;

ConstExp 
    : AddExp 
    {
        $$ = node("ConstExp",1,$1);
    };



%%

/// The error reporting function.
void yyerror(const char * s)
{
    // TO STUDENTS: This is just an example.
    // You can customize it as you like.
    fprintf(stderr, "error at line %d column %d: %s\n", lines, pos_start, s);
}

/// Parse input from file `input_path`, and prints the parsing results
/// to stdout.  If input_path is NULL, read from stdin.
///
/// This function initializes essential states before running yyparse().
syntax_tree *parse(const char *input_path)
{ 
    
    if (input_path != NULL) {
        if (!(yyin = fopen(input_path, "r"))) {
            fprintf(stderr, "[ERR] Open input file %s failed.\n", input_path);
            exit(1);
        }
    } else {
        yyin = stdin;
    }
   
    lines = pos_start = pos_end = 1;
    gt = new_syntax_tree();
    yyrestart(yyin);
    yyparse();
    return gt;
}

/// A helper function to quickly construct a tree node.
///
/// e.g. $$ = node("program", 1, $1);
syntax_tree_node *node(const char *name, int children_num, ...)
{
    syntax_tree_node *p = new_syntax_tree_node(name);
    syntax_tree_node *child;
    if (children_num == 0) {
        child = new_syntax_tree_node("epsilon");
        syntax_tree_add_child(p, child);
    } else {
        va_list ap;
        va_start(ap, children_num);
        for (int i = 0; i < children_num; ++i) {
            child = va_arg(ap, syntax_tree_node *);
            syntax_tree_add_child(p, child);
        }
        va_end(ap);
    }
    return p;
}
