%option noyywrap
%{
/*****************声明和选项设置  begin*****************/
#include <stdio.h>
#include <stdlib.h>

#include "syntax_tree.h"
#include "syntax_analyzer.h"

int lines;
int pos_start;
int pos_end;

void pass_node(char *text){
     yylval.node = new_syntax_tree_node(text);
}

/*****************声明和选项设置  end*****************/

%}




%%
 /* to do for students */
 /* two cases for you, pass_node will send flex's token to bison */

\/\*\/*([^\/]*[^\/\*]\/[\/]*)*[^\/]*\*\/ {
				pos_start=pos_end;
                pos_end=pos_start+2;
                int i=2;
      			while(yytext[i]!='*' || yytext[i+1]!='/')
      			{  			
      				if(yytext[i]=='\n')
      				{
						lines=lines+1;
						pos_end=1;
					}
					else
						pos_end=pos_end+1;
					i=i+1;
				}
				pos_end=pos_end+2;
				}
\/\/.*  {
    pos_start=pos_end;pos_end=pos_start+strlen(yytext);
}

\n {lines+=1;pos_end=1;}
\t {pos_start=pos_end;pos_end=pos_start+1;}
" " {pos_start=pos_end;pos_end=pos_start+1;}
\r {pos_start=pos_end;pos_end=pos_start+1;}
\s {pos_start=pos_end;pos_end=pos_start+1;}
const {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return CONST;}
int {pos_start=pos_end;pos_end=pos_start+3;pass_node(yytext);return INT;}
float {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return FLOAT;}
void {pos_start=pos_end;pos_end=pos_start+4;pass_node(yytext);return VOID;}
if {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return IF;}
while {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return WHILE;}
break {pos_start=pos_end;pos_end=pos_start+5;pass_node(yytext);return BREAK;}
continue {pos_start=pos_end;pos_end=pos_start+8;pass_node(yytext);return CONTINUE;}
else {pos_start=pos_end;pos_end=pos_start+4;pass_node(yytext);return ELSE;}
return {pos_start=pos_end;pos_end=pos_start+6;pass_node(yytext);return RETURN;}
\+ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return ADD;}
\- {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return SUB;}
\* {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return MUL;}
\/ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return DIV;}
\< {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LT;}
"<=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return LTE;}
\> {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return GT;}
">=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return GTE;}
"==" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return EQ;}
"!=" {pos_start=pos_end;pos_end=pos_start+2;pass_node(yytext);return NEQ;}
\= {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return ASSIN;}
\; {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return SEMICOLON;}
\, {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return COMMA;}
\! {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return NOT;}
\% {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return MOD;}
\& {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return AND;}
\| {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return OR;}
\( {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LPARENTHESE;}
\) {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RPARENTHESE;}
\[ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LBRACKET;}
\] {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RBRACKET;}
\{ {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return LBRACE;}
\} {pos_start=pos_end;pos_end=pos_start+1;pass_node(yytext);return RBRACE;}
[a-zA-Z_][a-zA-Z0-9_]* {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return IDENTIFIER;}
[1-9][0-9]*|0[0-7]*|(0x|0X)[0-9a-fA-F]* {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return INTCONST;}
[0-9]*\.[0-9]+ {pos_start=pos_end;pos_end=pos_start+strlen(yytext);pass_node(yytext);return FLOATCONST;}


. {pos_start=pos_end;pos_end=pos_start+strlen(yytext);return ERROR;}

 /****请在此补全所有flex的模式与动作  end******/
%%
