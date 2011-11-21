%{
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys\timeb.h>

#include "bes.h"

#undef DEBUG
    
// #define DEBUG

// gloabl data object
bes mybes;

// helpers
int gPos;
fpos_t pos;
time_t time1;
time_t time2;
time_t totalTime;
    
char *buffer;
__int64 buffloc;
__int64 fileLen;

int tmp;

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
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = NULL;
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
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = $2;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = $4;
//printf("global %d_%d\n", $1, $4);
}
;


formula:
atomicformula
| atomicformula 'o' 'r' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
#ifdef DEBUG
//printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
}	
disjunctive

| atomicformula 'a' 'n' 'd' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}	
conjunctive
;



atomicformula:
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
}

extra

{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
;

extra:
'f' 'a' 'l' 's' 'e' 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = F;
}

| 't' 'r' 'u' 'e' 
{	
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = T;
}

| localvariableidentifier 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = local;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = $1;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = mybes.blockCount;
}

| globalvariableidentifier 
{
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = global;
}
;

disjunctive:
atomicformula
| atomicformula 'o' 'r' 
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
disjunctive 
;

conjunctive:
atomicformula
| atomicformula 'a' 'n' 'd'	
{
#ifdef DEBUG
printf("Var[%d][%d][%d]: %p\n", mybes.blockCount, mybes.blocks[mybes.blockCount].eqnCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount, mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs);
#endif
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
}
conjunctive
;

%%


void parse(const char *fileName) {
    
	//helpers
	gPos = 0;
	time(&time1);
	time(&time2);
	totalTime = 0;

	/*
	//new parse()
	buffloc = 0;
	fopen_s(&yyin, fileName, "rb");
	_fseeki64(yyin, 0, SEEK_END);
	fileLen = _ftelli64(yyin);
	_fseeki64(yyin, 0, SEEK_SET);
	buffer = (char *) malloc(fileLen+1);
	fread(buffer, fileLen, 1, yyin);
	yyparse();	
	fclose(yyin);
	free(buffer);
	*/

	//old parse()
    fopen_s(&yyin, fileName, "r");
    yyparse();
    fclose(yyin);
}


int yylex(void) {

    int c;

    //remove spaces
    while ( (c = fgetc(yyin)) == ' ' || c =='\t' || c == '\n'){}
    //fprintf(stderr, "%c", c);
    
	fgetpos( yyin, &pos );
	if( (int) pos/(1024*1024) > gPos) {
		gPos = (int) pos/(1024*1024);
		time(&time1);
		printf("%d MB parsed: \t (%d - %d)\n", pos/(1024*1024), (int)(time1 - time2), (int)(totalTime += (time1 - time2)));
		time(&time2);
	}
	

    //lex digit
    if ( isdigit(c) ) {
        ungetc(c,yyin);
        fscanf_s(yyin,"%d",&yylval);
        return(NUMBER);
    }
    
    // fprintf(stderr, "%c", c);
    
    //otherwise, return char c
    return c;

/*
	if( buffloc/(1024*1024) > gPos) {
		gPos = buffloc/(1024*1024);
		time(&time1);
		printf("%d MB parsed \t (%d - %d)\n", buffloc/(1024*1024), (int)(time1 - time2), (int)(totalTime += (time1 - time2)));
		time(&time2);
	}

	while ( (c = buffer[buffloc]) == ' ' || c =='\t' || c == '\n') {
	
		//printf("%c", c);
		buffloc++;
	}

	//printf("buffloc: %d \t char: %c\n", buffloc, c);

	//lex digit
    if ( isdigit(c) ) {

		yylval = c - '0';

		while ( isdigit(buffer[buffloc+1]) ) {

			yylval = yylval*10 + (buffer[++buffloc] - '0');
		}

		buffloc++;
        return(NUMBER);
    }

return buffer[buffloc++];
*/
}


int yyerror(char * s) {
    fprintf(stderr,"error: %s\n",s);
    return 0;
}