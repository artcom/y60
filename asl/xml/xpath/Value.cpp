/**********************************************************************************************/

/*
**
** Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
**
** These coded instructions, statements, and computer programs contain
** unpublished proprietary information of ART+COM AG Berlin, and
** are copy protected by law. They may not be disclosed to third parties
** or copied or duplicated in any form, in whole or in part, without the
** specific, prior written permission of ART+COM AG Berlin.
**
*/

/**********************************************************************************************/

/*!
** \file xpath/Value.cpp
**
** \brief XPath values
**
** \author Hendrik Schober
**
** \date 2008-07-14
**
*/

/**********************************************************************************************/
#include "Value.h"

/**********************************************************************************************/
#include <cfloat>
#include <cassert>
#include <vector>
#include <sstream>

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        //struct LessByDocOrder : public std::binary_function< const dom::Node*
        //                                                   , const dom::Node*
        //                                                   , bool >
        //{
            bool LessByDocOrder::operator()(const dom::Node* lhs, const dom::Node* rhs) const
            {
                return -1 == lhs->compareByDocumentOrder(rhs->self().lock());
            }
        //};

        StringValue getStringValue(const dom::Node* node)
        {
            return StringValue( node->nodeValue() );
        }

        //class NodeSetValue : public TypedValue<Value_NodeSet> {
        //private:
            StringValue NodeSetValue::to(StringValue *) const
            {
                if( getValue().empty() ) {
                    return StringValue(String());
                }
                return getStringValue( *getValue().begin() );
            }
        //};

        //class NumberValue : public TypedValue<Value_Number> {
        //private:
            bool NumberValue::isNaN() const
            {
                return _isnan(getValue());
            }

            bool NumberValue::isFinite() const
            {
                return _finite(getValue());
            }

            StringValue NumberValue::to(StringValue *) const
            {
                if( isNaN() ) {
                    return StringValue("NaN");
                }
                if( !isFinite() ) {
                    return StringValue(getValue() > 0 ? "Infinity" : "-Infinity");
                }
                std::basic_ostringstream<String::value_type> oss;
                oss << getValue();
                return StringValue(oss.str());
            }
        //};

        //class StringValue : public TypedValue<Value_String> {
        //private:
            NumberValue StringValue::to(NumberValue *) const
            {
                std::basic_istringstream<String::value_type> iss(getValue());
                Number num;
                iss >> std::ws >> num;
                if( !iss ) {
                    throw BadCast( std::string("Cannot cast ")+getValue()+" to number", PLUS_FILE_LINE );
                }
                return NumberValue(num);
            }
        //};

    }

}

/**********************************************************************************************/
