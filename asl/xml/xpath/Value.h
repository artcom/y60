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

//#include "xpath_api.h"

#include <dom/Nodes.h>

#include "Types.h"

namespace xpath {
    class NumberValue;
    class BooleanValue;
    class StringValue;
    class NodeSetValue;

    class Value {
        public:
            virtual ~Value();
            enum ValueType { NullType, BooleanType, NumberType, StringType, NodeSetType };

            virtual ValueType type() = 0;

            virtual BooleanValue *toBoolean() { return 0; };
            virtual NumberValue *toNumber() { return 0; };
            virtual StringValue *toString() { return 0; };
            virtual NodeSetValue *toNodeSet() { return 0; };

            virtual BooleanValue *as(BooleanValue *) { return toBoolean(); };
            virtual NumberValue *as(NumberValue *) { return toNumber(); };
            virtual StringValue *as(StringValue *) { return toString(); };
            virtual NodeSetValue *as(NodeSetValue *) { return toNodeSet(); };
    };

    template <class VALUE_TYPE>
    bool equals_as(Value *left, Value *right) {
        VALUE_TYPE *lv = left->as((VALUE_TYPE*)0);
        VALUE_TYPE *rv = right->as((VALUE_TYPE*)0);
        bool retval = lv->getValue() == rv->getValue();
        delete lv;
        delete rv;
        return retval;
    }

    template <class VALUE_TYPE>
    bool smaller_than_as(Value *left, Value *right) {
        VALUE_TYPE *lv = left->as((VALUE_TYPE*)0);
        VALUE_TYPE *rv = right->as((VALUE_TYPE*)0);
        bool retval = lv->getValue() < rv->getValue();
        delete lv;
        delete rv;
        return retval;
    }

    class NullValue : public Value {
        public:
            ValueType type() { return NullType; };
    };

    class BooleanValue : public Value {
        public:
            BooleanValue(bool _value) : value( _value ) {};
            virtual ~BooleanValue();

            ValueType type() { return BooleanType; };

            bool getValue() { return value; };

            virtual BooleanValue *toBoolean() {
                return new BooleanValue( getValue());
            };
            virtual NumberValue *toNumber();
            virtual StringValue *toString();

        private:
            bool value;
    };

    class NumberValue : public Value {
        public:
            NumberValue(double _value) :
                value( _value ),
                isNaN( false ),
                isInfinity( false ) {};
            virtual ~NumberValue();

            ValueType type() { return NumberType; };

            double getValue() { return value; };

            virtual BooleanValue *toBoolean() {
                return new BooleanValue(!isNaN || value!=0);
            }
            virtual NumberValue *toNumber() {
                return new NumberValue(value);
            }
            virtual StringValue *toString();

            void negate() {
                value = - value;
            }

        private:
            bool isNaN;
            bool isInfinity;
            double value;
    };

    class StringValue : public Value {
        public:
            StringValue(const string &s) : value( s ) { }
            virtual ~StringValue();

            ValueType type() { return StringType; };

            virtual BooleanValue *toBoolean() {
                return new BooleanValue( (value.length() > 0) );
            }
            // ## TODO: recognize NaN, -Infinity and parse errors.
            virtual NumberValue *toNumber();
            virtual StringValue *toString() {
                return new StringValue(value);
            }

            const string & getValue() const { return value; };

        private:
            string value;
    };

    // NodeSetValues are unordered by default, i.e.
    // they contain a NodeSetRef which is ordered by pointer value.
    // this provides for fast insertion and lookup.
    //
    class NodeSetValue : public Value {
        public:
            NodeSetValue() : _myNodes( 0 ) {}
            // ownership of other is transferred to the new node set value.
            // XXX: This comment might somehow relate to the strange stuff in Context.h [DS]
            NodeSetValue( NodeSetRef other ) : _myNodes( other) {}
            virtual ~NodeSetValue() {
                delete _myNodes;
            }

            NodeSetRef takeNodes() {
                NodeSetRef retval = _myNodes;
                _myNodes = 0;
                return retval;
            }

            virtual BooleanValue *toBoolean() {
                if ( ! _myNodes) {
                    return new BooleanValue(false);
                }
                return new BooleanValue( _myNodes->size()!=0 ); 
            }
            virtual NumberValue *toNumber();
            virtual StringValue *toString();
            virtual NodeSetValue *toNodeSet();

            NodeSet::iterator begin() {
                assert(_myNodes);
                return _myNodes->begin();
            }
            NodeSet::iterator end() {
                assert(_myNodes);
                return _myNodes->end();
            }
            int length() {
                if ( ! _myNodes) {
                    return 0;
                }
                return _myNodes->size();
            }
            NodeRef item(int number) {
                assert(_myNodes); 
                if (!_myNodes) {
                    return 0; 
                }
                NodeSet::iterator iter = _myNodes->begin(); 
                while ((iter != _myNodes->end()) && number--) {
                    ++iter;
                }
                return (iter == _myNodes->end()) ? 0 : *iter;
            };

            Value::ValueType type() { return NodeSetType; };
        private:
            NodeSetRef _myNodes;
    };

} // end of namespace xpath

#endif
