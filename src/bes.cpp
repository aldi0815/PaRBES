//
//  bes.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//

#include <stdio.h>
#include <math.h>

#include "bes.h"

extern "C" bes mybes;

int computeNumVars() {

	int i;
	int numVars = 0;

	for (i=0; i< mybes.blockCount; i++) {
		numVars += mybes.blocks[i].eqnCount;
	}

	return numVars;
}

void printBES() {

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
                switch (mybes.blocks[i].eqns[j].rhs[k].type)
                {
                    case T: printf("true ");
                        break;
                        
                    case F: printf("false ");
                        break;
                        
                    case local: printf("X%d ", mybes.blocks[i].eqns[j].rhs[k].localRef);
                        break;
                        
                    case global: printf("X%d_%d ", mybes.blocks[i].eqns[j].rhs[k].localRef, mybes.blocks[i].eqns[j].rhs[k].globalRef);
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

void computeDistance(){
    int i, j, k, l;
	int tmpSum, numNeighbours;
	int avrgDist;
	int *dists;
    
	l = 0;
	avrgDist = 0;
	dists = (int*) malloc(sizeof(int)*mybes.numVars);

	// iterate over blocks
    for (i=0; i< mybes.blockCount; i++) {
      
		// iterate over equations
        for(j = 0; j < mybes.blocks[i].eqnCount; j++) {
    
			tmpSum = 0;
			numNeighbours = 0;

			// iterate over variables
            for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++) {

                switch (mybes.blocks[i].eqns[j].rhs[k].type) {    
                    case T: 
						printf("X%d -> T\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[j].rhs[k].localRef);
						tmpSum = tmpSum + abs(mybes.blocks[i].eqns[j].rhs[k].localRef - mybes.blocks[i].eqns[j].lhsId);
						numNeighbours++;
						break;
                    
					default: 
                        continue;
                } // end switch
            } //end variables

				if (0 != numNeighbours) {
					dists[l] = tmpSum/numNeighbours;
				}

				else {
					dists[l] = 0;
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
		avrgDist += dists[i];
	}
	avrgDist /= l;

//	printf("Average Distance: %d\n", avrgDist);
}

void initLHS() {

    int i, j, k;

	// iterate over blocks
	for (i=0; i< mybes.blockCount; i++) {
      
		// iterate over equations
		for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

			mybes.blocks[i].eqns[j].lhs = mybes.blocks[i].sign;
				
		} //end equation
	} // end block
}

void solveBES() {

	bool b;
    int i, j, k;


	initLHS();
    
	while(b) {

		// iterate over blocks
		for (i=0; i< mybes.blockCount; i++) {
      
			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				// iterate over variables
				for(k = 0; k < mybes.blocks[i].eqns[j].varCount; k++) {

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
					} // end switch
				} //end variables			
			} //end equation
		} // end block
	} // end while
}