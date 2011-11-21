//
//  main.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//

#include <stdio.h>
#include <windows.h>

#include "bes.h"
#include "helpers.h"
#include "solver1.h"

extern "C" void parse(const char *fileName);

extern "C" bes mybes;


int main (int argc, const char *argv[])
{
	/*test size of types on x64*/
	//printf("Sizeof(int) = %d", sizeof(int));
	//printf("Sizeof(Int64) = %d", sizeof(__int64));
	//printf("Sizeof(int*) = %d", sizeof(int*));
	//exit(-1);

	/*vars for Milans stuff*/
	//uint32_t left_size = 0;
	//uint32_t right_size = 0;
	//lhsVariable* lefts;
	//uint32_t* rights;
	//std::pair <lhsVariable*, uint32_t*> array_representation_of_bes;
	
	int strLen;
	char *infile;

	// put default inputfile here - or leave blank
	infile = "cwi_142_925.bcg.ndl.bes";
	infile = "adtest.bes";
	infile = "37_vasy_8082_42933.bcg.ndl.bes";
	infile = "01_vasy_0_1.bcg.nll.bes";
	// use filename from cmd in case it is supplied

	if (argv[2]) {
		strLen = strlen(argv[2]);
		infile = (char*) malloc(sizeof(char) * (strLen + 1));
		infile = strcpy(infile, argv[2]);
	}

	//infile = "04_cwi_3_14.bcg.ndl.bes";
	//parseBES(infile);
	//initBES();
	//evaluateBES(infile, 0);
	//writeBES2File(infile, mybes);

	switch (argv[1][1]) {

		case 'd':	parseBES(infile);
					initBES();
					writeBES2File(infile, mybes);
					break;

		case 'e':	parseBES(infile);
					initBES();
					evaluateBES(infile, 0);
					break;
		
		case 'g':	parseBES(infile);
					initBES();
					graphBES();
					break;

		case 'h':	printHelp();
					break;

		case 's':	parseBES(infile);
					initBES();
					solve();
					writeSolution2File(infile, mybes);
					break;

		case 'x':	parseBES(infile);
					initBES();
					reverseBES();
					writeBES2File(infile, mybes);
					break;

		case 'r':	parseBES(infile);
					initBES();
					randomizeBES();
					writeBES2File(infile, mybes);
					break;

		case 't':	parseBES(infile);
					for (int i = 0; i < 100; i++) {
						Sleep(500);
						initBES();
						evaluateRunTimes(infile, i+1);
					}
					break;

		default:	printf("unknown parameter!\n");
					exit(EXIT_FAILURE);
	}
	


	/*more of Milans stuff*/
	//solver1* mySolver = new solver1();

	//void* dataStructure = mySolver->createDataStructure(mybes);
	//mySolver->solve(dataStructure);

	// array_representation_of_bes = mySolver->get_array_representation_of_bes (&mybes, &left_size, &right_size);
	//lefts = array_representation_of_bes.first;
	//rights = array_representation_of_bes.second;

	//printf("%d\n", left_size);

	//mySolver->print_array_representation_of_bes (lefts, rights, left_size);

    return EXIT_SUCCESS;
}