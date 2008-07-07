//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "xpath_api.h"

#include <asl/string_functions.h>

#include "Expression.h"

namespace {

    using namespace xpath;

    const std::string TOKEN_AND = "and";
    const std::string TOKEN_OR = "or";

    int parseExpression(ExpressionPtr& e, const std::string &instring, int pos);
    int parseRelativePath(PathPtr& p, const std::string &instring, int pos);

    int
    parseArgumentList(Function::ArgumentList& arguments, const std::string &instring, int pos) {
        int nw_pos = pos;
        do {
            ExpressionPtr e;
            int nw_pos2;
            if ((nw_pos2 = parseExpression(e, instring, nw_pos)) != nw_pos) {
                arguments.push_back(e);
                nw_pos = asl::read_whitespace(instring, nw_pos2);
            }
        } while (instring[nw_pos++] == ',');

        if (instring[nw_pos-1] != ')') {
            AC_WARNING << "parse error at " << instring.substr(nw_pos-1);
            // ### parse error.
            return pos;
        }
        return nw_pos;
    }

    int
    parsePrimaryExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        if (instring[pos] == '(') {
            ++pos;
            int nw_pos = parseExpression(e, instring, pos);
            if ( nw_pos != pos) {
                nw_pos = asl::read_whitespace(instring, nw_pos);
                if (instring[nw_pos++] == ')') {
                    return nw_pos;
                }
            }
            AC_ERROR << "parse error at " << instring.substr(nw_pos-1) << " at " __FILE__ ":" << __LINE__;
            return pos;
        } else if ((instring[pos] == '\"') || (instring[pos] == '\'')) {
            const int nw_pos = asl::read_quoted_text(instring, pos, instring[pos], instring[pos]);
            if (nw_pos != pos) {
                std::string literalString = instring.substr(pos + 1, nw_pos - pos - 2);
                e = ExpressionPtr( new Literal(literalString) );
                return nw_pos;
            } else {
                AC_ERROR << "parse error at " __FILE__ ":" << __LINE__;
                return pos;
            }
        } else if (asl::is_digit(instring[pos])) {
            int nw_pos = pos;
            while ( asl::is_digit( instring[++nw_pos] ) );
            const std::string& numberstring = std::string( instring, pos, nw_pos );
            std::istringstream iss( numberstring );
            int num;
            iss >> num;
            assert(iss);
            e = ExpressionPtr( new Number(num) );
            assert(pos != nw_pos);
            return nw_pos;
        } else {
            const Function::FunctionType ft = Function::typeOf(instring, pos);
            if ( ft != Function::Unknown) {
                int nw_pos = asl::read_if_string(instring, pos, Function::nameOf(ft));
                nw_pos = asl::read_whitespace(instring, nw_pos);
                if (instring[nw_pos++] == '(') {
                    // function arguments
                    nw_pos = asl::read_whitespace(instring, nw_pos);
                    Function::ArgumentList arglist;
                    if (instring[nw_pos] != ')') {
                        nw_pos = parseArgumentList(arglist, instring, nw_pos);
                    } else {
                        nw_pos++;
                    }
                    e = ExpressionPtr( new Function(ft, arglist) );
                    return nw_pos;
                }
            }
        }

        PathPtr p( new Path() );
        const int nw_pos = parseRelativePath(p, instring, pos);
        if (nw_pos != pos) {
            pos = nw_pos;
            e = p;
        } else {
            AC_ERROR << "new path is " << *p;
            // ### parse error / not a primary expression
        }
        return pos;
    }

    int
    parseUnionExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parsePrimaryExpression(e, instring, pos)) != pos) {
            pos = asl::read_whitespace(instring, nw_pos);
            if (dynamic_cast_Ptr<Path>(e) && instring[pos] == '|') {

                SetExpressionPtr se( new SetExpression(SetExpression::Union) );
                e = se;
                se->appendPath(dynamic_cast_Ptr<Path>(e));

                while (instring[pos++] == '|') {
                    PathPtr p( new Path() );
                    if (nw_pos = parseRelativePath(p, instring, pos) != pos) {
                        pos = nw_pos;
                        se->appendPath(p);
                    }
                }
            }
        } else {
            pos = nw_pos;
        }
        return pos;
    }

    int
    parseUnaryExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        pos = asl::read_whitespace(instring, pos);
        UnaryExpression::OperatorType myType = UnaryExpression::Invalid;
        if (instring[pos] == '-') {
            myType = UnaryExpression::Minus;
            pos++;
        } else if (instring[pos] == '~') {
            myType = UnaryExpression::Tilde;
            pos++;
        } else if ((nw_pos = asl::read_if_string(instring, pos, "not")) != pos) {
            myType = UnaryExpression::Not;
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        if ((nw_pos = parseUnionExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
            if (myType != UnaryExpression::Invalid) {
                e = ExpressionPtr( new UnaryExpression(myType,e) );
            }
        }
        return pos;
    }

    int
    parseMultExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseUnaryExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        BinaryExpression::ExpressionType myType;
        if (instring[pos] == '*') {
            myType = BinaryExpression::Times;
            pos++;
        } else if ((nw_pos = asl::read_if_string(instring, pos, "div")) != pos) {
            myType = BinaryExpression::Div;
            pos = nw_pos;
        } else if ((nw_pos = asl::read_if_string(instring, pos, "mod")) != pos) {
            myType = BinaryExpression::Mod;
            pos = nw_pos;
        } else {
            return pos;
        };
        pos = asl::read_whitespace(instring, pos);

        ExpressionPtr secondE;
        pos = parseMultExpression(secondE, instring, pos);
        e = ExpressionPtr( new BinaryExpression(myType, e, secondE) );
        return pos;
    }

    int
    parseAddExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseMultExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        BinaryExpression::ExpressionType myType;
        if (instring[pos] == '+') {
            myType = BinaryExpression::Plus;
        } else if (instring[pos] == '-') {
            myType = BinaryExpression::Minus;
        } else {
            return pos;
        }
        pos++;
        ExpressionPtr secondE;
        pos = parseAddExpression(secondE, instring, pos);
        e = ExpressionPtr( new BinaryExpression(myType, e, secondE) );
        return pos;
    }

    int
    parseRelationalExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseAddExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        BinaryExpression::ExpressionType myType;
        if (instring[pos] == '>') {
            myType = instring[pos+1] == '=' ? BinaryExpression::GEqual : BinaryExpression::Greater;
        } else if(instring[pos] == '<') {
            myType = instring[pos+1] == '=' ? BinaryExpression::LEqual : BinaryExpression::Less;
        } else {
            return pos;
        }
        pos += (instring[pos+1]=='='?2:1);
        ExpressionPtr secondE;
        pos = parseRelationalExpression(secondE, instring, pos);
        e = ExpressionPtr( new BinaryExpression(myType, e, secondE) );
        return pos;
    }

    int
    parseEqualityExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseRelationalExpression(e, instring, pos)) != pos) {
        } else {
            AC_ERROR << "parse error for Relational Expression " << instring.substr(pos);
            e = ExpressionPtr();
            return pos;
        }
        nw_pos = asl::read_whitespace(instring, nw_pos);
        BinaryExpression::ExpressionType op;
        if (instring[nw_pos] == '=') {
            op = BinaryExpression::Equal;
            nw_pos++;
            nw_pos = asl::read_whitespace(instring, nw_pos);
        } else if (instring[nw_pos] == '!' && instring[nw_pos+1] == '=') {
            op = BinaryExpression::NotEqual;
            nw_pos+=2;
            nw_pos = asl::read_whitespace(instring, nw_pos);
        } else {
            return nw_pos;
        }

        ExpressionPtr secondE;
        int nw_pos2 = parseEqualityExpression(secondE, instring, nw_pos);
        if (nw_pos2 != nw_pos) {
            e = ExpressionPtr( new BinaryExpression(op, e, secondE) );
            return nw_pos2;
        } else {
            AC_ERROR << "XPath parse error.";
            return nw_pos;
        }
    }

    int
    parseAndExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseEqualityExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        if (instring.compare(pos, TOKEN_AND.length(), TOKEN_AND) == 0) {
            pos += TOKEN_AND.length();
            pos = asl::read_whitespace(instring, pos);

            ExpressionPtr secondE;
            pos = parseAndExpression(secondE, instring, pos);
            e = ExpressionPtr( new BinaryExpression(BinaryExpression::And, e, secondE) );
        } 
        return pos;
    }

    int
    parseOrExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        int nw_pos;
        if ((nw_pos = parseAndExpression(e, instring, pos)) != pos) {
            pos = nw_pos;
        }
        pos = asl::read_whitespace(instring, pos);
        if (instring.compare(pos, TOKEN_OR.length(), TOKEN_OR) == 0) {
            pos += TOKEN_OR.length();
            pos = asl::read_whitespace(instring, pos);

            ExpressionPtr secondE;
            pos = parseOrExpression(secondE, instring, pos);
            e = ExpressionPtr( new BinaryExpression(BinaryExpression::Or, e, secondE) );
        }
        return pos;
    }

    int
    parseExpression(ExpressionPtr& e, const std::string &instring, int pos) {
        return parseOrExpression(e, instring, pos);
    }

    int
    parsePredicates(Step &s, const std::string &instring, int pos) {
        int nw_pos = pos;
        while (instring[nw_pos] == '[') {
            ExpressionPtr e;
            nw_pos = parseExpression(e, instring, nw_pos+1);

            if (dynamic_cast_Ptr<xpath::Number>(e)) {
                s.appendPredicate( ExpressionPtr( new BinaryExpression( BinaryExpression::Equal
                                                                      , ExpressionPtr(new Function(Function::Position))
                                                                      , e ) ) );
            } else {
                s.appendPredicate(e);
            }

            if (instring[nw_pos++] != ']') {
                AC_WARNING << " XPath parse error: could not read predicate at " << instring.substr(pos);
                return pos;
            }
        }
        return nw_pos;
    }

    int
    parseNodeTest(Step &s, const std::string &instring, int pos) {
        int nw_pos;
        if (instring[pos] == '*') {
            s.setNodeTest(Step::TestPrincipalType);
            return pos+1;
        } else if ((nw_pos = asl::read_name(instring, pos)) != pos) {
            if (instring[nw_pos] == '(' && instring[nw_pos+1] == ')') {
                // it's supposed to be a nodeType test because it's
                // followed by an empty pair of braces
                Step::NodeTest myTest = Step::read_NodeTest(instring, pos);
                s.setNodeTest(myTest);
                pos = nw_pos + 2;
            } else { // it's a name test on the principal node type
                std::string nodeNameString = instring.substr(pos, nw_pos - pos);
                s.setNodeTest(Step::TestPrincipalType, nodeNameString);
                pos = nw_pos;
            }
            return pos;
        } else { // not a node test
            return pos;
        }
    }

    int
    parseStep(PathPtr& p, const std::string &instring, int pos) {
        Step::Axis a = Step::Invalid;
        StepPtr s;
        int nw_pos;
        if (instring[pos] == '.') {

            if (instring[pos+1] == '.') { // ".." as shorthand for "parent::node()"
                a = Step::Parent;
                nw_pos = pos + 2;
            } else { // "." as shorthand for "self::node()"
                a = Step::Self;
                nw_pos = pos + 1;
            }
            s = StepPtr( new Step(a, Step::TestPrincipalType) );
        } else {
            if (instring[pos] == '@') {
                // shorthand for "attribute ::".
                // name test and predicates will follow.
                a = Step::Attribute;
                nw_pos = pos + 1;
            } else if ((a = Step::read_axis(instring, pos)) != Step::Invalid) {
                AC_TRACE << " current pos = " << pos;
                nw_pos = asl::read_if_string(instring, pos, Step::stringForAxis(a));

                if (nw_pos == pos) {
                    AC_ERROR << " internal XPath parsing error: could not read step";
                    // internal consistency error
                    return pos;
                }

                if (instring[nw_pos] == ':' && instring[nw_pos+1] == ':') {
                    nw_pos += 2;
                } else {
                    AC_WARNING << " XPath parse error.";
                    return pos;
                }

            } else {
                // if axis and "::" are omitted,
                // the default axis "child" is used.
                // node test and predicates may follow.
                a = Step::Child;
                nw_pos = pos;
            }
            assert(a != Step::Invalid);
            s = StepPtr( new Step(a, Step::TestPrincipalType) );

            int nw_pos2 = parseNodeTest(*s, instring, nw_pos);
            if (nw_pos2 == nw_pos) {
                AC_WARNING << " XPath parse error.";
                return pos;
            }
            nw_pos = nw_pos2;
        }

        pos = parsePredicates(*s, instring, nw_pos);

        p->appendStep(s);
        return pos;
    }

    int
    parseRelativePath(PathPtr& p, const std::string &instring, int pos) {
        int nw_pos = parseStep(p, instring, pos);
        if (nw_pos == pos) {
            AC_WARNING << " XPath parse error.";
            return pos;
        }
        while (instring[nw_pos] == '/') {
            if (instring[nw_pos+1] == '/') {
                p->appendStep( StepPtr(new Step(Step::Descendant_Or_Self, Step::TestAnyNode)) );
                nw_pos += 2;
            } else {
                nw_pos++;
            }
            int nw_pos2 = parseStep(p, instring, nw_pos);
            if (nw_pos2 == nw_pos) {
                AC_ERROR << " parse error.";
                return pos;
            }
            nw_pos = nw_pos2;
        }
        return nw_pos;
    }

    int
    parsePath(PathPtr& p, const std::string &instring, int pos) {
        int nw_pos = pos;
        if (instring[pos] == '/') {
            if (instring[pos+1] == '/') {
                p->appendStep( StepPtr(new Step(Step::Descendant_Or_Self, Step::TestAnyNode)) );
                nw_pos+=2;
            } else {
                p->setAbsolute();
                nw_pos++;
            }
        }
        int retval;
        if ((retval = parseRelativePath(p, instring, nw_pos)) == nw_pos) {
            AC_WARNING << " XPath parse error.";
            return pos;
        }
        return retval;

    }
}

namespace xpath {

    PathPtr xpath_parse(const std::string &instring) {
        PathPtr p( new Path() );
        AC_DEBUG << "parsing path " << instring;
        if (parsePath(p, instring, 0) == 0) {
            // parse error
            AC_WARNING << "parse error. Intermediate result=" << *p;
            return PathPtr();
        }
        AC_DEBUG << "parsing result = " << *p;
        return p;
    }

    dom::NodePtr xpath_evaluate1(const PathPtr p, const dom::NodePtr theNode) {
        OrderedNodeSetPtr retval = p->evaluateAs<OrderedNodeSet>(theNode);
        if (!retval->empty()) {
            return *(retval->begin());
        }
        return dom::NodePtr();
    }

    NodeVectorPtr xpath_evaluate(PathPtr p, dom::NodePtr theNode) {
        return p->evaluateAs<NodeVector>(theNode);
    }

    NodeVectorPtr xpath_evaluate(const std::string& path, dom::NodePtr theNode) {
        PathPtr myPath = xpath_parse(path);
        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NodeVectorPtr();
        }
        return xpath_evaluate(myPath, theNode);
    }

    NodeSetPtr xpath_evaluateSet(PathPtr myPath, dom::NodePtr theNode) {
        return myPath->evaluate(theNode)->takeNodes();
    }

    NodeSetPtr xpath_evaluateSet(const std::string& path, dom::NodePtr theNode) {
        PathPtr myPath = xpath_parse(path);
        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NodeSetPtr();
        }
        return xpath_evaluateSet(myPath, theNode);
    }

    OrderedNodeSetPtr xpath_evaluateOrderedSet(PathPtr myPath, dom::NodePtr theNode) {
        return myPath->evaluateAs<OrderedNodeSet>(theNode);
    }

    OrderedNodeSetPtr xpath_evaluateOrderedSet(const std::string& path, dom::NodePtr theNode) {
        PathPtr myPath = xpath_parse(path);
        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return OrderedNodeSetPtr();
        }
        return xpath_evaluateOrderedSet(myPath, theNode);
    }

    void xpath_evaluate(PathPtr p, dom::NodePtr startingElement, NodeVector& results) {
        NodeVectorPtr retval = xpath_evaluate(p, startingElement);
        for (NodeVector::const_iterator i = retval->begin(); i != retval->end(); ++i) {
            results.push_back(*i);
        }
    }

    void xpath_evaluate(const std::string& p, const dom::NodePtr startingElement, NodeVector& results) {
        NodeVectorPtr retval = xpath_evaluate(p, startingElement);
        for (NodeVector::const_iterator i = retval->begin(); i != retval->end(); ++i) {
            results.push_back(*i);
        }
    }

   NodeVectorPtr findAll(dom::Node & theRoot, const std::string & theExpression) {
        NodeVectorPtr myResults( new NodeVector );
        xpath::PathPtr myPath = xpath::xpath_parse(theExpression);
        if (!myPath) {
            AC_ERROR << "xpath could not parse '"<< theExpression<< "'";
        } else {
            xpath::xpath_evaluate(myPath, theRoot.self().lock(), *myResults);
        }
        return myResults;
    }

    dom::NodePtr find(dom::Node & theRoot, const std::string & theExpression) {
        dom::NodePtr myResult;
        xpath::PathPtr myPath = xpath::xpath_parse(theExpression);
        if (!myPath) {
            AC_ERROR << "xpath could not parse '"<< theExpression<< "'";
        } else {
            dom::NodePtr myResultNode = xpath::xpath_evaluate1(myPath, theRoot.self().lock());
            if (myResultNode) {
                myResult = myResultNode->self().lock();
            }
        }
        return myResult;
    }

}
