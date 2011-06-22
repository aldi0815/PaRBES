//
//  bes.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//

#include <stdio.h>

#include "bes.h"

extern "C" bes mybes;

void printBES()
{
    int i, j, k;
    
    for (i=0; i< mybes.blockCount; i++) {
        
        // print block information
        printf("block ");
        if (mybes.blocks[i].sign) printf("nu ");
        else printf("mu ");
        printf("B%d ", mybes.blocks[i].blockidentifier);
        if (mybes.blocks[i].unique) printf("unique ");
        printf("mode %d", mybes.blocks[i].mode);
        printf("\t (%d eqations)\n", mybes.blocks[i].eqnCount);
        
        // print equations
        for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
            printf("X%d = ", mybes.blocks[i].eqns[j].lhsId);
            for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++)
            {
                switch (mybes.blocks[i].eqns[j].vars[k].type)
                {
                    case T: printf("true ");
                        break;
                        
                    case F: printf("false ");
                        break;
                        
                    case local: printf("X%d ", mybes.blocks[i].eqns[j].vars[k].localRef);
                        break;
                        
                    case global: printf("X%d_%d ", mybes.blocks[i].eqns[j].vars[k].localRef, mybes.blocks[i].eqns[j].vars[k].globalRef);
                        break;
                        
                    case conjunct: printf("and ");
                        break;
                        
                    case disjunct: printf("or ");
                        break;
                }
            }
            printf("\n");
        }
        printf("end block\n");
    }
    
}

void graphBES(){
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
                switch (mybes.blocks[i].eqns[j].vars[k].type)
                {
                    case true: printf("X%d -> T;", mybes.blocks[i].eqns[j].lhsId);
                        break;
                        
                    case false: printf("X%d -> F;", mybes.blocks[i].eqns[j].lhsId);
                        break;
                        
                    case local: printf("X%d -> X%d;", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].vars[k].localRef);
                        break;
                        
                    case global: printf("X%d -> X%d_%d;", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].vars[k].localRef, mybes.blocks[i].eqns[j].vars[k].globalRef);
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