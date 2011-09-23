//
//  main.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//

#include <stdio.h>

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

	// use filename from cmd in case it is supplied
	if (argv[2]) {
		strLen = strlen(argv[2]);
		infile = (char*) malloc(sizeof(char) * (strLen + 1));
		infile = strcpy(infile, argv[2]);
	}

	//for (int i = 0; i < argc; i++) {
	//	printf("argv[%d]: %s\n", i, argv[i]);
	//}

	// parse input BES file
	//printf("Parsing %-50.30s \t\t", infile);
	parse(infile);
	//printf("[done]\n");

	printf("%s\n", infile);

	switch (argv[1][1]) {

		case 'e':	initBES();
					evaluateBES(infile, 0);
					break;

		case 'h':	printHelp();
					break;

		case 's':	initBES();
					solve();
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