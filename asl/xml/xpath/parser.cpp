//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <asl/string_functions.h>
#include "Expression.h"

namespace xpath {

    int parseExpression(Expression **e, const std::string &instring, int pos);
    int parseRelativePath(Path *p, const std::string &instring, int pos);

    int parseArgumentList(std::list<Expression*> *arguments, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseArgumentList at " << instring.substr(pos);
#endif
	int nw_pos = pos;
	do {
	    Expression *e = 0;
	    int nw_pos2;
	    if ((nw_pos2 = parseExpression(&e, instring, nw_pos)) != nw_pos) {
		arguments->push_back(e);
		nw_pos = asl::read_whitespace(instring, nw_pos2);
	    }
	} while (instring[nw_pos++] == ',');

	if (instring[nw_pos-1] != ')') {
	    AC_WARNING << "parse error at " << instring.substr(nw_pos-1);
	    // ### parse error.
	    return pos;
	}
#if PARSER_DEBUG_VERBOSITY > 1
	AC_TRACE << "finished parsing argument list at " << instring.substr(pos);
#endif
	return nw_pos;
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
		nw_pos = asl::read_whitespace(instring, nw_pos);
		if (instring[nw_pos++] == ')') {
		    return nw_pos;
		}
	    }
	    AC_WARNING << "parse error at " << instring.substr(nw_pos-1) << " at " __FILE__ ":" << __LINE__;
	    return pos;
	} else if ((instring[pos] == '\"') || (instring[pos] == '\'')) {
	    if ((nw_pos = asl::read_quoted_text(instring, pos, instring[pos], instring[pos])) != pos) {
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
	    assert(pos != nw_pos);
	    return nw_pos;
	} else if ((ft = Function::typeOf(instring, pos)) != Function::Unknown) {
	    std::list<Expression*> *arglist = new std::list<Expression*>();
	    nw_pos = asl::read_if_string(instring, pos, Function::nameOf(ft));
	    nw_pos = asl::read_whitespace(instring, nw_pos);
	    if (instring[nw_pos++] == '(') {
		// function arguments
		nw_pos = parseArgumentList(arglist, instring, nw_pos);
		*e = new Function(ft, arglist);
		return nw_pos;
	    } else {
		delete arglist;
	    }
	}

	Path *p = new Path();
	nw_pos = parseRelativePath(p, instring, pos);
	if (nw_pos != pos) {
	    pos = nw_pos;
	    *e = p;
	} else {
	    AC_ERROR << "new path is " << *p;
	    delete p;
	    // ### parse error / not a primary expression
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "no expression at " << instring.substr(pos);
#endif
	}
	return pos;
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
	    AC_TRACE << "parsing success for Relational Expression before " << instring.substr(nw_pos);
#endif
	} else {
	    AC_WARNING << "parse error for Relational Expression " << instring.substr(pos);
	    delete *e;
	    return pos;
	}
	nw_pos = asl::read_whitespace(instring, nw_pos);
	BinaryExpression::ExpressionType op;
	if (instring[nw_pos] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(nw_pos);
#endif
	    op = BinaryExpression::Equal;
	    nw_pos++;
	    nw_pos = asl::read_whitespace(instring, nw_pos);
	} else if (instring[nw_pos] == '!' && instring[nw_pos+1] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(pos);
#endif
	    op = BinaryExpression::NotEqual;
	    nw_pos+=2;
	    nw_pos = asl::read_whitespace(instring, nw_pos);
	} else {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "no further parse EqualityExpression at " << instring.substr(pos);
#endif
	    return nw_pos;
	}

	Expression *secondE;
	int nw_pos2 = parseEqualityExpression(&secondE, instring, nw_pos);
	if (nw_pos2 != nw_pos) {
	    *e = new BinaryExpression(op, *e, secondE);
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "created equality expression " << **e;
#endif
	    return nw_pos2;
	} else {
	    AC_WARNING << "XPath parse error.";
	    delete secondE;
	    return nw_pos;
	}
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
	int nw_pos = pos;
	while (instring[nw_pos] == '[') {
	    Expression *e;
	    nw_pos = parseExpression(&e, instring, nw_pos+1);

	    if (dynamic_cast<xpath::Number*>(e)) {
		s.appendPredicate(new BinaryExpression(BinaryExpression::Equal, new Function(Function::Position, new std::list<xpath::Expression*>()), e));
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
	int nw_pos;
	if (instring[pos] == '.') {

            if (instring[pos+1] == '.') { // ".." as shorthand for "parent::node()"
		a = Step::Parent;
		nw_pos = pos + 2;
	    } else { // "." as shorthand for "self::node()"
		a = Step::Self;
		nw_pos = pos + 1;
	    }
	    s = new Step(a, Step::TestPrincipalType);
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
	    s = new Step(a, Step::TestPrincipalType);

	    int nw_pos2 = parseNodeTest(*s, instring, nw_pos);
	    if (nw_pos2 == nw_pos) {
		delete s;
		AC_WARNING << " XPath parse error.";
		return pos;
	    }
	    nw_pos = nw_pos2;
	}

	pos = parsePredicates(*s, instring, nw_pos);
#if PARSER_DEBUG_VERBOSITY > 1
	AC_TRACE << "parseStep done at " << instring.substr(pos) << " into " << *s;
#endif

	p->appendStep(s);
#if PARSER_DEBUG_VERBOSITY > 1
	AC_TRACE << "path is now " << *p;
#endif
	return pos;
    }

    int parseRelativePath(Path *p, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parseRelativePath at " << instring.substr(pos);
#endif
	int nw_pos = parseStep(p, instring, pos);
	if (nw_pos == pos) {
	    AC_WARNING << " XPath parse error.";
	    return pos;
	}
	while (instring[nw_pos] == '/') {
	    if (instring[nw_pos+1] == '/') {
		p->appendStep(new Step(Step::Descendant_Or_Self, Step::TestAnyNode));
		nw_pos += 2;
	    } else {
		nw_pos++;
	    };
	    int nw_pos2 = parseStep(p, instring, nw_pos);
	    if (nw_pos2 == nw_pos) {
		AC_WARNING << " parse error.";
		return pos;
	    }
	    nw_pos = nw_pos2;
	}
	return nw_pos;
    }

    int parsePath(Path *p, const std::string &instring, int pos) {
#ifdef DEBUG_PARSER_STATES
	AC_TRACE << "parsePath at " << instring.substr(pos);
#endif
	int nw_pos = pos;
        if (instring[pos] == '/') {
	    if (instring[pos+1] == '/') {
		p->appendStep(new Step(Step::Descendant_Or_Self, Step::TestAnyNode));
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
}; // namespace xpath
