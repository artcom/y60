//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Expression.h"

#include <cassert>
#include <functional>
#include <cmath>

#include <iostream>

#include <asl/Logger.h>
#include <dom/Nodes.h>

namespace {

    const std::string FUNCTIONNAME_LAST             = "last";
    const std::string FUNCTIONNAME_POSITION         = "position";
    const std::string FUNCTIONNAME_COUNT            = "count";
    const std::string FUNCTIONNAME_STARTSWITH       = "startswith";
    const std::string FUNCTIONNAME_CONCAT           = "concat";
    const std::string FUNCTIONNAME_CONTAINS         = "contains";
    const std::string FUNCTIONNAME_SUBSTRING        = "substring";
    const std::string FUNCTIONNAME_SUBSTRING_BEFORE = "substring_before";
    const std::string FUNCTIONNAME_SUBSTRING_AFTER  = "substring_after";
    const std::string FUNCTIONNAME_NOT              = "not";
    const std::string FUNCTIONNAME_UNKNOWN          = " *** unknown function *** ";
    const std::string FUNCTIONNAME_ID               = "id";
    const std::string FUNCTIONNAME_LOCAL_NAME       = "local-name";
    const std::string FUNCTIONNAME_NAMESPACE_URI    = "namespace-uri";
    const std::string FUNCTIONNAME_NAME             = "name";
    const std::string FUNCTIONNAME_STRING           = "std::string";
    const std::string FUNCTIONNAME_STRING_LENGTH    = "std::string-length";
    const std::string FUNCTIONNAME_NORMALIZE_SPACE  = "normalize-space";
    const std::string FUNCTIONNAME_TRANSLATE        = "translate";
    const std::string FUNCTIONNAME_BOOLEAN          = "boolean";
    const std::string FUNCTIONNAME_TRUE             = "true";
    const std::string FUNCTIONNAME_FALSE            = "false";
    const std::string FUNCTIONNAME_LANG             = "lang";
    const std::string FUNCTIONNAME_NUMBER           = "number";
    const std::string FUNCTIONNAME_SUM              = "sum";
    const std::string FUNCTIONNAME_FLOOR            = "floor";
    const std::string FUNCTIONNAME_CEILING          = "ceiling";
    const std::string FUNCTIONNAME_ROUND            = "round";

    const std::string AXISNAME_INVALID              = " *** invalid Axis *** ";
    const std::string AXISNAME_NEXT_SIBLING         = "next-sibling";
    const std::string AXISNAME_PREVIOUS_SIBLING     = "previous-sibling";
    const std::string AXISNAME_FOLLOWING_SIBLING    = "following-sibling";
    const std::string AXISNAME_PRECEDING_SIBLING    = "preceding-sibling";
    const std::string AXISNAME_CHILD                = "child";
    const std::string AXISNAME_PARENT               = "parent";
    const std::string AXISNAME_DESCENDANT           = "descendant";
    const std::string AXISNAME_ANCESTOR             = "ancestor";
    const std::string AXISNAME_FOLLOWING            = "following";
    const std::string AXISNAME_PRECEDING            = "preceding";
    const std::string AXISNAME_ANCESTOR_OR_SELF     = "ancestor-or-self";
    const std::string AXISNAME_DESCENDANT_OR_SELF   = "descendant-or-self";
    const std::string AXISNAME_SELF                 = "self";
    const std::string AXISNAME_NAMESPACE            = "namespace";
    const std::string AXISNAME_ATTRIBUTE            = "attribute";

    const std::string NODETEST_INVALID              = " *** invalid node test *** ";
    const std::string NODETEST_NODE                 = "node";
    const std::string NODETEST_COMMENT              = "comment";
    const std::string NODETEST_TEXT                 = "text";
    const std::string NODETEST_PI                   = "processing-instruction";

    template< typename ENUM >
    inline ENUM retcmp( const std::string& instring
                      , std::string::size_type pos
                      , const std::string& X
                      , ENUM yes, ENUM no) 
    {
        return asl::read_if_string(instring, pos, X) != pos ? yes : no;
    }

    template<class ITER>
    void descendReverse(ITER &resultset, dom::NodePtr curNode) {
        for (dom::NodePtr curChild = curNode->lastChild(); curChild; curChild = curChild->previousSibling())
        {
            descendReverse(resultset, curChild);
        }
        *resultset++ = curNode;
    }

    template<class CONT>
    void fillAxis(const xpath::Step& s, dom::NodePtr curNode, CONT &cont) {
        using namespace xpath;
        std::insert_iterator<CONT> resultset = std::inserter(cont, cont.end());
        dom::NodePtr origNode = curNode;
        // build up the axis with appropriate nodes containing passing the test
        switch(s.getAxis()) {
            case Step::Child:
                if (curNode->hasChildNodes()) {
                    for (curNode = curNode->firstChild(); curNode; curNode = curNode->nextSibling()) {
                        if (s.allows(curNode)) {
                            *resultset++ = curNode;
                        }
                    }
                }
                break;
            case Step::Parent:
                curNode = curNode->parentNode()->self().lock();
                if (curNode && s.allows(curNode)) {
                    *resultset++ = curNode;
                }
                break;
            case Step::Next_Sibling:
                curNode = curNode->nextSibling();
                if (curNode && s.allows(curNode)) {
                    *resultset++ = curNode;
                }
                break;
            case Step::Previous_Sibling:
                curNode = curNode->previousSibling();
                if (curNode && s.allows(curNode)) {
                    *resultset++ = curNode;
                }
                break;
            case Step::Preceding_Sibling:
                while (curNode = curNode->previousSibling()) {
                    if (s.allows(curNode)) {
                        *resultset++ = curNode;
                    }
                }
                break;
            case Step::Following_Sibling:
                while (curNode = curNode->nextSibling()) {
                    if (s.allows(curNode)) {
                        *resultset++ = curNode;
                    }
                }
                break;
            case Step::Following:
                origNode = dom::NodePtr();
                /* nobreak; */
            case Step::Descendant_Or_Self:
                if (origNode && s.allows(origNode)) {
                    *resultset++ = origNode;
                }
                /* nobreak; */
            case Step::Descendant:
                {
                    while (true) {
                        if (curNode->firstChild()) {
                            curNode = curNode->firstChild();
                        } else if (curNode == origNode) {
                            break;
                        } else if (curNode->nextSibling()) {
                            curNode = curNode->nextSibling();
                        } else {
                            dom::NodePtr tmp = curNode;
                            while ( (tmp = tmp->parentNode()->self().lock()) && tmp != origNode) {
                                if (tmp->nextSibling()) {
                                    curNode = tmp->nextSibling();
                                    break;
                                }
                            }
                            if ( !tmp || tmp == origNode ) {
                                break;
                            }
                        }

                        if (s.allows(curNode)) {
                            *resultset++ = curNode;
                        }
                    }
                }
                break;
            case Step::Preceding:
                for ( ; curNode; curNode = curNode->parentNode()->self().lock() ) {
                    while (curNode->previousSibling()) {
                        curNode = curNode->previousSibling();
                        AC_TRACE << " diving into " << *curNode;
                        descendReverse(resultset, curNode);
                    }
                }
                break;
            case Step::Self:
                if (s.allows(curNode)) {
                    *resultset++ = curNode;
                }
                break;
            case Step::Attribute:
                {
                    dom::NamedNodeMap & map = curNode->attributes();
                    for (unsigned int i = 0; i < map.length(); i++) {
                        if (s.allows(map.item(i))) {
                            *resultset++ = map.item(i);
                        }
                    }
                }
            case Step::Ancestor_Or_Self:
                if (s.allows(curNode)) {
                    *resultset++ = curNode;
                }
                /* nobreak; */
            case Step::Ancestor:
                if (curNode->nodeType()==dom::Node::ATTRIBUTE_NODE) {
                    /*
                       dom::Attribute curAttr = curNode;
                       curNode = curAttr.ownerElement();
                     */
                    curNode = curNode->parentNode()->self().lock();
                    if (s.allows(curNode)) {
                        *resultset++ = curNode;
                    }
                }
                while (curNode = curNode->parentNode()->self().lock()) {
                    if (s.allows(curNode)) {
                        *resultset++ = curNode;
                    }
                }
            case Step::Invalid:
            default:
                break;
        }
    };

    template<class CONT>
    void scanStep(const xpath::Step& s, dom::NodePtr origNode, CONT &results) {
        using namespace xpath;
        if ( 0 == s.count() ) {
            fillAxis(s, origNode, results);
            return;
        }

        NodeVector intermediateResult;
        fillAxis(s, origNode, intermediateResult);

        Step::const_iterator last = s.end(); 
        --last;

        Context subcontext;

        for (Step::const_iterator itS = s.begin(); itS != last; ++itS) {
            subcontext.size = intermediateResult.size();
            subcontext.position = 0;
            for (NodeVector::iterator itNV = intermediateResult.begin(); itNV != intermediateResult.end();) {
                subcontext.position++;
                subcontext.currentNode = (*itNV);
                ValuePtr tmp = (*itS)->evaluateExpression(subcontext);
                BooleanValue v = tmp->as<BooleanValue>();
                if (!v.getValue()) {
                    intermediateResult.erase(itNV++);
                }
                else ++itNV;
            }
        }

        std::insert_iterator<CONT> ins = std::inserter(results, results.end());
        subcontext.size = intermediateResult.size();
        subcontext.position = 0;
        for (NodeVector::const_iterator itNV = intermediateResult.begin(); itNV != intermediateResult.end(); ++itNV) {
            subcontext.position++;
            subcontext.currentNode = (*itNV);
            ValuePtr tmp = (*last)->evaluateExpression(subcontext);
            BooleanValue v = tmp->as<BooleanValue>();
            if (v.getValue()) {
                *ins++ = (*itNV);
            }
        }
    };

}

namespace xpath
{

    ValuePtr BinaryExpression::evaluateExpression(const Context &c) const {
        assert(_lvalue);
        assert(_rvalue);

        ValuePtr left  = _lvalue->evaluateExpression(c);
        ValuePtr right = _rvalue->evaluateExpression(c);

        assert(left);
        assert(right);

        if (_type < 16) { // comparison operators
            if ((left->type()==Value::NodeSetType) &&
                    (right->type()==Value::NodeSetType))
            {
                // find one pair of nodes whose std::string-values comparison evaluates true.

                NodeSetPtr nsr1 = left ->as<NodeSetValue>().takeNodes();
                NodeSetPtr nsr2 = right->as<NodeSetValue>().takeNodes();

                bool retval = false;

                if (_type == Equal || _type == NotEqual) {

                    std::set<std::string> sm1;
                    std::set<std::string> sm2;

                    NodeSet::const_iterator i1 = nsr1->begin();
                    NodeSet::const_iterator i2 = nsr2->begin();

                    while (i1 != nsr1->end() || i2 != nsr2->end()) {

                        const std::string& string1 = string_value_for(*i1);
                        if (!string1.empty()) {
                            if (sm2.find(string1) != sm2.end()) {
                                if (_type == Equal) {
                                    retval = true;
                                    break;
                                }
                            } else {
                                if (_type == NotEqual) {
                                    retval = true;
                                    break;
                                }
                            }
                            sm1.insert(string1);
                        }

                        std::string string2 = string_value_for(*i2);
                        if (!string2.empty()) {
                            if (sm1.find(string2) != sm1.end()) {
                                if (_type == Equal) {
                                    retval = true;
                                    break;
                                }
                            } else {
                                if (_type == NotEqual) {
                                    retval = true;
                                    break;
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
                    NodeSet::const_iterator ileft = nsr1->begin();
                    NodeSet::const_iterator iright = nsr2->begin();
                    while (ileft != nsr1->end() || ileft != nsr2->end()) {

                        if ( ( (_type==Less || _type==LEqual) 
                            && (string_value_for(*ileft) > extreme_left) )
                          ||   (_type == Greater || _type == GEqual)
                            && (string_value_for(*ileft) < extreme_left) ) {
                            extreme_left = string_value_for(*ileft);
                        }

                        if ( ( (_type == Less || _type == LEqual) 
                            && (string_value_for(*iright) < extreme_right) )
                          ||   (_type == Greater || _type == GEqual) 
                            && (string_value_for(*iright) > extreme_right) ) {
                            extreme_right = string_value_for(*iright);
                        }

                        if (extreme_left < extreme_right) {
                            if (_type == Less || _type == LEqual) {
                                retval = true;
                                break;
                            }
                        } else if (extreme_left == extreme_right) {
                            if (_type == LEqual || _type == GEqual) {
                                retval = true;
                                break;
                            }
                        } else {
                            if (_type == Greater || _type == GEqual) {
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
                return ValuePtr(new BooleanValue(retval));
            } else if ( left->type()==Value::NodeSetType 
                     || right->type()==Value::NodeSetType ) {
                // comparison of one nodeset with one other _type

                if (left->type()==Value::StringType || right->type()==Value::StringType) {
                    const int TRUTHVALUES_LEFT[] = { 1 << NotEqual | 1 << Less    | 1 << LEqual
                                                   , 1 << Equal    | 1 << LEqual  | 1 << GEqual
                                                   , 1 << NotEqual | 1 << Greater | 1 << GEqual };
                    const int TRUTHVALUES_RIGHT[] = { 
                        TRUTHVALUES_LEFT[2], TRUTHVALUES_LEFT[1], TRUTHVALUES_LEFT[0]
                    };

                    // one nodeset, one std::string: find one node whose std::string-value compares positive to the std::string
                    NodeSetValue nsv;
                    StringValue sv;
                    const int *truthTable;
                    if (left->type()==Value::StringType) {
                        nsv = right->as<NodeSetValue>();
                        sv = left->as<StringValue>();
                        truthTable = TRUTHVALUES_LEFT;
                    } else {
                        nsv = left->as<NodeSetValue>();
                        sv = right->as<StringValue>();
                        truthTable = TRUTHVALUES_RIGHT;
                    }

                    bool retval = false;

                    for (NodeSet::const_iterator it = nsv.begin(); it !=nsv.end(); ++it) {
                        std::string curstr = string_value_for(*it);
                        int cmp = curstr.compare(sv.getValue());
                        if (cmp < 0) cmp = -1;
                        if (cmp > 0) cmp = 1;
                        AC_TRACE << "  comparing std::string \"" << sv.getValue() <<
                                    "\" with node of value \"" <<curstr<<"\" is " << cmp;
                        if ( truthTable[ cmp+1 ] & ( 1 << _type )) {
                            retval = true;
                            break;
                        }
                    }

                    return ValuePtr(new BooleanValue(retval));
                } else if ( left->type() == Value::NumberType
                         || right->type() == Value::NumberType ) {
                    // one nodeset, one number: find one node whose std::string-value converted to number compares positive

                    ValuePtr theNodeSet;
                    ValuePtr theNumber;

                    if (left->type()==Value::NumberType) {
                        theNodeSet = right;
                        theNumber = left;
                    } else {
                        theNodeSet = left;
                        theNumber = right;
                    }
                    NumberValue nv = theNumber->as<NumberValue>();
                    NodeSetValue nsv = theNodeSet->as<NodeSetValue>();

                    double num = nv.getValue();
                    NodeSetPtr nsr = nsv.takeNodes();

                    //std::binary_function<dom::NodePtr, double, bool> theOp = getOpFor<double>(_type);

                    for (NodeSet::const_iterator it = nsr->begin(); it != nsr->end(); ++it) {
                        bool val;
                        double currentnum = number_value_for(*it);
                        switch(_type) {
                            case Equal:
                                val = (currentnum == num);
                                break;
                            case NotEqual:
                                val = (currentnum != num);
                                break;
                            case Greater:
                                val = (theNumber == left) ^ (number_value_for(*it) > num);
                                break;
                            case GEqual:
                                val = (theNumber == left) ^ (number_value_for(*it) >= num);
                                break;
                            case Less:
                                val = (theNumber == left) ^ (number_value_for(*it) < num);
                                break;
                            case LEqual:
                                val = (theNumber == left) ^ (number_value_for(*it) <= num);
                                break;
                            default:
                                AC_WARNING << "unsupported type " << _type;
                                assert(0); // XXX: replace with exception [DS]
                        }

                        if (val) {
                            return ValuePtr(new BooleanValue(true));
                        }
                    }
                    return ValuePtr(new BooleanValue(false));
                } else if ( left->type()==Value::BooleanType
                         || right->type()==Value::BooleanType ) {
                    // one nodeset, one boolean: convert both to boolean
                    bool a = left ->as<BooleanValue>().getValue();
                    bool b = right->as<BooleanValue>().getValue();

                    AC_TRACE << "boolean comparison " << _type << " " << a << ", " << b;

                    // XXX: WTF? Why are all of them true? [DS]
                    if (_type == Equal && a == b) return ValuePtr(new BooleanValue(true));
                    else if(_type == Equal && a != b) return ValuePtr(new BooleanValue(true)); // XXX
                    else if(_type == NotEqual && a != b) return ValuePtr(new BooleanValue(true));
                    else if(_type == GEqual && a >= b) return ValuePtr(new BooleanValue(true));
                    else if(_type == LEqual && a >= b) return ValuePtr(new BooleanValue(true)); // XXX
                    else if(_type == Less && a < b) return ValuePtr(new BooleanValue(true));
                    else if(_type == Greater && a > b) return ValuePtr(new BooleanValue(true));
                    else return ValuePtr(new BooleanValue(false));
                } else {
                    // should not happen.
                    AC_ERROR << "Type not implemented!";
                    assert(false);
                }
                return ValuePtr(new NullValue());
            } else {// no nodeset 
                if (_type==Equal || _type == NotEqual) {
                    // convert to simplest common type and compare
                    bool retval;
                    if (left->type() == Value::BooleanType || right->type() == Value::BooleanType) {
                        retval = equal_as<BooleanValue>(*left, *right);
                    } else if (left->type() == Value::NumberType || right->type() == Value::NumberType) {
                        retval = equal_as<NumberValue>(*left, *right);
                    } else {
                        retval = equal_as<StringValue>(*left, *right);
                    }
                    return ValuePtr( new BooleanValue(retval ^ (_type==NotEqual)) );
                } else { //  no nodeset and no equality operation
                    if (equal_as<NumberValue>(*left, *right)) {
                        return ValuePtr( new BooleanValue(_type==LEqual||_type==GEqual) );
                    } else { // numbers are different.
                        bool retval = ( _type==GEqual || _type == Greater ) ^
                            ( smaller_than_as<NumberValue>(*left, *right));
                        return ValuePtr(new BooleanValue(retval));
                    }
                }
            }
        } else if (_type < 32) { // "or" | "and"
            bool rb = right->as<BooleanValue>().getValue();
            bool lb = left ->as<BooleanValue>().getValue();
            if (_type==Or) {
                return ValuePtr( new BooleanValue(rb || lb) );
            } else if (_type==And) {
                return ValuePtr( new BooleanValue(rb && lb) );
            } else {
                // should not happen.
                AC_ERROR << "Not yet implemented!";
                assert(false);
            }
        } else { // "+", "-", "*", "/", "mod"
            double rn = right->as<NumberValue>().getValue();
            double ln = left ->as<NumberValue>().getValue();
            switch(_type) {
                case Plus : return ValuePtr( new NumberValue(ln + rn) );
                case Minus: return ValuePtr( new NumberValue(ln - rn) );
                case Times: return ValuePtr( new NumberValue(ln * rn) );
                case Div  : return ValuePtr( new NumberValue(ln / rn) );
                case Mod  : return ValuePtr( new NumberValue(ln - rn * floor(ln / rn)) );
                default:
                    // should not happen.
                    AC_ERROR << "Not yet implemented.";
                    assert(false);
            }
        }
        return ValuePtr( new NullValue() );
    }

    void BinaryExpression::serializeTo(std::ostream &os) const {
        BinaryExpressionPtr lv = dynamic_cast_Ptr<BinaryExpression>(_lvalue);
        if (lv) {
            os << "(";
        }
        _lvalue->serializeTo(os);
        if (lv) {
            os << ")";
        }

        switch(_type) {
            case Equal   : os << " = "  ; break;
            case NotEqual: os << " != " ; break;
            case Greater : os << " > "  ; break;
            case GEqual  : os << " >= " ; break;
            case Less    : os << " < "  ; break;
            case LEqual  : os << " <= " ; break;
            case Or      : os << " or " ; break;
            case And     : os << " and "; break;
            case Plus    : os << " + "  ; break;
            case Minus   : os << " - "  ; break;
            case Times   : os << " * "  ; break;
            case Div     : os << " div "; break;
            case Mod     : os << " mod "; break;
            default:
                os << " ** unknown operator ** ";
                break;
        }
        BinaryExpressionPtr rv = dynamic_cast_Ptr<BinaryExpression>(_rvalue);
        if (rv) {
            os << "(";
        }
        _rvalue->serializeTo(os);
        if (rv) {
            os << ")";
        }
    }

    void UnaryExpression::serializeTo(std::ostream &os) const {
        switch(_type) {
            case Minus : os << "-"; break;
            case Tilde : os << "~"; break;
            case Not   : os << "!"; break;
            default:
                AC_ERROR << "Not yet implemented!";
                assert(false);
        };
        _argument->serializeTo(os);
    }

    ValuePtr UnaryExpression::evaluateExpression(const Context &c) const {
        if (_type==Minus) {
            NumberValue retval = _argument->evaluateExpression(c)->as<NumberValue>().negate();
            return ValuePtr( new NumberValue(retval) );
        } else {
            AC_ERROR << "Expression not supported!";
            assert(false);
        }
        return ValuePtr( new NullValue() );
    }

    void Function::serializeTo(std::ostream &os) const {
        os << nameOf(_type);
        os << "(";

        ArgumentList::const_iterator it = _arguments.begin();
        if (it != _arguments.end()) {
            (*it)->serializeTo(os);

            for (++it; it != _arguments.end(); ++it) {
                os << ", ";
                (*it)->serializeTo(os);
            }
        }
        os << ")";
    }

    ValuePtr Function::evaluateExpression(const Context &c) const {
        switch(_type) {
            case Last:
                return ValuePtr( new NumberValue(c.size) );
            case Position:
                return ValuePtr( new NumberValue(c.position) );
            case Count:
                {
                    if (_arguments.empty()) {
                        return ValuePtr( new NumberValue(0) );
                    }

                    ValuePtr v = _arguments.front()->evaluateExpression(c);

                    AC_TRACE << "counting \"" << *(_arguments.front()) <<"\"...";

                    // convert into a nodeSet
                    NodeSetValuePtr nsv = dynamic_cast_Ptr<NodeSetValue>(v);

                    if (!nsv) {
                        AC_WARNING << " ... failed: argument not a node-set!";
                        return ValuePtr( new NumberValue(0) );
                    }

                    // and count the nodeSet
                    int retval = nsv->length();
                    AC_TRACE << " returning: " << retval;
                    return ValuePtr( new NumberValue(retval) );
                }

            case StartsWith:
                {
                    if ( _arguments.empty() ) {
                        AC_WARNING << "Warning: " << *this << " called with zero arguments.";
                        return ValuePtr( new NullValue() );
                    }

                    ArgumentList::const_iterator it = _arguments.begin();
                    ExpressionPtr firstExpr = *it;
                    ExpressionPtr secondExpr = *(++it);
                    assert(firstExpr);

                    if (!secondExpr) {
                        AC_WARNING << "Warning: " << *this << " called with only one argument.";
                        return ValuePtr( new NullValue() );
                    }

                    ValuePtr first = firstExpr->evaluateExpression(c);
                    ValuePtr second = secondExpr->evaluateExpression(c);

                    return ValuePtr( new BooleanValue( equal_as<StringValue>(*first, *second) ) );
                }

            case Concat:
                {
                    if ( _arguments.empty() ) {
                        return ValuePtr( new NullValue() );
                    }

                    std::string retval;
                    for (ArgumentList::const_iterator it = _arguments.begin(); it != _arguments.end(); ++it) {
                        retval+=(*it)->evaluateExpression(c)->as<StringValue>().getValue();
                    }
                    return ValuePtr( new StringValue(retval) );
                }

            case Contains:
                {
                    if ( _arguments.empty()) {
                        return ValuePtr( new NullValue() );
                    }

                    ArgumentList::const_iterator it = _arguments.begin();
                    ExpressionPtr firstExpr = *it;
                    ExpressionPtr secondExpr = *(++it);

                    if (!secondExpr) {
                        AC_WARNING << "ERROR: parameter missing to function call " << *this;
                        return ValuePtr( new NullValue() );
                    }

                    StringValue firstString  = firstExpr ->evaluateExpression(c)->as<StringValue>();
                    StringValue secondString = secondExpr->evaluateExpression(c)->as<StringValue>();

                    const bool retval = firstString.getValue().find(secondString.getValue()) >= 0;

                    return ValuePtr( new BooleanValue(retval) );
                }

            case Substring:
                {
                    if (_arguments.empty()) {
                        return ValuePtr( new NullValue() );
                    }

                    ArgumentList::const_iterator it = _arguments.begin();
                    ExpressionPtr firstExpr = *it;
                    ExpressionPtr secondExpr = *(++it);
                    ExpressionPtr thirdExpr;
                    if (++it != _arguments.end()) {
                        AC_TRACE << " three arguments for " << *this;
                        thirdExpr = *it;
                    }
                    assert(firstExpr);

                    if (!secondExpr) {
                        AC_TRACE << "substring expression: second argument is not a number.";
                        return ValuePtr( new NullValue() );
                    }

                    ValuePtr third;
                    if (thirdExpr) {
                        third = thirdExpr->evaluateExpression(c);
                    }

                    StringValue firstString  = firstExpr ->evaluateExpression(c)->as<StringValue>();
                    NumberValue secondNumber = secondExpr->evaluateExpression(c)->as<NumberValue>();
                    NumberValue thirdNumber;
                    if (thirdExpr) {
                        thirdNumber= third->as<NumberValue>();
                    }

                    int secondNumberValue = int(secondNumber.getValue()+0.5) - 1;
                    if (secondNumberValue < 0) {
                        AC_WARNING << "substring: round(secondNumberValue->getValue()+0.5) = " << secondNumberValue+1 << "is less than 1!";
                        secondNumberValue = 0;
                    }

                    AC_TRACE << "substring: complete std::string is \"" << firstString.getValue() << "\"";
                    int resultlength = firstString.getValue().length() - secondNumberValue;

                    // +1 for the terminating null character
                    if ( thirdNumber.getValue() + 1 < resultlength ) {
                        resultlength = int(thirdNumber.getValue()+0.5) + 1;
                    }

                    AC_TRACE << "substring: length = " << resultlength;

                    std::string result = firstString.getValue().substr(secondNumberValue, resultlength);

                    AC_TRACE << "substring expression evaluates to \"" << result << "\"";
                    return ValuePtr( new StringValue(result) );

                }
            case SubstringBefore:
                {
                    if (_arguments.empty()) {
                        return ValuePtr( new NullValue() );
                    }

                    ArgumentList::const_iterator it = _arguments.begin();
                    ExpressionPtr firstExpr = *it;
                    ExpressionPtr secondExpr = *(++it);
                    assert(firstExpr);

                    if (!secondExpr) {
                        return ValuePtr( new NullValue() );
                    }

                    StringValue firstString  = firstExpr ->evaluateExpression(c)->as<StringValue>();
                    StringValue secondString = secondExpr->evaluateExpression(c)->as<StringValue>();

                    const std::string::size_type pos = firstString.getValue().find(secondString.getValue());

                    return StringValuePtr( new StringValue(firstString.getValue().substr(0, pos)) );
                }
            case SubstringAfter:
                {
                    if (_arguments.empty()) {
                        return ValuePtr( new NullValue() );
                    }

                    ArgumentList::const_iterator it = _arguments.begin();
                    ExpressionPtr firstExpr = *it;
                    ExpressionPtr secondExpr = *(++it);

                    if (!secondExpr) {
                        return ValuePtr( new NullValue() );
                    }

                    StringValue firstString  = firstExpr->evaluateExpression(c) ->as<StringValue>();
                    StringValue secondString = secondExpr->evaluateExpression(c)->as<StringValue>();

                    const std::string::size_type pos = firstString.getValue().find(secondString.getValue());

                    return ValuePtr(  new StringValue(secondString.getValue().substr(pos+secondString.getValue().length())) );
                }
            case True:
                return ValuePtr( new BooleanValue(true) );
            case False:
                return ValuePtr( new BooleanValue(false) );
            case Not:
                    return ValuePtr( new BooleanValue(_arguments.front()->evaluateExpression(c)->as<BooleanValue>()) );
            case Boolean:
                    return ValuePtr( new BooleanValue(_arguments.front()->evaluateExpression(c)->as<BooleanValue>()) );
            case Number:
                    return ValuePtr( new NumberValue(_arguments.front()->evaluateExpression(c)->as<NumberValue>()) );
            case String:
                    return ValuePtr( new StringValue(_arguments.front()->evaluateExpression(c)->as<StringValue>()) );
            case Sum:
                {
                    NodeSetPtr nodes = _arguments.front()->evaluateExpression(c)->as<NodeSetValue>().takeNodes();
                    double retval = 0;
                    for( NodeSet::const_iterator it = nodes->begin(); it != nodes->end(); ++it) {
                        retval += number_value_for(*it);
                    }
                    return ValuePtr( new NumberValue(retval) );
                }
            case Floor:
                    return ValuePtr( new NumberValue( std::floor(_arguments.front()->evaluateExpression(c)->as<NumberValue>().getValue()) ) );
            case Ceiling:
                    return ValuePtr( new NumberValue( std::ceil (_arguments.front()->evaluateExpression(c)->as<NumberValue>().getValue()) ) );
            case Round:
                    return ValuePtr( new NumberValue( std::floor(_arguments.front()->evaluateExpression(c)->as<NumberValue>().getValue()) ) );
            case StringLength:
                    return ValuePtr( new NumberValue( _arguments.front()->evaluateExpression(c)->as<StringValue>().getValue().length() ) );
            case NormalizeSpace:
                {
                    const std::string& s = _arguments.front()->evaluateExpression(c)->as<StringValue>().getValue();
                    std::string retval;
                    for (std::string::size_type i = asl::read_whitespace(s, 0); i < s.length(); i++) {
                        retval+=s[i];
                        i = asl::read_whitespace(s, i);
                    }
                    return ValuePtr( new StringValue(s) );
                }
            case Translate:
                {
                    ArgumentList::const_iterator arg = _arguments.begin();
                    const std::string&    s = (*arg)->evaluateExpression(c)->as<StringValue>().getValue();
                    const std::string& from = (*(++arg))->evaluateExpression(c)->as<StringValue>().getValue();
                    const std::string& to   = (*(++arg))->evaluateExpression(c)->as<StringValue>().getValue();

                    std::string retval;
                    for (std::string::size_type i = 0; i < s.length(); i++) {
                        std::string::size_type pos;
                        if ((pos = from.find(s[i])) && pos < to.length()) {
                            retval+=to[pos];
                        } else {
                            retval+=s[i];
                        }
                    }
                    return ValuePtr( new StringValue(s) );
                }
            case Id:
            case Name:
            case LocalName:
            case NamespaceURI:
            case Lang:
            case Unknown:
            default:
                AC_ERROR << " ** access to unknown function ** ";
                return ValuePtr( new BooleanValue(false) );
        };
    };

    std::string Function::nameOf(Function::FunctionType ft) {
        switch(ft) {
            case Last            : return FUNCTIONNAME_LAST;
            case Position        : return FUNCTIONNAME_POSITION;
            case Count           : return FUNCTIONNAME_COUNT;
            case StartsWith      : return FUNCTIONNAME_STARTSWITH;
            case Concat          : return FUNCTIONNAME_CONCAT;
            case Contains        : return FUNCTIONNAME_CONTAINS;
            case Substring       : return FUNCTIONNAME_SUBSTRING;
            case SubstringBefore : return FUNCTIONNAME_SUBSTRING_BEFORE;
            case SubstringAfter  : return FUNCTIONNAME_SUBSTRING_AFTER;
            case Not             : return FUNCTIONNAME_NOT;
            case Id              : return FUNCTIONNAME_ID;
            case LocalName       : return FUNCTIONNAME_LOCAL_NAME;
            case NamespaceURI    : return FUNCTIONNAME_NAMESPACE_URI;
            case Name            : return FUNCTIONNAME_NAME;
            case String          : return FUNCTIONNAME_STRING;
            case StringLength    : return FUNCTIONNAME_STRING_LENGTH;
            case NormalizeSpace  : return FUNCTIONNAME_NORMALIZE_SPACE;
            case Translate       : return FUNCTIONNAME_TRANSLATE;
            case Boolean         : return FUNCTIONNAME_BOOLEAN;
            case True            : return FUNCTIONNAME_TRUE;
            case False           : return FUNCTIONNAME_FALSE;
            case Lang            : return FUNCTIONNAME_LANG;
            case Number          : return FUNCTIONNAME_NUMBER;
            case Sum             : return FUNCTIONNAME_SUM;
            case Floor           : return FUNCTIONNAME_FLOOR;
            case Ceiling         : return FUNCTIONNAME_CEILING;
            case Round           : return FUNCTIONNAME_ROUND;
            default              : return FUNCTIONNAME_UNKNOWN;
        }
    }

    Function::FunctionType Function::typeOf(const std::string& instring, std::string::size_type pos) {
        switch(instring[pos]) {
            case 'l':
                switch(instring[pos+2]) {
                    case 's': return retcmp(instring, pos, FUNCTIONNAME_LAST      , Function::Last     , Function::Unknown);
                    case 'n': return retcmp(instring, pos, FUNCTIONNAME_LANG      , Function::Lang     , Function::Unknown);
                    case 'c': return retcmp(instring, pos, FUNCTIONNAME_LOCAL_NAME, Function::LocalName, Function::Unknown);
                    default : return Function::Unknown;
                }
            case 'p': return retcmp(instring, pos, FUNCTIONNAME_POSITION, Function::Position, Function::Unknown);
            case 'c':
                switch(instring[pos+3]) {
                    case 'n': return retcmp(instring, pos, FUNCTIONNAME_COUNT   , Function::Count   , Function::Unknown);
                    case 'l': return retcmp(instring, pos, FUNCTIONNAME_CEILING , Function::Ceiling , Function::Unknown);
                    case 'c': return retcmp(instring, pos, FUNCTIONNAME_CONCAT  , Function::Concat  , Function::Unknown);
                    case 't': return retcmp(instring, pos, FUNCTIONNAME_CONTAINS, Function::Contains, Function::Unknown);
                    default : return Function::Unknown;
                }
            case 's':
                switch(instring[pos+2]) {
                    case 'a': return retcmp(instring, pos, FUNCTIONNAME_STARTSWITH, Function::StartsWith, Function::Unknown);
                    case 'm': return retcmp(instring, pos, FUNCTIONNAME_SUM       , Function::Sum       , Function::Unknown);
                    case 'r': 
                        if (instring.length() > pos + 6 && instring[pos+6] == '-') {
                            return retcmp(instring, pos, FUNCTIONNAME_STRING_LENGTH, Function::StringLength, Function::Unknown);
                        } else {
                            return retcmp(instring, pos, FUNCTIONNAME_STRING       , Function::String      , Function::Unknown);
                        }
                    case 'b':
                        if (instring.length() > pos + 9 && instring[pos+9] == '-') {
                            switch(instring[pos+10]) {
                                case 'a': return retcmp(instring, pos, FUNCTIONNAME_SUBSTRING_AFTER , Function::SubstringAfter , Function::Unknown);
                                case 'b': return retcmp(instring, pos, FUNCTIONNAME_SUBSTRING_BEFORE, Function::SubstringBefore, Function::Unknown);
                                default : return retcmp(instring, pos, FUNCTIONNAME_SUBSTRING       , Function::Substring      , Function::Unknown);
                            }
                        }
                        return retcmp(instring, pos, FUNCTIONNAME_SUBSTRING, Function::Substring, Function::Unknown);
                }
            case 'n':
                switch(instring[pos+2]) {
                    case 'm': return retcmp(instring, pos, FUNCTIONNAME_NUMBER         , Function::Number        , Function::Unknown);
                    case 'r': return retcmp(instring, pos, FUNCTIONNAME_NORMALIZE_SPACE, Function::NormalizeSpace, Function::Unknown);
                    case 't': return retcmp(instring, pos, FUNCTIONNAME_NOT            , Function::Not           , Function::Unknown);
                    case 'a': 
                        if (instring.length() > pos +3 && instring[pos+4] == 's') {
                            return retcmp(instring, pos, FUNCTIONNAME_NAMESPACE_URI, Function::NamespaceURI, Function::Unknown);
                        } else {
                            return retcmp(instring, pos, FUNCTIONNAME_NAME         , Function::Name        , Function::Unknown);
                        }
                }
            case 'i': return retcmp(instring, pos, FUNCTIONNAME_ID, Function::Id, Function::Unknown);
            case 't': 
                if (instring[pos+2]== 'u') {
                    return retcmp(instring, pos, FUNCTIONNAME_TRUE     , Function::True, Function::Unknown);
                } else {
                    return retcmp(instring, pos, FUNCTIONNAME_TRANSLATE, Function::Translate, Function::Unknown);
                }
            case 'b': return retcmp(instring, pos, FUNCTIONNAME_BOOLEAN, Function::Boolean, Function::Unknown);
            case 'f':
                if (instring[pos+1]== 'a') {
                    return retcmp(instring, pos, FUNCTIONNAME_FALSE, Function::False, Function::Unknown);
                } else {
                    return retcmp(instring, pos, FUNCTIONNAME_FLOOR, Function::Floor, Function::Unknown);
                }
            case 'r': return retcmp(instring, pos, FUNCTIONNAME_ROUND, Function::Round, Function::Unknown);
            default : return Function::Unknown;
        }
    }

    void Step::serializeNodeTest(std::ostream &os) const {
        switch (_test) {
            case TestPrincipalType         : os << ( !_nodeTestName.empty() ? _nodeTestName : std::string("*") ); break;
            case TestAnyNode               : os << "node()"; break;
            case TestCommentNode           : os << "comment()"; break;
            case TestTextNode              : os << "text()"; break;
            case TestProcessingInstruction : os << "processinginstruction(" << _nodeTestName << ")"; break;
            case InvalidTest               : os << "invalid"; break;
        }
        if (_test != TestPrincipalType && _test != TestProcessingInstruction && !_nodeTestName.empty()) {
            os << "[self::node() == \"" << _nodeTestName << "\"]";
        }
    }

    const std::string &
    Step::stringForAxis(Step::Axis a) {
        switch(a) {
            case Next_Sibling       : return AXISNAME_NEXT_SIBLING;
            case Previous_Sibling   : return AXISNAME_PREVIOUS_SIBLING;
            case Following_Sibling  : return AXISNAME_FOLLOWING_SIBLING;
            case Preceding_Sibling  : return AXISNAME_PRECEDING_SIBLING;
            case Child              : return AXISNAME_CHILD;
            case Parent             : return AXISNAME_PARENT;
            case Descendant         : return AXISNAME_DESCENDANT;
            case Ancestor           : return AXISNAME_ANCESTOR;
            case Following          : return AXISNAME_FOLLOWING;
            case Preceding          : return AXISNAME_PRECEDING;
            case Ancestor_Or_Self   : return AXISNAME_ANCESTOR_OR_SELF;
            case Descendant_Or_Self : return AXISNAME_DESCENDANT_OR_SELF;
            case Self               : return AXISNAME_SELF;
            case Namespace          : return AXISNAME_NAMESPACE;
            case Attribute          : return AXISNAME_ATTRIBUTE;
            default                 : return AXISNAME_INVALID;
        }
    }

    // returns position past end of s if s is the exact next part
    inline
    std::string::size_type read_if_string( const std::string &    is
                                         , std::string::size_type pos
                                         , const std::string &    s ) {
        std::string::size_type i = 0;
        std::string::size_type n = asl::minimum(s.size(),is.size()-pos);
        while (i<n && pos < is.size() && is[pos+i] == s[i]) {
            ++i;
        }
        if (i == s.size()) {
            return pos + i;
        }
        return pos;
    }

    Step::Axis Step::read_axis(const std::string &instring, int pos) {
        switch(instring[pos]) {
            case 'a':
                if (instring.length() <= pos + 8) {
                    return retcmp(instring, pos, AXISNAME_ANCESTOR, Step::Ancestor, Step::Invalid);
                } else switch(instring[pos+8]) {
                    case '-': return retcmp(instring, pos, AXISNAME_ANCESTOR_OR_SELF, Step::Ancestor_Or_Self, Step::Invalid);
                    case 'e': return retcmp(instring, pos, AXISNAME_ATTRIBUTE       , Step::Attribute       , Step::Invalid);
                    default : return retcmp(instring, pos, AXISNAME_ANCESTOR        , Step::Ancestor        , Step::Invalid);
                }
            case 'c': return retcmp(instring, pos, AXISNAME_CHILD, Step::Child, Step::Invalid);
            case 'd':
                if (instring.length() <= pos + 10) {
                    return retcmp(instring, pos, AXISNAME_DESCENDANT, Step::Descendant, Step::Invalid);
                } else switch(instring[pos+10]) {
                    case '-': return retcmp(instring, pos, AXISNAME_DESCENDANT_OR_SELF, Step::Descendant_Or_Self, Step::Invalid);
                    default : return retcmp(instring, pos, AXISNAME_DESCENDANT        , Step::Descendant        , Step::Invalid);
                }
            case 'f': 
                if (instring.length() <= pos + 9) {
                    return retcmp(instring, pos, AXISNAME_FOLLOWING, Step::Following, Step::Invalid);
                } else switch(instring[pos+9]) {
                    case '-': return retcmp(instring, pos, AXISNAME_FOLLOWING_SIBLING, Step::Following_Sibling, Step::Invalid);
                    default : return retcmp(instring, pos, AXISNAME_FOLLOWING        , Step::Following        , Step::Invalid);
                }
            case 'n':
                if (instring.length() <= pos + 8) {
                    return Step::Invalid;
                } else switch(instring[pos+1]) {
                    case 'a': return retcmp(instring, pos, AXISNAME_NAMESPACE   , Step::Namespace   , Step::Invalid);
                    case 'e': return retcmp(instring, pos, AXISNAME_NEXT_SIBLING, Step::Next_Sibling, Step::Invalid);
                    default : return Step::Invalid;
                }
            case 'p':
                if (instring.length() <= pos + 6) {
                    return Step::Invalid;
                } else switch(instring[pos+3]) {
                    case 'e': return retcmp(instring, pos, AXISNAME_PARENT, Step::Parent, Step::Invalid);
                    case 'c': 
                        if (instring.length() <= pos + 9) {
                            return retcmp(instring, pos, AXISNAME_PRECEDING, Step::Preceding, Step::Invalid);
                        } else switch(instring[pos+9]) {
                            case '-': return retcmp(instring, pos, AXISNAME_PRECEDING_SIBLING, Step::Preceding_Sibling, Step::Invalid);
                            default : return retcmp(instring, pos, AXISNAME_PRECEDING        , Step::Preceding        , Step::Invalid);
                        }
                    case 'v': return retcmp(instring, pos, AXISNAME_PREVIOUS_SIBLING, Step::Previous_Sibling, Step::Invalid);
                }
            case 's': return retcmp(instring, pos, AXISNAME_SELF, Step::Self, Step::Invalid);
            default : return Step::Invalid;
        }
    }

    Step::NodeTest Step::read_NodeTest(const std::string &instring, int pos) {
        switch(instring[pos]) {
            case 'n': return retcmp(instring, pos, NODETEST_NODE   , Step::TestAnyNode              , Step::InvalidTest);
            case 'c': return retcmp(instring, pos, NODETEST_COMMENT, Step::TestCommentNode          , Step::InvalidTest);
            case 't': return retcmp(instring, pos, NODETEST_TEXT   , Step::TestTextNode             , Step::InvalidTest);
            case 'p': return retcmp(instring, pos, NODETEST_PI     , Step::TestProcessingInstruction, Step::InvalidTest);
            default : return Step::InvalidTest;
        }
    }

    bool Step::allows(dom::NodePtr n) const
    {
        if (_nodeTestName.length()) {
            if (_test == TestProcessingInstruction) {
                // ### node test name contains the parameter to the processing
                // instruction. handle accordingly.
            } else {
                if (n->nodeName() != _nodeTestName) {
                    return false;
                }
            }
        }
        switch (_test)
        {
        case TestPrincipalType:
            switch (_axis)
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

    void Step::scan(dom::NodePtr from, NodeSet &into) const {
        scanStep(*this, from, into);
    }

    void Step::scan(dom::NodePtr from, NodeVector &into) const {
        scanStep(*this, from, into);
    }

    void Step::scan(dom::NodePtr from, OrderedNodeSet &into) const {
        scanStep(*this, from, into);
    }

    void Step::serializeTo(std::ostream & os) const {
        os << stringForAxis(_axis);
        os << "::";
        serializeNodeTest(os);
        for (const_iterator it = begin(); it != end(); ++it) {
            os << "[";
            (*it)->serializeTo(os);
            os << "]";
        }
    }

    void Path::evaluateInto(NodeSetPtr workingset, NodeSet &returnContainer) const {
        if (_absolute) {
            NodeSetPtr newResults( new NodeSet() );
            NodeSet::const_iterator resIter = newResults->end();

            for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end(); ++it)
            {
                dom::NodePtr thisOne = *it;
                while (thisOne->parentNode()) {
                    thisOne = thisOne->parentNode()->self().lock();
                };
                newResults->insert(thisOne);
            }
            workingset = newResults;
        }

        StepList::const_iterator lastStep = _steps.end();
        lastStep--;
        for (StepList::const_iterator s = _steps.begin(); s != lastStep; ++s) {
            NodeSetPtr nextStepSet( new NodeSet() );
            for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end(); ++it) {
                (*s)->scanInto(*it, *nextStepSet);
            }
            workingset = nextStepSet;
        };
        for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end(); ++it) {
            (*lastStep)->scanInto(*it, returnContainer);
        }
    }

    void Path::evaluateInto(NodeSetPtr workingset, NodeVector & returnContainer) const {
        // avoid duplicates in intermediate results
        OrderedNodeSet lastStepSet;
        evaluateInto(workingset, lastStepSet);

        // but allow them in returnContainer
        copy(lastStepSet.begin(), lastStepSet.end(), std::inserter(returnContainer, returnContainer.end()));
    }

    void Path::evaluateInto(NodeSetPtr workingset, OrderedNodeSet &returnContainer) const {
        if (_absolute) {
            if (_steps.begin() == _steps.end()) {
                for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end();++it) {
                    dom::NodePtr thisOne = *it;
                    while (thisOne->parentNode()) {
                        thisOne = thisOne->parentNode()->self().lock();
                    }
                    returnContainer.insert(thisOne);
                }
                return;
            } else {
                NodeSetPtr newResults( new NodeSet() );
                for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end();++it) {
                    dom::NodePtr thisOne = *it;
                    while (thisOne && thisOne->parentNode()->self()) {
                        thisOne = thisOne->parentNode()->self().lock();
                    }
                    newResults->insert(thisOne);
                }
                workingset = newResults;
            }
        }

        StepList::const_iterator lastStep = _steps.end();
        lastStep--;
        for (StepList::const_iterator itS = _steps.begin(); itS != lastStep; ++itS) {
            NodeSetPtr nextStepSet( new NodeSet() );
            for (NodeSet::const_iterator itNS = workingset->begin(); itNS != workingset->end(); ++itNS) {
                (*itS)->scanInto(*itNS, *nextStepSet);
            }
            workingset = nextStepSet;
        }
        for (NodeSet::const_iterator it = workingset->begin(); it != workingset->end(); ++it) {
            (*lastStep)->scanInto(*it, returnContainer);
        }
    }

    void Path::serializeTo(std::ostream &os) const {
        for (StepList::const_iterator s = _steps.begin(); s != _steps.end();++s) {
            if ((s != _steps.begin()) || _absolute) {
                os << "/";
            }
            (*s)->serializeTo(os);
        }
    }

    void SetExpression::serializeTo(std::ostream &os) const {
        PathList::const_iterator it = _sets.begin();
        if (*it) {
            (*it)->serializeTo(os);
            while ((++it) != _sets.end()) {
                os << " | ";
                (*it)->serializeTo(os);
            }
        }
    }

    ValuePtr SetExpression::evaluateExpression(const Context &c) const {
        if (_sets.empty()) {
            return ValuePtr( new NullValue() );
        }
        PathList::const_iterator it = _sets.begin();
        NodeSetPtr v = (*it)->evaluate(c.currentNode)->takeNodes();

        while (++it != _sets.end()) {
            NodeSetPtr vcur = (*it)->evaluate(c.currentNode)->takeNodes();

            NodeSetPtr vres( new NodeSet );
            switch(_op) {
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
            v = vres; // shouldn't this be: v.insert( vres.begin(), vres.end() ); ??? 
        }
        return ValuePtr( new NodeSetValue(v) );
    }

}

