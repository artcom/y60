#include <stdlib.h>

#include "Value.h"

namespace xpath {

    Value::~Value() {
    };

    BooleanValue::~BooleanValue() {
    };

    NumberValue::~NumberValue() {
    };

    NumberValue *BooleanValue::toNumber() {
        return new NumberValue(value!=0?1:0);
    };

    StringValue *BooleanValue::toString() {
        return new StringValue(value?"true":"false");
    };

    StringValue *NumberValue::toString() {
        if (isNaN)
            return new StringValue("NaN");
        if (isInfinity)
            return new StringValue("-Infinity"+(value<0?0:1));
        std::string result;
        std::ostringstream stream(result);
        stream << value;
        return new StringValue(result);
    };

    StringValue *NodeSetValue::toString() {
        NodeRef stringnode = item(0);
        if (!stringnode)
            return new StringValue(NULL);

	return new StringValue(string_value_for(stringnode));
    };

    string string_value_for(NodeRef n) {
	string retval;

        switch(n->nodeType())
            {
            case dom::Node::DOCUMENT_NODE:
            case dom::Node::ELEMENT_NODE:
		for (int i = 0; i < n->childNodesLength(); i++) {
		    retval += string_value_for(&*n->childNode(i));
		}
		break;
            case dom::Node::ATTRIBUTE_NODE:
            case dom::Node::TEXT_NODE:
                retval = n->nodeValue();
		break;
            default:
		AC_ERROR << " could not determine string_value for " << n->nodeType();
		retval = "";
            };
	return retval;
    }

    double number_value_for(const string &s) {
	double num;
	std::istringstream iss(s);
	if (!(iss >> num)) {
	    return 0;
	};
	int pos = iss.tellg();
	if (asl::read_whitespace(s, pos++) != s.length()) { return 0; };
	return num;
    }

    NumberValue *StringValue::toNumber() {
	int num = 0;
	std::istringstream iss(value);
	iss >> num;
	return new NumberValue(num);
    };

    StringValue::~StringValue() {
    };

    NumberValue *NodeSetValue::toNumber() {
        string nodeContent = string_value_for(item(0));
        std::stringstream myStream(nodeContent);
	double num;
	myStream >> num;
        return new NumberValue(num);
    };

    NodeSetValue *NodeSetValue::toNodeSet() {
	// WARNING: this practivally invalidates the current node set.
	// use only in a destructive setting, i.e. if you don't need
	// the old nodeset any more after conversion.
	return new NodeSetValue(takeNodes());
    };
};
