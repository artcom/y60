#include "context.h"
#include "syntaxtree.h"

#include "parser.h"

//#define DEBUG_PARSER_STATES
//#define PARSER_DEBUG_VERBOSITY 2

namespace xpath {

    typedef unsigned char Char;
    const int MAX_CHAR_VALUE = 255;
    typedef std::string String;
    const String EMPTY_STRING ("");

    inline bool is_space(Char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    inline bool is_alpha(Char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }
    inline bool is_printable(Char c) {
        return (c >= 0x20 && c < 0x7f);
    }
    inline bool is_ascii(Char c) {
        return (c <= 0x7f);
    }
    inline bool is_utf8_sequence_followup(Char c) {
        return (c >= 0x80) && (c <= 0xBF);
    }
    inline bool is_utf8_sequence_start(Char c) {
        return (c & 0xC0) == 0xC0;
    }
    inline bool is_utf8_multibyte(Char c) {
        return is_utf8_sequence_start(c) || is_utf8_sequence_followup(c);
    }
    inline bool is_underscore(Char c) {
        return c == '_';
    }
    inline bool is_colon(Char c) {
        return c == ':';
    }
    inline bool is_dot(Char c) {
        return c == '.';
    }
    inline bool is_hyphen(Char c) {
        return c == '-';
    }

    inline bool is_digit(Char c) {
        return c >= '0' && c <= '9';
    }

    const String PUBID_SPECIAL_CHARS = "-'()+,./:=?;!*#@$_%";
    inline bool is_pubid_special(Char c) {
        for (unsigned int i = 0; i < PUBID_SPECIAL_CHARS.size();++i) {
            if (c == PUBID_SPECIAL_CHARS[i]) {
                return true;
            }
        }
        return false;
    }
    inline bool is_pubid(Char c) {
        return is_alpha(c) || is_digit(c) || is_space(c) || is_pubid_special(c);
    }


    inline bool is_ABCDEF(Char c) {
        return c >= 'A' && c <= 'F';
    }
    inline bool is_abcdef(Char c) {
        return c >= 'a' && c <= 'f';
    }
    inline unsigned int digit_to_num(Char digit) {
        return digit - '0';
    }
    inline unsigned int ABCDEF_to_num(Char digit) {
        return digit - 'A' + 10;
    }
    inline unsigned int abcdef_to_num(Char digit) {
        return digit - 'a' + 10;
    }

    const Char AMP = '&';
    const Char LT = '<';
    const Char GT = '>';
    const Char QUOTE = '"';
    const Char APOS = '\'';
    const Char C9 = 9;
    const Char C10 = 10;
    const Char C13 = 13;
    const Char SLASH = '/';
    const Char EQUAL = '=';
    const Char SEMI = ';';
    const Char Cx = 'x';

   /*
    datachar    ::= '&amp;' | '&lt;' | '&gt;' | '&quot;'
    | '&#9;'| '&#10;'| '&#13;'
    | (Char - ('&' | '<' | '>' | '"' | #x9 | #xA | #xD))

    */

    const String ENT_AMP("&amp;");
    const String ENT_LT("&lt;");
    const String ENT_GT("&gt;");
    const String ENT_QUOT("&quot;");
    const String ENT_APOS("&apos;");
    const String ENT_9("&#9;");
    const String ENT_10("&#10;");
    const String ENT_13("&#13;");
    const String ENT_DEC("&#");
    const String ENT_SEMI(";");

    const String COMMENT_BGN("<!--");
    const String COMMENT_END("-->");

    const String CDATA_BGN("<![CDATA[");
    const String CDATA_END("]]>");

    const String DOCTYPE_BGN("<!DOCTYPE");
    const String DOCTYPE_END(">");

    const String ENTITY_BGN("<!ENTITY");
    const String ENTITY_END(">");

    const String PI_BGN("<?");
    const String PI_END("?>");

    const String ETAG_BGN("</");
    const String ETAG_END(">");


    std::string as_decimal_entity(Char c) {
        unsigned char uc = c;
        String result = ENT_DEC;
        result+=asl::as_string(static_cast<unsigned>(uc));
        result+=SEMI;
        return result;
    }

    /// convert a ascii hex digit to its binary value. ('2' -> 2, 'A' -> 10)
    /// returns false if not a hex digit
    bool hex_to_num(Char digit, unsigned int & num) {
        if (is_digit(digit)) {
            num = digit_to_num(digit);
            return true;
        } else if (is_ABCDEF(digit)) {
            num = ABCDEF_to_num(digit);
            return true;
        } else if (is_abcdef(digit)) {
            num = abcdef_to_num(digit);
            return true;
        }
        return false;
    }

    bool is_decimal_number(const String & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0)
            return false;
        unsigned int result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            if (is_digit(s[i])) {
                result*=10;
                result+=digit_to_num(s[i]);
            } else
                return false;
            if (result > max_value)
                return false;
        }
        r = result;
        return true;
    }

    bool is_hex_number(const String & s, unsigned int & r, unsigned int max_value = MAX_CHAR_VALUE) {
        if (s.size() == 0) {
            return false;
	}
        unsigned int result = 0;
        for (unsigned int i = 0; i< s.size();++i) {
            unsigned int num;
            if (hex_to_num(s[i],num)) {
                result*=16;
                result+=num;
            } else {
                return false;
	    }
            if (result > max_value) {
                return false;
	    }
        }
        r = result;
        return true;
    }

    // returns the position of the first non-whitespace char
    int read_whitespace(const String & is, int pos) {
        while (pos < is.size() && (is_space(is[pos]))) {
            ++pos;
        }
        return pos;
    }

    inline bool is_name_start_char(Char c) {
        return is_alpha(c) || is_underscore(c) || is_colon(c);
    }
    inline bool is_name_char(Char c) {
        return is_name_start_char(c) || is_digit(c) || is_dot(c) || is_hyphen(c);
    }

    // returns the position of the first non-namechar
    int read_name(const String& is,int pos) {
        if ( is_name_start_char(is[pos]) && pos < is.size() ) {
            ++pos;
            while (is_name_char(is[pos]) && pos < is.size() ) {
                ++pos;
            }
        }
        return pos;
    }

    // returns pos + 1 if c is at pos
    inline int read_if_char(const String & is,int pos, Char c) {
        if ( is[pos] == c && pos < is.size() ) {
            return pos + 1;
        }
        return pos;
    }

    // returns position past end of s if s is the exact next part
    inline int read_if_string(const String & is,int pos, const String & s) {
        int i = 0;
        int n = asl::minimum(s.size(),is.size()-pos);
        while (i<n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size())
            return pos + i;
        return pos;
    }

    // returns pos of next delim
    int read_text(const String & is,int pos, Char delim = LT) {
        int last_non_white = pos;
        while ( pos < is.size() && is[pos] != delim ) {
            if (!is_space(is[pos])) last_non_white = pos;
            ++pos;
        }
        return last_non_white+1;
    }
    // returns position past second quote char if at pos is a quote char
    int read_quoted_text(const String & is,int pos,
        Char opening_qoute,Char closing_qoute ) {
        if (is[pos]==opening_qoute) {
            ++pos;
            while ( pos < is.size() && is[pos] != closing_qoute ) {
                ++pos;
            }
            if (is[pos]==closing_qoute) ++pos;
        }
        return pos;
    }
    int read_quoted_text(const String & is,int pos) {
        if (is[pos] == QUOTE)
            return read_quoted_text(is,pos,QUOTE,QUOTE);
        else
            return read_quoted_text(is,pos,APOS,APOS);
    }

    void copy_between_quotes(const String& is,int pos, int end_pos, String & dest) {
        dest=is.substr(pos+1,end_pos-pos-2);
    }
    // returns position past "right" if sequence starts with "left"
    int read_if_between(const String & is,int pos, const String & left, const String & right) {
        int left_end = read_if_string(is,pos,left);
        if (left_end > pos) {
            int right_begin = is.find(right,left_end);
            if (right_begin != String::npos)
                pos = right_begin + right.size();
        }
        return pos;
    }

    // utility for previous function to copy what's between into string dest
    void copy_between(const String & is,int pos, const String & left, const String & right,
		      int right_end_pos, String & dest) {
        int begin = pos+left.size();
        dest = is.substr(begin,right_end_pos-begin-right.size());
    }

    // read <!-- comment -->
    int read_comment(const String& is,int pos,String & comment) {
        int new_pos = read_if_between(is,pos,COMMENT_BGN,COMMENT_END);
        if (new_pos > pos) {
            copy_between(is,pos,COMMENT_BGN,COMMENT_END,new_pos,comment);
        }
        return new_pos;
    }

    // read <?target processing_instruction?>
    int read_processing_instruction(const String & is,int pos,
                                    String & target,String & proc_instr) {
        int new_pos = read_if_between(is,pos,PI_BGN,PI_END);
        if (new_pos > pos) {
            int instr_pos = read_name(is,pos+2);
            if (instr_pos > pos+2) {
                target = is.substr(pos+2,instr_pos-pos-2);
                instr_pos = read_whitespace(is,instr_pos);
                proc_instr = is.substr(instr_pos,new_pos-instr_pos-2);
            }
        }
        return new_pos;
    }

    // read <![CDATA[ some otherwise entity-encoded <characters> may appear <!here/> ]]>
    int read_cdata(const String & is,int pos,String & cdata) {
        int new_pos = read_if_between(is,pos,CDATA_BGN,CDATA_END);
        if (new_pos > pos) {
            copy_between(is,pos,CDATA_BGN,CDATA_END,new_pos,cdata);
        }
        return new_pos;
    }

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
		pos = read_whitespace(instring, pos);
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
	    if ((nw_pos = read_quoted_text(instring, pos, '\"', '\"')) != pos) {
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
	} else if (is_digit(instring[pos])) {
	    nw_pos = pos;
	    while (is_digit(instring[++nw_pos]));
	    string numberstring = string(instring, pos, nw_pos);
	    std::istringstream iss(numberstring);
	    int num;
	    iss >> num;
	    *e = new Number(num);
	    return nw_pos;
	} else if ((ft = Function::typeOf(instring, pos)) != Function::Unknown) {
	    std::list<Expression*> *arglist = new std::list<Expression*>();
	    pos = read_if_string(instring, pos, Function::nameOf(ft));
	    pos = read_whitespace(instring, pos);
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
	    pos = read_whitespace(instring, nw_pos);
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
	pos = read_whitespace(instring, pos);
	UnaryExpression::OperatorType myType = UnaryExpression::Invalid;
	if (instring[pos] == '-') {
	    myType = UnaryExpression::Minus;
	    pos++;
	} else if (instring[pos] == '~') {
	    myType = UnaryExpression::Tilde;
	    pos++;
	} else if ((nw_pos = read_if_string(instring, pos, "not")) != pos) {
	    myType = UnaryExpression::Not;
	    pos = nw_pos;
	};
	pos = read_whitespace(instring, pos);
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
	pos = read_whitespace(instring, pos);
	BinaryExpression::ExpressionType myType;
	if (instring[pos] == '*') {
	    myType = BinaryExpression::Times;
	    pos++;
	} else if ((nw_pos = read_if_string(instring, pos, "div")) != pos) {
	    myType = BinaryExpression::Div;
	    pos = nw_pos;
	} else if ((nw_pos = read_if_string(instring, pos, "mod")) != pos) {
	    myType = BinaryExpression::Mod;
	    pos = nw_pos;
	} else {
	    return pos;
	};
	pos = read_whitespace(instring, pos);

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
	pos = read_whitespace(instring, pos);
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
	pos = read_whitespace(instring, pos);
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
	pos = read_whitespace(instring, pos);
	if (instring[pos] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(pos);
#endif
	    pos++;
	    pos = read_whitespace(instring, pos);
	} else if (instring[pos] == '!' && instring[pos+1] == '=') {
#if PARSER_DEBUG_VERBOSITY > 1
	    AC_TRACE << "found EqualityExpression at " << instring.substr(pos);
#endif
	    pos+=2;
	    pos = read_whitespace(instring, pos);
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
	pos = read_whitespace(instring, pos);
	if (!(instring.compare(TOKEN_AND), pos, TOKEN_AND.length())) {
	    pos += TOKEN_AND.length();
	    pos = read_whitespace(instring, pos);

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
	pos = read_whitespace(instring, pos);
	if (!(instring.compare(TOKEN_OR), pos, TOKEN_OR.length())) {
	    pos += TOKEN_OR.length();
	    pos = read_whitespace(instring, pos);

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
	} else if ((nw_pos = read_name(instring, pos)) != pos) {
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
		int nw_pos = read_if_string(instring, pos, Step::stringForAxis(a));
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

    Path *parse(const std::string &instring) {
	Path *p = new Path();
        parsePath(p, instring, 0);
	return p;
    }

    NodeSetRef evaluate(std::string path, xpath::NodeRef theNode) {
        
        xpath::Path *myPath = xpath::parse(path);

        if (!myPath) {
            AC_ERROR << "XPath parse error.";
            return NULL;
        } else {
            xpath::NodeSetValue *value = myPath->evaluate(theNode);
            AC_INFO << "evaluated path " << *myPath << ":";
            for (int i = 0; i < value->length(); i++) {
                AC_INFO << "\tresult " <<i<<": "<< *value->item(i);
            }
            AC_INFO << "/evaluated.";

            NodeSetRef retval = value->takeNodes();
            delete value;
            delete myPath;
            return retval;
        }
    }
}
