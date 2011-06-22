%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>

#include "bes.h"

#undef DEBUG
    
// #define DEBUG

// gloabl data object
bes mybes;
    
FILE *yyin;
    
%}

%token NUMBER

%%

command:
{mybes.blockCount = 0; mybes.blocks = NULL;}
blocklist
;

blocklist:
block
| blocklist block
;

block:
{
#ifdef DEBUG
printf("Block[%d]:\n", mybes.blockCount);
#endif
mybes.blocks = realloc(mybes.blocks, (mybes.blockCount+1) * sizeof(besblock));
mybes.blocks[mybes.blockCount].eqns = NULL; 
mybes.blocks[mybes.blockCount].eqnCount = 0;
}
'b' 'l' 'o' 'c' 'k' sign blockidentifier unique mode 'i' 's'
equationlist
'e' 'n' 'd' 'b' 'l' 'o' 'c' 'k'
{
mybes.blockCount++;
}
;

sign:
'm' 'u' {mybes.blocks[mybes.blockCount].sign = 0;}
| 'n' 'u' {mybes.blocks[mybes.blockCount].sign = 1;}
;

blockidentifier: 
'B' NUMBER {mybes.blocks[mybes.blockCount].blockidentifier = $1;}
;

unique: 
'u' 'n' 'i' 'q' 'u' 'e' {mybes.blocks[mybes.blockCount].unique = 1;}
|
;

mode:
'm' 'o' 'd' 'e' NUMBER {mybes.blocks[mybes.blockCount].mode = $1;}
|
;

equationlist:
equation
| equationlist equation
;

equation:
{	
#ifdef DEBUG
printf("Eqn[%d][%d]:\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount);
#endif
mybes.blocks[mybes.blockCount].eqns = realloc(mybes.blocks[mybes.blockCount].eqns, (mybes.blocks[mybes.blockCount].eqnCount+1) * sizeof(eqn));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount = 0;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = NULL;
}

localvariableidentifier '=' formula

{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].lhsId = $2;
//printf("Eqn#: %d -- Eqn*: %p -- Val: %d\n", mybes.blocks[mybes.blockCount].eqnCount, &mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount], $2);
mybes.blocks[mybes.blockCount].eqnCount++; 
}
;

localvariableidentifier:
'X' NUMBER { $$ = yylval; }
;

globalvariableidentifier:
'X' NUMBER '_' NUMBER 
{ 
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = $2;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = $4;
//printf("global %d_%d\n", $1, $4);
}
;


formula:
atomicformula
| atomicformula 'o' 'r' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
#ifdef DEBUG
//printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
}	
disjunctive

| atomicformula 'a' 'n' 'd' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}	
conjunctive
;



atomicformula:
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
}

extra

{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
;

extra:
'f' 'a' 'l' 's' 'e' 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = F;
}

| 't' 'r' 'u' 'e' 
{	
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = T;
}

| localvariableidentifier 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = local;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = $1;
}

| globalvariableidentifier 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = global;
}
;

disjunctive:
atomicformula
| atomicformula 'o' 'r' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
disjunctive 
;

conjunctive:
atomicformula
| atomicformula 'a' 'n' 'd'	
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].vars[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
conjunctive
;

%%


void parse(const char *fileName) {
    
    // timing information
    //unsigned int time, subtime;
    
    fopen_s(&yyin, fileName, "r");;
    
//    printf("parsing... %s (%p)\n", fileName, yyin);
    
    yyparse();
    fclose(yyin);
    
//    printf("... done!\n");
}


int yylex(void) {

    int c;
    
    //remove spaces
    while ( (c = fgetc(yyin)) == ' ' || c =='\t' || c == '\n'){}
    //fprintf(stderr, "%c", c);
    
    //lex digit
    if ( isdigit(c) ) {
        ungetc(c,yyin);
        fscanf_s(yyin,"%d",&yylval);
        return(NUMBER);
    }
    
    // fprintf(stderr, "%c", c);
    
    //otherwise, return char c
    return c;
}


int yyerror(char * s) {
    fprintf(stderr,"error: %s\n",s);
    return 0;
}