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
    var* vars;
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
    besblock* blocks;
}bes;

void printBES();
void graphBES();

#endif