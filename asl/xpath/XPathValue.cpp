/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// This file is part of the ART+COM Standard Library (asl).
//
// It is distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)             
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

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
#include "XPathValue.h"

/**********************************************************************************************/
#include <cfloat>
#include <cassert>
#include <vector>
#include <sstream>

/**********************************************************************************************/
#include <asl/math/numeric_functions.h>

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
                return asl::isNaN(getValue());
            }

            bool NumberValue::isFinite() const
            {
                return asl::isFinite(getValue());
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
