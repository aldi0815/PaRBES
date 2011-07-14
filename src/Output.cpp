#include "Output.h"
#include<iostream>

Output::Output(void)
{
}


Output::~Output(void)
{
}

void Output::setResult(Node* n) {
	result = n;
}

Node* Output::getResult() {
	return result;
}

void Output::setMy(bool my) {
	this->my = my;
}

bool Output::getMy() {
	return my;
}