
#include <assert.h>
#include <functional>
#include <math.h>

#include <iostream>

#include <asl/Logger.h>
#include <dom/Nodes.h>

#include "Expression.h"

#define INTERPRETER_DEBUG

namespace xpath
{

    const string Function::FUNCTIONNAME_LAST = "last";
    const string Function::FUNCTIONNAME_POSITION = "position";
    const string Function::FUNCTIONNAME_COUNT = "count";
    const string Function::FUNCTIONNAME_STARTSWITH = "startswith";
    const string Function::FUNCTIONNAME_CONCAT = "concat";
    const string Function::FUNCTIONNAME_CONTAINS = "contains";
    const string Function::FUNCTIONNAME_SUBSTRING = "substring";
    const string Function::FUNCTIONNAME_SUBSTRING_BEFORE = "substring_before";
    const string Function::FUNCTIONNAME_SUBSTRING_AFTER = "substring_after";
    const string Function::FUNCTIONNAME_NOT = "not";
    const string Function::FUNCTIONNAME_UNKNOWN = " *** unknown function *** ";
    const string Function::FUNCTIONNAME_ID = "id";
    const string Function::FUNCTIONNAME_LOCAL_NAME = "local-name";
    const string Function::FUNCTIONNAME_NAMESPACE_URI = "namespace-uri";
    const string Function::FUNCTIONNAME_NAME = "name";
    const string Function::FUNCTIONNAME_STRING = "string";
    const string Function::FUNCTIONNAME_STRING_LENGTH = "string-length";
    const string Function::FUNCTIONNAME_NORMALIZE_SPACE = "normalize-space";
    const string Function::FUNCTIONNAME_TRANSLATE = "translate";
    const string Function::FUNCTIONNAME_BOOLEAN = "boolean";
    const string Function::FUNCTIONNAME_TRUE = "true";
    const string Function::FUNCTIONNAME_FALSE = "false";
    const string Function::FUNCTIONNAME_LANG = "lang";
    const string Function::FUNCTIONNAME_NUMBER = "number";
    const string Function::FUNCTIONNAME_SUM = "sum";
    const string Function::FUNCTIONNAME_FLOOR = "floor";
    const string Function::FUNCTIONNAME_CEILING = "ceiling";
    const string Function::FUNCTIONNAME_ROUND = "round";

    const string Step::AXISNAME_INVALID           (" *** invalid Axis *** ");
    const string Step::AXISNAME_NEXT_SIBLING      ("next-sibling");
    const string Step::AXISNAME_PREVIOUS_SIBLING  ("previous-sibling");
    const string Step::AXISNAME_FOLLOWING_SIBLING ("following-sibling");
    const string Step::AXISNAME_PRECEDING_SIBLING ("preceding-sibling");
    const string Step::AXISNAME_CHILD             ("child");
    const string Step::AXISNAME_PARENT            ("parent");
    const string Step::AXISNAME_DESCENDANT        ("descendant");
    const string Step::AXISNAME_ANCESTOR          ("ancestor");
    const string Step::AXISNAME_FOLLOWING         ("following");
    const string Step::AXISNAME_PRECEDING         ("preceding");
    const string Step::AXISNAME_ANCESTOR_OR_SELF  ("ancestor-or-self");
    const string Step::AXISNAME_DESCENDANT_OR_SELF("descendant-or-self");
    const string Step::AXISNAME_SELF              ("self");
    const string Step::AXISNAME_NAMESPACE         ("namespace");
    const string Step::AXISNAME_ATTRIBUTE         ("attribute");
    
    const string Step::NODETEST_INVALID = " *** invalid node test *** ";
    const string Step::NODETEST_NODE = "node";
    const string Step::NODETEST_COMMENT = "comment";
    const string Step::NODETEST_TEXT = "text";
    const string Step::NODETEST_PI = "processing-instruction";

    BinaryExpression::BinaryExpression(BinaryExpression::ExpressionType _type, Expression *_lvalue, Expression *_rvalue)
    {
        type = _type;
        lvalue = _lvalue;
        rvalue = _rvalue;
    };

    Value *BinaryExpression::evaluateExpression(const Context &c)
    {
        assert(lvalue);
        assert(rvalue);

        Value *left = lvalue->evaluateExpression(c);
        Value *right = rvalue->evaluateExpression(c);

        assert(left);
        assert(right);

        if (type<16) // comparison operators
        {
        if ((left->type()==Value::NodeSetType) && (right->type()==Value::NodeSetType))
            {
                // find one pair of nodes whose string-values comparison evaluates true.

		NodeSetValue *nsv1 = left->toNodeSet();
		delete left;
		NodeSetRef nsr1 = nsv1->takeNodes();
		delete nsv1;
		
		NodeSetValue *nsv2 = right->toNodeSet();
		delete right;
		NodeSetRef nsr2 = nsv2->takeNodes();
		delete nsv2;

		bool retval = false;

		if (type == Equal || type == NotEqual) {

		    std::set<string> sm1;
		    std::set<string> sm2;
		        
		    NodeSet::iterator i1 = nsr1->begin();
		    NodeSet::iterator i2 = nsr2->begin();
		    
		    while (i1 != nsr1->end() || i2 != nsr2->end()) {
			
			string string1 = string_value_for(*i1);
			if (string1.length()) {
			    
			    if (sm2.find(string1) != sm2.end()) {
				if (type == Equal) {
				    retval = true;
				    break;
				}
			    } else {
				if (type == NotEqual) {
				    retval = true;
				    break;
				}
			    }
			    sm1.insert(string1);
			}
			
			std::string string2 = string_value_for(*i2);
			if (string2.length()) {
			    {
				if (sm1.find(string2) != sm1.end()) {
				    if (type == Equal) {
					retval = true;
					break;
				    }
				} else {
				    if (type == NotEqual) {
					retval = true;
					break;
				    }
				}
			    }
			    sm2.insert(string2);
			}
			
			if (i1 != nsr2->end()) {
			    ++i1;
			}
			
			if (i2 != nsr2->end()) {
			    ++i2;
			}
		    }
		} else {
		    std::string extreme_left;
		    std::string extreme_right;
		    NodeSet::iterator ileft = nsr1->begin();
		    NodeSet::iterator iright = nsr2->begin();
		    while (ileft != nsr1->end() || ileft != nsr2->end()) {
			
			if ( (  (type == Less || type == LEqual) && (string_value_for(*ileft) > extreme_left) )
			     || (type == Greater || type == GEqual) && (string_value_for(*ileft) < extreme_left) )
			{
			    extreme_left = string_value_for(*ileft);
			}

			if ( (  (type == Less || type == LEqual) && (string_value_for(*iright) < extreme_right) )
			     || (type == Greater || type == GEqual) && (string_value_for(*iright) > extreme_right) )
			{
			    extreme_right = string_value_for(*iright);
			}

			if (extreme_left < extreme_right) {
			    if (type == Less || type == LEqual)
				{
				    retval = true;
				    break;
				}
			} else if (extreme_left == extreme_right) {
			    if (type == LEqual || type == GEqual)
				{
				    retval = true;
				    break;
				}
			} else {
			    if (type == Greater || type == GEqual)
				{
				    retval = true;
				    break;
				}
			}

			if (ileft != nsr2->end()) {
			    ++ileft;
			}
			
			if (iright != nsr2->end()) {
			    ++iright;
			}
		    }
		}
                delete nsr1; delete nsr2;
		return new BooleanValue(retval);		
	    }
	else if (left->type()==Value::NodeSetType || right->type()==Value::NodeSetType)
            {
                // comparison of one nodeset with one other type

                if (left->type()==Value::StringType || right->type()==Value::StringType)
                {
		    const int TRUTHVALUES_LEFT[] = { 1<<NotEqual | 1<<Less | 1<<LEqual,
						     1<<Equal | 1<<LEqual | 1 <<GEqual,
						     1<<NotEqual | 1<<Greater | 1 <<GEqual };
		    const int TRUTHVALUES_RIGHT[] = { TRUTHVALUES_LEFT[2], TRUTHVALUES_LEFT[1], TRUTHVALUES_LEFT[0] };

                    // one nodeset, one string: find one node whose string-value compares positive to the string
                    NodeSetValue *nsv;
                    StringValue *sv;
                    bool stringleft;
		    const int *truthTable;
                    if (left->type()==Value::StringType)
                    {
                        // WARNING: toNodeSet() invalidates right.
			// which is okay, provided that right is
			// no longer accessed; which actually is the case,
			// see the "delete right" below.
                        nsv = right->toNodeSet();
                        sv = left->toString();
                        truthTable = TRUTHVALUES_LEFT;
                    }
                    else
                    {
                        // WARNING: toNodeSet() invalidates left,
			// which is okay, provided that left is
			// no longer accessed; which actually is the case,
			// see the "delete left" below.
                        nsv = left->toNodeSet();
                        sv = right->toString();
			truthTable = TRUTHVALUES_RIGHT;
                    };

                    delete left;
                    delete right;
                    bool retval = false;
#ifdef INTERPRETER_DEBUG
		    AC_INFO << " string - nodeset comparison "<< *this<<" of type " << type << " with value " << sv->getValue();
#endif

                    for (NodeSet::iterator i = nsv->begin(); i !=nsv->end(); ++i) {
			string curstr = string_value_for(*i);
			AC_INFO << "  comparing string \"" << sv->getValue() << "\" with node of value \"" <<curstr<<"\"";
			int cmp = curstr.compare(sv->getValue());
			
			if (truthTable[cmp+1]&(1<<type)) {
			    retval = true;
			    break;
			}
                    };
		    
#ifdef INTERPRETER_DEBUG
                    if (retval) {
			AC_TRACE <<" succeeded for string "<< sv->getValue();
                    }
#endif
                    delete sv; delete nsv;
                    return new BooleanValue(retval);
                }
                else if (left->type()==Value::NumberType || right->type()==Value::NumberType)
                {
                    // one nodeset, one number: find one node whose string-value converted to number compares positive

                    Value *theNodeSet;
		    Value *theNumber;

                    if (left->type()==Value::NumberType)
                    {
			theNodeSet = right;
			theNumber = left;
		    } else {
			theNodeSet = left;
			theNumber = right;
		    }
		    NumberValue *nv = theNumber->toNumber();
		    NodeSetValue *nsv = theNodeSet->toNodeSet();

		    delete theNumber; delete theNodeSet;

		    double num = nv->getValue();
		    NodeSetRef nsr = nsv->takeNodes();

		    delete nsv;

		    //std::binary_function<NodeRef, double, bool> theOp = getOpFor<double>(type);

		    for (NodeSet::iterator i = nsr->begin(); i != nsr->end(); ++i) {
			bool val;
			double currentnum = number_value_for(*i);
			switch(type) {
			case Equal:
			    val = (currentnum == num);
			    break;
			case NotEqual:
			    val = (currentnum != num);
			    break;

			case Greater:
			    val = (theNumber == left) ^ (number_value_for(*i) > num);
			    break;
			case GEqual:
			    val = (theNumber == left) ^ (number_value_for(*i) >= num);
			    break;
			case Less:
			    val = (theNumber == left) ^ (number_value_for(*i) < num);
			    break;
			case LEqual:
			    val = (theNumber == left) ^ (number_value_for(*i) <= num);
			    break;
			}

			if (val) {
			    delete nsr;
			    return new BooleanValue(true);
			}
		    }
		    delete nsr;
		    return new BooleanValue(false);
                }
                else if (left->type()==Value::BooleanType || right->type()==Value::BooleanType)
                {
                    // one nodeset, one boolean: convert both to boolean
                    BooleanValue *a = left->toBoolean();
		    BooleanValue *b = right->toBoolean();
		    delete left;
		    delete right;
                }
                else
                {
		    // should not happen.
                    AC_ERROR << "Type not implemented!";
                    assert(false);
                }
                return new NullValue();
            }
            else // no nodeset
            {
                if (type==Equal || type == NotEqual)
                {
                    // convert to simplest common type and compare
                    bool retval;
                    if (left->type()==Value::BooleanType || right->type()==Value::BooleanType) {
                        retval = equals_as<BooleanValue>(left, right);
                    } else if (left->type()==Value::NumberType || right->type()==Value::NumberType) {
                        retval = equals_as<NumberValue>(left, right);
                    } else {
                        retval = equals_as<StringValue>(left, right);
                    }
                    delete left; delete right;
                    return new BooleanValue(retval ^ (type==NotEqual));
                }
                else //  no nodeset and no equality operation
                {
                    if (equals_as<NumberValue>(left, right)) {
                        delete left; delete right;
                        return new BooleanValue(type==LEqual||type==GEqual);
                    } else { // numbers are different.
                        bool retval = ( type==LEqual || type == Less ) ^
                            ( smaller_than_as<NumberValue>(left, right));
                        delete left; delete right;
                        return new BooleanValue(retval);
                    }
                };
            };
        }
        else if (type<32) // "or" | "and"
        {
            BooleanValue *r = right->toBoolean();
            BooleanValue *l = left->toBoolean();
            bool rb = r->getValue();
            bool lb = l->getValue();
            delete right; delete r;
            delete left; delete l;
            if (type==Or) {
                return new BooleanValue(rb || lb);
            } else if (type==And) {
                return new BooleanValue(rb && lb);
            } else {
		// should not happen.
                AC_ERROR << "Not yet implemented!";
                assert(false);
                return new NullValue();
            }
        }
        else // "+", "-", "*", "/", "mod"
        { 
            NumberValue *r = right->toNumber();
            NumberValue *l = left->toNumber();
            double rn = r->getValue();
            double ln = l->getValue();
            delete right; delete r;
            delete left; delete l;
            switch(type)
            {
            case Plus:
                return new NumberValue(ln + rn);
            case Minus:
                return new NumberValue(ln - rn);
            case Times:
                return new NumberValue(ln * rn);
            case Div:
                return new NumberValue(ln / rn);
            case Mod:
                return new NumberValue(ln - rn * floor(ln / rn));
            default:
		// should not happen.
                AC_ERROR << "Not yet implemented.";
                assert(false);
                return new NullValue();
            };
        }
    };

    void BinaryExpression::serializeTo(std::ostream &os)
    {
        BinaryExpression *lv = dynamic_cast<BinaryExpression*>(lvalue);
        if (lv) {
            os << "(";
        }
        lvalue->serializeTo(os);
        if (lv) {
            os << ")";
        }

        switch(type)
        {
        case Equal:
            os << " = ";
            break;
        case NotEqual:
            os << " != ";
            break;
        case Greater:
            os << " > ";
            break;
        case GEqual:
            os << " >= ";
            break;
        case Less:
            os << " < ";
            break;
        case LEqual:
            os << " <= ";
            break;
        case Or:
            os << " or ";
            break;
        case And:
            os << " and ";
            break;
        case Plus:
            os << " + ";
            break;
        case Minus:
            os << " - ";
            break;
        case Times:
            os << " * ";
            break;
        case Div:
            os << " div ";
            break;
        case Mod:
            os << " mod ";
            break;
        default:
            os << " ** unknown operator ** ";
            break;
        }
        BinaryExpression *rv = dynamic_cast<BinaryExpression*>(rvalue);
        if (rv) {
            os << "(";
        }
        rvalue->serializeTo(os);
        if (rv) {
            os << ")";
        }
    };

    void UnaryExpression::serializeTo(std::ostream &os)
    {
        switch(type)
        {
        case Minus:
            os << "-";
            break;
        case Tilde:
            os << "~";
            break;
        case Not:
            os << "!";
        default:
            AC_ERROR << "Not yet implemented!";
            assert(false);
        };
        argument->serializeTo(os);
    }

    Value *UnaryExpression::evaluateExpression(const Context &c)
    {
        if (type==Minus) {
            Value *subvalue = argument->evaluateExpression(c);
            NumberValue *retval = subvalue->toNumber();
            delete subvalue;
            retval->negate();
            return retval;
        } else {
            assert(false);
        }
        return new NullValue();
    }

    void SetExpression::serializeTo(std::ostream &os)
    {
        std::list<Path *>::iterator i = sets.begin();
        if (*i) {
            (*i)->serializeTo(os);
            while ((++i) != sets.end()) {
                os << " | ";
                (*i)->serializeTo(os);
            };
        }
    };

    Value *SetExpression::evaluateExpression(const Context &c)
    {
        std::list<Path*>::iterator i = sets.begin();

        NodeSetValue *firstValue = (*i)->evaluate(c.currentNode);

        NodeSetRef v = firstValue->takeNodes();
        delete firstValue;

        while (++i != sets.end())
        {
            NodeSetValue *thisValue = (*i)->evaluate(c.currentNode);
            NodeSetRef vcur = thisValue->takeNodes();
            delete thisValue;

            NodeSetRef vres = new NodeSet();
            switch(m_op)
            {
            case Union:
                std::set_union(v->begin(), v->end(), vcur->begin(), vcur->end(), std::inserter(*vres, vres->begin()));
                break;
            case Intersection:
                std::set_intersection(v->begin(), v->end(), vcur->begin(), vcur->end(), std::inserter(*vres, vres->begin()));
                break;
            case Difference:
                std::set_difference(v->begin(), v->end(), vcur->begin(), vcur->end(), std::inserter(*vres, vres->begin()));
                break;
            }
            delete vcur;
            delete v;
            v = vres;
        }
        return new NodeSetValue(v);
    };

    Function::~Function()
    {
        delete arguments;
    }

    void Function::serializeTo(std::ostream &os)
    {
        os << nameOf(type);
        os << "(";

        std::list<Expression*>::iterator i = arguments->begin();
        if (i != arguments->end()) {
            (*i)->serializeTo(os);

            for (++i; i != arguments->end();++i) {
                os << ", ";
                (*i)->serializeTo(os);
            };
        };
        os << ")";
    };

    Value *Function::evaluateExpression(const Context &c)
    {
        switch(type)
        {
        case Last:
            return new NumberValue(c.size);
        case Position:
            return new NumberValue(c.position);
        case Count:
            {
                if (!arguments) {
                    return new NumberValue(0);
                }

                Value *v = arguments->front()->evaluateExpression(c);

                AC_TRACE << "counting \"" << *(arguments->front()) <<"\"...";

                // convert into a nodeSet
                NodeSetValue *nsv = dynamic_cast<NodeSetValue*>(v);

                if (!nsv) {
                    delete v;
                    AC_WARNING << " ... failed: argument not a node-set!";
                    return new NumberValue(0);
                }

                // and count the nodeSet
                int retval = nsv->length();
                delete nsv;
                AC_TRACE << " returning: " << retval;
                return new NumberValue(retval);
            }

        case StartsWith:
            {
                if (!arguments || !arguments->size()) {
                    AC_WARNING << "Warning: " << *this << " called with zero arguments.";
                    return new NullValue();
                }

                std::list<Expression*>::iterator i = arguments->begin();
                Expression *firstExpr = *i;
                Expression *secondExpr = *(++i);
                assert(firstExpr);

                if (!secondExpr) {
                    AC_WARNING << "Warning: " << *this << " called with only one argument.";
                    return new NullValue();
                }

                Value *first = firstExpr->evaluateExpression(c);
                Value *second = secondExpr->evaluateExpression(c);

                bool retval = equals_as<StringValue>(first, second);
                delete first; delete second; return new BooleanValue(retval);
            }

        case Concat:
            {
                if (!arguments || !arguments->size()) {
                    return new NullValue();
                }

                string retval;
                for (std::list<Expression*>::iterator i = arguments->begin(); i != arguments->end(); ++i) {
                    Value *currentvalue = (*i)->evaluateExpression(c);
                    StringValue *currentstring = currentvalue->toString();
                    retval+=currentstring->getValue();
                    delete currentstring;
                    delete currentvalue;
                }
                return new StringValue(retval);
            }

        case Contains:
            {
                if (!arguments || !arguments->size()) {
                    return new NullValue();
                }

                std::list<Expression*>::iterator i = arguments->begin();
                Expression *firstExpr = *i;
                Expression *secondExpr = *(++i);

                if (!secondExpr) {
                    AC_WARNING << "ERROR: parameter missing to function call " << *this;
                    return new NullValue();
                }

                Value *first = firstExpr->evaluateExpression(c);
                Value *second = secondExpr->evaluateExpression(c);

                StringValue *firstString = first->toString();
                StringValue *secondString = second->toString();

                delete first;
                delete second;

                bool retval = firstString->getValue().find(secondString->getValue()) >= 0;

                delete firstString;
                delete secondString;

                return new BooleanValue(retval);
            }

        case Substring:
            {
                if (!arguments || !arguments->size()) {
                    return new NullValue();
                }

                std::list<Expression*>::iterator i = arguments->begin();
                Expression *firstExpr = *i;
                Expression *secondExpr = *(++i);
                Expression *thirdExpr = NULL;
		if (++i != arguments->end()) {
		    AC_TRACE << " three arguments for " << *this;
		    thirdExpr = *i;
		}
                assert(firstExpr);

                if (!secondExpr) {
		    AC_TRACE << "substring expression: second argument is not a number.";
                    return new NullValue();
                }

                Value *first = firstExpr->evaluateExpression(c);
                Value *second = secondExpr->evaluateExpression(c);
                Value *third = NULL;
                if (thirdExpr) {
                    third = thirdExpr->evaluateExpression(c);
                }

                StringValue *firstString = first->toString();
                NumberValue *secondNumber = second->toNumber();
                NumberValue *thirdNumber = NULL;
                if (thirdExpr) {
                    thirdNumber= third->toNumber();
                }

                delete first;
                delete second;

                if (third) {
                    delete third;
                }

                int secondNumberValue = int(secondNumber->getValue()+0.5) - 1;
		if (secondNumberValue < 0) {
		    AC_WARNING << "substring: round(secondNumberValue->getValue()+0.5) = " << secondNumberValue+1 << "is less than 1!";
		    secondNumberValue = 0;
		}

                int resultlength;
		AC_TRACE << "substring: complete string is \"" << firstString->getValue() << "\"";
                resultlength = firstString->getValue().length() - secondNumberValue;

                // +1 for the terminating null character
                if (thirdNumber && thirdNumber->getValue() + 1 < resultlength) {
                    resultlength = int(thirdNumber->getValue()+0.5) + 1;
                }

		AC_TRACE << "substring: length = " << resultlength;

                string result = firstString->getValue().substr(secondNumberValue, resultlength);

                delete firstString;
                delete secondNumber;
                if (thirdNumber) {
                    delete thirdNumber;
                }

		AC_TRACE << "substring expression evaluates to \"" << result << "\"";
                StringValue * resultvalue = new StringValue(result);
                return resultvalue;

            }

        case SubstringBefore:
            {
                if (!arguments || !arguments->size())
                    return new NullValue();

                std::list<Expression*>::iterator i = arguments->begin();
                Expression *firstExpr = *i;
                Expression *secondExpr = *(++i);
                assert(firstExpr);

                if (!secondExpr) {
                    return new NullValue();
                }

                Value *first = firstExpr->evaluateExpression(c);
                Value *second = secondExpr->evaluateExpression(c);

                StringValue *firstString = first->toString();
                StringValue *secondString = second->toString();

                delete first;
                delete second;

                int endPos = firstString->getValue().find(secondString->getValue());

                StringValue *retval = new StringValue(firstString->getValue().substr(0, endPos));

                delete firstString;
                delete secondString;

                return retval;
            }

        case SubstringAfter:
            {
                if (!arguments || !arguments->size()) {
                    return new NullValue();
                }

                std::list<Expression*>::iterator i = arguments->begin();
                Expression *firstExpr = *i;
                Expression *secondExpr = *(++i);

                if (!secondExpr) {
                    return new NullValue();
                }

                Value *first = firstExpr->evaluateExpression(c);
                Value *second = secondExpr->evaluateExpression(c);

                StringValue *firstString = first->toString();
                StringValue *secondString = second->toString();

                delete first;
                delete second;

                int pos = firstString->getValue().find(secondString->getValue());

                StringValue *retval = new StringValue(secondString->getValue().substr(pos+secondString->getValue().length()));
                delete firstString;
                delete secondString;

                return retval;
            }
        case Not:
            {
                Value *result = arguments->front()->evaluateExpression(c);
                BooleanValue *v = result->toBoolean();
                bool vval = v->getValue();
                delete result;
                delete v;
                return new BooleanValue(!vval);
            }

        case Unknown:
        default:
            AC_ERROR << " ** access to unknown function ** ";
            return new BooleanValue(false);
        };
    };

    string Function::nameOf(Function::FunctionType ft)
    {
        switch(ft)
        {
        case Last:
            return FUNCTIONNAME_LAST;
        case Position:
            return FUNCTIONNAME_POSITION;
        case Count:
            return  FUNCTIONNAME_COUNT;
        case StartsWith:
            return  FUNCTIONNAME_STARTSWITH;
        case Concat:
            return  FUNCTIONNAME_CONCAT;
        case Contains:
            return FUNCTIONNAME_CONTAINS;
        case Substring:
            return FUNCTIONNAME_SUBSTRING;
        case SubstringBefore:
            return FUNCTIONNAME_SUBSTRING_BEFORE;
        case SubstringAfter:
            return FUNCTIONNAME_SUBSTRING_AFTER;
        case Not:
            return FUNCTIONNAME_NOT;
        default:
            return FUNCTIONNAME_UNKNOWN;
        }
    };
    
#define RETCMP(instring, pos, X, yes, no) \
return (asl::read_if_string(instring, pos, X) != pos) ? yes : no;

    Function::FunctionType Function::typeOf(string instring, int pos)
    {
	switch(instring[pos]) {
	case 'l':
	    switch(instring[pos+2]) {
	    case 's':
		RETCMP(instring, pos, FUNCTIONNAME_LAST, Function::Last, Function::Unknown);
	    case 'n':
		RETCMP(instring, pos, FUNCTIONNAME_LANG, Function::Lang, Function::Unknown);
	    case 'c':
		RETCMP(instring, pos, FUNCTIONNAME_LOCAL_NAME, Function::LocalName, Function::Unknown);
	    };
	case 'p':
	    RETCMP(instring, pos, FUNCTIONNAME_POSITION, Function::Position, Function::Unknown);
	case 'c':
	    switch(instring[pos+3]) {
	    case 'n':
		RETCMP(instring, pos, FUNCTIONNAME_COUNT, Function::Count, Function::Unknown);
	    case 'l':
		RETCMP(instring, pos, FUNCTIONNAME_CEILING, Function::Ceiling, Function::Unknown);
	    case 'c':
		RETCMP(instring, pos, FUNCTIONNAME_CONCAT, Function::Concat, Function::Unknown);
	    case 't':
		RETCMP(instring, pos, FUNCTIONNAME_CONTAINS, Function::Contains, Function::Unknown);
	    default:
		return Function::Unknown;
	    }
	case 's':
	    switch(instring[pos+2]) {
	    case 'a':
		RETCMP(instring, pos, FUNCTIONNAME_STARTSWITH, Function::StartsWith, Function::Unknown);
	    case 'm':
		RETCMP(instring, pos, FUNCTIONNAME_SUM, Function::Sum, Function::Unknown);
	    case 'r':
		if (instring.length() > pos + 6 && instring[pos+6] == '-') {
		    RETCMP(instring, pos, FUNCTIONNAME_STRING_LENGTH, Function::StringLength, Function::Unknown);
		} else {
		    RETCMP(instring, pos, FUNCTIONNAME_STRING, Function::String, Function::Unknown);
		}
	    case 'b':
		if (instring.length() > pos + 9 && instring[pos+9] == '-') {
		    switch(instring[pos+10]) {
		    case 'a':
			RETCMP(instring, pos, FUNCTIONNAME_SUBSTRING_AFTER,  Function::SubstringAfter, Function::Unknown);
		    case 'b':
			RETCMP(instring, pos, FUNCTIONNAME_SUBSTRING_BEFORE, Function::SubstringBefore, Function::Unknown);
		    default:
			RETCMP(instring, pos, FUNCTIONNAME_SUBSTRING, Function::Substring, Function::Unknown);
		    }
		}
		RETCMP(instring, pos, FUNCTIONNAME_SUBSTRING, Function::Substring, Function::Unknown);
	    }
	case 'n':
	    switch(instring[pos+2]) {
	    case 'm':
		RETCMP(instring, pos, FUNCTIONNAME_NUMBER, Function::Number, Function::Unknown);
	    case 'r':
		RETCMP(instring, pos, FUNCTIONNAME_NORMALIZE_SPACE, Function::NormalizeSpace, Function::Unknown);
	    case 't':
		RETCMP(instring, pos, FUNCTIONNAME_NOT, Function::Not, Function::Unknown);
	    case 'a':
		if (instring.length() > pos +3 && instring[pos+4] == 's') {
		    RETCMP(instring, pos, FUNCTIONNAME_NAMESPACE_URI, Function::NamespaceURI, Function::Unknown);
		} else {
		    RETCMP(instring, pos, FUNCTIONNAME_NAME, Function::Name, Function::Unknown);
		}
	    }
	case 'i':
	    RETCMP(instring, pos, FUNCTIONNAME_ID, Function::Id, Function::Unknown);
	case 't':
	    if (instring[pos+2]== 'u') {
		RETCMP(instring, pos, FUNCTIONNAME_TRUE, Function::True, Function::Unknown);
	    } else {
		RETCMP(instring, pos, FUNCTIONNAME_TRANSLATE, Function::Translate, Function::Unknown);
	    }
	case 'b':
	    RETCMP(instring, pos, FUNCTIONNAME_BOOLEAN, Function::Boolean, Function::Unknown);
	case 'f':
	    if (instring[pos+1]== 'a') {
		RETCMP(instring, pos, FUNCTIONNAME_FALSE, Function::False, Function::Unknown);
	    } else {
		RETCMP(instring, pos, FUNCTIONNAME_FLOOR, Function::Floor, Function::Unknown);
	    }
	case 'r':
	    RETCMP(instring, pos, FUNCTIONNAME_ROUND, Function::Round, Function::Unknown);
	default:
	    return Function::Unknown;
	}
    };

    void Step::serializeNodeTest(std::ostream &os)
    {
        switch (test)
        {
        case TestPrincipalType:
            if (nodeTestName.length()) {
                os << nodeTestName;
            } else {
                os << "*";
            }
            break;
        case TestAnyNode:
            os << "node()";
            break;
        case TestCommentNode:
            os << "comment()";
            break;
        case TestTextNode:
            os << "text()";
            break;
        case TestProcessingInstruction:
            os << "processinginstruction(";
            os << nodeTestName << ")";
            break;
        case InvalidTest:
            os << "invalid";
            break;
        };
        if (test != TestPrincipalType && test != TestProcessingInstruction && nodeTestName.length()) {
            os << "[self::node() == \"" << nodeTestName << "\"]";
        }
    };

    Step::Step(Step::Axis _axis, Step::NodeTest _test, string name)
    {
        axis = _axis;
        test = _test;
        nodeTestName = name;
    };

    Step::Step()
    {
        axis = Child;
        test = TestPrincipalType;
    };

    Step::~Step()
    {
        for (std::list<Expression *>::iterator i = predicates.begin(); i != predicates.end(); ++i) { delete *i; };
    };

    const string &Step::stringForAxis(Step::Axis a)
    {
        switch(a) {
        case Next_Sibling:       return AXISNAME_NEXT_SIBLING;
        case Previous_Sibling:   return AXISNAME_PREVIOUS_SIBLING;
        case Following_Sibling:  return AXISNAME_FOLLOWING_SIBLING;
        case Preceding_Sibling:  return AXISNAME_PRECEDING_SIBLING;
        case Child:              return AXISNAME_CHILD;
        case Parent:             return AXISNAME_PARENT;
        case Descendant:         return AXISNAME_DESCENDANT;
        case Ancestor:           return AXISNAME_ANCESTOR;
        case Following:          return AXISNAME_FOLLOWING;
        case Preceding:          return AXISNAME_PRECEDING;
        case Ancestor_Or_Self:   return AXISNAME_ANCESTOR_OR_SELF;
        case Descendant_Or_Self: return AXISNAME_DESCENDANT_OR_SELF;
        case Self:               return AXISNAME_SELF;
        case Namespace:          return AXISNAME_NAMESPACE;
        case Attribute:          return AXISNAME_ATTRIBUTE;
        default:                 return AXISNAME_INVALID;
        };
    };

    // returns position past end of s if s is the exact next part
    inline int read_if_string(const string & is,int pos, const string & s) {
        int i = 0;
        int n = asl::minimum(s.size(),is.size()-pos);
        while (i<n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size()) {
            return pos + i;
        }
        return pos;
    }

    Step::Axis Step::read_axis(const string &instring, int pos) {
        switch(instring[pos]) {
        case 'a':
            if (instring.length() <= pos + 8) {
                RETCMP(instring, pos, Step::AXISNAME_ANCESTOR, Step::Ancestor, Step::Invalid);
            } else switch(instring[pos+8]) {
            case '-':
                RETCMP(instring, pos, Step::AXISNAME_ANCESTOR_OR_SELF, Step::Ancestor_Or_Self, Step::Invalid);
            case 'e':
                RETCMP(instring, pos, Step::AXISNAME_ATTRIBUTE, Step::Attribute, Step::Invalid);
            default:
                RETCMP(instring, pos, Step::AXISNAME_ANCESTOR, Step::Ancestor, Step::Invalid);
            }
        case 'c':
            RETCMP(instring, pos, Step::AXISNAME_CHILD, Step::Child, Step::Invalid);
        case 'd':
            if (instring.length() <= pos + 10) {
                RETCMP(instring, pos, Step::AXISNAME_DESCENDANT, Step::Descendant, Step::Invalid);
            } else switch(instring[pos+10]) {
            case '-':
                RETCMP(instring, pos, Step::AXISNAME_DESCENDANT_OR_SELF, Step::Descendant_Or_Self, Step::Invalid);
            default:
                RETCMP(instring, pos, Step::AXISNAME_DESCENDANT, Step::Descendant, Step::Invalid);
            }
        case 'f':
            if (instring.length() <= pos + 9) {
                RETCMP(instring, pos, Step::AXISNAME_FOLLOWING, Step::Following, Step::Invalid);
            } else switch(instring[pos+9]) {
            case '-':
                RETCMP(instring, pos, Step::AXISNAME_FOLLOWING_SIBLING, Step::Following_Sibling, Step::Invalid);
            default:
                RETCMP(instring, pos, Step::AXISNAME_FOLLOWING, Step::Following, Step::Invalid);
            }
        case 'n':
            switch(instring[pos+1]) {
            case 'a':
                RETCMP(instring, pos, Step::AXISNAME_NAMESPACE, Step::Namespace, Step::Invalid);
            case 'e':
                RETCMP(instring, pos, Step::AXISNAME_NEXT_SIBLING, Step::Next_Sibling, Step::Invalid);
            default:
                return Step::Invalid;
            }
        case 'p':
            switch(instring[pos+3]) {
            case 'e':
                RETCMP(instring, pos, Step::AXISNAME_PARENT, Step::Parent, Step::Invalid);
            case 'c':
                if (instring.length() <= pos + 9) {
                    RETCMP(instring, pos, Step::AXISNAME_PRECEDING, Step::Preceding, Step::Invalid);
                } else switch(instring[pos+9]) {
                case '-':
                    RETCMP(instring, pos, Step::AXISNAME_PRECEDING_SIBLING, Step::Preceding_Sibling, Step::Invalid);
                default:
                    RETCMP(instring, pos, Step::AXISNAME_PRECEDING, Step::Preceding, Step::Invalid);
                }
            case 'v':
                RETCMP(instring, pos, Step::AXISNAME_PREVIOUS_SIBLING, Step::Previous_Sibling, Step::Invalid);
            }
        case 's':
            RETCMP(instring, pos, Step::AXISNAME_SELF, Step::Self, Step::Invalid);
        default:
            return Step::Invalid;
        }
    }

    Step::NodeTest Step::read_NodeTest(const string &instring, int pos) {
        switch(instring[pos]) {
        case 'n':
            RETCMP(instring, pos, Step::NODETEST_NODE, Step::TestAnyNode, Step::InvalidTest);
        case 'c':
            RETCMP(instring, pos, Step::NODETEST_COMMENT, Step::TestCommentNode, Step::InvalidTest);
        case 't':
            RETCMP(instring, pos, Step::NODETEST_TEXT, Step::TestTextNode, Step::InvalidTest);
        case 'p':
            RETCMP(instring, pos, Step::NODETEST_PI, Step::TestProcessingInstruction, Step::InvalidTest);
        default:
            return Step::InvalidTest;
        }
    }

    bool Step::allows(dom::Node *n)
    {
        if (nodeTestName.length()) {
            if (test == TestProcessingInstruction) {
                // ### node test name contains the parameter to the processing
                // instruction. handle accordingly.
            } else {
                if (n->nodeName() != nodeTestName) {
                    return false;
                }
            }
        }
        switch (test)
        {
        case TestPrincipalType:
            switch (axis)
            {
            case Attribute:
                return n->nodeType()==dom::Node::ATTRIBUTE_NODE;
            case Namespace:
                // ### not yet implemented, node type NAMESPACE does not exist.
                //return n->nodeType()==dom::Node::NAMESPACE_NODE;
            default:
                return n->nodeType()==dom::Node::ELEMENT_NODE;
            }
        case TestAnyNode:
            return true;
        case TestCommentNode:
            return n->nodeType()==dom::Node::COMMENT_NODE;
        case TestTextNode:
            return n->nodeType()==dom::Node::TEXT_NODE;
        case TestProcessingInstruction:
            return n->nodeType()==dom::Node::PROCESSING_INSTRUCTION_NODE;
        default:
            return false;
        };
    };

    template<class CONT>
    void fillAxis(Step *s, NodeRef curNode, CONT &cont)
    {
	std::insert_iterator<CONT> resultset = std::inserter(cont, cont.end());
        NodeRef origNode = curNode;
        // build up the axis with appropriate nodes containing passing the test
        switch(s->getAxis())
        {
        case Step::Child:
            if (curNode->hasChildNodes()) {
                for (curNode = &*curNode->firstChild(); curNode; curNode = &*curNode->nextSibling()) {
                    if (s->allows(curNode)) {
#ifdef INTERPRETER_DEBUG
			AC_TRACE << "appending " << *curNode << " into container of " << cont.size() << " nodes.";
#endif
                        *resultset++ = curNode;
                    }
                }
	    }
            break;
        case Step::Parent:
            curNode = curNode->parentNode();
            if (curNode && s->allows(curNode)) {
		*resultset++ = curNode;
            }
            break;
        case Step::Next_Sibling:
            curNode = &*curNode->nextSibling();
            if (curNode && s->allows(curNode)) {
		*resultset++ = curNode;
            }
            break;
        case Step::Previous_Sibling:
            curNode = &*curNode->previousSibling();
            if (curNode && s->allows(curNode)) {
		*resultset++ = curNode;
            }
            break;
        case Step::Preceding_Sibling:
            while (curNode = &*curNode->previousSibling()) {
                if (s->allows(curNode)) {
		    *resultset++ = curNode;
                }
            }
            break;
        case Step::Following_Sibling:
            while (curNode = &*curNode->nextSibling()) {
                if (s->allows(curNode)) {
		    *resultset++ = curNode;
                }
            }
            break;
        case Step::Descendant_Or_Self:
            if (s->allows(curNode)) {
		*resultset++ = curNode;
            }
            /* nobreak; */
        case Step::Descendant:
            {
                while (true)
                {
                    if (curNode->firstChild()){
                        curNode = &*curNode->firstChild();
                    } else if (curNode == origNode) {
                        break;
                    } else
        // yes, this is what we want.
        case Step::Following:
			if (curNode->nextSibling()) {
                        curNode = &*curNode->nextSibling();
                    } else {
                        NodeRef tmp = curNode;
                        while ((tmp = tmp->parentNode()) != origNode) {
                            if (tmp->nextSibling()) {
                                curNode = &*tmp->nextSibling();
                                break;
                            }
                        }
                        if (tmp == origNode) {
                            break;
                        }
                    };

                    if (s->allows(curNode)) {
#ifdef INTERPRETER_DEBUG
			AC_TRACE << "appending " << *curNode << " into container of " << cont.size() << " nodes.";
#endif
			*resultset++ = curNode;
                    }
                };
            };
	    break;
 	    while (true)
                {
                    if (curNode->firstChild()){
                        curNode = &*curNode->firstChild();
                    } else if (curNode == origNode) {
                        break;
                    } else
        case Step::Preceding:
			if (curNode->previousSibling()) {
                        curNode = &*curNode->previousSibling();
                    } else {
                        NodeRef tmp = curNode;
                        while ((tmp = tmp->parentNode()) != origNode) {
                            if (tmp->previousSibling()) {
                                curNode = &*tmp->nextSibling();
                                break;
                            }
                        }
                        if (tmp == origNode) {
                            break;
                        }
                    };

                    if (s->allows(curNode)) {
			*resultset++ = curNode;
                    }
                };
            break;
        case Step::Self:
            if (s->allows(curNode)) {
		*resultset++ = curNode;
            }
            break;
        case Step::Attribute:
            {
                dom::NamedNodeMap map = curNode->attributes();
                for (unsigned int i = 0; i < map.length(); i++)
                    if (s->allows(&*map.item(i)))
			*resultset++ = &*map.item(i);
            };
        case Step::Ancestor_Or_Self:
            if (s->allows(curNode)) {
		*resultset++ = curNode;
            }
            /* nobreak; */
        case Step::Ancestor:
            if (curNode->nodeType()==dom::Node::ATTRIBUTE_NODE)
            {
                /*
                  dom::Attribute curAttr = curNode;
                  curNode = curAttr.ownerElement();
                */
                curNode = curNode->parentNode();
                if (s->allows(curNode)) {
		    *resultset++ = curNode;
                }
            }
            while (curNode = curNode->parentNode()) {
                if (s->allows(curNode)) {
		    *resultset++ = curNode;
                }
            }
        case Step::Invalid:
        default:
            break;
        };
#ifdef INTERPRETER_DEBUG
	AC_TRACE << "evaluating step " << *s << " on " << *origNode << ": selected " << cont.size() << " candidates...";
#endif
    };

    template<class CONT>
    void scanStep(Step *s, NodeRef origNode, CONT &results)
    {
	bool have_predicates = (s->predicates.begin() != s->predicates.end());
	if (!have_predicates) {
	    fillAxis(s, origNode, results);
	    return;
	}

        NodeListRef intermediateResult = new NodeList();
	fillAxis(s, origNode, *intermediateResult);

#ifdef INTERPRETER_DEBUG
        AC_TRACE << "now evaluating predicates of " << *s;
        AC_TRACE << " on " << origNode->nodeName() << (origNode->parentNode() ? (" inside " + origNode->parentNode()->nodeName()):"") << ":";
        AC_TRACE << "starting with " << intermediateResult->size() << " " << Step::stringForAxis(s->getAxis()) << " nodes";

#endif
	std::list<Expression*>::iterator last = s->predicates.end(); 
	--last;

        Context subcontext;

        for (std::list<Expression*>::iterator i = s->predicates.begin(); i != last; ++i) {
#ifdef INTERPRETER_DEBUG
            AC_TRACE << "filtering by predicate [" << *(*i) << "] in a context of " << intermediateResult->size() << " nodes:";
#endif
            subcontext.size = intermediateResult->size();
            subcontext.position = 0;
            for (NodeList::iterator j = intermediateResult->begin(); j != intermediateResult->end();) {
#ifdef INTERPRETER_DEBUG
		AC_TRACE << "evaluating...";
#endif
                subcontext.position++;
                subcontext.currentNode = (*j);
                Value *tmp = (*i)->evaluateExpression(subcontext);
                BooleanValue *v = tmp->toBoolean();
                delete tmp;
                if (!v->getValue()) {
#ifdef INTERPRETER_DEBUG
                    AC_TRACE << " kicking out " << subcontext.currentNode->nodeName() << " " << subcontext.position << " of " << subcontext.size << " because the predicate [" << (**i) << "] evaluates false.";
#endif
                    intermediateResult->erase(j++);
                }
		else ++j;
                delete v;
            }
        }

	std::insert_iterator<CONT> ins = std::inserter(results, results.end());
#ifdef INTERPRETER_DEBUG
	AC_TRACE << "filtering by last predicate [" << *(*last) << "] in a context of " << intermediateResult->size() << " nodes:";
#endif
	subcontext.size = intermediateResult->size();
	subcontext.position = 0;
	for (NodeList::iterator j = intermediateResult->begin(); j != intermediateResult->end(); ++j) {
	    subcontext.position++;
	    subcontext.currentNode = (*j);
	    Value *tmp = (*last)->evaluateExpression(subcontext);
	    BooleanValue *v = tmp->toBoolean();
	    delete tmp;
	    if (v->getValue()) {
#ifdef INTERPRETER_DEBUG
		AC_TRACE << " picking up " << subcontext.currentNode->nodeName() << " " << subcontext.position << " of " << subcontext.size << " into container of " << results.size() << " elements";
		AC_TRACE << " because the predicate [" << (**last) << "] evaluates true.";
#endif
		*ins++ = (*j);
	    }
	    delete v;
	}
    };

    void Step::scan(NodeRef from, NodeSet &into) {
	scanStep(this, from, into);
    }

    void Step::scan(NodeRef from, NodeList &into) {
	scanStep(this, from, into);
    }

    void Step::scan(NodeRef from, OrderedNodeSet &into) {
	scanStep(this, from, into);
    }

    void Step::serializeTo(std::ostream & os) {
        os << stringForAxis(axis);
        os << "::";
        serializeNodeTest(os);
        for (std::list<Expression*>::iterator i = predicates.begin(); i != predicates.end(); ++i) {
            os << "[";
            (*i)->serializeTo(os);
            os << "]";
        };
    };

    void Step::appendPredicate(Expression *e) {
        predicates.push_back(e);
    };

    void Step::prependPredicate(Expression *e) {
        predicates.push_front(e);
    };

    void Step::insertPredicate(int at, Expression *e) {
        std::list<Expression*>::iterator i = predicates.begin();
        while (i != predicates.end() && at--) { ++i; }
        predicates.insert(i, e);
    };

    Expression* Step::takeFirst() { Expression *retval = predicates.front(); predicates.pop_front(); return retval; };
    Expression* Step::takeLast() { Expression *retval = predicates.back(); predicates.pop_back(); return retval; };
    Expression* Step::take(int at) {
        std::list<Expression*>::iterator i = predicates.begin();
        while (i != predicates.end() && at--) ++i;
        Expression *retval = *i;
        predicates.erase(i);
        return retval;
    };

    Expression* Step::predicate(int at) {
        std::list<Expression*>::iterator i = predicates.begin();
        while (i != predicates.end() && at--) ++i;
        return *i;
    };

    int Step::count() const { return predicates.size(); };

    Path::Path() {
        absolute = false;
    };

    Value* Path::evaluateExpression(const Context &c) {
        return evaluate(c.currentNode);
    };

    NodeSetValue *Path::evaluate(NodeRef n) {
        return new NodeSetValue(evaluateAs<NodeSet>(n));
    };

    void Path::evaluateInto(NodeSetRef workingset, OrderedNodeSet &returnContainer) {

        if (absolute) {
	    if (steps.begin() == steps.end()) {
#ifdef INTERPRETER_DEBUG
		AC_TRACE<<"abolute path with no steps. going up to document from... " << workingset->size() << " nodes.";
#endif
		for (NodeSet::iterator i = workingset->begin(); i != workingset->end();++i)
		    {
			NodeRef thisOne = *i;
			while (thisOne->parentNode()) { thisOne = thisOne->parentNode(); };
			returnContainer.insert(thisOne);
		    }
		delete workingset;
		return;
	    } else {
#ifdef INTERPRETER_DEBUG
		AC_TRACE<<"abolute path. going up to document from... " << workingset->size() << " nodes.";
#endif
		NodeSetRef newResults = new NodeSet();
		
		for (NodeSet::iterator i = workingset->begin(); i != workingset->end();++i)
		    {
			NodeRef thisOne = *i;
			while (thisOne->parentNode()) { thisOne = thisOne->parentNode(); };
			newResults->insert(thisOne);
		    }
		delete workingset;
		workingset = newResults;
	    }
        }

	std::list<Step*>::iterator lastStep = steps.end();
	lastStep--;
        for (std::list<Step*>::iterator s = steps.begin(); s != lastStep; ++s) {
#ifdef INTERPRETER_DEBUG
	    AC_TRACE << "scanning intermediate step " << (**s) << " with "<<workingset->size()<<" nodes into List:";
#endif
	    NodeSetRef nextStepSet = new NodeSet();
	    for (NodeSet::iterator i = workingset->begin(); i != workingset->end(); ++i) {
		(*s)->scan(*i, *nextStepSet);
	    };
	    delete workingset;
	    workingset = nextStepSet;
	}
#ifdef INTERPRETER_DEBUG
	AC_TRACE << "scanning last step " << (**lastStep) << " with "<<workingset->size()<<" nodes into List:";
#endif
	OrderedNodeSetRef finalResult = new OrderedNodeSet();
	for (NodeSet::iterator i = workingset->begin(); i != workingset->end(); ++i) {
	    (*lastStep)->scan(*i, returnContainer);
	};
    };

    void Path::evaluateInto(NodeSetRef workingset, NodeList &returnContainer) {

	// avoid duplicates in intermediate results
	OrderedNodeSetRef lastStepSet = new OrderedNodeSet();
	evaluateInto(workingset, *lastStepSet);

	// but allow them in returnContainer
	copy(lastStepSet->begin(), lastStepSet->end(), std::inserter(returnContainer, returnContainer.end()));
	delete lastStepSet;
    }

    void Path::evaluateInto(NodeSetRef workingset, NodeSet &returnContainer) {
        if (absolute) {
#ifdef INTERPRETER_DEBUG
            AC_TRACE<<"abolute path. going up to document from... " << workingset->size() << " nodes.";
#endif
            NodeSetRef newResults = new NodeSet();
	    NodeSet::iterator resIter = newResults->end();

            for (NodeSet::iterator i = workingset->begin(); i != workingset->end();++i)
            {
                NodeRef thisOne = *i;
                while (thisOne->parentNode()) { thisOne = thisOne->parentNode(); };
                newResults->insert(thisOne);
            }
            delete workingset;
            workingset = newResults;
        }

	std::list<Step*>::iterator lastStep = steps.end();
	lastStep--;
        for (std::list<Step*>::iterator s = steps.begin(); s != lastStep; ++s) {
#ifdef INTERPRETER_DEBUG
            AC_TRACE << "scanning step " << (**s) << " with "<<workingset->size()<<" nodes into Set:";
#endif
	    NodeSetRef nextStepSet = new NodeSet();
            for (NodeSet::iterator i = workingset->begin(); i != workingset->end(); ++i) {
                (*s)->scan(*i, *nextStepSet);
            };
            delete workingset;
            workingset = nextStepSet;
        };
#ifdef INTERPRETER_DEBUG
	AC_TRACE << "scanning last step " << (**lastStep) << " with "<<workingset->size()<<" nodes into List:";
#endif
	for (NodeSet::iterator i = workingset->begin(); i != workingset->end(); ++i) {
	    (*lastStep)->scan(*i, returnContainer);
	};
	delete workingset;
    };

    void Path::serializeTo(std::ostream &os) {
        for (std::list<Step*>::iterator s = steps.begin(); s != steps.end();++s) {
            if ((s != steps.begin()) || absolute) {
                os << "/";
            }
            (*s)->serializeTo(os);
        };
    };

};

std::ostream &operator<<(std::ostream &stream, xpath::SyntaxNode &myNode) {
    myNode.serializeTo(stream);
    return stream;
}
