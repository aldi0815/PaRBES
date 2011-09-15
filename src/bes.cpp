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

			mybes.blocks[i].eqns[j].lhs = mybes.blocks[i].sign;	// initial approximation
			mybes.blocks[i].eqns[j].count = 0; //debug info

		} //end equation
	} // end block
}

double* solveBES() {

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

	while(b) {

		b = false;
		changes = 0;

		iterations++;
		res[0] = iterations;

		res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));

		// iterate over blocks
		for (i=0; i< mybes.blockCount; i++) {
      
			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				//initial value of current variable
				tmp1 = mybes.blocks[i].eqns[j].lhs;

				if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = true;			// terminal true
				else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = false;	// terminal false
				else if ( mybes.blocks[i].eqns[j].varCount > 0 ) { 					// variable(s)

					// assignment of truth value of the first var of the rhs
					tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;

					// iterate over remaining rhs variables
					if (mybes.blocks[i].eqns[j].varCount > 1 ) {

						for(k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

								switch (mybes.blocks[i].eqns[j].rhs[1].type)
								{   
									case conjunct:
										//tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[i].eqns[j].rhs[k].localRef].lhs;
										tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;
                        
									case disjunct:
										//tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[i].eqns[j].rhs[k].localRef].lhs;
										tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;

									default:
										continue;
								} // end switch
						}//end variables
					}

				} // end if	

				
				if (tmp1 != tmp2) {
					mybes.blocks[i].eqns[j].lhs = tmp2;
					changes++;
					b = true; // continue as long as variables change
				}

			} //end equation
		} // end block

		sumChanges += changes;
		res[iterations+1] = (double) changes/(double) mybes.numVars;

	} // end while

	res[1] = sumChanges;	
	return res;
}

double* parSolveBES() {

	bool b;			// true as long as variables change within an iteration
	//bool tmp1, tmp2;		// store previous value of lhs
    int i;//j,k;			// for loops
	int iterations, changes, sumChanges;
	double* res;

	b = true;
	iterations = 0;
	sumChanges = 0;
	res = (double*) malloc(sizeof(double) * 2);

	initLHS();
	res[1] = computeDistance();

	while(b) {

		b = false;
		changes = 0;

		iterations++;
		res[0] = iterations;

		res = (double*) realloc(res, (sizeof(double) * (iterations + 2)));

		//printf("%d ", iterations);

		// iterate over blocks
		for (i=0; i< mybes.blockCount; i++) {
      
			// iterate over equations
			cilk_for (int j = 0; j < mybes.blocks[i].eqnCount; j++) {

				bool tmp1, tmp2;

				tmp1 = mybes.blocks[i].eqns[j].lhs;

				if ( mybes.blocks[i].eqns[j].rhs[0].type == T) tmp2 = 1;			// terminal true
				else if ( mybes.blocks[i].eqns[j].rhs[0].type == F ) tmp2 = 0;		// terminal false
				else if ( mybes.blocks[i].eqns[j].varCount > 0 ) { 					// variable(s)

					// assignment of truth value of the first var of the rhs
					tmp2 = mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[0].globalRef].refs[mybes.blocks[i].eqns[j].rhs[0].localRef]].lhs;

					// iterate over remaining rhs variables
					if (mybes.blocks[i].eqns[j].varCount > 1 ) {

						for(int k = 2; k < mybes.blocks[i].eqns[j].varCount; k += 2) {

								switch (mybes.blocks[i].eqns[j].rhs[1].type)
								{   
									case conjunct:
										//tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[i].eqns[j].rhs[k].localRef].lhs;
										tmp2 &= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;
                        
									case disjunct:
										//tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[i].eqns[j].rhs[k].localRef].lhs;
										tmp2 |= mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].eqns[mybes.blocks[mybes.blocks[i].eqns[j].rhs[k].globalRef].refs[mybes.blocks[i].eqns[j].rhs[k].localRef]].lhs;
										break;

									default:
										continue;
								} // end switch
						}//end variables
					}

				} // end if	

				
				if (tmp1 != tmp2) {
					mybes.blocks[i].eqns[j].lhs = tmp2;
					changes++;
					b = true; // continue as long as variables change
				}

			} //end equation
		} // end block

		sumChanges += changes;
		res[iterations+1] = (double) changes/(double) mybes.numVars;

	} // end while

	res[1] = sumChanges;	
	return res;
}

void reverseBES() {

	int i,j;
	int tmpRef;
	eqn tmp;

	printf("Reversing BES... \t\t\t\t\t\t");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {
      
			// iterate over equations
			for(j = 0; j < (mybes.blocks[i].eqnCount)/2; j++) {

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[mybes.blocks[i].eqnCount - (j + 1)];
				mybes.blocks[i].eqns[mybes.blocks[i].eqnCount - (j + 1)] = tmp;

				tmpRef = mybes.blocks[i].refs[j];
				mybes.blocks[i].refs[j] = (mybes.blocks[i].eqnCount - (j + 1));
				mybes.blocks[i].refs[mybes.blocks[i].eqnCount - (j + 1)] = tmpRef;
			}
		}
	printf("[done]\n");
}

void randomizeBES() {

	int i,j;
	int rnd;
	int tmpRef;

	srand(time(NULL));

	eqn tmp;

	printf("Randomizing BES... \t\t\t\t\t");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount; j++) {

				do {

					rnd = rand() % mybes.blocks[i].eqnCount;
				
				} while (rnd == j); //((mybes.blocks[i].eqns[rnd].lhsId == j) || (mybes.blocks[i].eqns[j].lhsId == rnd) || (rnd == j));

				tmp = mybes.blocks[i].eqns[rnd];
				mybes.blocks[i].eqns[rnd] = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = tmp;


				tmpRef = mybes.blocks[i].refs[rnd];
				mybes.blocks[i].refs[rnd] = mybes.blocks[i].refs[j];
				mybes.blocks[i].refs[j] = tmpRef;

				//printf("%d\n%d\n\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[mybes.blocks[i].eqnCount-(j+1)].lhsId);
			}
		}
	printf("[done]\n");
}

void shuffleBES() {

	int i,j;
	int tmpRef;

	eqn tmp;

	printf("Randomizing BES... \t\t\t\t\t");

		// iterate over blocks
		for (i = 0; i < mybes.blockCount; i++) {

			// iterate over equations
			for(j = 0; j < mybes.blocks[i].eqnCount - 1; j += 2) {

				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[j + 1];
				mybes.blocks[i].eqns[j + 1] = tmp;


				tmpRef = mybes.blocks[i].refs[j];
				mybes.blocks[i].refs[j] = mybes.blocks[i].refs[j + 1];
				mybes.blocks[i].refs[j + 1] = tmpRef;

				//printf("%d\n%d\n\n", mybes.blocks[i].eqns[j].lhsId, mybes.blocks[i].eqns[mybes.blocks[i].eqnCount-(j+1)].lhsId);
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


				tmpRef = mybes.blocks[i].refs[j];
				mybes.blocks[i].refs[j] = mybes.blocks[i].refs[j + 1];
				mybes.blocks[i].refs[j + 1] = tmpRef;

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
	eqn tmp;

	// iterate over blocks
	for (i = 0; i < mybes.blockCount; i++) {

		// iterate over equations
		for(j = 0; j < (mybes.blocks[i].eqnCount-1)/intVal; j++) {

			if( j % (intVal -1)) {
				
				tmp = mybes.blocks[i].eqns[j];
				mybes.blocks[i].eqns[j] = mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount-1)/intVal];
				mybes.blocks[i].eqns[j + (j % (intVal -1)) * (mybes.blocks[i].eqnCount-1)/intVal] = tmp;
			}
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

	printf("reordering in ascending order of RHSs... \t\t");

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

					tmpRef = mybes.blocks[i].refs[k];
					mybes.blocks[i].refs[k] = mybes.blocks[i].refs[j];
					mybes.blocks[i].refs[j] = tmpRef;
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
	printf("Solving original BES... \t\t\t\t\t");
	data[0] = solveBES();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 10));
	strcpy(solFile, filename);
	strcat(solFile, ".orig.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

	//parallel resolution of BES
	tmp = computeDistance();
	data = (double**) realloc(data, sizeof(double*) * 2);
	printf("Solving original BES in parallel... \t\t\t\t");
	data[1] = parSolveBES();
	printf("[done]\n");

	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 10));
	strcpy(solFile, filename);
	strcat(solFile, ".orig.par");
	writeSolution2File(solFile, mybes);
	free(solFile);

	//reversed BES
	reverseBES();
	//tmp = computeDistance();
	data = (double**) realloc(data, sizeof(double*) * 3);
	printf("Solving reversed BES... \t\t\t\t\t");
	data[2] = solveBES();
	printf("[done]\n");

	reverseBES();
	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".rev.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

	//parallel resolution of reversed BES
	//tmp = computeDistance();
	reverseBES();
	data = (double**) realloc(data, sizeof(double*) * 4);
	printf("Solving reversed BES in parallel... \t\t\t\t");
	data[3] = parSolveBES();
	printf("[done]\n");

	reverseBES();
	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".rev.par");
	writeSolution2File(solFile, mybes);
	free(solFile);

	/*start uncomment here*/
	////strLen = strlen(filename);
	////solFile = (char*) malloc(sizeof(char) * (strLen + 5));
	////strcpy(solFile, filename);
	////strcat(solFile, "init");

	////writeSolution2File(solFile, mybes);
	////free(solFile);

	
	//orderAscending();
	
	//tmp = computeDistance();
	//if( tmp < min) {min = tmp; minIter++;}
	//if( tmp > max) {max = tmp; maxIter++;}
	
	//ascending ordering BES
	shuffleBES();
	data = (double**) realloc(data, sizeof(double*) * 5);
	printf("Solving BES with ascending RHSs... \t\t\t");
	data[4] = solveBES();
	printf("[done]\n");


	strLen = strlen(filename);
	solFile = (char*) malloc(sizeof(char) * (strLen + 9));
	strcpy(solFile, filename);
	strcat(solFile, ".rnd.seq");
	writeSolution2File(solFile, mybes);
	free(solFile);

	data = (double**) realloc(data, sizeof(double*) * 6);
	printf("Solving BES with ascending RHSs in parallel... \t\t");
	data[5] = parSolveBES();
	printf("[done]\n");


	////ascending ordering BES (reverse BES)
	//reverseBES();
	////tmp = computeDistance();
	//if( tmp < min) {min = tmp; minIter++;}
	//if( tmp > max) {max = tmp; maxIter++;}
	//data = (double**) realloc(data, sizeof(double*) * 7);
	//printf("Solving BES with descending RHSs... \t\t\t");
	//data[6] = solveBES();
	//printf("[done]\n");


	//data = (double**) realloc(data, sizeof(double*) * 8);
	//printf("Solving BES with descending RHSs in parallel... \t");
	//data[7] = parSolveBES();
	//printf("[done]\n");
	/*end uncomment here*/


	//reversed BES (data[1])
	//reverseBES();
	//renumberBES();
	//tmp = computeDistance();
	//if( tmp < min) {min = tmp; minIter++;}
	//if( tmp > max) {max = tmp; maxIter++;}
	//data = (double**) realloc(data, sizeof(double*) * 2);
	//data[1] = solveBES();

	//strLen = strlen(filename);
	//solFile = (char*) malloc(sizeof(char) * (strLen + 4));
	//strcpy(solFile, filename);
	//strcat(solFile, "rev");

	//writeBES2File(filename, mybes);
	//reverseBES();
	//writeSolution2File(solFile, mybes);
	//free(solFile);


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

	//for(i = 0; i < runs; i++) {
	//	randomizeBES();
	//	renumberBES();
	//	tmp = computeDistance();

	//	if( tmp < min) {
	//		min = tmp;
	//		minIter++;
	//		data = (double**) realloc(data, sizeof(double*) * (minIter + maxIter + 1));
	//		data[minIter + maxIter] = solveBES();
	//	}

	//	if( tmp > max) {
	//		max = tmp;
	//		maxIter++;
	//		data = (double**) realloc(data, sizeof(double*) * (minIter + maxIter + 1));
	//		data[minIter + maxIter] = solveBES();
	//	}
	//}

	//printf("avrg:\t %4.1f\n", avrg);
	//printf("min:\t %4.1f (%d)\n", min, minIter);
	//printf("max:\t %4.1f (%d)\n", max, maxIter);

	//printf("Computations: %d\n", (minIter + maxIter + 1));

	////for(i = 0; i < (minIter + maxIter + 1); i++) {
	////	printf("Data[%d][0] %f\n", i, data[i][0]);
	////}

	//writeEval2File(filename, data, (minIter + maxIter + 1));

	writeEval2File(filename, data, 6);


	// dealocate memory
	//for (i=0; i < rows; i++) {
	//	free(data[i]);
	//}
	//free(data);
}