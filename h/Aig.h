#pragma once
#include "Node.h"
#include "Input.h"
#include "And.h"
#include "Output.h"
#include <stdint.h>

class Aig
{
private:
	int size;
	int tail;
	Input* CONST_0;
	Input* CONST_1;
	Output* outputs;

	Node* eliminate(Node* n, uint32_t index, bool my);
	Node* substitute(Node* n, uint32_t index);
	Node* copyTree(Node* n);
	Node* solve(Node* n);
public:
	Aig(int size);
	~Aig(void);

	Node* createInput(uint32_t index);
	Node* createOr(Node* l, Node* r);
	Node* createAnd(Node* l, Node* r);
	void print();
	void print(Node* n);
	void eliminate(int i);

	void substitute(int search, int replace);
	void setOutput(bool my, uint32_t index, Node* rhs);
	void solveEquation(uint32_t index);
	Node* getZeroNode();
	Node* getOneNode();
	uint32_t getSize();
};