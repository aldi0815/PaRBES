#pragma once
#include "node.h"

class And :	public Node
{
public:
	Node* left;
	Node* right;
public:
	And(void);
	And(Node* a);
	And(Node* a, Node* b);
	~And(void);
};

