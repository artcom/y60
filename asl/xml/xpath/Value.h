//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_VALUE_H
#define XPATH_VALUE_H

#include <cassert>

#include <asl/Ptr.h>
#include <asl/Exception.h>

#include <dom/Nodes.h>

#include "Types.h"

namespace xpath {

    class NumberValue ;
    class BooleanValue;
    class StringValue ;
    class NodeSetValue;

    DEFINE_EXCEPTION(BadCast, asl::Exception);

    class Value;
    typedef asl::Ptr<Value       ,ThreadingModel>   ValuePtr;
    typedef asl::Ptr<NumberValue ,ThreadingModel>   NumberValuePtr;
    typedef asl::Ptr<BooleanValue,ThreadingModel>   BooleanValuePtr;
    typedef asl::Ptr<StringValue ,ThreadingModel>   StringValuePtr;
    typedef asl::Ptr<NodeSetValue,ThreadingModel>   NodeSetValuePtr;

    class Value {
        public:
            enum ValueType { NullType
                           , BooleanType
                           , NumberType
                           , StringType
                           , NodeSetType };

            virtual ~Value() {}

            virtual ValueType type() const = 0;

            template< typename VALUE_TYPE >
            VALUE_TYPE as() const {return as(static_cast<VALUE_TYPE*>(0));}

        private:
            virtual BooleanValue toBoolean() const;
            virtual NumberValue  toNumber () const; 
            virtual StringValue  toString () const;
            virtual NodeSetValue toNodeSet() const;

            BooleanValue as(BooleanValue*) const;
            NumberValue  as(NumberValue *) const;
            StringValue  as(StringValue *) const;
            NodeSetValue as(NodeSetValue*) const;
    };

    template <class VALUE_TYPE>
    inline bool equal_as(const Value& left, const Value& right) {
        return left.as<VALUE_TYPE>().getValue() == right.as<VALUE_TYPE>().getValue();
    }

    template <class VALUE_TYPE>
    inline bool equal_as(const ValuePtr& left, const ValuePtr& right) {
        return equal_as(*left,*right);
    }

    template <class VALUE_TYPE>
    inline bool smaller_than_as(const Value& left, const Value& right) {
        return left.as<VALUE_TYPE>().getValue() < right.as<VALUE_TYPE>().getValue();
    }

    template <class VALUE_TYPE>
    inline bool smaller_than_as(const ValuePtr& left, const ValuePtr& right) {
        return smaller_than_as(*left,*right);
    }

    class NullValue : public Value {
        public:
            ValueType type() const { return NullType; };
    };

    class BooleanValue : public Value {
        public:
            explicit BooleanValue(bool _value) : value( _value ) {};

            ValueType type() const { return BooleanType; };

            bool getValue() const { return value; };

        private:
            bool value;

            virtual BooleanValue toBoolean() const;
            virtual NumberValue  toNumber () const;
            virtual StringValue  toString () const;
    };

    class NumberValue : public Value {
        public:
            explicit NumberValue(double _value = double()) : value( _value ) {}

            ValueType type() const  { return NumberType; }

            double getValue() const { return value; }

            NumberValue negate()    { value = - value; return *this; }

        private:
            double value;

            virtual BooleanValue toBoolean() const;
            virtual NumberValue  toNumber () const;
            virtual StringValue  toString () const;
    };

    class StringValue : public Value {
        public:
            explicit StringValue(const std::string &s = "") : value( s ) {}

            ValueType type() const { return StringType; };

            const std::string & getValue() const { return value; };

        private:
            std::string value;

            virtual BooleanValue toBoolean() const;
            virtual NumberValue  toNumber () const;
            virtual StringValue  toString () const;
    };

    // NodeSetValues are unordered by default, i.e.
    // they contain a NodeSetPtr which is ordered by pointer value.
    // this provides for fast insertion and lookup.
    //
    class NodeSetValue : public Value {
        public:
            typedef NodeSet::size_type              size_type;

            NodeSetValue() : _myNodes( 0 ) {}

            // ownership of other is transferred to the new node set value.
            // XXX: This comment might somehow relate to the strange stuff in Context.h [DS]
            explicit NodeSetValue(NodeSetPtr rhs) : _myNodes(rhs) {}

            Value::ValueType type() const           { return NodeSetType; };

            NodeSetPtr takeNodes() {
                NodeSetPtr retval = _myNodes;
                _myNodes = NodeSetPtr();
                return retval;
            }

            NodeSet::iterator begin()              { assert(_myNodes); return _myNodes->begin(); }
            NodeSet::iterator end()                { assert(_myNodes); return _myNodes->end  (); }

            NodeSet::const_iterator begin() const  { assert(_myNodes); return _myNodes->begin(); }
            NodeSet::const_iterator end() const    { assert(_myNodes); return _myNodes->end  (); }

            size_type length() const               { return _myNodes ? _myNodes->size() : 0; }

//             dom::Node* item(size_type number) const {
//                 assert(_myNodes); 
//                 if (!_myNodes) {
//                     return 0; 
//                 }
//                 NodeSet::iterator iter = _myNodes->begin(); 
//                 while ((iter != _myNodes->end()) && number--) {
//                     ++iter;
//                 }
//                 return (iter == _myNodes->end()) ? 0 : *iter;
//             };

        private:
            NodeSetPtr _myNodes;

            virtual BooleanValue toBoolean() const {return BooleanValue( length()!=0 );}
            virtual NumberValue  toNumber () const;
            virtual StringValue  toString () const;
            virtual NodeSetValue toNodeSet() const;
    };

    inline BooleanValue Value::as(BooleanValue*) const { return toBoolean(); };
    inline NumberValue  Value::as(NumberValue *) const { return toNumber (); };
    inline StringValue  Value::as(StringValue *) const { return toString (); };
    inline NodeSetValue Value::as(NodeSetValue*) const { return toNodeSet(); };

    inline BooleanValue Value       ::toBoolean() const {return BooleanValue(bool());}
    inline NumberValue  Value       ::toNumber () const {return NumberValue (double());} 
    inline StringValue  Value       ::toString () const {return StringValue (std::string());}
    inline NodeSetValue Value       ::toNodeSet() const {return NodeSetValue();}

    inline BooleanValue BooleanValue::toBoolean() const {return *this;};
    inline NumberValue  BooleanValue::toNumber () const {return NumberValue(getValue());}
    inline StringValue  BooleanValue::toString () const {return StringValue(getValue()?"true":"false");}

    inline BooleanValue NumberValue ::toBoolean() const {return BooleanValue(getValue()!=0);}
    inline NumberValue  NumberValue ::toNumber () const {return *this;}

    inline BooleanValue StringValue ::toBoolean() const {return BooleanValue(getValue().length() > 0);}
    inline StringValue  StringValue ::toString () const {return *this;}
    inline NumberValue  StringValue ::toNumber () const {return NumberValue( number_value_for(getValue()) ); }

    inline StringValue  NodeSetValue::toString () const {return length() ? StringValue( string_value_for(*_myNodes->begin()) ) : StringValue(std::string()); }
    inline NodeSetValue NodeSetValue::toNodeSet() const {return NodeSetValue(_myNodes);}
    inline NumberValue  NodeSetValue::toNumber () const {return NumberValue( number_value_for(toString().getValue()) );}

} // end of namespace xpath

#endif
