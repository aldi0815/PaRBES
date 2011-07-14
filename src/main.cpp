//
//  main.cpp
//  PaRBES
//
//  Created by Alexander Ditter on 19.05.11.
//  Copyright 2011 University of Bamberg. All rights reserved.
//

#include <stdio.h>

#include "bes.h"
#include "solver1.h"

extern "C" void parse(const char *fileName);

extern "C" bes mybes;

int main (int argc, const char *argv[])
{
	uint32_t left_size = 0;
	uint32_t right_size = 0;

	lhsVariable* lefts;
    uint32_t* rights;
    std::pair <lhsVariable*, uint32_t*> array_representation_of_bes;

	//parse(argv[1]); 
	parse("vasy_1_4.bcg.ndl.bes"); 
    //graphBES();

	solver1* mySolver = new solver1();

	void* dataStructure = mySolver->createDataStructure(mybes);
	mySolver->solve(dataStructure);

	 array_representation_of_bes = mySolver->get_array_representation_of_bes (&mybes, &left_size, &right_size);
     lefts = array_representation_of_bes.first;
     rights = array_representation_of_bes.second;

	

	//printf("%d\n", left_size);

	//mySolver->print_array_representation_of_bes (lefts, rights, left_size);


	 mySolver->createAIG(lefts, rights, left_size);
    
    return 0;
}