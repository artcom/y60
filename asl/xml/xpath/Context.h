#ifndef XPATH_CONTEXT_H
#define XPATH_CONTEXT_H

#include "Value.h"

namespace xpath
{

  class Context
    {
    public:
	Context() { };
	Context(NodeRef n) { currentNode = n; position = 1; size=1; };
	Context(const Context &other) { size = other.size; position = other.position; currentNode = other.currentNode; };

	// context position
	NodeRef currentNode;

	int position;

	int size;
    };

};

#endif

