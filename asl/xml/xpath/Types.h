#ifndef XPATH_TYPES_H
#define XPATH_TYPES_H

namespace xpath
{
    typedef std::string string;

    typedef dom::Node *NodeRef;

    typedef std::set<NodeRef> NodeSet;
    typedef NodeSet *NodeSetRef;

    typedef std::vector<NodeRef> NodeList;
    typedef NodeList *NodeListRef;

    struct DocOrderLess :
	public std::binary_function<const NodeRef, const NodeRef, bool>
    {
	bool operator()(NodeRef a, NodeRef b) const {

	    // now be cool and really fast :-)

	    if (a == b) return false;

	    std::list<NodeRef> parentsa;
	    while (a) {
		parentsa.push_front(a);
		a = a->parentNode();
	    }
	    std::list<NodeRef> parentsb;
	    while (b) {
		parentsb.push_front(b);
		b = b->parentNode();
	    }
	    std::list<NodeRef>::const_iterator ia = parentsa.begin();
	    std::list<NodeRef>::const_iterator ib = parentsb.begin();

	    while (ia != parentsa.end() && ib != parentsb.end() && (*ia == *ib)) { ++ia; ++ib;};
	    if (ia == parentsa.end()) {
		// a is ancestor of b
		return false;
	    }
	    if (ib != parentsb.end()) {
		// *ia shares a parent with *ib.
		for(NodeRef tmpNode = *ib; tmpNode; tmpNode = &*tmpNode->nextSibling()) {
		    if (tmpNode == *ia) {
			return false;
		    }
		}
	    }
	    return true;
	}
    };

    string string_value_for(const NodeRef);
    double number_value_for(const string &);
    inline double number_value_for(const NodeRef n) { return number_value_for(string_value_for(n)); };

    struct StringLess :
	public std::binary_function<const NodeRef, const NodeRef, bool>
    {
	bool operator()(const NodeRef a, const NodeRef b) const {
	    return string_value_for(a) < string_value_for(b);
	}
    };
    
    struct NumberLess :
	public std::binary_function<const NodeRef, const NodeRef, bool>
    {
	bool operator()(const NodeRef a, const NodeRef b) const {
	    return number_value_for(a) < number_value_for(b);
	}
    };

    typedef std::set<NodeRef, DocOrderLess> OrderedNodeSet;
    typedef OrderedNodeSet *OrderedNodeSetRef;
};
#endif
