#include "Aig.h"
#include <stdio.h>
#include "And.h"
#include <iostream>
using namespace std;

Aig::Aig(int size)
{
	this->size = size;
	outputs = new Output[size+1];
	for(int i = 0; i <= size; i++) {
		outputs[i].index = i;
		outputs[i].inverted = false;
	}
	tail = size+2;

	CONST_0 = new Input();
	CONST_0->index = 0;
	CONST_0->inverted = false;
	CONST_1 = new Input(CONST_0);
	CONST_1->inverted = true;
}

Aig::~Aig(void)
{
}

Node* Aig::createInput(uint32_t index){
	return new Input(index+1);
}

void Aig::setOutput(bool my, uint32_t index, Node* rhs) {
	index++;
	rhs->index = index;
	outputs[index].setMy(my);
	outputs[index].setResult(rhs);
}

Node* Aig::getZeroNode() {
	return CONST_0;
}

Node* Aig::getOneNode(){
	return CONST_1;
}

uint32_t Aig::getSize() {
	return size;
}

Node* Aig::createOr(Node* n1, Node* n2){
	((And*)n1)->inverted = !((And*)n1)->inverted;
	((And*)n2)->inverted = !((And*)n2)->inverted;
	And* a1 = new And(n1, n2);
	a1->inverted = true;
	a1->index = tail++;
	And* a2 = new And(a1, CONST_1);
	return a2;
}

Node* Aig::createAnd(Node* n1, Node* n2){
	And* a = new And(n1, n2);
	return a;
}

// print all outputs
void Aig::print(){
	for(int i = 1; i <= size; i++) {
		std::cout << outputs[i].index-1 << "=";
		print(outputs[i].getResult());
		std::cout << " ";
	}
	printf("\n");
}

// print a single tree
void Aig::print(Node* n){
	And* a = dynamic_cast<And*> (n);
	if(a != NULL){	// Gate
		//a->print();
		if(a->inverted) {
			cout << "!";
		}
		cout << "(";
		print(a->left);
		cout << "^";
		print(a->right);
		cout << ")";
	}
	Input* i = dynamic_cast<Input*> (n);
	if(i != NULL){	// Input
		i->print();
	}
}

void Aig::eliminate(int index) {
	index++;
	bool my = outputs[index].getMy();
	Node* root = outputs[index].getResult();
	outputs[index].setResult(eliminate(root, index, my));
}

Node* Aig::eliminate(Node* n, uint32_t index, bool my) {
	Input* i = dynamic_cast<Input*> (n);
	if(i != NULL){	// Input
		if(i->index == index){
			// invertiert?
			if(i->inverted) {
				n = my ? CONST_1 : CONST_0;
			} else {
				n = my ? CONST_0 : CONST_1;
			}
		}
	}
	And* a = dynamic_cast<And*> (n);
	if(a != NULL) {	// Gate
		a->left = eliminate(a->left, index, my);
		a->right = eliminate(a->right, index, my);
	}
	return n;
}

void Aig::substitute(int i, int index) {
	i++;
	index++;
	outputs[i].setResult(substitute(outputs[i].getResult(), index));
}

Node* Aig::substitute(Node* n, uint32_t index) {
	Input* i = dynamic_cast<Input*> (n);
	if(i != NULL){	// Input
		if(i->index == index){
			//TODO: copyTree?
			Node* tmp = copyTree(outputs[index].getResult());
			if(i->inverted) {
				tmp->inverted = true;
			}
			return tmp;
		}
	}
	And* a = dynamic_cast<And*> (n);
	if(a != NULL) {	// Gate
		a->left = substitute(a->left, index);
		a->right = substitute(a->right, index);
	}
	return n;
}

Node* Aig::copyTree(Node* n) {
	Node* neu;
	Input* i = dynamic_cast<Input*> (n);
	if(i != NULL){	// Input
		neu = i;
	}
	And* a = dynamic_cast<And*> (n);
	if(a != NULL) {	// Gate
		And* tmp = new And(n);
		tmp->index = tail++;
		tmp->left = copyTree(a->left);
		tmp->right = copyTree(a->right);
		neu = tmp;
	}
	return neu;
}

void Aig::solveEquation(uint32_t index) {
	index++;
	Node* root = outputs[index].getResult();
	outputs[index].setResult(solve(root));
	// can be done parallel
	for(int i = index+1; i <= size; i++) {
		bool my = !outputs[i].getResult()->inverted;
		outputs[i].setResult(eliminate(outputs[i].getResult(), index, my));
	}
}

Node* Aig::solve(Node* n) {
	And* a = dynamic_cast<And*> (n);
	if(a != NULL) {	// Gate
		Input* left = dynamic_cast<Input*> (a->left);
		if(left == NULL){	// no Input
			a->left = solve(a->left);
		}
		Input* right = dynamic_cast<Input*> (a->right);
		if(right == NULL){	// no Input
			a->right = solve(a->right);
		}
		if(a->left == CONST_0 || a->right == CONST_0) {
			n = a->inverted ? CONST_1 : CONST_0;
		} else {
			n = a->inverted ? CONST_0 : CONST_1;
		}
	}
	return n;
}