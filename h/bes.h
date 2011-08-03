//
//  bes.h
//  PaRBES
//
//  Created by Alexander Ditter on 16.06.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//


#ifndef BES_H
#define BES_H

#include <stdlib.h>

typedef enum varType{
      F,
      T,
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
    var* rhs;
}eqn;

typedef struct besblock{
    int sign;
    int blockidentifier;
    int unique;
    int mode;
    int eqnCount;
    eqn* eqns;
}besblock;

typedef struct bes{
    int blockCount;
	int numVars;
    besblock* blocks;
}bes;


int computeNumVars();
void computeDistance();
void printBES();
void graphBES();

#endif