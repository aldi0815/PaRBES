#include "And.h"


And::And(void)
{
	left = 0;
	right = 0;
	inverted = false;
}


And::~And(void)
{
}

And::And(Node* n)
{
	And* a = (And*) n;
	inverted = a->inverted;
	left = a->left;
	right = a->right;
	index = a->index;
}

And::And(Node* a, Node* b) {
	left = a;
	right = b;
	inverted = false;
}