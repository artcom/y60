//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_EXPRESSION_H
#define XPATH_EXPRESSION_H

#include <vector>

#include <asl/Ptr.h>

#include <dom/Nodes.h>

#include "Context.h"
#include "Value.h"

namespace xpath
{
    class Expression;
    typedef asl::Ptr<Expression,ThreadingModel>     ExpressionPtr;
    class BinaryExpression;
    typedef asl::Ptr<BinaryExpression,ThreadingModel>
                                                    BinaryExpressionPtr;
    class Step;
    typedef asl::Ptr<Step,ThreadingModel>           StepPtr;
    class Path;
    typedef asl::Ptr<Path,ThreadingModel>           PathPtr;
    class SetExpression;
    typedef asl::Ptr<SetExpression,ThreadingModel>  SetExpressionPtr;

    class SyntaxNode
    {
        public:
            virtual ~SyntaxNode() {}

            std::string serialize() const {
                std::ostringstream oss;
                serializeTo(oss);
                return oss.str();
            }

            virtual void serializeTo(std::ostream &) const = 0;

        private:
            friend std::ostream &operator<<(std::ostream &stream, const SyntaxNode &myNode);
    };

    class Expression : public SyntaxNode {
        public:
            virtual ValuePtr evaluateExpression(const Context &) const = 0;
    };

    class Literal : public Expression {
        public:
            Literal(const std::string &data)                        : _value(data) {}

            ValuePtr evaluateExpression(const Context &c) const       {return ValuePtr(new StringValue(_value));}

            virtual void serializeTo(std::ostream &os)  const         {os << '\"' << _value << '\"';}

        private:
            std::string _value;
    };

    class Number : public Expression {
        public:
            Number(double data)                                     : _value(data) {}

            ValuePtr evaluateExpression(const Context &) const        {return ValuePtr(new NumberValue(_value));}

            virtual void serializeTo(std::ostream &os) const          { os << _value; }

        private:
            double _value;
    };

    class BinaryExpression : public Expression {
        public:
            enum ExpressionType { Equal
                                , NotEqual
                                , Less=4
                                , Greater
                                , LEqual
                                , GEqual
                                , Or = 16
                                , And
                                , Plus = 32
                                , Minus
                                , Times
                                , Div
                                , Mod
                                };

            BinaryExpression( ExpressionType type
                            , ExpressionPtr lvalue
                            , ExpressionPtr rvalue)                 : _type(type), _lvalue(lvalue)
                                                                                 , _rvalue(rvalue) {}

            virtual ValuePtr evaluateExpression(const Context &) const;

            virtual void serializeTo(std::ostream &) const;

        private:
            ExpressionType _type;
            // both must one of (path, expressions, literal, number).
            ExpressionPtr _lvalue;
            ExpressionPtr _rvalue;
    };

    class UnaryExpression : public Expression {
        public:
            enum OperatorType { Minus
                              , Tilde
                              , Not
                              , Invalid 
                              };

            UnaryExpression( OperatorType type
                           , ExpressionPtr argument )               : _type(type), _argument(argument) {}

            virtual ValuePtr evaluateExpression(const Context &) const;

            virtual void serializeTo(std::ostream &) const;

        private:
            OperatorType _type;
            ExpressionPtr _argument;
    };

    class Function: public Expression {
        public:
            typedef std::vector<ExpressionPtr>                        ArgumentList;
            typedef ArgumentList::size_type                           size_type;

            enum FunctionType { Unknown
                              , Last
                              , Position
                              , Count
                              , StartsWith
                              , Concat
                              , Contains
                              , Substring
                              , SubstringBefore
                              , SubstringAfter
                              , Not
                              // To be implemented:
                              , Id
                              , LocalName
                              , NamespaceURI
                              , Name
                              , String
                              , StringLength
                              , NormalizeSpace
                              , Translate
                              , Boolean
                              , True
                              , False
                              , Lang
                              , Number
                              , Sum
                              , Floor
                              , Ceiling
                              , Round
                              };

            Function(FunctionType type)                             : _type(type), _arguments() {}

            Function(FunctionType type, const ArgumentList& args)   : _type(type), _arguments(args) {}

            virtual ValuePtr evaluateExpression(const Context &) const;

            virtual void serializeTo(std::ostream &) const;

            static std::string nameOf(FunctionType);
            static FunctionType typeOf(const std::string& instring, std::string::size_type pos=0);

        private:
            FunctionType _type;
            ArgumentList _arguments;
    };


    /* bit-level explanation of axis enum values:
       next-sibling:                                            
       previous-sibling:                                  backward
       following-sibling:                        indirect
       preceding-sibling:                        indirect backward
       child:                           vertical
       parent:                          vertical          backward
       descendant:                      vertical indirect
       ancestor:                        vertical indirect backward
       next?---------------include-self----------------------------
       previous?-----------include-self---------------------backward-
       following:          include-self          indirect
       preceding:          include-self          indirect backward
       child-or-self?------include-self-vertical-------------------
       parent-or-self?-----include-self-vertical-------------------
       ancestor-or-self:   include-self vertical indirect
       descendant-or-self: include-self vertical indirect backward
    
       namespace
       self
       attribute
     */

    class Step : public SyntaxNode {
        private:
            typedef std::vector<ExpressionPtr>                        PredicateList;
        public:

            typedef PredicateList::size_type                          size_type;
            typedef PredicateList::const_iterator                     const_iterator;

            enum Axis { Invalid = 255
                      , Next_Sibling = 0
                      , Previous_Sibling
                      , Following_Sibling
                      , Preceding_Sibling
                      , Child
                      , Parent
                      , Descendant
                      , Ancestor
                      , Following = 10
                      , Preceding
                      , Ancestor_Or_Self = 14
                      , Descendant_Or_Self
                      , Self
                      , Namespace
                      , Attribute
                      };

            static const std::string &stringForAxis(Axis a);

            static Axis read_axis(const std::string &instring, int pos);

            // --------------------------------

            enum NodeTest { TestPrincipalType
                          , TestAnyNode
                          , TestCommentNode
                          , TestTextNode
                          , TestProcessingInstruction
                          , InvalidTest 
                          };

            static NodeTest read_NodeTest(const std::string &instring, int pos);

            // ---------------------------------

            Step()                                                  : _axis(Child)
                                                                    , _test(TestPrincipalType)
                                                                    , _nodeTestName()
                                                                    , _predicates() {}

            //Step(Axis axis)                                         : _axis(axis), _test()
            //                                                        , _nodeTestName()
            //                                                        , _predicates() {}

            Step(Axis axis, NodeTest test)                          : _axis(axis), _test(test)
                                                                    , _nodeTestName()
                                                                    , _predicates() {}

            Step(Axis axis, NodeTest test, const std::string& name) : _axis(axis), _test(test)
                                                                    , _nodeTestName(name)
                                                                    , _predicates() {}

            // used from fillAxis() to evaluate a node test
            bool allows(dom::NodePtr n) const;

            void serializeNodeTest(std::ostream &) const;

            template <class T>
            void scanInto(dom::NodePtr from, T &into) const           {scan(from,into);}

            // ---------------------------------

            void setAxis(Axis a)                                      { _axis = a; }
            Axis getAxis() const                                      { return _axis; }

            void setNodeTest(NodeTest n, const std::string &name="")  { _test = n; _nodeTestName = name; };
            void appendPredicate(ExpressionPtr e)                     { _predicates.push_back(e); }
            void prependPredicate(ExpressionPtr e)                    { _predicates.insert(_predicates.begin(),e); }
            void insertPredicate(size_type idx, ExpressionPtr e)      { _predicates.insert(findPredicate(idx),e); }

            ExpressionPtr takeFirst()                                 { return take(_predicates.begin()); }
            ExpressionPtr takeLast()                                  { return take(_predicates.end()-1); }
            ExpressionPtr take(size_type idx)                         { return take(findPredicate(idx)); }

            ExpressionPtr predicate(size_type idx)                    { return *findPredicate(idx); }

            size_type count() const                                   { return _predicates.size(); }
            const_iterator begin() const                              { return _predicates.begin(); }
            const_iterator end  () const                              { return _predicates.end  (); }

            virtual void serializeTo(std::ostream &) const;

        private:
            PredicateList               _predicates;
            Axis                        _axis;
            NodeTest                    _test;
            std::string                 _nodeTestName;

            // puts results from from into into
            void scan(dom::NodePtr from, NodeSet & into) const;
            void scan(dom::NodePtr from, NodeVector & into) const;
            void scan(dom::NodePtr from, OrderedNodeSet & into) const;

            PredicateList::iterator findPredicate(size_type idx)
            {
                PredicateList::iterator result = _predicates.begin();
                while( result != _predicates.end() && idx ) {
                    --idx;
                    ++result;
                }
                assert( result != _predicates.end() );
                return result;
            }

            ExpressionPtr take(PredicateList::iterator it) {
                assert( it != _predicates.end() );
                ExpressionPtr retval = *it;
                _predicates.erase(it);
                return retval;
            }

    };

    // this class contains a complete XPath that can
    // a) be evaluated on a context node to yield a NodeSet.
    // b) be serialized to a complete  expression.

    class Path : public Expression {
        private:
            typedef std::vector<StepPtr>                              StepList;
        public:
            typedef StepList::size_type                               size_type;

            Path()                                                  : _steps(), _absolute(false) {}

            ValuePtr evaluateExpression(const Context &c) const       {return ValuePtr(evaluate(c.currentNode));}

            NodeSetValuePtr evaluate(const dom::NodePtr n) const      {return NodeSetValuePtr( new NodeSetValue(evaluateAs<NodeSet>(n)) );}

            virtual void serializeTo(std::ostream &) const;

            template<class T>
            asl::Ptr<T,ThreadingModel> evaluateAs(const dom::NodePtr input) const {
                NodeSetPtr theseNodes( new NodeSet() );
                theseNodes->insert(input);
                asl::Ptr<T,ThreadingModel> retval( new T );
                evaluateInto(theseNodes, *retval);
                return retval;
            }

            void setAbsolute(bool a = true)                           { _absolute = a; }

            bool isAbsolute()                                         { return _absolute; }

            void appendStep(StepPtr nextStep)                         { _steps.push_back(nextStep); }
            void prependStep(StepPtr prevStep)                        { _steps.insert(_steps.begin(),prevStep); }

            size_type count() const                                   { return _steps.size(); }

            StepPtr takeFirst()                                       { return take(_steps.begin());}
            StepPtr takeLast()                                        { return take(_steps.end()-1);}

        private:
            StepList _steps;
            bool _absolute;

            void evaluateInto(NodeSetPtr, NodeSet &) const;
            void evaluateInto(NodeSetPtr, OrderedNodeSet &) const;
            void evaluateInto(NodeSetPtr, NodeVector &) const;

            StepPtr take(StepList::iterator it) {
                assert( it != _steps.end() );
                StepPtr retval = *it;
                _steps.erase(it);
                return retval;
            }

    };

    class SetExpression: public Expression {
        private:
            typedef std::vector<PathPtr>                              PathList;
        public:
            enum SetOperation { Union
                              , Intersection
                              , Difference 
                              };

            SetExpression(SetOperation op)                          : _op(op) {}

            void appendPath(PathPtr p)                                { _sets.push_back(p); }

            virtual ValuePtr evaluateExpression(const Context &) const;

            virtual void serializeTo(std::ostream &) const;

        private:
            SetOperation _op;
            PathList _sets;
    };

    inline std::ostream &operator<<(std::ostream &stream, const SyntaxNode &myNode) {
        myNode.serializeTo(stream);
        return stream;
    }
} // namespace

#endif
