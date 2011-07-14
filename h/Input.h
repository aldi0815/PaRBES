#pragma once
#include "node.h"
#include <stdint.h>

class Input :
	public Node
{
public:
	Input(void);
	Input(Node* n);
	Input(uint32_t index);
	~Input(void);
};

