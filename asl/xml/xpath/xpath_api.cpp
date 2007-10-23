#include "Context.h"
#include "Expression.h"

#include "xpath_api.h"

#define DEBUG_RESULTS

namespace xpath {

    // defined in parser.cpp, for which no Header file exists.
    int parsePath(Path *p, const std::string &instring, int pos);

    Path *xpath_parse(const std::string &instring) {
	Path *p = new Path();
	AC_INFO << "parsing path " << instring;
        if (parsePath(p, instring, 0) == 0) {
	    // parse error
	    AC_INFO << "parse error. Intermediate result=" << *p;
	    delete p;
	    return NULL;
	}
	AC_INFO << "parsing result = " << *p;
	return p;
    }

    dom::Node *xpath_evaluate1(Path *p, dom::Node *theNode) {

	OrderedNodeSetRef retval = p->evaluateAs<OrderedNodeSet>(theNode);

#ifdef DEBUG_RESULTS
	    AC_INFO << "evaluated path contains " << retval->size() << " nodes.";

            for (OrderedNodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
		try {
		    AC_TRACE << " * " << (*i)->nodeName() << " "
                             << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
		    if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
			for (int j = 0; j < (*i)->attributesLength(); j++) {
			    AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
			}
		    }
		} catch(asl::Exception &e) {
		    AC_TRACE << " oops...";
		}
	    }
#endif
	if (retval->begin() != retval->end()) {
	    NodeRef retval1 = *(retval->begin());
	    delete retval;
	    return retval1;
	} else {
	    delete retval;
	    return NULL;
	}
    }

    std::vector<dom::Node *> *xpath_evaluate(Path *p, dom::Node *theNode) {
	xpath::NodeListRef retval = p->evaluateAs<NodeList>(theNode);

#ifdef DEBUG_RESULTS
	    AC_INFO << "evaluated path contains " << retval->size() << " nodes.";

            for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
		try {
		    AC_TRACE << " * " << (*i)->nodeName() << " "
                             << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
		    if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
			for (int j = 0; j < (*i)->attributesLength(); j++) {
			    AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
			}
		    }
		} catch(asl::Exception &e) {
		    AC_TRACE << " oops...";
		}
	    }
#endif
	return retval;
    }

    std::vector<dom::Node *> *xpath_evaluate(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
	    NodeListRef retval = xpath_evaluate(myPath, theNode);
            delete myPath;
            return retval;
	}
    }

    std::set<dom::Node *> *xpath_evaluateSet(xpath::Path *myPath, dom::Node *theNode) {

	NodeSetValue *retValue = myPath->evaluate(theNode);
	NodeSetRef retval = retValue->takeNodes();
#ifdef DEBUG_RESULTS
	    AC_INFO << "evaluated path contains " << retval->size() << " nodes.";

            for (NodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
		try {
		    AC_TRACE << " * " << (*i)->nodeName() << " "
                             << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
		    if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
			for (int j = 0; j < (*i)->attributesLength(); j++) {
			    AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
			}
		    }
		} catch(asl::Exception &e) {
		    AC_TRACE << " oops...";
		}
	    }
#endif
	delete retValue;
	return retval;
    }

    std::set<dom::Node *> *xpath_evaluateSet(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
	    NodeSetRef retval = xpath_evaluateSet(myPath, theNode);
            delete myPath;
            return retval;
	}
    }

    OrderedNodeSetRef xpath_evaluateOrderedSet(xpath::Path *myPath, dom::Node *theNode) {
        OrderedNodeSetRef retval = myPath->evaluateAs<OrderedNodeSet>(theNode);
#ifdef DEBUG_RESULTS
        AC_INFO << "evaluated path contains " << retval->size() << " nodes.";

        for (OrderedNodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
            try {
                AC_TRACE << " * " << (*i)->nodeName() << " "
                         << ((*i)->nodeType() == dom::Node::TEXT_NODE ? (*i)->nodeValue():"");
                if ((*i)->nodeType() == dom::Node::ELEMENT_NODE) {
                    for (int j = 0; j < (*i)->attributesLength(); j++) {
                        AC_TRACE << "   " << (*i)->getAttribute(j)->nodeName() << "=" << (*i)->getAttribute(j)->nodeValue();
                    }
                }
            } catch(asl::Exception &e) {
                AC_TRACE << " oops...";
            }
        }
#endif
        return retval;
    }

    OrderedNodeSetRef xpath_evaluateOrderedSet(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
	    OrderedNodeSetRef retval = xpath_evaluateOrderedSet(myPath, theNode);
            delete myPath;
            return retval;
	}
    }

    void xpath_return(Path *p) {
	delete p;
    }

    void xpath_evaluate(Path *p, dom::Node *startingElement, std::vector<dom::NodePtr> &results) {

	std::vector<dom::Node *> *retval = xpath_evaluate(p, startingElement);

	for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
	    results.push_back((*i)->self().lock());
	}

	delete retval;
    }

    void xpath_evaluate(std::string p, dom::Node *startingElement, std::vector<dom::NodePtr> &results) {

	std::vector<dom::Node *> *retval = xpath_evaluate(p, startingElement);

	for (std::vector<dom::Node *>::iterator i = retval->begin(); i != retval->end(); ++i) {
	    results.push_back((*i)->self().lock());
	}

	delete retval;
    }

}
