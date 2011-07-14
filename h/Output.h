#pragma once
#include "node.h"
class Output :
	public Node
{
private:
	Node* result;
	bool my;
public:
	Output(void);
	~Output(void);
	void setResult(Node* n);
	Node* getResult();
	void setMy(bool my);
	bool getMy();
};

