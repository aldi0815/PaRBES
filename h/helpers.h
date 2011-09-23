#include <iostream>
#include <fstream>
#include <iomanip>

#include "bes.h"

using namespace std;

void printHelp();

int writeBES2File(const char* filename, bes a);
int writeEval2File(const char* filename, double** data, int rows);
int writeSolution2File(const char* filename, bes a);