//
//  bes.h
//  PaRBES
//
//  Created by Alexander Ditter on 16.06.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//


#ifndef BES_H
#define BES_H

#include <cilk\cilk_api.h>
#include <cilk/cilk.h>
#include <cilk/common.h>

typedef enum varType{
      T,
      F,
      global,
      local,
	  conjunct,
	  disjunct
}varType;

typedef struct var{
    varType type;
    int globalRef;
    int localRef;
}var;

typedef struct eqn{
    int varCount;
    int lhsId;
	int lhs;
	int count;
    var* rhs;
}eqn;

typedef struct besblock{
    int sign;
    int blockidentifier;
    int unique;
    int mode;
    int eqnCount;
    eqn* eqns;
	int* refs;
}besblock;

typedef struct bes{
    int blockCount;
	int numVars;
    besblock* blocks;
}bes;

void parseBES(const char *fileName);

int initBES();
double computeDistance();
void printBES();
void graphBES();
double* solveBES();
double* solveBESBottomUp();
void solveBESReverse();
void reverseBES();
void randomizeBES();
void sortByTerminals();
void renumberBES();
void orderDescending();
void orderAscending();
void printNumVars();
void reorderBES();
void interleaveBS(int intVal);

void shuffleBES();
int parSolveBES();
void solve();

void evaluateBES(const char* filename, int runs);
void evaluateRunTimes(char* filename, int iteration);

#endif