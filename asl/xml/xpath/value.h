#ifndef VALUE_H
#define VALUE_H

#include <stdlib.h>
#include <malloc.h>

#include <dom/Nodes.h>

namespace xpath
{
    typedef std::string string;
    typedef dom::Node *NodeRef;
    typedef std::set<dom::Node *> NodeSet;
    typedef NodeSet *NodeSetRef;


    class NumberValue;
    class BooleanValue;
    class StringValue;
    class NodeSetValue;

#define DECLARE_TYPED_BINOP(TYPE, OPNAME, OP, RESULTTYPE)     \
   RESULTTYPE OPNAME ## As ## TYPE (Value *one, Value *two) { \
      TYPE ## Value *lv = one->to ## TYPE();                  \
      TYPE ## Value *rv = two->to ## TYPE();                  \
      RESULTTYPE retval = lv OP rv;                           \
      delete lv; delete rv;                                   \
      return RESULTTYPE; }

    class Value
    {
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

      /*
      DECLARE_TYPED_BINOP(Boolean, equals, ==, bool);
      DECLARE_TYPED_BINOP(String, equals, ==, bool);
      DECLARE_TYPED_BINOP(Number, equals, ==, bool);
      DECLARE_TYPED_BINOP(NodeSet, equals, ==, bool);
      DECLARE_TYPED_BINOP(Boolean, smaller, <, bool);
      DECLARE_TYPED_BINOP(String, smaller, <, bool);
      DECLARE_TYPED_BINOP(Number, smaller, <, bool);
      DECLARE_TYPED_BINOP(NodeSet, smaller, <, bool);
      */
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

    class NullValue : public Value
    {
    public:
      ValueType type() { return NullType; };
    };

    class BooleanValue : public Value
    {
    public:
      BooleanValue(bool _value) { value = _value; };
      virtual ~BooleanValue();

      ValueType type() { return BooleanType; };

      bool getValue() { return value; };

      virtual BooleanValue *toBoolean() { return new BooleanValue(getValue()); };
      virtual NumberValue *toNumber();
      virtual StringValue *toString();

    private:
      bool value;
    };

    class NumberValue : public Value
    {
    public:
      NumberValue(double _value) { value = _value; isNaN = false; isInfinity = false; };
      virtual ~NumberValue();

      ValueType type() { return NumberType; };

      double getValue() { return value; };

      virtual BooleanValue *toBoolean() { return new BooleanValue(!isNaN || value!=0); };
      virtual NumberValue *toNumber() { return new NumberValue(value); };
      virtual StringValue *toString();

      void negate() { value = -value; };

    private:
      bool isNaN; bool isInfinity;
      double value;
    };

    class StringValue : public Value
    {
    public:
      StringValue(const string &s)
	  { value = s; };
      virtual ~StringValue();

      ValueType type() { return StringType; };

      virtual BooleanValue *toBoolean() { return new BooleanValue( (value.length() > 0) ); };
      // ## TODO: recognize NaN, -Infinity and parse errors.
      virtual NumberValue *toNumber() { int num = 0; std::istringstream iss(value); iss >> num; return new NumberValue(num); };
      virtual StringValue *toString() { return new StringValue(value); };

      const string &getValue() const { return value; };

      static StringValue *stringValueFor(dom::Node *);

    private:
      string value;
    };

    class NodeSetValue : public Value
    {
    public:
	NodeSetValue() {_myNodes = NULL;};
	// ownership of other is transferred to the new node set value.
	NodeSetValue(NodeSetRef other) { _myNodes = other; };
	virtual ~NodeSetValue() { delete _myNodes; }

	NodeSetRef takeNodes() { NodeSetRef retval = _myNodes; _myNodes = NULL; return retval; };

      virtual BooleanValue *toBoolean() { if (!_myNodes) return new BooleanValue(false); return new BooleanValue( _myNodes->size()!=0 ); };
      virtual NumberValue *toNumber();
      virtual StringValue *toString();
      virtual NodeSetValue *toNodeSet();

      NodeSet::iterator begin() { assert(_myNodes); return _myNodes->begin(); };
      NodeSet::iterator end() { assert(_myNodes); return _myNodes->end(); };
      int length() { if (!_myNodes) return 0; return _myNodes->size(); };
      NodeRef item(int number) {
	  assert(_myNodes); 
	  if (!_myNodes) return NULL; 
	  NodeSet::iterator iter = _myNodes->begin(); 
	  while ((iter != _myNodes->end()) && number--) ++iter;
	  return (iter == _myNodes->end()) ? NULL: *iter;
      };

      Value::ValueType type() { return NodeSetType; };
      private:
      NodeSetRef _myNodes;
    };

};

#endif
