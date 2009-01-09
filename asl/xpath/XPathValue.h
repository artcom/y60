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
** \file xpath/XPathValue.h
**
** \brief XPath values
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/

#if !defined(XPATH_VALUE_H)
#define XPATH_VALUE_H

/**********************************************************************************************/
#include <string>
#include <set>

/**********************************************************************************************/
#include <asl/base/Exception.h>
#include <asl/base/Ptr.h>
#include <asl/dom/Nodes.h>

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        DEFINE_EXCEPTION(BadCast, asl::Exception);

        enum ValueType { Value_NodeSet     = 0x1
                       , Value_Boolean     = 0x2
                       , Value_Number      = 0x4
                       , Value_String      = 0x8
                       };

        struct LessByDocOrder : public std::binary_function< const dom::Node*
                                                           , const dom::Node*
                                                           , bool >
        {
            bool operator()(const dom::Node* lhs, const dom::Node* rhs) const;
        };

        typedef std::set<const dom::Node*,LessByDocOrder> NodeSet;
        typedef double                                    Number;
        typedef bool                                      Boolean;
        typedef std::string                               String;

        class Object;
        class ValueBase;
        class NodeSetValue;
        class BooleanValue;
        class NumberValue;
        class StringValue;

        StringValue getStringValue(const dom::Node* node);

        class Object {
        public:
            explicit Object(const asl::Ptr<ValueBase> v): myValue_(v) {}

            explicit Object(const NodeSetValue& v);
            explicit Object(const BooleanValue& v);
            explicit Object(const NumberValue & v);
            explicit Object(const StringValue & v);

            explicit Object(const NodeSet& v);
            explicit Object(const Boolean& v);
            explicit Object(const Number & v);
            explicit Object(const String & v);

            ValueType getType() const;

            template< class ConcreteValueType >
            ConcreteValueType as() const;

        private:
            asl::Ptr<ValueBase>   myValue_;
        };

        class ValueBase {
        public:
            virtual ~ValueBase()                          {}

            virtual ValueType getType() const           = 0;

            template< class ConcreteValueType >
            ConcreteValueType as() const                  {return to(reinterpret_cast<ConcreteValueType*>(NULL));}

        private:
            friend class Object;
            virtual NodeSetValue to(NodeSetValue*) const= 0;
            virtual BooleanValue to(BooleanValue*) const= 0;
            virtual NumberValue  to(NumberValue *) const= 0;
            virtual StringValue  to(StringValue *) const= 0;
        };

        template< ValueType Type_ >
        class TypedValue : public ValueBase {
        public:
            virtual ValueType getType() const             {return Type_;}
        };

        class NodeSetValue : public TypedValue<Value_NodeSet> {
        public:
            explicit NodeSetValue(const NodeSet& nset)  : nodeSet_(nset) {}

            const NodeSet& getValue() const               {return nodeSet_;}
        private:
            virtual NodeSetValue to(NodeSetValue*) const  {return *this;}
            virtual BooleanValue to(BooleanValue*) const;
            virtual NumberValue  to(NumberValue *) const;
            virtual StringValue  to(StringValue *) const;

            NodeSet                   nodeSet_;
        };

        class BooleanValue : public TypedValue<Value_Boolean> {
        public:
            explicit BooleanValue(const Boolean b)      : boolean_(b) {}

            Boolean getValue() const                      {return boolean_;}
        private:
            virtual NodeSetValue to(NodeSetValue*) const;
            virtual BooleanValue to(BooleanValue*) const  {return *this;}
            virtual NumberValue  to(NumberValue *) const;
            virtual StringValue  to(StringValue *) const;

            Boolean                   boolean_;
        };

        class NumberValue : public TypedValue<Value_Number> {
        public:
            explicit NumberValue(const Number number)   : number_(number) {}

            Number getValue() const                       {return number_;}

            bool isNaN() const;

            bool isFinite() const;
        private:
            virtual NodeSetValue to(NodeSetValue*) const;
            virtual BooleanValue to(BooleanValue*) const;
            virtual NumberValue  to(NumberValue *) const  {return *this;}
            virtual StringValue  to(StringValue *) const;

            Number                    number_;
        };

        class StringValue : public TypedValue<Value_String> {
        public:
            explicit StringValue(const String& str)     : str_(str) {}

            const String& getValue() const                {return str_;}
        private:
            virtual NodeSetValue to(NodeSetValue*) const;
            virtual BooleanValue to(BooleanValue*) const;
            virtual NumberValue  to(NumberValue *) const;
            virtual StringValue  to(StringValue *) const  {return *this;}

            String                    str_;
        };

        inline Object::Object(const NodeSetValue& v)             : myValue_(new NodeSetValue(v)) {}
        inline Object::Object(const BooleanValue& v)             : myValue_(new BooleanValue(v)) {}
        inline Object::Object(const NumberValue & v)             : myValue_(new NumberValue (v)) {}
        inline Object::Object(const StringValue & v)             : myValue_(new StringValue (v)) {}
        inline Object::Object(const NodeSet& v)                  : myValue_(new NodeSetValue(v)) {}
        inline Object::Object(const Boolean& v)                  : myValue_(new BooleanValue(v)) {}
        inline Object::Object(const Number & v)                  : myValue_(new NumberValue (v)) {}
        inline Object::Object(const String & v)                  : myValue_(new StringValue (v)) {}
        inline ValueType Object::getType() const                   {return myValue_->getType();}
        template< class ConcreteValueType >
        inline ConcreteValueType Object::as() const                {return myValue_->as<ConcreteValueType>();}

        inline BooleanValue NodeSetValue::to(BooleanValue*) const  {return BooleanValue(!getValue().empty());}
        inline NumberValue  NodeSetValue::to(NumberValue *) const  {return as<StringValue>().as<NumberValue>();}

        inline NodeSetValue BooleanValue::to(NodeSetValue*) const  {return NodeSetValue(NodeSet());}
        inline NumberValue  BooleanValue::to(NumberValue *) const  {return NumberValue(getValue()?1:0);}
        inline StringValue  BooleanValue::to(StringValue *) const  {return StringValue(getValue()?"true":"false");}

        inline NodeSetValue NumberValue ::to(NodeSetValue*) const  {return NodeSetValue(NodeSet());}
        inline BooleanValue NumberValue ::to(BooleanValue*) const  {return BooleanValue(getValue()!=0 && !isNaN());}

        inline NodeSetValue StringValue ::to(NodeSetValue*) const  {return NodeSetValue(NodeSet());}
        inline BooleanValue StringValue ::to(BooleanValue*) const  {return BooleanValue(!getValue().empty());}

    }

}

/**********************************************************************************************/

#endif //!defined(XPATH_VALUE_H)
