#pragma once


#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <set>
#include <vector>

#include "bes.h"

//typedef unsigned long int uint32_t;

class lhsVariable
{
//private:
public:
    uint32_t value;

    lhsVariable() : value(0) {}
    lhsVariable( uint32_t & val ) : value(val) {}

    inline uint32_t getIndex() const { return value & 0x1FFFFFFF; };
    inline void setIndex (uint32_t n) { value =  n | (value & 0xE0000000); };

    inline bool getValue() const { return value & 0x80000000; };
    inline void setTrue()  { value = value | 0x80000000; };
    inline void setFalse()  { value = value & ~0x80000000; };


    inline bool getType() const { return value & 0x40000000; };
    inline void setCon()  { value = value | 0x40000000 ; };
    inline void setDis()  { value = value & ~0x40000000 ; };

    inline bool isSolved() const  { return value & 0x20000000; };
    inline void setSolved() { value = value |  0x20000000; };
    inline void setUnsolved() { value = value & ~0x20000000; };
};


class solver1
{
public:
	solver1(void);
	~solver1(void);

	std::pair <lhsVariable*, uint32_t*> get_array_representation_of_bes (bes* input_bes, uint32_t* left_size, uint32_t* right_size);
	void print_array_representation_of_bes (lhsVariable* lefts, uint32_t* rights, uint32_t left_size);

	void* createDataStructure(bes);
	void createAIG(lhsVariable* lefts, uint32_t* rights, uint32_t left_size);
	bool solve(void*);
};

