#include "Node.h"

#include<iostream>

Node::Node(void)
{
}


Node::~Node(void)
{
}

void Node::print(){
	if(index == 0) {
		if(inverted) {
			std::cout << "true";
		} else {
			std::cout << "false";
		}
	} else {
		if(inverted) {
			std::cout << "!";
		}
		std::cout << index-1;
	}
}