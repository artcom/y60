#include <assert.h>

#include <iostream>
#include <math.h>

#include "xpath/syntaxtree.h"
#include <dom/Nodes.h>
#include "asl/Logger.h"

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

        // ## TODO
        Value *left = lvalue->evaluateExpression(c);
        Value *right = rvalue->evaluateExpression(c);

        assert(left);
        assert(right);

        if (type<16) // comparison operators
        {
        if ((left->type()==right->type()) && (right->type() ==Value::NodeSetType))
            {
                AC_ERROR << "Not yet implemented.";
		return new NullValue();
                // ## find one pair of string-values of one node each that evaluates to true.
            }
            else if (left->type()==Value::NodeSetType || right->type()==Value::NodeSetType)
            {
                // one nodeset, one other type
                if (left->type()==Value::StringType || right->type()==Value::StringType)
                {
                    // one nodeset, one string: find one node whose string-value compares positive to the string
                    NodeSetValue *nsv;
                    StringValue *sv;
                    bool stringleft;
                    if (left->type()==Value::StringType)
                    {
                        // ### WARNING: toNodeSet() invalidates right.
                        nsv = right->toNodeSet();
                        sv = left->toString();
                        stringleft = true;
                    }
                    else
                    {
                        // ### WARNING: toNodeSet() invalidates left.
                        nsv = left->toNodeSet();
                        sv = right->toString();
                        stringleft = false;
                    };

                    delete left;
                    delete right;
                    bool retval = false;
                    for (NodeSet::iterator i = nsv->begin(); i !=nsv->end(); ++i)
                    {
                        string curstr;
                        // ### TODO: calculate the string-value correctly.
                        // for elements and documents, this is the concatenation
                        // of the node values of all text descendants.
                        switch ((*i)->nodeType())
                        {
                        case dom::Node::ELEMENT_NODE:
                        case dom::Node::DOCUMENT_NODE:
                            curstr = (*i)->nodeValue();
                            break;
                        case dom::Node::ATTRIBUTE_NODE:
                        case dom::Node::TEXT_NODE:
                            curstr = (*i)->nodeValue();
                            break;
                        default:
                            curstr = "";
                            break;
                        }

                        if (((type==Equal || type==LEqual || type == GEqual) && curstr == sv->getValue()) || (((type == Less || type == LEqual) == !stringleft) && (curstr < sv->getValue())) || (((type == Greater || type == GEqual) == !stringleft) && (curstr > sv->getValue())))
                        {
                            retval = true;
                            break;
                        }
                    };

#ifdef INTERPRETER_DEBUG
                    if (retval) {
                        AC_TRACE << "string - nodeset comparison succeeded in expression: " << *this;
		    }
#endif
                    delete sv; delete nsv;
                    return new BooleanValue(retval);
                }
                else if (left->type()==Value::NumberType || right->type()==Value::NumberType)
                {
                    AC_ERROR << "Not yet implemented!";
                    // one nodeset, one number: find one node whose string-value converted to number compares positive
                }
                else if (left->type()==Value::BooleanType || right->type()==Value::BooleanType)
                {
                    AC_ERROR << "Not yet implemented!";
                    assert(false);
                    // one nodeset, one boolean: convert both to boolean
                    // return compare(left->toBoolean(), type, right->toBoolean);
                }
                else
                {
                    AC_ERROR << "Not yet implemented!";
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
                return new NumberValue(rn + ln);
            case Minus:
                return new NumberValue(rn - ln);
            case Times:
                return new NumberValue(rn * ln);
            case Div:
                return new NumberValue(rn / ln);
            case Mod:
                return new NumberValue(rn - ln * floor(rn / ln));
            default:
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
                Expression *thirdExpr = *(++i);
                assert(firstExpr);

                if (!secondExpr) {
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

                int secondNumberValue = int(secondNumber->getValue()+0.5);

                int resultlength;
                // +1 for the terminating null character, +1 for secondNumber starting at 1
                resultlength = firstString->getValue().length() - secondNumberValue;

                // +1 for the terminating null character
                if (thirdNumber && thirdNumber->getValue() + 1 < resultlength) {
                    resultlength = int(thirdNumber->getValue()+0.5) + 1;
		}

                string result = firstString->getValue().substr(secondNumberValue, resultlength);

                delete firstString;
                delete secondNumber;
                if (thirdNumber) {
                    delete thirdNumber;
		}

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
    
    Function::FunctionType Function::typeOf(string instring, int pos)
    {
        /*
          switch(instring[pos+3]) {
          case 'n': //count
            
          case 'a': //contains
            
          case 'c': //concat
            
          case 'r': //starts-with
            
          case 's': //substring, substring-before, substring-after
            
          case 'i': //position
            
          default:  //not
            
          }
        */

        if (!instring.compare("last"))
            return Last;
        else if (!instring.compare("position"))
            return Position;
        else if (!instring.compare("count"))
            return Count;
        else if (!instring.compare("starts-with"))
            return StartsWith;
        else if (!instring.compare("concat"))
            return Concat;
        else if (!instring.compare("contains"))
            return Contains;
        else if (!instring.compare("substring"))
            return Substring;
        else if (!instring.compare("substring-before"))
            return SubstringBefore;
        else if (!instring.compare("substring-after"))
            return SubstringAfter;
        else if (!instring.compare("not"))
            return Not;
        else return Unknown;
    };

    /*
      NodeTest::NodeTest(NodeTest::TestType _type, const string &_data)
      {
      type = _type;
      switch(type)
      {
      case Any:
      name = 0;
      break;
      case Name:
      name = strdup(_data);
      break;
      case Type:
      name = 0;
      if (!strcasecmp(_data, "comment"))
      nodeType = Comment;
      else if (!strcasecmp(_data, "text"))
      nodeType = Text;
      else if (!strcasecmp(_data, "ProcessingInstruction"))
      nodeType = ProcessingInstruction;
      else if (!strcasecmp(_data, "node"))
      nodeType = Node;
      else nodeType = Invalid;
      };
      };

    */

    bool Step::passesNodeTest(dom::Node *n)
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

#define RETCMP(instring, pos, X, yes, no) \
return (read_if_string(instring, pos, X) != pos) ? yes : no;

    Step::Axis Step::read_axis(const string &instring, int pos) {
        switch(instring[pos]) {
        case 'a':
            if (instring.length() <= pos + 8) {
                RETCMP(instring, pos, Step::AXISNAME_ANCESTOR, Step::Ancestor, Step::Invalid);
            } else switch(instring[pos+8]) {
            case '-':
                RETCMP(instring, pos, Step::AXISNAME_ANCESTOR_OR_SELF, Step::Ancestor_Or_Self, Step::Invalid);
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

    NodeSetRef Step::scan(NodeRef curNode)
    {
	NodeSetRef resultset = new NodeSet();
        NodeRef origNode = curNode;
        // build up the axis with appropriate nodes containing passing the test
        switch(axis)
        {
        case Child:
            if (curNode->hasChildNodes()) {
                for (curNode = &*curNode->firstChild(); curNode; curNode = &*curNode->nextSibling()) {
                    if (passesNodeTest(curNode)) {
                        resultset->insert(curNode);
		    }
		}
	    }
            break;
        case Parent:
            curNode = curNode->parentNode();
            if (curNode && passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            break;
        case Next_Sibling:
            curNode = &*curNode->nextSibling();
            if (curNode && passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            break;
        case Previous_Sibling:
            curNode = &*curNode->previousSibling();
            if (curNode && passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            break;
        case Preceding_Sibling:
            while (curNode = &*curNode->previousSibling()) {
                if (passesNodeTest(curNode)) {
                    resultset->insert(curNode);
		}
	    }
            break;
        case Following_Sibling:
            while (curNode = &*curNode->nextSibling()) {
                if (passesNodeTest(curNode)) {
                    resultset->insert(curNode);
		}
	    }
            break;
        case Descendant_Or_Self:
            if (passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            /* nobreak; */
        case Descendant:
            {
                while (true)
                {
                    if (curNode->firstChild()){
                        curNode = &*curNode->firstChild();
                    } else if (curNode == origNode) {
                        break;
                    } else if (curNode->nextSibling()) {
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

                    if (passesNodeTest(curNode)) {
                        resultset->insert(curNode);
		    }
                };
            };
#ifdef INTERPRETER_DEBUG
            AC_TRACE << "descendant axis yielded " << resultset->size() << " nodes.";
#endif
            break;
        case Self:
            if (passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            break;
        case Attribute:
            {
                dom::NamedNodeMap map = curNode->attributes();
                for (unsigned int i = 0; i < map.length(); i++)
                    if (passesNodeTest(&*map.item(i)))
                        resultset->insert(&*map.item(i));
            };
        case Ancestor_Or_Self:
            if (passesNodeTest(curNode)) {
                resultset->insert(curNode);
	    }
            /* nobreak; */
        case Ancestor:
            if (curNode->nodeType()==dom::Node::ATTRIBUTE_NODE)
            {
                /*
                  dom::Attribute curAttr = curNode;
                  curNode = curAttr.ownerElement();
                */
                curNode = curNode->parentNode();
                if (passesNodeTest(curNode)) {
                    resultset->insert(curNode);
		}
            }
            while (curNode = curNode->parentNode()) {
                if (passesNodeTest(curNode)) {
                    resultset->insert(curNode);
		}
	    }
        case Following:
        case Preceding:
        case Invalid:
        default:
            break;
        };

#ifdef INTERPRETER_DEBUG
	AC_TRACE << "selected " << resultset->size() << " " << Step::stringForAxis(axis) << " nodes"
		 << " of " << origNode->nodeName();
	if (origNode->parentNode()) {
	    AC_TRACE << " inside " << origNode->parentNode()->nodeName();
	}
#endif
        for (std::list<Expression*>::iterator i = predicates.begin(); i != predicates.end(); ++i) {
#ifdef INTERPRETER_DEBUG
            AC_TRACE << "evaluating predicate " << *(*i) << " on a set of " << resultset->size() << " nodes:";
#endif
            Context subcontext;
            subcontext.size = resultset->size();
            subcontext.position = 0;
            for (NodeSet::iterator j = resultset->begin(); j != resultset->end();) {
                subcontext.position++;
                subcontext.currentNode = (*j);
                Value *tmp = (*i)->evaluateExpression(subcontext);
                BooleanValue *v = tmp->toBoolean();
                delete tmp;
                if (!v->getValue()) {
#ifdef INTERPRETER_DEBUG
                    AC_TRACE << " kicking out " << subcontext.currentNode->nodeName() << " " << subcontext.position << " of " << subcontext.size << " because the predicate [" << (**i) << "] evaluates false.";
#endif
                    resultset->erase(j++);
                }
		else ++j;
                delete v;
            }
        }
	return resultset;
    };

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
        NodeSetRef initialSet = new NodeSet();
        initialSet->insert(n);

#ifdef INTERPRETER_DEBUG
        AC_TRACE<<"initial set contains " << initialSet->size() << "nodes.";
#endif

        NodeSetRef result = evaluateAll(initialSet);
        return new NodeSetValue(result);
    };

    NodeSetRef Path::evaluateAll(NodeSetRef workingset) {
        if (absolute) {
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

        for (std::list<Step*>::iterator s = steps.begin(); s != steps.end();++s) {
#ifdef INTERPRETER_DEBUG
            AC_TRACE << "scanning step " << (**s) << " with "<<workingset->size()<<" nodes :";
#endif
	    NodeSetRef nextStepSet = new NodeSet();
            for (NodeSet::iterator i = workingset->begin(); i != workingset->end(); ++i) {
               NodeSetRef newResults = (*s)->scan(*i);
	       for (NodeSet::iterator j = newResults->begin(); j != newResults->end(); ++j) {
		   nextStepSet->insert(*j);
	       }
	       delete newResults;
            };
            delete workingset;
            workingset = nextStepSet;
        };
        return workingset;
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
