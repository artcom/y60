#ifndef XPATH_EXPRESSION_H
#define XPATH_EXPRESSION_H

#include <dom/Nodes.h>

#include <functional>

#include "Context.h"
#include "Value.h"

namespace xpath
{
        typedef std::string string;

        class SyntaxNode
	{
	public:
	    virtual ~SyntaxNode(){};
	    virtual void serializeTo(std::ostream &) = 0;
	    string serialize() { string retval; std::ostringstream oss(retval); serializeTo(oss); return retval; };
	};

        class Expression : public SyntaxNode
	{
	public:
	    virtual Value *evaluateExpression(const Context &) = 0;
	};

        class Literal : public Expression
	{
	public:
	    Literal(const string &data) { value = data; };
	    virtual ~Literal() { };
	    virtual void serializeTo(std::ostream &os) { os << "\"" << value << "\""; };
	    virtual Value *evaluateExpression(const Context &c) { return new StringValue(value); };

	private:
	    string value;
	};

        class Number : public Expression
	{
	public:
	    Number(int _value) { value = _value; };
	    virtual ~Number() {};
	    virtual void serializeTo(std::ostream &os) { os << value; };
	    virtual Value *evaluateExpression(const Context &)  { return new NumberValue(value); };

	private:
	    int value;
	};

        class BinaryExpression : public Expression
	{
	public:
	    enum ExpressionType { Equal, NotEqual, 
				  Less=4, Greater, LEqual, GEqual, 
				  Or = 16, And,
				  Plus = 32, Minus, Times, Div, Mod };

	    BinaryExpression(ExpressionType, Expression *, Expression *);
	    virtual ~BinaryExpression() { if (lvalue) delete lvalue; if (rvalue) delete rvalue; };

	    virtual void serializeTo(std::ostream &);
	    virtual Value *evaluateExpression(const Context &);

	private:

	    ExpressionType type;
	    // both must one of (path, expressions, literal, number).
	    Expression *lvalue, *rvalue;
	};

        class UnaryExpression: public Expression
        {
        public:
            enum OperatorType { Minus, Tilde, Not, Invalid };
            UnaryExpression(OperatorType _type, Expression *_argument) { type = _type; argument = _argument; };
            virtual ~UnaryExpression() { if (argument) delete argument; };

            virtual void serializeTo(std::ostream &);
            virtual Value *evaluateExpression(const Context &);

        private:
            OperatorType type;
            Expression *argument;
        };

        class Function: public Expression
	{
	public:

	    enum FunctionType { Unknown, Last, Position, Count, StartsWith, Concat, Contains, Substring, SubstringBefore, SubstringAfter, Not,
				// To be implemented:
				Id, LocalName, NamespaceURI, Name, 
				String, StringLength, NormalizeSpace, Translate, 
				Boolean, True, False, Lang, 
				Number, Sum, Floor, Ceiling, Round
	    };

	    static const string FUNCTIONNAME_LAST;
	    static const string FUNCTIONNAME_POSITION;
	    static const string FUNCTIONNAME_COUNT;
	    static const string FUNCTIONNAME_STARTSWITH;
	    static const string FUNCTIONNAME_CONCAT;
	    static const string FUNCTIONNAME_CONTAINS;
	    static const string FUNCTIONNAME_SUBSTRING;
	    static const string FUNCTIONNAME_SUBSTRING_BEFORE;
	    static const string FUNCTIONNAME_SUBSTRING_AFTER;
	    static const string FUNCTIONNAME_NOT;
	    static const string FUNCTIONNAME_ID;
	    static const string FUNCTIONNAME_LOCAL_NAME;
	    static const string FUNCTIONNAME_NAMESPACE_URI;
	    static const string FUNCTIONNAME_NAME;
	    static const string FUNCTIONNAME_STRING;
	    static const string FUNCTIONNAME_STRING_LENGTH;
	    static const string FUNCTIONNAME_NORMALIZE_SPACE;
	    static const string FUNCTIONNAME_TRANSLATE;
	    static const string FUNCTIONNAME_BOOLEAN;
	    static const string FUNCTIONNAME_TRUE;
	    static const string FUNCTIONNAME_FALSE;
	    static const string FUNCTIONNAME_LANG;
	    static const string FUNCTIONNAME_NUMBER;
	    static const string FUNCTIONNAME_SUM;
	    static const string FUNCTIONNAME_FLOOR;
	    static const string FUNCTIONNAME_CEILING;
	    static const string FUNCTIONNAME_ROUND;
	    static const string FUNCTIONNAME_UNKNOWN;

	    Function(FunctionType _type, std::list<Expression*> *_arguments) { type = _type; arguments = _arguments; };
	    virtual ~Function();

	    virtual void serializeTo(std::ostream &);

	    static string nameOf(FunctionType);
	    static FunctionType typeOf(string instring, int pos=0);
	    virtual Value *evaluateExpression(const Context &);

	private:
	    FunctionType type;
	    std::list<Expression*> *arguments;
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

        class Step : public SyntaxNode
	{
	public:

	    // -----------------------------

	    static const string AXISNAME_INVALID           ;
	    static const string AXISNAME_NEXT_SIBLING      ;
	    static const string AXISNAME_PREVIOUS_SIBLING  ;
	    static const string AXISNAME_FOLLOWING_SIBLING ;
	    static const string AXISNAME_PRECEDING_SIBLING ;
	    static const string AXISNAME_CHILD             ;
	    static const string AXISNAME_PARENT            ;
	    static const string AXISNAME_DESCENDANT        ;
	    static const string AXISNAME_ANCESTOR          ;
	    static const string AXISNAME_FOLLOWING         ;
	    static const string AXISNAME_PRECEDING         ;
	    static const string AXISNAME_ANCESTOR_OR_SELF  ;
	    static const string AXISNAME_DESCENDANT_OR_SELF;
	    static const string AXISNAME_SELF              ;
	    static const string AXISNAME_NAMESPACE         ;
	    static const string AXISNAME_ATTRIBUTE         ;

	    enum Axis {
		    Invalid = 255,
		    Next_Sibling = 0,
		    Previous_Sibling,
		    Following_Sibling,
		    Preceding_Sibling,
		    Child,
		    Parent,
		    Descendant,
		    Ancestor,
		    Following = 10,
		    Preceding,
		    Ancestor_Or_Self = 14,
		    Descendant_Or_Self,
		    Self,
		    Namespace,
		    Attribute
		};

	    static const string &stringForAxis(Axis a);

	    static Axis read_axis(const string &instring, int pos);

	    // --------------------------------

	    enum NodeTest { TestPrincipalType,
			    TestAnyNode,
			    TestCommentNode,
			    TestTextNode,
			    TestProcessingInstruction,
	                    InvalidTest };

	    static const string NODETEST_INVALID;
	    static const string NODETEST_NODE;
	    static const string NODETEST_COMMENT;
	    static const string NODETEST_TEXT;
	    static const string NODETEST_PI;

	    static NodeTest read_NodeTest(const string &instring, int pos);

	    // used from fillAxis() to evaluate a node test
	    bool allows(NodeRef n);

	    void serializeNodeTest(std::ostream &);

	    // ---------------------------------

	    Step();
	    Step(Axis, NodeTest=TestPrincipalType, string="");
	    virtual ~Step();

	    template <class T>
		void scanInto(NodeRef from, T &into) {
		scan(from, into);
	    }

	    virtual void serializeTo(std::ostream &);


	    void setAxis(Axis a) { axis = a; };
	    inline Axis getAxis() { return axis; };

	    void setNodeTest(NodeTest n, const string &name="") { test = n; nodeTestName = name; };
	    void appendPredicate(Expression *);
	    void prependPredicate(Expression *);
	    void insertPredicate(int i, Expression *e);

	    Expression* takeFirst();
	    Expression* takeLast();
	    Expression* take(int i);

	    Expression* predicate(int i);
	    int count() const;

	    std::list<Expression*> predicates;

	private:
	    // puts results from from into into
	    void scan(NodeRef from, NodeSet &into);
	    void scan(NodeRef from, NodeList &into);
	    void scan(NodeRef from, OrderedNodeSet &into);

	    Axis axis;
	    NodeTest test;
	    string nodeTestName;

	};

    // this class contains a complete XPath that can
    // a) be evaluated on a context node to yield a NodeSet.
    // b) be serialized to a complete  expression.

        class Path : public Expression
	{
	public:
	    Path();
	    virtual void serializeTo(std::ostream &);
	    virtual Value *evaluateExpression(const Context &c);
	    virtual ~Path() { for (std::list<Step *>::iterator i = steps.begin(); i != steps.end(); ++i) delete *i; };
	    NodeSetValue *evaluate(NodeRef);

	    template <class T>
		T *evaluateAs(NodeRef input) {
		NodeSetRef theseNodes = new NodeSet();
		theseNodes->insert(input);
		T *retval = new T;
		evaluateInto(theseNodes, *retval);
		return retval;
	    }

	    void setAbsolute(bool a=true) { absolute = a; };
	    bool isAbsolute() { return absolute; };

	    void appendStep(Step *nextStep) { steps.push_back(nextStep); };
	    void prependStep(Step *prevStep) { steps.push_front(prevStep); };

	    int count() { return steps.size(); };

	    // a list does not support random access, so
	    // hide this call: Step *step(int i) { return steps.item(i); };

	    Step *takeFirst() { Step *retval = steps.front(); steps.pop_front(); return retval;};
	    Step *takeLast() { Step *retval = steps.back(); steps.pop_back(); return retval; };

	private:
	    void evaluateInto(NodeSetRef, NodeSet &);
	    void evaluateInto(NodeSetRef, OrderedNodeSet &);
	    void evaluateInto(NodeSetRef, NodeList &);

	    std::list<Step*> steps;
	    bool absolute;
	};

        class SetExpression: public Expression
	{
	public:
	    enum SetOperation { Union, Intersection, Difference };
	    SetExpression(SetOperation op) { m_op=op; };

	    void appendPath(Path *p) { sets.push_back(p); };

	    virtual void serializeTo(std::ostream &);
	    virtual Value *evaluateExpression(const Context &);

	private:
	    SetOperation m_op;
	    std::list<Path *> sets;
	};

}; // namespace

std::ostream &operator<<(std::ostream &stream, xpath::SyntaxNode &myNode);
#endif
