#include "Input.h"


Input::Input(void)
{
}

Input::Input(Node* n)
{
	index = n->index;
	inverted = n->inverted;
}

Input::Input(uint32_t index){
	this->index = index;
	inverted = false;
}

Input::~Input(void)
{
}

