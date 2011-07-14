#pragma once

class Node
{
public:
	int index;
	bool inverted;
public:
	Node(void);
	~Node(void);
	virtual void print();
};

