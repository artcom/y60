#ifndef XPATH_TYPES_H
#define XPATH_TYPES_H

#ifdef DEBUG
#define INTERPRETER_DEBUG
#define DEBUG_PARSER_STATES
#define PARSER_DEBUG_VERBOSITY 2
#endif

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
	    if (ib == parentsb.end()) {
		// since a != b,
		// b is an ancestor of a.
		return false;
	    }
	    if (ia != parentsa.end()) {
		// *ia shares a parent with *ib.
		for(NodeRef tmpNode = *ib; tmpNode; tmpNode = &*tmpNode->nextSibling()) {
		    if (tmpNode == *ia) {
			// b is before a
#ifdef DEBUG
			AC_TRACE << **ib << " is before " << **ia;
#endif
			return false;
		    }
		}
	    }
	    // a != b, b is not an ancestor of a, b is not before a:
	    // b is descendant of a OR b is before a.
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
