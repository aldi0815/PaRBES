//
//  bes.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//


#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

#include "helpers.h"

#include "bes.h"

extern "C" bes mybes;


void parseBES(const char* fileName) {

	int c;
	int blockCount;
	int eqnCount[2];
	int identifier;
	char *buffer;
	FILE *myFile;
	__int64 buffloc;
	__int64 fileLen;

	buffloc = 0;
	if ( 0 != fopen_s(&myFile, fileName, "rb")) {printf("Opening file failed!\n");}
	if ( 0 != _fseeki64(myFile, 0, SEEK_END)) {printf("Seeking end failed!\n");}
	fileLen = _ftelli64(myFile);
	_fseeki64(myFile, 0, SEEK_SET);
	buffer = (char *) malloc(fileLen+1);
	fread(buffer, fileLen, 1, myFile);
	//printf("file name: %s\n", fileName);
	//printf("file pointer: %p\n", myFile);
	//printf("file length: %d\n", fileLen);


	blockCount = 0;
	eqnCount[0] = 0;
	eqnCount[1] = 0;
	mybes.blockCount = 0;
	mybes.blocks = NULL;

	//first pass
	while (fileLen > buffloc) {

		while ( (c = buffer[buffloc]) == ' ' || c =='\t') {buffloc++;}

		switch (buffer[buffloc]) {

		case 'b':
			if (buffer[buffloc + 1] != 'l') printf("syntax error - block\n");
			else {buffloc += 5;}
			break;

		case 'm':
			if (buffer[buffloc + 1] == 'u') {buffloc += 2;}
			else if (buffer[buffloc + 1] == 'o') {buffloc += 5; buffloc += 2;}
			else {printf("syntax error - mu / mode\n");}
			break;

		case 'n':
			if (buffer[buffloc + 1] != 'u') printf("syntax error - nu\n");
			else {buffloc += 2;}
			break;

		case 'B':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - B\n");
			else { 

				buffloc++;

				if ( isdigit(buffer[buffloc]) ) {

					buffloc++;
					while ( isdigit(buffer[buffloc]) ) {buffloc++;}
				}
			}
			break;

		case 'u':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - unique\n");
			else {buffloc += 6;}
			break;

		case '\n':

			buffloc++;

			while ( (c = buffer[buffloc]) == ' ' || c =='\t') {buffloc++;}

			switch (buffer[buffloc]) {
			
			case 'X':
				if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - LHS\n");
				else {
					
					eqnCount[blockCount]++; // eqn count !!
					buffloc++;
					buffloc++;
					while ( isdigit(buffer[buffloc]) ) {buffloc++;}
				}
				break;
			
			default:
				break;

			}
			break;

		case 'X':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - RHS\n");
			else {

				buffloc++;
				buffloc++;
				while ( isdigit(buffer[buffloc]) ) {buffloc++;}
			}

			if( buffer[buffloc] == '_' ) {

				buffloc++;

				if ( isdigit(buffer[buffloc]) ) {

					buffloc++;
					while ( isdigit(buffer[buffloc]) ) {buffloc++;}
				}
			}
			break;
		
		case 'a':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - and\n");
			else {buffloc += 3;}
			break;

		case 'o':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - or\n");
			else {buffloc += 2;}
			break;

		case 't':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - true\n");
			else {buffloc += 4;}
			break;

		case 'f':
			if (buffer[buffloc + 1] != 'a') printf("syntax error - false\n");
			else {buffloc += 5;}
			break;

		case 'e':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - end block\n");
			else {
				blockCount++; // block count!!
				buffloc += 9;
			}
		break;

		default:
			buffloc++;
			break;
		}
	} // end of first pass


	mybes.blocks = (besblock*) malloc(blockCount * sizeof(besblock));
	mybes.blocks[0].eqns = (eqn*) malloc(eqnCount[0] * sizeof(eqn));
	mybes.blocks[1].eqns = (eqn*) malloc(eqnCount[1] * sizeof(eqn));

	buffloc = 0;

	while (fileLen > buffloc) {

		//printf("Buffer[%d]: %c\n", buffloc, buffer[buffloc]);

		while ( (c = buffer[buffloc]) == ' ' || c =='\t') {
	
			//printf("%c", c);
			buffloc++;
		}

		switch (buffer[buffloc]) {

		case 'b':
			if (buffer[buffloc + 1] != 'l') printf("syntax error - block\n");
			else {

				mybes.blocks[mybes.blockCount].unique = 0;
				mybes.blocks[mybes.blockCount].eqnCount = -1; // in order to get the index right when recognizing eqns
				buffloc += 5;
			}
			break;

		case 'm':
			if (buffer[buffloc + 1] == 'u') {mybes.blocks[mybes.blockCount].sign = 0; buffloc += 2;}
			else if (buffer[buffloc + 1] == 'o') {buffloc += 5; mybes.blocks[mybes.blockCount].mode = (buffer[buffloc] - '0'); buffloc += 2;}
			else {printf("syntax error - mu / mode\n");}
			break;

		case 'n':
			if (buffer[buffloc + 1] != 'u') printf("syntax error - nu\n");
			else {mybes.blocks[mybes.blockCount].sign = 1; buffloc += 2;}
			break;

		case 'B':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - B\n");
			else { 

				buffloc++;
				identifier = -1;

				if ( isdigit(buffer[buffloc]) ) {

					identifier =  (buffer[buffloc] - '0');
					buffloc++;

					while ( isdigit(buffer[buffloc]) ) {

						identifier = identifier*10 + (buffer[buffloc] - '0');
						buffloc++;
					}
				}

				mybes.blocks[mybes.blockCount].blockidentifier = identifier;
			}
			break;

		case 'u':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - unique\n");
			else {mybes.blocks[mybes.blockCount].unique = 1; buffloc += 6;}
			break;

		case '\n':

			buffloc++;

			while ( (c = buffer[buffloc]) == ' ' || c =='\t') {
	
				//printf("%c", c);
				buffloc++;
			}

			switch (buffer[buffloc]) {
			
			case 'X':
				if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - LHS\n");
				else {
			
					mybes.blocks[mybes.blockCount].eqnCount++;
					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount = 0;
					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = NULL;
					
					buffloc++;
					identifier = -1;
					identifier =  (buffer[buffloc] - '0');
					buffloc++;

					while ( isdigit(buffer[buffloc]) ) {

						identifier = identifier*10 + (buffer[buffloc] - '0');
						buffloc++;
					}

					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].lhsId = identifier;
				}
				break;
			
			default:
				break;

			}
			break;

		case 'X':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - RHS\n");
			else {

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));

				buffloc++;
				identifier = -1;
				identifier =  (buffer[buffloc] - '0');
				buffloc++;

				while ( isdigit(buffer[buffloc]) ) {

					identifier = identifier*10 + (buffer[buffloc] - '0');
					buffloc++;
				}

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = identifier;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = mybes.blockCount;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = local;
			}

			if( buffer[buffloc] == '_' ) {

				buffloc++;
				identifier = -1;

				if ( isdigit(buffer[buffloc]) ) {

					identifier = (buffer[buffloc] - '0');
					buffloc++;

					while ( isdigit(buffer[buffloc]) ) {

						identifier = identifier*10 + (buffer[buffloc] - '0');
						buffloc++;
					}
				}

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = identifier;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = global;
			}
			mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
			break;
		
		case 'a':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - and\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 3;
			}
			break;

		case 'o':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - or\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 2;
			}
			break;

		case 't':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - true\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = T;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 4;
			}
			break;

		case 'f':
			if (buffer[buffloc + 1] != 'a') printf("syntax error - false\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = F;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 5;
			}
			break;

		case 'e':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - end block\n");
			else {
				mybes.blocks[mybes.blockCount].eqnCount++; // because we started with -1 !!
				mybes.blockCount++;
				buffloc += 9;
			}
		break;

		default:
			buffloc++;
			break;
		}
	}

/* backup
while (fileLen > buffloc) {

		//printf("Buffer[%d]: %c\n", buffloc, buffer[buffloc]);

		while ( (c = buffer[buffloc]) == ' ' || c =='\t') {
	
			//printf("%c", c);
			buffloc++;
		}

		switch (buffer[buffloc]) {

		case 'b':
			if (buffer[buffloc + 1] != 'l') printf("syntax error - block\n");
			else {

				mybes.blocks = (besblock*) realloc(mybes.blocks, (mybes.blockCount+1) * sizeof(besblock));
				mybes.blocks[mybes.blockCount].eqns = NULL; 
				mybes.blocks[mybes.blockCount].eqnCount = -1; // in order to get the index right when recognizing eqns
				buffloc += 5;
			}
			break;

		case 'm':
			if (buffer[buffloc + 1] == 'u') {mybes.blocks[mybes.blockCount].sign = 0; buffloc += 2;}
			else if (buffer[buffloc + 1] == 'o') {buffloc += 5; mybes.blocks[mybes.blockCount].mode = buffer[buffloc]; buffloc += 2;}
			else {printf("syntax error - mu / mode\n");}
			break;

		case 'n':
			if (buffer[buffloc + 1] != 'u') printf("syntax error - nu\n");
			else {mybes.blocks[mybes.blockCount].sign = 1; buffloc += 2;}
			break;

		case 'B':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - B\n");
			else { 

				buffloc++;
				identifier = 0;

				if ( isdigit(buffer[buffloc]) ) {

					identifier = atoi(&buffer[buffloc]);

					while ( isdigit(buffer[buffloc+1]) ) {

						identifier = identifier*10 + atoi(&buffer[++buffloc]);
					}
				}

				mybes.blocks[mybes.blockCount].blockidentifier = identifier;
				buffloc++;
			}
			break;

		case 'u':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - unique\n");
			else {mybes.blocks[mybes.blockCount].unique = 1; buffloc += 6;}
			break;

		case '\n':

			buffloc++;

			while ( (c = buffer[buffloc]) == ' ' || c =='\t') {
	
				//printf("%c", c);
				buffloc++;
			}

			switch (buffer[buffloc]) {
			
			case 'X':
				if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - LHS\n");
				else {

					mybes.blocks[mybes.blockCount].eqnCount++;
					mybes.blocks[mybes.blockCount].eqns = (eqn*) realloc(mybes.blocks[mybes.blockCount].eqns, (mybes.blocks[mybes.blockCount].eqnCount+1) * sizeof(eqn));
					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount = 0;
					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = NULL;
					

					identifier = 0;

					if ( isdigit(buffer[buffloc]) ) {

						identifier = atoi(&buffer[buffloc]);

						while ( isdigit(buffer[buffloc+1]) ) {

							identifier = identifier*10 + atoi(&buffer[++buffloc]);
						}
					}

					mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].lhsId = identifier;
					buffloc++;
				}
				break;
			
			default:
				break;

			}
			break;

		case 'X':
			if ( !isdigit(buffer[buffloc + 1]) ) printf("syntax error - RHS\n");
			else {

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));

				identifier = 0;

				if ( isdigit(buffer[buffloc]) ) {

					identifier = atoi(&buffer[buffloc]);

					while ( isdigit(buffer[buffloc+1]) ) {

						identifier = identifier*10 + atoi(&buffer[++buffloc]);
					}
				}

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].localRef = identifier;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = mybes.blockCount;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = local;
			}

			if( buffer[buffloc + 1] == '_' ) {

				buffloc += 2;
				
				identifier = 0;

				if ( isdigit(buffer[buffloc]) ) {

					identifier = atoi(&buffer[buffloc]);

					while ( isdigit(buffer[buffloc+1]) ) {

						identifier = identifier*10 + atoi(&buffer[++buffloc]);
					}
				}

				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].globalRef = identifier;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = global;
			}
			mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
			buffloc++;
			break;
		
		case 'a':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - and\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = conjunct;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 3;
			}
			break;

		case 'o':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - or\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = disjunct;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 2;
			}
			break;

		case 't':
			if (buffer[buffloc + 1] != 'r') printf("syntax error - true\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = T;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 4;
			}
			break;

		case 'f':
			if (buffer[buffloc + 1] != 'a') printf("syntax error - false\n");
			else {
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs = (var*) realloc(mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs, (mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount+1) * sizeof(var));
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].rhs[mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount].type = F;
				mybes.blocks[mybes.blockCount].eqns[mybes.blocks[mybes.blockCount].eqnCount].varCount++;
				buffloc += 5;
			}
			break;

		case 'e':
			if (buffer[buffloc + 1] != 'n') printf("syntax error - end block\n");
			else {
				mybes.blockCount++;
				buffloc += 9;
			}
		break;

		default:
			buffloc++;
			break;
		}
	}
*/





	free(buffer);
	fclose(myFile);
}


int initBES() {

	int i,j;
	int numVars = 0;

	for (i = 0; i < mybes.blockCount; i++) {

		numVars += mybes.blocks[i].eqnCount;
		mybes.blocks[i].refs = (int*) malloc(sizeof(int) * mybes.blocks[i].eqnCount);

		for (j = 0; j < mybes.blocks[i].eqnCount; j++) {

			mybes.blocks[i].refs[j] = j;
		}
	}

	mybes.numVars = numVars;

	return 1;
}

void printBES() {

    int i, j, k;
    
	printf("printBES()\n");
	printf("block count: %d\n", mybes.blockCount);

    for (i=0; i< mybes.blockCount; i++) {
        
        // print block information
        printf("block");
        if (mybes.blocks[i].sign) printf(" nu");
        else printf(" mu");
        printf(" B%d", mybes.blocks[i].blockidentifier);
        if (mybes.blocks[i].unique) printf(" unique");
        printf(" mode %d is\n", mybes.blocks[i].mode);
        //printf("\t (%d eqations)\n", mybes.blocks[i].eqnCount);
        
        // print equations
        for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
            printf("X%d =", mybes.blocks[i].eqns[j].lhsId);
            for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++)
            {
                switch (mybes.blocks[i].eqns[j].rhs[k].type)
                {
                    case T: printf(" true");
                        break;
                        
                    case F: printf(" false");
                        break;
                        
                    case local: printf(" X%d", mybes.blocks[i].eqns[j].rhs[k].localRef);
                        break;
                        
                    case global: printf(" X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
                        break;
                        
                    case conjunct: printf(" and");
                        break;
                        
                    case disjunct: printf(" or");
                        break;
                }
            }
            printf("\n");
        }
        printf("end block\n");
    }
    
}

void graphBES() {
    int i, j, k;
    
    for (i=0; i< mybes.blockCount; i++) {
        
        // print block information
//        printf("block ");
//        if (mybes.blocks[i].sign) printf("nu ");
//        else printf("mu ");
//        
//        printf("B%d \n", mybes.blocks[i].blockidentifier);
        printf("digraph finite_state_machine {\n");
        
        // print equations
        for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
            
            for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++)
            {
                switch (mybes.blocks[i].eqns[j].rhs[k].type)
                {
                    case T: printf("X%d -> T;", mybes.blocks[i].eqns[j].lhsId);
                        break;
                        
                    case F: printf("X%d -> F;", mybes.blocks[i].eqns[j].lhsId);
                        break;
                        
                    case local: printf("X%d -> X%d;", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].rhs[k].localRef);
                        break;
                        
                    case global: printf("X%d -> X%d_%d;", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
                        break;
                    default: 
                        continue;
                }
                printf("\n");
            }
        }
//        printf("end block\n");
    }
    printf("}");
}

double computeDistance(){
    int i, j, k, l;
	int tmpSum, numNeighbours;
	double avrgDist;
	double *dists;
    
	l = 0;
	avrgDist = 0;

	dists = (double*) malloc(sizeof(double) * mybes.numVars);

	// iterate over blocks
    for (i=0; i< mybes.blockCount; i++) {
      
		// iterate over equations
        for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
    
			tmpSum = 0;
			numNeighbours = 0;

			// iterate over variables
            for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++) {

                switch (mybes.blocks[i].eqns[j].rhs[k].type) {    
                    case local: 
						//printf("X%d -> T\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].rhs[k].localRef);
						tmpSum += abs(mybes.blocks[i].eqns[j].lhsId - mybes.blocks[i].eqns[j].rhs[k].localRef);
						numNeighbours++;
						break;

					default:
						break;
                } // end switch

            } //end variables

			dists[l] = 0;

			if (0 != numNeighbours && 0 != tmpSum) {
				dists[l] = tmpSum / numNeighbours;
			}

			//printf("%d\n", dists[l]);
			l++;

#ifdef DEBUG
                printf("%d \t neighbours %d: \t sum: %d \t distance: %d\n", l, numNeighbours, tmpSum, dists[l]);
#endif			
		} //end equation
    } // end block

	for( i = 0; i < l; i++)
	{
		avrgDist += (dists[i] / l);
#ifdef DEBUG
		if( avrgDist < 0) {printf("Error: average distance < 0!\n", avrgDist, i, dists[i]); exit(-1);};
#endif
	}
	
	//avrgDist /= (float) l;

	//printf("f(x) = %d\n", avrgDist);

	free(dists);

	return avrgDist;
}

void initLHS() {

    int i, j;

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {
      
		// iterate over equations
		for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

			//mybes.blocks[i].eqns[j].lhs = (!mybes.blocks[i].sign);	// inverse initial approximation
			mybes.blocks[i].eqns[j].lhs = mybes.blocks[i].sign;	// initial approximation
			mybes.blocks[i].eqns[j].count = 0; //debug info

		} //end equation
	} // end block
}

double* solveBES() {

	bool b = true;		// true as long as variables change within an iteration
	bool bb = true;
	bool tmp1, tmp2;	// store previous value of lhs
    int i, j, k;		// loops
	int iterations, changes, sumChanges;
	double* res;

	sumChanges = 0;
	iterations = 0;
	res = (double*) malloc(sizeof(double) * 2);

	initLHS();
	res[1] = computeDistance();

	while(b) {

		b = false;
		changes = 0;

		iterations++;
		res[0] = iterations;

		res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			while(bb) {

				bb = false;

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				//initial value of current variable
				tmp1 = mybes.blocks[i].eqns[j].lhs;

				if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = true;			// terminal true
				else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = false;	// terminal false

				else if ( mybes.blocks[i].eqns[j].rhs[0].type == local || mybes.blocks[i].eqns[j].rhs[0].type == global ) { // variable(s)

					// assignment of truth value of the first var of the rhs
					tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;
					//printf("X%d_%d = X%d_%d = %d\n", i, mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].rhs[0].globalRef, mybes.blocks[i].eqns[j].rhs[0].localRef, tmp);

					//printf("X%d = X%d_%d", j, mybes.blocks[i].eqns[j].rhs[0].localRef, mybes.blocks[i].eqns[j].rhs[0].globalRef);

					// iterate over remaining rhs variables
					if (mybes.blocks[i].eqns[j].varCount > 1 ) {

						for(k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

								switch (mybes.blocks[i].eqns[j].rhs[1].type)
								{   
									case conjunct:
										//printf(" and X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
										tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;
                        
									case disjunct:
										//printf(" or X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
										tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;

									default:
										continue;
								} // end switch
						}//end variables
					}

				} // end if	

				//printf("\n");
				
				if (tmp1 != tmp2) {
					mybes.blocks[i].eqns[j].lhs = tmp2;
					changes++;
					b = true; // continue as long as variables change
					bb = true;
				}

			} //end equation
		}
		} // end block

		sumChanges += changes;
		res[iterations+1] = (double) changes; // /(double) mybes.numVars;

	} // end while

	for (i = 1; i < iterations; i++) {

		res[i+1] /= (double) sumChanges;
	}

	res[1] = sumChanges;	
	return res;
}

double* solveBESBottomUp() {

	bool b = true;		// true as long as variables change within an iteration
	bool tmp1, tmp2;	// store previous value of lhs
    int i, j, k;		// loops
	int iterations, changes, sumChanges;
	double* res;

	sumChanges = 0;
	iterations = 0;
	res = (double*) malloc(sizeof(double) * 2);

	initLHS();
	//res[1] = computeDistance();

	// loop over entire BES
	while(b) {

		b = false;
		//changes = 0;
		//iterations++;
		//res[0] = iterations;
		//res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));

		// iterate over blocks
		for (i = mybes.blockCount - 1; i >= 0; i--) {


			bool bb = true;

			while(bb) {

				bb = false;

				changes = 0;
				iterations++;
				res[0] = iterations;
				res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));


				// iterate over equations
				for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

					//initial value of current variable
					tmp1 = mybes.blocks[i].eqns[j].lhs;

					if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = true;			// terminal true
					else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = false;	// terminal false

					else if ( mybes.blocks[i].eqns[j].rhs[0].type == local || mybes.blocks[i].eqns[j].rhs[0].type == global ) { // variable(s)

						// assignment of truth value of the first var of the rhs
						//printf("X%d = X%d_%d", j, mybes.blocks[i].eqns[j].rhs[0].localRef, mybes.blocks[i].eqns[j].rhs[0].globalRef);
						tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;
					
						// iterate over remaining rhs variables
						if (mybes.blocks[i].eqns[j].varCount > 1 ) {

							for(k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

									switch (mybes.blocks[i].eqns[j].rhs[1].type)
									{   
										case conjunct:
											//printf(" and X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;
                        
										case disjunct:
											//printf(" or X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;

										default:
											continue;
									} // end switch
							}//end variables
						}

					} // end if	

					//printf("\n");
				
					if (tmp1 != tmp2) {
						mybes.blocks[i].eqns[j].lhs = tmp2;
						changes++;
						b = true;	// continue as long as variables change
						bb = true;	
					} //end if

				} //end for equations

				sumChanges += changes;
				res[iterations+1] = (double) changes; // /(double) mybes.numVars;

			} // end while eqnations change

		} // end for blocks

		//sumChanges += changes;
		//res[iterations+1] = (double) changes; // /(double) mybes.numVars;

	} // end while blocks change

	for (i = 1; i < iterations; i++) {

		res[i+1] /= (double) sumChanges;
	}

	res[1] = sumChanges;	
	return res;
}

double* parSolveBESBottomUp() {

	bool b = true;		// true as long as variables change within an iteration
    int i;		// loops
	int iterations, changes, sumChanges;
	double* res;

	sumChanges = 0;
	iterations = 0;
	res = (double*) malloc(sizeof(double) * 2);

	initLHS();
	//res[1] = computeDistance();

	// loop over entire BES
	while(b) {

		b = false;
		//changes = 0;
		//iterations++;
		//res[0] = iterations;
		//res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));

		// iterate over blocks
		for (i = mybes.blockCount - 1; i >= 0; i--) {


			bool bb = true;

			while(bb) {

				bb = false;

				changes = 0;
				iterations++;
				res[0] = iterations;
				res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));
				res[iterations] = 0;


				// iterate over equations
				cilk_for (int j = 0; j < mybes.blocks[i].eqnCount; j++) {

					//initial value of current variable
					bool tmp1, tmp2;

					tmp1 = mybes.blocks[i].eqns[j].lhs;

					if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = true;			// terminal true
					else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = false;	// terminal false

					else if ( mybes.blocks[i].eqns[j].rhs[0].type == local || mybes.blocks[i].eqns[j].rhs[0].type == global ) { // variable(s)

						// assignment of truth value of the first var of the rhs
						//printf("X%d = X%d_%d", j, mybes.blocks[i].eqns[j].rhs[0].localRef, mybes.blocks[i].eqns[j].rhs[0].globalRef);
						tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;
					
						// iterate over remaining rhs variables
						if (mybes.blocks[i].eqns[j].varCount > 1 ) {

							for(int k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

									switch (mybes.blocks[i].eqns[j].rhs[1].type)
									{   
										case conjunct:
											//printf(" and X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;
                        
										case disjunct:
											//printf(" or X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;

										default:
											continue;
									} // end switch
							}//end variables
						}

					} // end if	

					//printf("\n");
				
					if (tmp1 != tmp2) {
						mybes.blocks[i].eqns[j].lhs = tmp2;
						b = true;	// continue as long as variables change
						bb = true;	
					} //end if

				} //end for equations

			} // end while eqnations change

		} // end for blocks

		//sumChanges += changes;
		//res[iterations+1] = (double) changes; // /(double) mybes.numVars;

	} // end while blocks change

	return res;
}

int parSolveBES() {
	
	bool b;		// true as long as variables change within an iteration
    int i;		// loops
	int iterations;

	b = true;
	iterations = 0;

	initLHS();

	// loop over entire BES
	while(b) {

		b = false;

		// iterate over blocks
		for (i = mybes.blockCount - 1; i >= 0; i--) {


			bool bb = true;

			while(bb) {

				bb = false;
				iterations++;

				// iterate over equations
				cilk_for (int j = 0; j < mybes.blocks[i].eqnCount; j++) {

					bool tmp1, tmp2;	// store previous value of lhs

					//initial value of current variable
					tmp1 = mybes.blocks[i].eqns[j].lhs;

					if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = true;			// terminal true
					else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = false;	// terminal false

					else if ( mybes.blocks[i].eqns[j].rhs[0].type == local || mybes.blocks[i].eqns[j].rhs[0].type == global ) { // variable(s)

						// assignment of truth value of the first var of the rhs
						//printf("X%d = X%d_%d", j, mybes.blocks[i].eqns[j].rhs[0].localRef, mybes.blocks[i].eqns[j].rhs[0].globalRef);
						tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;
					
						// iterate over remaining rhs variables
						if (mybes.blocks[i].eqns[j].varCount > 1 ) {

							for(int k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

									switch (mybes.blocks[i].eqns[j].rhs[1].type)
									{   
										case conjunct:
											//printf(" and X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;
                        
										case disjunct:
											//printf(" or X%d_%d", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
											tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
											break;

										default:
											continue;
									} // end switch
							}//end variables
						}

					} // end if	

					//printf("\n");
				
					if (tmp1 != tmp2) {
						mybes.blocks[i].eqns[j].lhs = tmp2;
						//b = true;	// continue as long as variables change
						bb = true;	
					} //end if

				} //end for equations

				b = bb;

			} // end while eqnations change

		} // end for blocks

	} // end while blocks change

	return iterations;
}

void reverseBES() {

	int i,j;
	int tmpRef;
	eqn tmp;

	printf("Reversing BES... \t\t\t\t\t\t\t");

		//// iterate over blocks
		//printf("\n");
		//for (i = 0; i < mybes.blockCount; i++) {

		//	// iterate over equations
		//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		//
		//		printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

		//	}
		//}
		//printf("- - -\n");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {
      
			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount / 2; j++) {

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount - 1) - j];
				mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount - 1) - j] = tmp;

				tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount - 1) - j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount - 1) - j].lhsId] = tmpRef;
			}
		}

		//// iterate over blocks
		//for (i = 0; i < mybes.blockCount; i++) {

		//	// iterate over equations
		//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		//
		//		printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

		//	}
		//}

	printf("[done]\n");
}

void randomizeBES() {

	int i,j;
	int rnd;
	int tmpRef;

	srand(time(NULL));

	eqn tmp;

	printf("Randomizing BES... \t\t\t\t\t\t\t");

		//// iterate over blocks
		//printf("\n");
		//for (i = 0; i < mybes.blockCount; i++) {

		//	// iterate over equations
		//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		//
		//		printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

		//	}
		//}
		//printf("- - -\n");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			if ( mybes.blocks[i].eqnCount > 1) {
			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				
				do {

					rnd = rand() % mybes.blocks[i].eqnCount;
				
				} while (rnd == j); //((mybes.blocks[i].eqns[rnd].lhsId == j) || (mybes.blocks[i].eqns[j].lhsId == rnd) || (rnd == j));
	

				tmp = mybes.blocks[i].eqns[rnd];
				mybes.blocks[i].eqns[rnd] = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = tmp;

				tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[rnd].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[rnd].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = tmpRef;

				//printf("%d\n%d\n\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[mybes.blocks[i].eqnCount-(j+1)].lhsId);
			}
			}
		}

		//// iterate over blocks
		//for (i = 0; i < mybes.blockCount; i++) {

		//	// iterate over equations
		//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		//
		//		printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

		//	}
		//}

	printf("[done]\n");
}

void shuffleBES() {

	int i,j;
	int tmpRef;

	eqn tmp;

	printf("Shuffling BES... \t\t\t\t\t");

		//// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		
				printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

			}
		}

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 1; j < mybes.blocks[i].eqnCount; j += 2) {

				printf("j: %d \t eqnCount: %d\n", j, mybes.blocks[i].eqnCount);

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + (j - 1)];
				mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + j] = tmp;

				tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + (j -1)].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + (j-1)].lhsId] = tmpRef;

				//printf("%d\n%d\n\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[mybes.blocks[i].eqnCount-(j+1)].lhsId);
			}
		}

		//// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		
				printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

			}
		}

	printf("[done]\n");
}

void reorderBES() {

	int i,j;
	int tmpRef;
	eqn tmp;

	printf("Reordering BES... \t\t\t\t\t");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount - 1; j += 2) {

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[j + 1];
				mybes.blocks[i].eqns[j + 1] = tmp;


				tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[(mybes.blocks[i].eqnCount / 2) + j].lhsId] = tmpRef;

				//printf("%d\n%d\n\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[mybes.blocks[i].eqnCount-(j+1)].lhsId);
			}
		}
	printf("[done]\n");
}

void sortByTerminals() {

	int i,j;
	int index;
	eqn tmp;

	index = 0;

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {

		// iterate over equations
		for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

			if(mybes.blocks[i].eqns[j].rhs[0].type == T || mybes.blocks[i].eqns[j].rhs[0].type == F) {
				
				tmp = mybes.blocks[i].eqns[index];
				mybes.blocks[i].eqns[index] = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = tmp;
				index++;
			}
		}
	}

	printf("%d terminals reordered\n", index);
}

void interleaveBES(int intVal) {

	int i,j;
	int tmpRef;
	eqn tmp;

		//// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		
				printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

			}
		}

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {

		// iterate over equations
		for(j = 0; j < (mybes.blocks[i].eqnCount - 1) / intVal; j++) {

			if( j % (intVal - 1)) {
				
				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount - 1) / intVal];
				mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount-1)/intVal] = tmp;

				tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount-1)/intVal].lhsId];
				mybes.blocks[i].refs[mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount-1)/intVal].lhsId] = tmpRef;
			}
		}
	}

		//// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
		
				printf("Ref: %d \t LHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId);

			}
		}

}

void renumberBES() {

    int i, j, k;		// for loops
	int **oldVars;


	oldVars = (int**) malloc(sizeof(int*) * mybes.blockCount);

	// iterate over blocks
	for (i=0; i < mybes.blockCount; i++) {

		oldVars[i] = (int*) malloc(sizeof(int) * mybes.blocks[i].eqnCount);

	} // end block


	// iterate over blocks
	for (i=0; i < mybes.blockCount; i++) {
      
		// iterate over equations
		for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

			oldVars[i][j] = mybes.blocks[i].eqns[j].lhsId;
			mybes.blocks[i].eqns[j].lhsId = j;

		} //end equation
	} // end block


	// iterate over blocks
	for (i=0; i < mybes.blockCount; i++) {

		// iterate over equations
		for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				// iterate over variables
				for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++) {

					switch (mybes.blocks[i].eqns[j].rhs[k].type)
					{
						case local:
							//if(mybes.blocks[i].eqns[j].rhs[k].localRef == oldVars[mybes.blocks[i].eqns[j].rhs[k].globalRef][mybes.blocks[i].eqns[j].rhs[k].localRef]) printf("Var %d unchanged\n", mybes.blocks[i].eqns[j].rhs[k].localRef);
							//printf("Var %d_%d_%d = %d\n", i, j, k, mybes.blocks[i].eqns[j].rhs[k].localRef);
							mybes.blocks[i].eqns[j].rhs[k].localRef = oldVars[i][mybes.blocks[i].eqns[j].rhs[k].localRef];
							//printf("Var %d_%d_%d = %d\n\n", i, j, k, mybes.blocks[i].eqns[j].rhs[k].localRef);
							break;
                        
						case global:
							//if(mybes.blocks[i].eqns[j].rhs[k].localRef == oldVars[mybes.blocks[i].eqns[j].rhs[k].globalRef][mybes.blocks[i].eqns[j].rhs[k].localRef]) printf("Var %d unchanged\n", mybes.blocks[i].eqns[j].rhs[k].localRef);
							//printf("Var %d_%d_%d = %d\n", i, j, k, mybes.blocks[i].eqns[j].rhs[k].localRef);
							mybes.blocks[i].eqns[j].rhs[k].localRef = oldVars[mybes.blocks[i].eqns[j].rhs[k].globalRef][mybes.blocks[i].eqns[j].rhs[k].localRef];
							
							//printf("Var %d_%d_%d = %d\n\n", i, j, k, mybes.blocks[i].eqns[j].rhs[k].localRef);
							break;

                        
						//case conjunct: printf("and ");
						//	break;
      //                  
						//case disjunct: printf("or ");
						//	break;
						default:
							break;
					}
			} //end variables
		} //end equation
	} // end block


	// dealocate the memory
	for (i=0; i < mybes.blockCount; i++) {
		free(oldVars[i]);
	}
	free(oldVars);
}

void orderAscending() {

	int i,j;
	int tmpRef;
	int steps;

	eqn tmp;

	steps = 1;

	printf("reordering in ascending order of RHSs... \t\t\t\t");
	
	/// iterate over blocks
	//printf("\n");	
	//for (i = 0; i < mybes.blockCount; i++) {

	//	// iterate over equations
	//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
	//	
	//		printf("Ref: %d \t LHS: %d \t RHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].varCount);

	//	}
	//}

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {

		printf("[");
      
		// iterate over equations
		for (j = 0; j < mybes.blocks[i].eqnCount; j++) {

			for (int k = j; k < mybes.blocks[i].eqnCount; k++) {

				if (mybes.blocks[i].eqns[j].varCount > mybes.blocks[i].eqns[k].varCount) {

					tmp = mybes.blocks[i].eqns[k];
					mybes.blocks[i].eqns[k] = mybes.blocks[i].eqns[j];
					mybes.blocks[i].eqns[j] = tmp;

					tmpRef = mybes.blocks[i].refs[mybes.blocks[i].eqns[k].lhsId];
					mybes.blocks[i].refs[mybes.blocks[i].eqns[k].lhsId] = mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId];
					mybes.blocks[i].refs[mybes.blocks[i].eqns[j].lhsId] = tmpRef;
				}
			}
		
			//printf("%d\n", changes);

			if ( j == ((mybes.blocks[i].eqnCount / 10) * steps)) {
				printf("*");
				//printf("%d", j);
				steps++;
			}
		}
		printf("]");
	}

	//// iterate over blocks
	//printf("\n");	
	//for (i = 0; i < mybes.blockCount; i++) {

	//	// iterate over equations
	//	for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
	//	
	//		printf("Ref: %d \t LHS: %d \t RHS: %d\n", mybes.blocks[i].refs[j], mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].varCount);

	//	}
	//}

	printf("\n");
}

void orderDescending() {

	int i,j;
	int steps;
	int changes;
	int min, max;
	int minIdx, maxIdx;

	eqn tmp;

	steps = 1;

	printf("reordering in ascending order of RHSs... \t\t");

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {

		printf("[");
      
		// iterate over equations
		for (j = 0; j < mybes.blocks[i].eqnCount; j++) {

			changes = 0;
			min = max = 0;
			minIdx = j;
			maxIdx = mybes.blocks[i].eqnCount - j;

			for (int k = j; k < mybes.blocks[i].eqnCount; k++) {

				
				if (mybes.blocks[i].eqns[k].varCount > max) {max = mybes.blocks[i].eqns[k].varCount; maxIdx = k;}//printf("j:%d \t k:%d\t", j, k);printf("minIdx:%d \t maxIdx:%d\n", minIdx, maxIdx);}
				else if (mybes.blocks[i].eqns[k].varCount < min) {min = mybes.blocks[i].eqns[k].varCount; minIdx = k;}//printf("j:%d \t k:%d\t", j, k);printf("minIdx:%d \t maxIdx:%d\n", minIdx, maxIdx);}
				
			}

			if (mybes.blocks[i].eqns[j].varCount != mybes.blocks[i].eqns[minIdx].varCount) {

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[minIdx];
				mybes.blocks[i].eqns[minIdx] = tmp;
			}

			//if (mybes.blocks[i].eqns[mybes.blocks[i].eqnCount - (j + 1)].varCount != mybes.blocks[i].eqns[maxIdx].varCount) {

			//	tmp = mybes.blocks[i].eqns[mybes.blocks[i].eqnCount - (j + 1)];
			//	mybes.blocks[i].eqns[mybes.blocks[i].eqnCount - (j + 1)] = mybes.blocks[i].eqns[maxIdx];
			//	mybes.blocks[i].eqns[maxIdx] = tmp;
			//}
		
			//printf("j:%d \t %d\n", j, changes);

			if ( j == ((mybes.blocks[i].eqnCount / 10) * steps)) {
				printf("*");
				//printf("%d", j);
				steps++;
			}
		}
		printf("]");
	}
	printf("\n");
}

void printNumRHSVars() {

	int i,j;

	eqn tmp;

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {
      
		// iterate over equations
		for (j = 0; j < mybes.blocks[i].eqnCount; j++) {

			printf("%d\n", mybes.blocks[i].eqns[j].varCount);
		}
	}
}

void solve() {

	printf("Iterations: %d\n", parSolveBES());
	
	if(mybes.blocks[0].eqns[0].lhs == 0) printf("False\n");
	else if (mybes.blocks[0].eqns[0].lhs == 1) printf("False\n");
	else printf("Error!\n");
}

void evaluateBES(const char* filename, int runs) {

	int i;
	int strLen;
	double tmp;
	double avrg, min, max;
	int minIter, maxIter;

	char *solFile;

	double** data;
	int rows, columns;

	//initialization
	strLen = 0;
	rows = columns = 0;
	minIter = maxIter = 0;

	//unmodified BES
	tmp = computeDistance();
	max = min = avrg = tmp;
	data = (double**) malloc(sizeof(double*));
	printf("Solving original BES... \t\t\t\t\t\t");
	data[0] = solveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 10));
	strcpy(solFile, filename);
	strcat(solFile, ".orig.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);


	////parallel resolution of BES
	__cilkrts_end_cilk();  
	if (0!= __cilkrts_set_param("nworkers","8")) {
		printf("Failed to set worker count\n");
	}
	__cilkrts_init(); 
	printf("Solving original BES using Clik Plus with [%d] workers... \t\t\t\t", __cilkrts_get_nworkers());
	data = (double**) realloc(data, sizeof(double*) * 2);
	data[1] = parSolveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 10));
	strcpy(solFile, filename);
	strcat(solFile, ".orig.par");
	writeSolution2File(solFile, mybes);
	free(solFile);


	//reversed BES
	reverseBES();
	data = (double**) realloc(data, sizeof(double*) * 3);
	printf("Solving reversed BES... \t\t\t\t\t\t");
	data[2] = solveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".rev.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

	////parallel resolution of reversed BES
	//reverseBES();
	//data = (double**) realloc(data, sizeof(double*) * 4);
	//printf("Solving reversed BES in parallel... \t\t\t\t");
	//data[3] = parSolveBES();
	//printf("[done]\n");

	//reverseBES();
	//strLen = strlen(filename);
	//solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	//strcpy(solFile, filename);
	//strcat(solFile, ".rev.par");
	//writeSolution2File(solFile, mybes);
	//free(solFile);

	/*start uncomment here*/
	////strLen = strlen(filename);
	////solFile = (char*) malloc(sizeof(char) * (strLen + 5));
	////strcpy(solFile, filename);
	////strcat(solFile, "init");

	////writeSolution2File(solFile, mybes);
	////free(solFile);
	
	//randomized BES
	randomizeBES();
	data = (double**) realloc(data, sizeof(double*) * 4);
	printf("Solving randomized BES... \t\t\t\t\t\t");
	data[3] = solveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".rnd.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

/*

	//ascending RHSs
	orderAscending();
	data = (double**) realloc(data, sizeof(double*) * 4);
	printf("Solving BES with ascending RHSs... \t\t\t\t\t");
	data[3] = solveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".asc.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);
	//data = (double**) realloc(data, sizeof(double*) * 6);
	//printf("Solving BES with ascending RHSs in parallel... \t\t");
	//data[5] = parSolveBES();
	//printf("[done]\n");


	//descending RHSs
	reverseBES();
	data = (double**) realloc(data, sizeof(double*) * 5);
	printf("Solving BES with descending RHSs... \t\t\t\t\t");
	data[4] = solveBESBottomUp();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".dsc.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

*/

	//data = (double**) realloc(data, sizeof(double*) * 8);
	//printf("Solving BES with descending RHSs in parallel... \t");
	//data[7] = parSolveBES();
	//printf("[done]\n");



	//interleave BES by factor 2 (data[2])
	//reverseBES();
	//renumberBES();
	//interleaveBES(2);
	//renumberBES();
	//tmp = computeDistance();
	//if( tmp < min) {min = tmp; minIter++;}
	//if( tmp > max) {max = tmp; maxIter++;}
	//data = (double**) realloc(data, sizeof(double*) * 3);
	//data[2] = solveBES();

	writeEval2File(filename, data, 4);

	//dealocate memory
	for (i=0; i < rows; i++) {

		free(data[i]);
	}

	free(data);
}

void evaluateRunTimes(char* filename, int iteration){

	int i;
	ofstream runTimes;
	LONGLONG ts, te;
	LONGLONG freq;
	double base;

	//HANDLE hProcess;
	//DWORD_PTR pAffinityMask = 0x8;
	//hProcess = GetCurrentProcess();
	//SetProcessAffinityMask(hProcess, pAffinityMask);

	//QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	//printf("Frequency: %d\n", (LARGE_INTEGER*)freq);

	runTimes.open("runTimes.dat", ofstream::app);
	if( ! runTimes.is_open() ) printf("[Error: could not open %s!]\n", "runTimes.dat");

	QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
	runTimes << freq << " ";

	runTimes << filename[0] << filename[1] << " " << iteration << " ";

	randomizeBES();

	//unmodified BES
	QueryPerformanceCounter((LARGE_INTEGER*)&ts);
	solveBESBottomUp();
	QueryPerformanceCounter((LARGE_INTEGER*)&te);

	// select relative or absolute values
	base = te - ts;		//relative
	base = 1.0;			// absolute

	runTimes << (te - ts)/base;
	cout << "seq" << " ";

	//pAffinityMask = 0xFF;
	//SetProcessAffinityMask(hProcess, pAffinityMask);
		
		__cilkrts_end_cilk();
		if (0!= __cilkrts_set_param("nworkers","1"))
		 {
			printf("Failed to set worker count\n");
		 }
		//__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << " ";

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;		


		__cilkrts_end_cilk();
		if (0!= __cilkrts_set_param("nworkers","2"))
		 {
			printf("Failed to set worker count\n");
		 }
		//__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << " ";

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;


		__cilkrts_end_cilk();
		if (0!= __cilkrts_set_param("nworkers","4"))
		 {
			printf("Failed to set worker count\n");
		 }
		//__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << " ";

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;


		__cilkrts_end_cilk();
		if (0!= __cilkrts_set_param("nworkers","8"))
		 {
			printf("Failed to set worker count\n");
		 }
		//__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << " ";

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;

/*
		if (0!= __cilkrts_set_param("nworkers","8"))
		 {
			printf("Failed to set worker count\n");
		 }
		__cilkrts_end_cilk();  
		__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << " ";

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;

		Sleep(1000);

		if (0!= __cilkrts_set_param("nworkers","8"))
		 {
			printf("Failed to set worker count\n");
		 }
		__cilkrts_end_cilk();  
		__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << endl;

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;


		if (0!= __cilkrts_set_param("nworkers","32"))
		 {
			printf("Failed to set worker count\n");
		 }
		__cilkrts_end_cilk();  
		__cilkrts_init(); 
		cout << __cilkrts_get_nworkers() << endl;

		QueryPerformanceCounter((LARGE_INTEGER*)&ts);
		parSolveBESBottomUp();
		QueryPerformanceCounter((LARGE_INTEGER*)&te);
		runTimes << " " << (te - ts)/base;
*/

	////reversed BES
	//reverseBES();
	//QueryPerformanceCounter((LARGE_INTEGER*)&ts);
	//solveBESBottomUp();
	//QueryPerformanceCounter((LARGE_INTEGER*)&te);
	//runTimes << " " << te-ts;

	//QueryPerformanceCounter((LARGE_INTEGER*)&ts);
	//parSolveBESBottomUp();
	//QueryPerformanceCounter((LARGE_INTEGER*)&te);
	//runTimes << " " << te-ts;


	////randomized BES
	//randomizeBES();
	//QueryPerformanceCounter((LARGE_INTEGER*)&ts);
	//solveBESBottomUp();
	//QueryPerformanceCounter((LARGE_INTEGER*)&te);
	//runTimes << " " << te-ts;

	//QueryPerformanceCounter((LARGE_INTEGER*)&ts);
	//parSolveBESBottomUp();
	//QueryPerformanceCounter((LARGE_INTEGER*)&te);
	//runTimes << " " << te-ts;

	runTimes << endl;

	runTimes.close();

}