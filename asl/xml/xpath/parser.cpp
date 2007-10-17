#include "context.h"
#include "syntaxtree.h"

#include "parser.h"

#include <asl/string_functions.h>

//#define DEBUG_PARSER_STATES
//#define PARSER_DEBUG_VERBOSITY 2

namespace xpath {

    int parseExpression(Expression **e, const std::string &instring, int pos);
    int parseRelativePath(Path *p, const std::string &instring, int pos);

    int parseArgumentList(std::list<Expression*> *arguments, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseArgumentList at " << instring.substr(pos);
#endif
	do {
	    Expression *e = 0;
	    int nw_pos = parseExpression(&e, instring, pos);
	    if (nw_pos != pos) {
		arguments->push_back(e);
		pos = asl::read_whitespace(instring, pos);
	    }
	} while (instring[pos++] == ',');

	if (instring[pos++] != ')') {
	    // ### parse error.
	}
	return pos;
    }

    int parsePrimaryExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parsePrimaryExpression at " << instring.substr(pos);
#endif
	Function::FunctionType ft;
	int nw_pos;

	if (instring[pos] == '(') {
	    pos++;
	    if ((nw_pos = parseExpression(e, instring, pos)) != pos) {
		if (instring[nw_pos+1] == ')') {
		    return nw_pos+1;
		}
	    }
	    AC_WARNING << "parse error at " __FILE__ ":" << __LINE__;
	    return pos;
	} else if (instring[pos] == '\"') {
	    if ((nw_pos = asl::read_quoted_text(instring, pos, '\"', '\"')) != pos) {
#if PARSER_DEBUG_VERBOSITY > 1
		AC_TRACE << "pos = " << pos << " nw_pos = " << nw_pos;
#endif
		std::string literalString = instring.substr(pos + 1, nw_pos - pos - 2);
#if PARSER_DEBUG_VERBOSITY > 1
		AC_TRACE << "found literal \"" << literalString << "\"";
#endif
		*e = new Literal(literalString);
		return nw_pos;
	    } else {
		AC_WARNING << "parse error at " __FILE__ ":" << __LINE__;
		return pos;
	    }
	} else if (asl::is_digit(instring[pos])) {
	    nw_pos = pos;
	    while (asl::is_digit(instring[++nw_pos]));
	    string numberstring = string(instring, pos, nw_pos);
	    std::istringstream iss(numberstring);
	    int num;
	    iss >> num;
	    *e = new Number(num);
	    return nw_pos;
	} else if ((ft = Function::typeOf(instring, pos)) != Function::Unknown) {
	    std::list<Expression*> *arglist = new std::list<Expression*>();
	    pos = asl::read_if_string(instring, pos, Function::nameOf(ft));
	    pos = asl::read_whitespace(instring, pos);
	    if (instring[pos] == '(') {
		// function arguments
		pos = parseArgumentList(arglist, instring, pos);
		*e = new Function(ft, arglist);
		return pos;
	    }
	    AC_WARNING << "parse error at " __FILE__ ":" << __LINE__;
	    return pos;
	} else {
	    Path *p = new Path();
	    nw_pos = parseRelativePath(p, instring, pos);
	    if (nw_pos != pos) {
		pos = nw_pos;
		*e = p;
	    } else {
		// ### parse error / not a primary expression
		AC_WARNING << "parse error at " __FILE__ ":" << __LINE__;
	    }
	    return pos;
	}
    }

    int parseUnionExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseUnionExpression at " << instring.substr(pos);
#endif
	int nw_pos;
	if ((nw_pos = parsePrimaryExpression(e, instring, pos)) != pos) {
	    pos = asl::read_whitespace(instring, nw_pos);
	    if (dynamic_cast<Path*>(*e) && instring[pos] == '|') {

		SetExpression *se = new SetExpression(SetExpression::Union);
		*e = se;
		se->appendPath(dynamic_cast<Path*>(*e));

		while (instring[pos++] == '|') {
		    Path *p = new Path();
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

    int parseUnaryExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseUnaryExpression at " << instring.substr(pos);
#endif
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
	};
	pos = asl::read_whitespace(instring, pos);
	if ((nw_pos = parseUnionExpression(e, instring, pos)) != pos) {
	    pos = nw_pos;
	    if (myType != UnaryExpression::Invalid)
		*e = new UnaryExpression(myType, *e);
	}
	return pos;
    }

    int parseMultExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseMultExpression at " << instring.substr(pos);
#endif
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

	Expression *secondE;
	pos = parseMultExpression(&secondE, instring, pos);
	*e = new BinaryExpression(myType, *e, secondE);
	return pos;
    }

    int parseAddExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseAddExpression at " << instring.substr(pos);
#endif
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
	Expression *secondE;
	pos = parseAddExpression(&secondE, instring, pos);
	*e = new BinaryExpression(myType, *e, secondE);
	return pos;
    }

   int parseRelationalExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseRelationalExpression at " << instring.substr(pos);
#endif
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
	Expression *secondE;
	pos = parseRelationalExpression(&secondE, instring, pos);
	*e = new BinaryExpression(myType, *e, secondE);
	return pos;
    }

    int parseEqualityExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseEqualityExpression at " << instring.substr(pos);
#endif
	int nw_pos;
	if ((nw_pos = parseRelationalExpression(e, instring, pos)) != pos) {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "parsing success for Relational Expression before " << instring.substr(pos);
#endif
	    pos = nw_pos;
	}
	pos = asl::read_whitespace(instring, pos);
	if (instring[pos] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(pos);
#endif
	    pos++;
	    pos = asl::read_whitespace(instring, pos);
	} else if (instring[pos] == '!' && instring[pos+1] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(pos);
#endif
	    pos+=2;
	    pos = asl::read_whitespace(instring, pos);
	} else {
	    return pos;
	}

	Expression *secondE;
	pos = parseEqualityExpression(&secondE, instring, pos);
	*e = new BinaryExpression(BinaryExpression::Equal, *e, secondE);
#if PARSER_DEBUG_VERBOSITY > 1
	AC_TRACE << "created equality expression " << **e;
#endif
	return pos;
    }

    const std::string TOKEN_AND = "and";

    int parseAndExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseAndExpression at " << instring.substr(pos);
#endif
	int nw_pos;
	if ((nw_pos = parseEqualityExpression(e, instring, pos)) != pos) {
	    pos = nw_pos;
	}
	pos = asl::read_whitespace(instring, pos);
	if (!(instring.compare(TOKEN_AND), pos, TOKEN_AND.length())) {
	    pos += TOKEN_AND.length();
	    pos = asl::read_whitespace(instring, pos);

	    Expression *secondE;
	    pos = parseAndExpression(&secondE, instring, pos);
	    *e = new BinaryExpression(BinaryExpression::And, *e, secondE);
	}
	return pos;
    }

    const std::string TOKEN_OR = "or";

    int parseOrExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseOrExpression at " << instring.substr(pos);
#endif
	int nw_pos;
	if ((nw_pos = parseAndExpression(e, instring, pos)) != pos) {
	    pos = nw_pos;
	}
	pos = asl::read_whitespace(instring, pos);
	if (!(instring.compare(TOKEN_OR), pos, TOKEN_OR.length())) {
	    pos += TOKEN_OR.length();
	    pos = asl::read_whitespace(instring, pos);

	    Expression *secondE;
	    pos = parseOrExpression(&secondE, instring, pos);
	    *e = new BinaryExpression(BinaryExpression::Or, *e, secondE);
	}
	return pos;
    }

    int parseExpression(Expression **e, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseExpression at " << instring.substr(pos);
#endif
	return parseOrExpression(e, instring, pos);
    }

    int parsePredicates(Step &s, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parsePredicates at " << instring.substr(pos);
#endif
	while (instring[pos] == '[') {
	    Expression *e;
	    pos = parseExpression(&e, instring, pos+1);

	    if (dynamic_cast<xpath::Number*>(e)) {
		s.appendPredicate(new BinaryExpression(BinaryExpression::Equal, new Function(Function::Position, new std::list<xpath::Expression*>()), e));
	    } else {
                s.appendPredicate(e);
	    }

	    if (instring[pos++] != ']') {
		// XXX parse error
	    }
	}
	return pos;
    }

    int parseNodeTest(Step &s, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseNodeTest at " << instring.substr(pos);
#endif
	int nw_pos;
	if (instring[pos] == '*') {
	    s.setNodeTest(Step::TestPrincipalType);
	    return pos+1;
	} else if ((nw_pos = asl::read_name(instring, pos)) != pos) {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "parseNodeTest: parsed name test " << instring.substr(pos, nw_pos - pos);
#endif
	    if (instring[nw_pos] == '(' && instring[nw_pos+1] == ')') {
		// it's supposed to be a nodeType test because it's
		// followed by an empty pair of braces
		Step::NodeTest myTest = Step::read_NodeTest(instring, pos);
		s.setNodeTest(myTest);
#if PARSER_DEBUG_VERBOSITY > 1
		AC_TRACE << "parsed node type test into step " << s;
#endif
		pos = nw_pos + 2;
	    } else { // it's a name test on the principal node type
		string nodeNameString = instring.substr(pos, nw_pos - pos);
		s.setNodeTest(Step::TestPrincipalType, nodeNameString);
		pos = nw_pos;
	    }
	    return pos;
	} else { // not a node test
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "parseNodeTest: couldn't parse node test here!";
#endif
	    return pos;
	}
    }

    int parseStep(Path *p, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseStep at " << instring.substr(pos);
#endif
	Step::Axis a = Step::Invalid;
	Step *s;
	if (instring[pos] == '.') {
            if (instring[pos+1] == '.') { // ".." as shorthand for "parent::node()"
		a = Step::Parent;
		pos+=2;
	    } else { // "." as shorthand for "self::node()"
		a = Step::Self;
		pos++;
	    }
	    s = new Step(a, Step::TestPrincipalType);
	} else {
	    if ((a = Step::read_axis(instring, pos)) != Step::Invalid) {
		int nw_pos = asl::read_if_string(instring, pos, Step::stringForAxis(a));
		if (instring[nw_pos] == ':' && instring[nw_pos+1] == ':') {
		    pos = nw_pos + 2;
		} else {
		    // if axis and "::" are omitted,
		    // the default axis "child" is used.
		    // node test and predicates may follow.
		    a = Step::Child;
		    pos = nw_pos;
		}
	    } else if (instring[pos] == '@') {
		// shorthand for "attribute ::".
		// name test and predicates will follow.
		a = Step::Attribute;
		pos++;
	    } else {
		a = Step::Child;
	    }
	    assert(a != Step::Invalid);
	    s = new Step(a, Step::TestPrincipalType);
	    // parse "nodetest predicates"
	    pos = parseNodeTest(*s, instring, pos);
	    pos = parsePredicates(*s, instring, pos);
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "parseStep done at " << instring.substr(pos) << " into " << *s;
#endif
	}
	p->appendStep(s);
#if PARSER_DEBUG_VERBOSITY > 1
	AC_TRACE << "path is now "; << *p;
#endif
	return pos;
    }

    int parseRelativePath(Path *p, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseRelativePath at " << instring.substr(pos);
#endif
	pos = parseStep(p, instring, pos);
	while (instring[pos] == '/') {
	    if (instring[pos+1] == '/') {
		p->appendStep(new Step(Step::Descendant_Or_Self, Step::TestAnyNode));
		pos += 2;
	    } else {
		pos++;
	    };
	    pos = parseStep(p, instring, pos);
	}
	return pos;
    }

    int parsePath(Path *p, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parsePath at " << instring.substr(pos);
#endif
        if (instring[pos] == '/') {
	    if (instring[pos+1] == '/') {
		p->appendStep(new Step(Step::Descendant_Or_Self, Step::TestAnyNode));
		pos+=2;
	    } else {
                p->setAbsolute();
		pos++;
	    }
	}
	return parseRelativePath(p, instring, pos);
    }

    Path *xpath_parse(const std::string &instring) {
	Path *p = new Path();
        parsePath(p, instring, 0);
	return p;
    }

    std::set<dom::Node *> *xpath_evaluate(Path *p, dom::Node *theNode) {
	xpath::NodeSetValue *value = p->evaluate(theNode);
	std::set<dom::Node *> *retval = value->takeNodes();
	delete value;
	return retval;
    }

    std::set<dom::Node *> *xpath_evaluate(std::string path, dom::Node *theNode) {
        xpath::Path *myPath = xpath_parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
	    NodeSetRef retval = xpath_evaluate(myPath, theNode);
            delete myPath;
            return retval;
	}
    }

    void xpath_return(Path *p) {
	delete p;
    }

    void xpath_evaluate(Path *p, dom::Node *startingElement, std::vector<dom::NodePtr> &results) {

	xpath::NodeSetValue *value = p->evaluate(startingElement);
	std::set<dom::Node *> *retval = value->takeNodes();

	for (xpath::NodeSet::iterator i = retval->begin(); i != retval->end(); ++i) {
	    results.push_back((*i)->self().lock());
	}

	delete value;
    }

}
