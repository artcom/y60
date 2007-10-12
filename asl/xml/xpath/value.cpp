#include <stdlib.h>

#include "xpath/value.h"

namespace xpath {

    Value::~Value()
    {
    };

    BooleanValue::~BooleanValue()
    {
    };

    NumberValue::~NumberValue()
    {
    };

    NumberValue *BooleanValue::toNumber()
    {
        return new NumberValue(value!=0?1:0);
    };

    StringValue *BooleanValue::toString()
    {
        return new StringValue(value?"true":"false");
    };

    StringValue *NumberValue::toString()
    {
        if (isNaN)
            return new StringValue("NaN");
        if (isInfinity)
            return new StringValue("-Infinity"+(value<0?0:1));
        std::string result;
        std::ostringstream stream(result);
        stream << value;
        return new StringValue(result);
    };

    StringValue *NodeSetValue::toString()
    {
        NodeRef stringnode = item(0);
        if (!stringnode)
            return new StringValue(NULL);
        switch(stringnode->nodeType())
            {
            case dom::Node::ELEMENT_NODE:
            case dom::Node::DOCUMENT_NODE:
                // ### TODO serialize text node descendants
                return new StringValue(stringnode->nodeValue());
            case dom::Node::TEXT_NODE:
            case dom::Node::ATTRIBUTE_NODE:
            default:
                return new StringValue(stringnode->nodeValue());
            }
    };

    StringValue *StringValue::stringValueFor(NodeRef n)
    {
        if (!n)
            return new StringValue(NULL);
        switch(n->nodeType())
            {
            case dom::Node::DOCUMENT_NODE:
            case dom::Node::ELEMENT_NODE:
                return new StringValue(n->nodeName());
                // ###
            case dom::Node::ATTRIBUTE_NODE:
            case dom::Node::TEXT_NODE:
            default:
                return new StringValue(n->nodeValue());
            };
    };

    StringValue::~StringValue()
    {
    };

    NumberValue *NodeSetValue::toNumber() {
        // ## TODO: handle empty nodesets correctly.

        string nodeContent;
        std::stringstream myStream(nodeContent);
        item(0)->print(myStream);
	double num;
	myStream >> num;
        return new NumberValue(num);
    };

    NodeSetValue *NodeSetValue::toNodeSet() {
	// ### WARNING: this practivally invalidates the current node set.
	// use only in a destructive setting, i.e. if you don't need
	// the old nodeset any more after conversion.
	return new NodeSetValue(takeNodes());
    };
};
