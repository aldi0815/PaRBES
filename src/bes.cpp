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

#include "helpers.h"

#include "bes.h"

extern "C" bes mybes;

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
	res[1] = computeDistance();

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
	//tmp = computeDistance();
	//data = (double**) realloc(data, sizeof(double*) * 2);
	//printf("Solving original BES in parallel... \t\t\t\t");
	//data[1] = parSolveBES();
	//printf("[done]\n");

	//strLen = strlen(filename);
	//solFile = (char*) malloc(sizeof(char) * (strLen + 10));
	//strcpy(solFile, filename);
	//strcat(solFile, ".orig.par");
	//writeSolution2File(solFile, mybes);
	//free(solFile);

	//reversed BES
	reverseBES();
	data = (double**) realloc(data, sizeof(double*) * 2);
	printf("Solving reversed BES... \t\t\t\t\t\t");
	data[1] = solveBESBottomUp();
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
	data = (double**) realloc(data, sizeof(double*) * 3);
	printf("Solving randomized BES... \t\t\t\t\t\t");
	data[2] = solveBESBottomUp();
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

	writeEval2File(filename, data, 3);


	//dealocate memory
	for (i=0; i < rows; i++) {

		free(data[i]);
	}

	free(data);
}