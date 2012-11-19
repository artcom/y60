
/*

S.M.A.R.T. - (Small?)Smart Multitreaded Application RunTime

What features should a good general purpose rapid application development language have?

- Syntax similiar to C, C++, JAVA, C# etc.
- ALGOL-like Orthogonality (everything is an expression, you can put an expression anywhere?)
- Smalltalk-like simplicity and metaclasses
- Lisp-like elegance and self-description capabilities and dynamic symbol processing power
- C++ like compile-time type checking and template expansion
- native XML-DOM access
- javascript-like generic variables
- Dylan like shortcuts and syntactic sugar
- powerful container classes
- native database access
- easy and straightforward binding to native objects

in particular:

- garbage collection
- constructors and destructors
- scopes
- objects
- small subset of native types
- singletons


hardcoded types (atoms):

atomic types: bool byte char int float name var? op? type?

literals:
bool: true, false
byte: 00 - FF
char: 'a'-'z' etc.
int: 12345
float: 1.2345
type: <hallo>
name: bla
scope: { statement;statement; }
statement: call(list) | lhs = expression

expression = unaryexpr | binaryexpr

unaryexpr = op expr
binaryexpr = expr op expr

structures: const<type> var<type> string<type>, set<type>, binding<type, type> function<type>

operators: . [] , {} () <> ;


statement: name(expr,expr,expr)

every expression yields a type/value pair; a value can be a type as well (of type type)

a type has a name and a value list (which can be types or values or empty)
a value has a type and a value

bool plus{int a, int b};

plus(int a, int b) = { a + b };

bool = type<>(bool)

<TrueBool> = bool(true);

<rect> = {top = int, bottom = int, left = int, right = int };
<zerorect> = rect(0,0,0,0);
<emptyrect> = rect : {true(top >= bottom) or true(left >= right) } ;

name


, : a , b

type<

var<bool> = name("myValue");
myValue = bool(true);

name<myValue>


type name op expression;

interface <var<bool>





type: literal type bool byte word char int float double name string block raster map vector list expression statement scope

bool:




-----------------

values:

type: <bool>

bool: #true

int: 12345

float: 1.0

string: "abcdef"

name: abc

list: { 0 , "abc", 1, x } // any type

vector: [ 0, 1, 2, 3, 4 ] // all of same type

map: ( x = 1, y = 2 , z = 3) // any type


<int> + <int> : @plusIntInt;



- a name can be bound to any value
- everything is a value
- every value has a type
- a type is also a value

<int> a; // binds name a to a value of type int
a = 2; // assign 2

a = <int>





myFunction


function myFunction = (bool x = true, count y = 0) : { x * y };

{
    bool b = true;
    count c = 1;
    number x = 2.0;
    string myString = "bla";

    object myObject = {
        count c = 0;
        function init() = {
            run();
        };
    };
}

x = { 1,2,3 }

y = [4, 5, 6]

define abs(x) = { if (x > 0)


program = {statement}

program := statement;<program>

statement := declaration|assignment

declaration := type name

type = ("bool"|"count"|"number"|"string"|"object")
name = [a-z,A-Z][a-z,A-Z,0-9,_]

assignment := adress = value

value := name|constant|expression


adress = declaration

==================================================

Performance tests yield the following results:

- the naive approach is overall as fast as the spidermonkey js engine; in some interesting case much faster (8x)
- C++ native loops and function calls are still 100x faster

==============================================================



Basics Logical Language Structures:


List (of anything)
Vector
Sequence
Alternative
Option
Loop





*/


//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: dyscript.cpp,v $
//   $Author: pavel $
//   $Revision: 1.4 $
//   $Date: 2005/03/07 20:28:27 $
//
//   Description: Quick-and-dirty wrapper to call our JS test scripts
//
//=============================================================================

//#include "jssettings.h"
//#include "xpshell.h"

#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/UnitTest.h>
#include <asl/base/Time.h>

#include <asl/base/Dashboard.h>
#include <asl/math/linearAlgebra.h>

#include <asl/dom/Value.h>

#include <vector>
#include <map>
#include <string>
#include <iostream>

#include "Lexer.h"

using namespace std;

#undef MAKE_SCOPE_TIMER
#define MAKE_SCOPE_TIMER(x)

namespace dys {

    typedef asl::SingleThreaded ThreadingModel;

    class Scope;
    class ValueBase;
    //typedef asl::Ptr<ValueBase, ThreadingModel> ValuePtr;
    typedef asl::Ptr<ValueBase, ThreadingModel> ValuePtr;

    class Rule;
    typedef asl::Ptr<Rule, ThreadingModel> RulePtr;

    class Rule {
    public:
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) = 0;
        virtual RulePtr clone() const = 0;
    };

    class RuleHolder : public Rule {
    public:
        RuleHolder () {}
        RuleHolder(const Rule & theRule) {
           _myRule = theRule.clone();
        }
        Rule & operator=(const Rule & theRule) {
            _myRule = theRule.clone();
            return *this;
        }
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) {
            return _myRule->parse(theProgram, theTokenList, theTokenIndex);
        }
        RulePtr clone() const {
            return RulePtr(new RuleHolder(*_myRule));
        }
    private:
        RulePtr _myRule;
    };

    class Sequence : public Rule {
    public:
        Sequence() {}
        Sequence(const Sequence & theSequence) {
            _mySequence = theSequence._mySequence;
        }
        Sequence & operator=(const Sequence & theSequence) {
            _mySequence = theSequence._mySequence;
            return *this;
        }
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) {
            unsigned int myTokenIndex = theTokenIndex;
            for (int i = 0; i < _mySequence.size(); ++i) {
                unsigned int myNewTokenIndex = _mySequence[i]->parse(theProgram, theTokenList, myTokenIndex);
                if (myNewTokenIndex == myTokenIndex) {
                    return theTokenIndex;
                }
                myTokenIndex = myNewTokenIndex;
            }
            return myTokenIndex;
        }
        RulePtr clone() const {
            return RulePtr(new Sequence(*this));
        }
        void addNextToken(RulePtr theRule) {
            _mySequence.push_back(theRule);
        }
    private:
        std::vector<RulePtr> _mySequence;
    };

    class Optional : public RuleHolder {
    public:
        Optional () {}
        Optional(const Rule & theRule) : RuleHolder(theRule) {}
        Optional(const Optional & theRule) : RuleHolder(theRule) {}

        Optional & operator=(const Optional & theRule) {
            _myRule = theRule.clone();
            return *this;
        }
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) {
            unsigned int myTokenIndex = _myRule->parse(theProgram, theTokenList, theTokenIndex);
            if (myTokenIndex == theTokenIndex && _myNextRule) {
                return _myNextRule->parse(theProgram, theTokenList, theTokenIndex);
            }
            return myTokenIndex;
        }
        RulePtr clone() const {
            return RulePtr(new Optional(*_myRule));
        }
    private:
        RulePtr _myRule;
        RulePtr _myNextRule;
    };

    class Alternative : public Rule {
    public:
        Alternative() {}
        Alternative(const Alternative & theAlternative) {
            _myAlternatives = theAlternative._myAlternatives;
        }
        Alternative & operator=(const Alternative & theAlternative) {
            _myAlternatives = theAlternative._myAlternatives;
            return *this;
        }
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) {
            unsigned int myBestAlternative = _myAlternatives.size();
            unsigned int myBestTokenIndex = theTokenList.size();
            for (int i = 0; i < _myAlternatives.size(); ++i) {
                unsigned int myMatchTokenIndex = _myAlternatives[i]->parse(theProgram, theTokenList, theTokenIndex);
                if (myMatchTokenIndex > myBestTokenIndex) {
                    myBestAlternative = i;
                    myBestTokenIndex = myMatchTokenIndex;
                }
            }
            if (myBestAlternative != _myAlternatives.size()) {
                return myBestTokenIndex;
            } else {
                return theTokenIndex;
            }
        }
        RulePtr clone() const {
            return RulePtr(new Alternative(*this));
        }
        void addAlternative(RulePtr theRule) {
            _myAlternatives.push_back(theRule);
        }
    private:
        std::vector<RulePtr> _myAlternatives;
    };
    Alternative operator|(Alternative & theLeftRule, const Rule & theRightRule) {
        theLeftRule.addAlternative(theRightRule.clone());
        return theLeftRule;
    }
    Sequence operator&(Sequence & theLeftRule, const Rule & theRightRule) {
        theLeftRule.addNextToken(theRightRule.clone());
        return theLeftRule;
    }
    Alternative operator|(const Rule & theLeftRule, const Rule & theRightRule) {
        Alternative myNewAlternative;
        myNewAlternative.addAlternative(theLeftRule.clone());
        myNewAlternative.addAlternative(theRightRule.clone());
        return myNewAlternative;
    }
    Sequence operator&(const Rule & theLeftRule, const Rule & theRightRule) {
        Sequence myNewSequence;
        myNewSequence.addNextToken(theLeftRule.clone());
        myNewSequence.addNextToken(theRightRule.clone());
        return myNewSequence;
    }
    Optional operator~(const Rule & theRule) {
        return Optional(theRule);
    }
    Sequence operator&(const Optional & theLeftRule, const Rule & theRightRule) {
        Sequence myNewSequence;
//        myNewSequence.addOptionalToken(theLeftRule);
 //       myNewSequence.addNextToken(theRightRule);
        return myNewSequence;
    }
    Sequence operator&(Sequence & theLeftRule, const Optional & theRightRule) {
        theLeftRule.addNextToken(theRightRule.clone());
        return theLeftRule;
    }

    class Match : public Rule {
    public:
        Match(const Match & theMatch) : _myToken(theMatch._myToken) {}
        Match & operator=(const Match & theMatch) {
            _myToken = theMatch._myToken;
            return *this;
        }
        Match(TokenId theToken) : _myToken(theToken) {}
        virtual unsigned int parse(const String & theProgram, const TokenList & theTokenList, unsigned int theTokenIndex) {
            if (theTokenList[theTokenIndex]->getId() == _myToken && theTokenIndex<theTokenList.size()) {
                return theTokenIndex+1;
            }
            return theTokenIndex;
        }
        RulePtr clone() const {
            return RulePtr(new Match(*this));
        }
    private:
        TokenId _myToken;
    };

    class DyscriptParser : public DyscriptLexer {
    public:
        RuleHolder Statement;
        RuleHolder Block;
        RuleHolder VariableStatement;
        RuleHolder EmptyStatement;
        RuleHolder ExpressionStatement;
        RuleHolder IfStatement;
        RuleHolder IterationStatement;
        RuleHolder ContinueStatement;
        RuleHolder BreakStatement;
        RuleHolder ReturnStatement;
        RuleHolder WithStatement;
        RuleHolder LabelledStatement;
        RuleHolder SwitchStatement;
        RuleHolder ThrowStatement;
        RuleHolder TryStatement;
        RuleHolder FunctionDeclaration;
        RuleHolder StatementList;

        RuleHolder FunctionExpression;;
        RuleHolder FormalParameterList;
        RuleHolder FunctionBody;
        RuleHolder Program;
        RuleHolder SourceElements;
        RuleHolder SourceElement;

        DyscriptParser()
        {
            Statement =
                Block |
                VariableStatement |
                EmptyStatement |
                ExpressionStatement |
                IfStatement |
                IterationStatement |
                ContinueStatement |
                BreakStatement |
                ReturnStatement |
                WithStatement |
                LabelledStatement |
                SwitchStatement |
                ThrowStatement |
                TryStatement;

            Block = Match(OPEN_CURLY_BRACE) & Optional(StatementList) & Match(CLOSE_CURLY_BRACE);
            StatementList =
                Statement |
                StatementList & Statement;

            // Functions & Programs
            FunctionDeclaration =
                Match(FUNCTION) & Match(IDENTIFIER) & Optional(FormalParameterList) & Block;
/*
            FunctionExpression =
                Match(FUNCTION) & Optional(IDENTIFIER) & Optional(FormalParameterList) & Block;

            FormalParameterList =
                Identifier |
                FormalParameterList & Match(IDENTIFIER);

            FunctionBody =
                SourceElements;

            Program =
                SourceElements;

            SourceElements =
                SourceElement | SourceElements & SourceElement;

            SourceElement =
                Statement |
                FunctionDeclaration;
                */
       }
        unsigned int parse(const String & is, TokenList & theTokens) {
            return 0;
        }

    };

    class Statement {
    public:
        //virtual unsigned int parse(const String & is, unsigned int thePos, Parser & theParser) = 0;
        //virtual void print(ostream & os, Printer & thePrinter) = 0;
        virtual void exec(Scope * theScope) = 0;
        virtual ~Statement() {};
    };
    typedef asl::Ptr<Statement,ThreadingModel> StatementPtr;

    class Expression : public Statement {
    public:
        virtual ValueBase * eval(Scope * theScope) = 0;
        virtual void exec(Scope * theScope) {
            eval(theScope);
        }
        virtual ~Expression() {};
    };
    typedef asl::Ptr<Expression,ThreadingModel> ExpressionPtr;


    class ScriptFunction;
    typedef asl::Ptr<ScriptFunction,ThreadingModel> ScriptFunctionPtr;


    typedef void (*UnaryIntOp)(int &);
    typedef void (*BinaryIntOp)(int &, int, int);

    class ValueBase : public asl::ReadableBlock { //: public Expression {
    public:
        operator const void*() const {
            return getBool() ? this : NULL;
        }
        virtual bool getBool() const = 0;
        virtual void setBool(bool b) = 0;

        virtual int getInt() const = 0;
        virtual void setInt(int i) = 0;

        virtual std::string getString() const = 0;
        virtual void setString(const string & theString) = 0;

        virtual void setBlock(const asl::ReadableBlock & theBlock) = 0;

        virtual void assign(const ValueBase & theValue) = 0;

        virtual asl::Matrix4f * getMatrix4f() {
            return 0;
        }
        virtual ScriptFunctionPtr getScriptFunction() {
            return ScriptFunctionPtr(0);
        }

        virtual ~ValueBase() {};
        virtual void callUnaryIntOp(UnaryIntOp theOp) {
            int myInt = getInt();
            theOp(myInt);
            setInt(myInt);
        }
        virtual void assignBinaryIntOpResult(BinaryIntOp theOp, ValueBase & theValue0, ValueBase & theValue1) {
            int myInt = getInt();
            theOp(myInt, theValue0.getInt(), theValue1.getInt());
            setInt(myInt);
        }
        virtual void increment() {
            setInt(getInt()+1);
        }
     };

    template <class T>
    struct RTTI_Traits {
        template <class U>
        static
        T * getTPtr(U *) {
            return 0;
        }
    };

    template<>
    struct RTTI_Traits<asl::Matrix4f> {
        template <class U>
        static
        asl::Matrix4f * getTPtr(U *) {
            return 0;
        }
        static
        asl::Matrix4f * getTPtr(asl::Matrix4f * up) {
            return up;
        }
    };
    template<>
    struct RTTI_Traits<ScriptFunctionPtr> {
        template <class U>
        static
        ScriptFunctionPtr * getTPtr(U *) {
            return 0;
        }
        static
        ScriptFunctionPtr * getTPtr(ScriptFunctionPtr * up) {
            return up;
        }
    };

    template <class T>
    class Value : public ValueBase, public asl::PoolAllocator<Value<T> > {
    public:
        Value() {}
        Value(const T & theValue) {
            _myValue.getData() = theValue;
        }
        virtual bool getBool() const {
            return getInt() != 0;
        }
        virtual void setBool(bool b) {
            _myValue.getData() = asl::as<T>(asl::as_string(b));
        }
        virtual int getInt() const {
            return asl::as<int>(getString());
        }
        virtual void setInt(int i) {
            _myValue.getData() = asl::as<T>(asl::as_string(i));
        }
        virtual std::string getString() const {
            return asl::as_string(_myValue.getData());
        }
        virtual void setString(const string & theString) {
            _myValue.getData() = asl::as<T>(theString);
        }
        virtual void setBlock(const asl::ReadableBlock & theBlock) {
            _myValue.assign(theBlock);
        }
        virtual asl::Matrix4f * getMatrix4f() {
            return RTTI_Traits<asl::Matrix4f>::getTPtr(&_myValue.getData());
        }
        virtual ScriptFunctionPtr getScriptFunction() {
            return *RTTI_Traits<ScriptFunctionPtr>::getTPtr(&_myValue.getData());
        }
        virtual void assign(const ValueBase & theValue) {
            _myValue.assign(theValue);
        }
        //virtual ValueBase * eval(Scope * theScope) {
        //    return this;
        //}
        virtual const unsigned char * begin() const {
           return _myValue.begin();
        }
        virtual const unsigned char * end() const {
           return _myValue.end();
        }
   private:
        asl::FixedBlock<T> _myValue;
    };

    void incrementOp(int & i) {
        ++i;
    }
    void plusOp(int & i, int a, int b) {
        i = a + b;
    }

    template <>
    class Value<int> : public ValueBase, public asl::PoolAllocator<Value<int> > {
    public:
        Value(int theValue) {
            _myValue = theValue;
        }
        virtual bool getBool() const {
            return _myValue != 0;
        }
        virtual void setBool(bool b) {
            _myValue = b;
        }
        virtual int getInt() const {
            return _myValue;
        }
        virtual void setInt(int i) {
            _myValue = i;
        }
        virtual std::string getString() const {
            return asl::as_string(_myValue);
        }
        virtual void setString(const string & theString) {
            _myValue = asl::as<int>(theString);
        }
        virtual void setBlock(const asl::ReadableBlock & theBlock) {
            _myValue = *reinterpret_cast<const int*>(theBlock.begin());
        }
        virtual void assign(const ValueBase & theValue) {
            _myValue = theValue.getInt();
        }
        virtual const unsigned char * begin() const {
           return reinterpret_cast<const unsigned char *>(&_myValue);
        }
        virtual const unsigned char * end() const {
           return reinterpret_cast<const unsigned char *>(&_myValue) + sizeof(_myValue);
        }
        virtual void callUnaryIntOp(UnaryIntOp theOp) {
            theOp(_myValue);
        }
        virtual void assignBinaryIntOpResult(BinaryIntOp theOp, ValueBase & theValue0, ValueBase & theValue1) {
            theOp(_myValue, theValue0.getInt(), theValue1.getInt());
        }
        virtual void increment() {
            ++_myValue;
        }
    private:
        int _myValue;
    };

    template <>
    class Value<bool> : public ValueBase, public asl::PoolAllocator<Value<int> > {
    public:
        Value(int theValue) {
            _myValue = theValue;
        }
        virtual bool getBool() const {
            return _myValue;
        }
        virtual void setBool(bool b) {
            _myValue = b;
        }
        virtual int getInt() const {
            return _myValue;
        }
        virtual void setInt(int i) {
            _myValue = i;
        }
        virtual std::string getString() const {
            return asl::as_string(_myValue);
        }
        virtual void setString(const string & theString) {
            _myValue = asl::as<bool>(theString);
        }
        virtual void setBlock(const asl::ReadableBlock & theBlock) {
            _myValue = *reinterpret_cast<const bool*>(theBlock.begin());
        }
        virtual void assign(const ValueBase & theValue) {
            _myValue = theValue.getBool();
        }
        virtual const unsigned char * begin() const {
           return reinterpret_cast<const unsigned char *>(&_myValue);
        }
        virtual const unsigned char * end() const {
           return reinterpret_cast<const unsigned char *>(&_myValue) + sizeof(_myValue);
        }
    private:
        bool _myValue;
    };

    typedef std::string Name;
    //typedef std::map<Name, ValuePtr> NameValueMap;
    typedef std::map<Name, ValueBase *> NameValueMap;

    struct NameValue /* : public Expression*/ {
        NameValue(const Name & theName, ValuePtr theValue)
            : name(theName), value(theValue)
        {}
        //virtual void eval(Scope * theScope);
        Name name;
        ValuePtr value;
    };

    typedef std::vector<NameValue> NameValueList;
    typedef asl::Ptr<NameValueList, ThreadingModel> NameValueListPtr;

    typedef std::vector<ValuePtr> ValueList;
    typedef asl::Ptr<ValueList, ThreadingModel> ValueListPtr;
/*
    class Function : public Expression {
    public:
        virtual void eval(Scope * theScope) = 0;
        void passParameters(NameValueListPtr theList);
        void passParameters(ValueListPtr theValues);
    };
    typedef asl::Ptr<Function,ThreadingModel> FunctionPtr;
  */


    class Scope : public Expression {
    public:
        Scope() : _myParent(0) {}
        Scope(NameValueListPtr theParameters)
            : _myParameters(theParameters), _myParent(0)
        {
        }
        void passParameters(NameValueListPtr theList) {
#ifdef FAST_PARM
            _myParameters = theList;
#else
            for (int i = 0; i < theList->size(); ++i) {
                int myIndex = findNamedParameterIndex((*theList)[i].name);
                if (myIndex >= 0) {
                    (*_myParameters)[myIndex].value->assign(*((*theList)[i].value));
                }
            }
#endif
        }
        void passParameters(ValueListPtr theValues) {
            for (int i = 0; i < theValues->size(); ++i) {
#ifdef FAST_PARM
                (*_myParameters)[i].value = (*theValues)[i];
#else
                (*_myParameters)[i].value->assign(*((*theValues)[i]));
#endif
            }
        }
        void setParameter(int theIndex, ValueBase * theValue) {
#ifdef FAST_PARM
                (*_myParameters)[theIndex].value = theValue;
#else
                (*_myParameters)[theIndex].value->assign(*theValue);
#endif
        }
        /*
        void fetchParametersFromResultStack(Scope * theSourceScope) {
            for (int i = 0; i < _myParameters->size(); ++i) {
                ValuePtr myValue;
                if (theSourceScope->popValue(myValue)) {
#ifdef FAST_PARM
                    (*_myParameters)[i].value = myValue;
#else
                    (*_myParameters)[i].value->assign(*myValue);
#endif
                } else {
                    cerr << "fetchParametersFromResultStack: not enough parameters" << endl;
                }
            }
        }
        */
        /*
        // pass named values
        void passParameters(const NameValueList & theList) {
        for (int i = 0; i < _myParameters.size(); ++i) {
        declare(_myParameters[i].name, _myParameters[i].value);
        }
        for (int i = 0; i < theList.size(); ++i) {
        assign(theList[i].name, theList[i].value);
        }
        }
        // pass unnamed values
        void passParameters(const std::vector<ValuePtr> & theValues) {
        if (theValues.size() != _myParameters.size()) {
        cerr <<"passParameters: parameter count mismatch"<< endl;
        return;
        }
        for (int i = 0; i < theValues.size(); ++i) {
        declare(_myParameters[i].name, _myParameters[i].value);
        _myParameters[i].value->assign(*theValues[i]);
        }
        }
        */
        void exec(Scope * theScope) {
            _myParent = theScope;
            for (int i = 0; i < _myStatements.size(); ++i) {
                _myStatements[i]->exec(this);
            }
            _myVariables.clear();
        }
        ValueBase * eval(Scope * theScope) {
            _myParent = theScope;
            for (int i = 0; i < _myStatements.size(); ++i) {
                _myStatements[i]->exec(this);
            }
            _myVariables.clear();
//            return _myReturnValue.getNativePtr();
            return _myReturnValue;
        }
        /*
        ValuePtr eval(Scope * theScope, NameValueList & theArguments) {
        _myVariables.clear();
        passParameters(theArguments);
        return eval(theScope);
        }
        ValuePtr eval(Scope * theScope, std::vector<ValuePtr> & theArguments) {
        _myVariables.clear();
        passParameters(theArguments);
        return eval(theScope);
        }
        */
        void declare(const Name & theName, ValueBase * theValue) {
            //cerr << "Scope::declare name='"<<theName<<", value = '"<< theValue->getString()<<"'"<<endl;
            NameValueMap::iterator myIt = _myVariables.find(theName);
            if (myIt != _myVariables.end()) {
                cerr << "#ERROR: variable '"<<theName<<"' already declared in scope, redefining it" << endl;
            }
            _myVariables[theName] = theValue; //OWN
        }
        /*
        void assign(const Name & theName, ValuePtr theValue) {
        NameValueMap::iterator myIt = _myVariables.find(theName);
        if (myIt == _myVariables.end()) {
        cerr << "#WARNING: variable '"<<theName<<"' not declared in scope, defining it" << endl;
        }
        _myVariables[theName] = theValue;
        }
        */
        void append(StatementPtr theStatement) {
            _myStatements.push_back(theStatement);
        }
        int findNamedParameterIndex(const Name & theName) {
            if (_myParameters) {
                for (int i = 0; i < _myParameters->size();++i) {
                    if ((*_myParameters)[i].name == theName) {
                        return i;
                    }
                }
            }
            return -1;
        }
        ValueBase * findNamedValue(const Name & theName) {
            int myParamIndex = findNamedParameterIndex(theName);
            if (myParamIndex >= 0) {
                return (*_myParameters)[myParamIndex].value.getNativePtr();
            }
            NameValueMap::iterator myIt = _myVariables.find(theName);
            if (myIt == _myVariables.end()) {
                if (_myParent) {
                    return _myParent->findNamedValue(theName);
                }
                cerr << "#ERROR: Undeclared Variable: "<<theName<<endl;
                return 0;//ValuePtr(0);
            }
            return myIt->second;
        }
        void dump(const std::string & theHeader) {
            cerr<<theHeader<<endl;
            for (NameValueMap::const_iterator myIterator = _myVariables.begin();
                myIterator !=_myVariables.end(); ++myIterator)
            {
                cerr << myIterator->first << " = " << myIterator->second->getString()<<endl;
            }
        }
        /*
        void pushValue(ValuePtr theValue) {
            _myReturnValues.push_back(theValue);
        }
        bool popValue(ValuePtr & theValue) {
            if (!_myReturnValues.empty()) {
                theValue = _myReturnValues.back();
                _myReturnValues.pop_back();
                return true;
            }
            return false;
        }
        */
#ifdef SMART_RVAL
        void setReturnValue(ValuePtr theValue) {
            _myReturnValue = theValue;
        }
        ValuePtr getReturnValue() {
            return _myReturnValue;
        }
    private:
        ValuePtr _myReturnValue;
#else
        void setReturnValue(ValueBase * theValue) {
            _myReturnValue = theValue;
        }
        ValueBase * getReturnValue() {
            return _myReturnValue;
        }
    private:
        ValueBase * _myReturnValue;
#endif
        NameValueListPtr _myParameters;
        NameValueMap _myVariables;
        std::vector<StatementPtr> _myStatements;
        Scope * _myParent;
        //std::vector<ValuePtr> _myReturnValues;
    };
    typedef asl::Ptr<Scope,ThreadingModel> ScopePtr;
    /*
    inline
    void NamedValue::eval(Scope * theScope) {
        theScope->pushValue(value);
    }
    */

    class ArgumentList /* : public Statement*/ {
    public:
        ArgumentList() {}
        ArgumentList(std::vector<ExpressionPtr> theExpressions)
            : _myArgumentExpressions(theExpressions)
        {}
        void exec(Scope * theScope, Scope * theFunctionScope) {
            for (int i = 0; i < _myArgumentExpressions.size(); ++i) {
                theFunctionScope->setParameter(i,_myArgumentExpressions[i]->eval(theScope));
            }
        }
        void append(ExpressionPtr theNewArg) {
            _myArgumentExpressions.push_back(theNewArg);
        }
    private:
        std::vector<ExpressionPtr> _myArgumentExpressions;
    };
    typedef asl::Ptr<ArgumentList,ThreadingModel> ArgumentListPtr;

    struct Dummy {};

    class ScriptFunction : public Scope {
    public:
        typedef Dummy value_type;
        ScriptFunction() : Scope() {}
        ScriptFunction(NameValueListPtr theParameters)
            : Scope(theParameters)
        {}
    private:
    };
    typedef asl::Ptr<ScriptFunction,ThreadingModel> ScriptFunctionPtr;
    //typedef dom::ComplexValue<ScriptFunction> ScriptFunctionValue;
    typedef Value<ScriptFunctionPtr> ScriptFunctionValue;
    /*
    inline
    void binarize(const ScriptFunctionPtr & myValue, asl::WriteableStream & theDest) {
            //theDest.appendCountedString(myValue);
    };
    inline
    unsigned long debinarize(ScriptFunctionPtr & myValue, const asl::ReadableStream & theSource, unsigned long thePos = 0) {
        //return theSource.readCountedString(myValue,thePos);
        return thePos;
    };
    */
    inline
    std::ostream& operator<<(std::ostream& os, const ScriptFunctionPtr & n) {
        os << "function";
        return os;
    }

    inline
    std::istream& operator>>(std::istream& is, ScriptFunctionPtr & n) {
        return is;
    }
/*
    class NativeFunction : public Function {
    public:
        typedef Dummy value_type;
        NativeFunction(const Name & theName, NameValueList & theParameters) {}
        virtual ValuePtr eval(Scope * theScope) {}
        virtual ValuePtr eval(Scope * theScope, NameValueList & theArguments) {}
        virtual ValuePtr eval(Scope * theScope, std::vector<ValuePtr> & theArguments) {}
    private:
    };
    typedef dom::ComplexValue<NativeFunction> NativeFunctionValue;
*/
    class FunctionCall : public Expression {
    public:
        FunctionCall(const Name & theName, ArgumentListPtr theArgumentList)
            : _myName(theName), _myArgumentList(theArgumentList), _myCachedFunction(0) {}

        void exec(Scope * theScope) {
            if (!_myCachedFunction) {
                ValueBase * myValue = theScope->findNamedValue(_myName);
                if (myValue) {
                    ScriptFunctionPtr myScriptFunction = myValue->getScriptFunction();
                    if (myScriptFunction) {
                        _myCachedFunction = myScriptFunction.getNativePtr();
#if 1
                    } else {
                        cerr << "#ERROR: FunctionCall: argument not a function" << endl;
                    }
#else
                    } else {
                        NativeFunction * myNativeFunction = dom::dynamic_cast_Value<NativeFunction>(myValue.get());
                        _myCachedFunction = myNativeFunction;
                    }
#endif
                }

            }
            _myArgumentList->exec(theScope, _myCachedFunction);
            _myCachedFunction->eval(theScope);
        }
        ValueBase * eval(Scope * theScope) {
            exec(theScope);
//            return theScope->getReturnValue().getNativePtr();
            return _myCachedFunction->getReturnValue();
        }
    private:
        Name _myName;
        ScriptFunction * _myCachedFunction;
        ArgumentListPtr _myArgumentList;
    };

    class Declaration : public Statement {
    public:
        Declaration(const Name & theName, ValuePtr theValue, ExpressionPtr theExpression)
            : _myName(theName), _myInitExpression(theExpression), _myValue(theValue)
        {}
        void exec(Scope * theScope) {
            _myValue->assign(*_myInitExpression->eval(theScope));
            theScope->declare(_myName, _myValue.getNativePtr());
        }
    private:
        Name _myName;
        ExpressionPtr _myInitExpression;
        ValuePtr _myValue;
    };

    class Return : public Statement {
    public:
        Return(ExpressionPtr theExpression)
            : _myExpression(theExpression)
        {}
        void exec(Scope * theScope) {
            theScope->setReturnValue(_myExpression->eval(theScope));
        }
    private:
        ExpressionPtr _myExpression;
    };

    class ValueExpression : public Expression {
    public:
        ValueExpression(ValuePtr theValue) : _myValue(theValue) {}
        ValueBase * eval(Scope *) {
            return _myValue.getNativePtr();
        }
    private:
        ValuePtr _myValue;
    };

    class Variable : public Expression {
    public:
        Variable(const Name & theName) : _myName(theName), _myValueCache(0) {}
        ValueBase * eval(Scope * theScope) {
            if (!_myValueCache) {
                //_myValueOwner = theScope->findNamedValue(_myName);
                //_myValueCache = _myValueOwner.getNativePtr();
                _myValueCache = theScope->findNamedValue(_myName);
            }
            return _myValueCache;
        }
    private:
        ValueBase * _myValueCache;
        Name _myName;
//        ValuePtr _myValueOwner;
    };

    class TimerStarter : public Statement {
    public:
        TimerStarter(const Name & theName)  {
           _myTimer = asl::getDashboard().getTimer(theName);
        }
        void exec(Scope * theScope) {
            _myTimer->start();
        }
    private:
        asl::TimerPtr _myTimer;
    };

    class TimerStopper : public Statement {
    public:
        TimerStopper(const Name & theName, unsigned long theCount) : _myCount(theCount)  {
           _myTimer = asl::getDashboard().getTimer(theName);
        }
        void exec(Scope * theScope) {
            _myTimer->stop(_myCount);
        }
    private:
        asl::TimerPtr _myTimer;
        unsigned long _myCount;
    };

    typedef void (*UnaryOperationPtr)(ValueBase *  theValue);
    typedef void (*UnaryFunctionPtr)(ValueBase *  theValue, ValueBase * theResult);
    typedef void (*BinaryOperationPtr)(ValueBase *  theValue0, ValueBase * theValue1);
    typedef void (*BinaryFunctionPtr)(ValueBase *  theValue0, ValueBase * theValue1, ValueBase * theResult);

    struct Op {
        static void plus(ValueBase * theValue0, ValueBase * theValue1, ValueBase * theResult) {
            theResult->setInt(theValue0->getInt() + theValue1->getInt());
        }
        static void less(ValueBase * theValue0, ValueBase * theValue1, ValueBase * theResult) {
            theResult->setBool(theValue0->getInt() < theValue1->getInt());
        }
        static void increment(ValueBase * theValue) {
            theValue->setInt(theValue->getInt()+1);
        }
        static void multiply(ValueBase * theObject, ValueBase * theValue) {
            asl::Matrix4f * myObj = theObject->getMatrix4f();
            asl::Matrix4f * myArg = theValue->getMatrix4f();

            if (myObj && myArg) {
                myObj->postMultiply(*myArg);
            }
        }

        static void product(ValueBase * theValue0, ValueBase * theValue1, ValueBase * theResult) {
            asl::Matrix4f * myObj1 = theValue0->getMatrix4f();
            asl::Matrix4f * myObj2 = theValue1->getMatrix4f();
            asl::Matrix4f * myResult = theResult->getMatrix4f();

            if (myObj1 && myObj2 && myResult) {
                *myResult = *myObj1;
                myResult->postMultiply(*myObj2);
            } else {
                AC_ERROR << "product: operands are not a Matrix4f" << endl;
            }
        }
    };

    class UnaryExpression : public Expression {
    public:
        UnaryExpression(UnaryFunctionPtr theOperation, ExpressionPtr theSubExpression, ValuePtr theReturnValue)
            : _myOperation(theOperation), _mySubExpression(theSubExpression), _myReturnValue(theReturnValue) {}
        ValueBase * eval(Scope * theScope) {
            (*_myOperation)(_mySubExpression->eval(theScope), _myReturnValue.getNativePtr());
            return _myReturnValue.getNativePtr();
        }
    private:
        UnaryFunctionPtr _myOperation;
        ExpressionPtr _mySubExpression;
        ValuePtr _myReturnValue;
    };

     class UnaryStatement : public Statement {
     public:
        UnaryStatement(UnaryOperationPtr theOperation, ExpressionPtr theSubExpression)
            : _myOperation(theOperation), _mySubExpression(theSubExpression) {}
        void exec(Scope * theScope) {
            (*_myOperation)(_mySubExpression->eval(theScope));
        }
     private:
        UnaryOperationPtr _myOperation;
        ExpressionPtr _mySubExpression;
    };

     class Increment : public Statement {
     public:
        Increment(ExpressionPtr theSubExpression)
            : _mySubExpression(theSubExpression), _myExpCache(_mySubExpression.getNativePtr()) {}
        void exec(Scope * theScope) {
            _myExpCache->eval(theScope)->increment();
        }
     private:
        ExpressionPtr _mySubExpression;
        Expression * _myExpCache;
    };

    class FastUnaryStatement : public Statement {
    public:
        FastUnaryStatement(UnaryIntOp theOperation, ExpressionPtr theSubExpression)
            : _myOperation(theOperation), _mySubExpression(theSubExpression) {}
        void exec(Scope * theScope) {
            ValueBase * myValue = _mySubExpression->eval(theScope);
            myValue->callUnaryIntOp(_myOperation);
        }
    private:
        UnaryIntOp _myOperation;
        ExpressionPtr _mySubExpression;
    };


    class BinaryExpression : public Expression {
    public:
        BinaryExpression(
            BinaryFunctionPtr theOperation,
            ExpressionPtr theSubExpression0,
            ExpressionPtr theSubExpression1,
            ValuePtr theReturnValue)
            :
            _myOperation(theOperation),
            _mySubExpression0(theSubExpression0),
            _mySubExpression1(theSubExpression1),
            _myReturnValue(theReturnValue)
        {}
        ValueBase * eval(Scope * theScope) {
            (*_myOperation)(_mySubExpression0->eval(theScope),_mySubExpression1->eval(theScope), _myReturnValue.getNativePtr());
            return _myReturnValue.getNativePtr();
        }
    private:
        BinaryFunctionPtr _myOperation;
        ExpressionPtr _mySubExpression0;
        ExpressionPtr _mySubExpression1;
        ValuePtr _myReturnValue;
    };
    class BinaryStatement : public Statement {
    public:
        BinaryStatement(BinaryOperationPtr theOperation, ExpressionPtr theSubExpression0, ExpressionPtr theSubExpression1)
            : _myOperation(theOperation), _mySubExpression0(theSubExpression0), _mySubExpression1(theSubExpression1)
        {}
        void exec(Scope * theScope) {
            (*_myOperation)(_mySubExpression0->eval(theScope),_mySubExpression1->eval(theScope));
        }
    private:
        BinaryOperationPtr _myOperation;
        ExpressionPtr _mySubExpression0;
        ExpressionPtr _mySubExpression1;
    };

    class Assignment : public Statement {
    public:
        Assignment(ExpressionPtr theLHS, ExpressionPtr theRHS)
            : _myLHS(theLHS), _myRHS(theRHS)
        {}
        void exec(Scope * theScope) {
            ValueBase * myRHSVal = _myRHS->eval(theScope);
            ValueBase * myLHSVal = _myLHS->eval(theScope);
            myLHSVal->assign(*myRHSVal);
        }
    private:
        ExpressionPtr _myRHS;
        ExpressionPtr _myLHS;
    };

    class AssignBinaryIntOpResult : public Statement {
    public:
        AssignBinaryIntOpResult(ExpressionPtr theLHS,
                                BinaryIntOp theOp,
                                ExpressionPtr theArg0,
                                ExpressionPtr theArg1)
            : _myLHS(theLHS), _myOp(theOp), _myArg0(theArg0), _myArg1(theArg1)
        {}
        void exec(Scope * theScope) {
            ValueBase * myLHSVal = _myLHS->eval(theScope);
            myLHSVal->assignBinaryIntOpResult(_myOp, *_myArg0->eval(theScope), *_myArg1->eval(theScope));
        }
    private:
        ExpressionPtr _myLHS;
        ExpressionPtr _myArg0;
        ExpressionPtr _myArg1;
        BinaryIntOp _myOp;
    };

    class Less : public Expression {
    public:
        Less (
            ExpressionPtr theSubExpression0,
            ExpressionPtr theSubExpression1,
            ValuePtr theReturnValue)
            :
            _mySubExpression0(theSubExpression0),
            _mySubExpression1(theSubExpression1),
            _myReturnValue(theReturnValue)
        {}
        ValueBase * eval(Scope * theScope) {
            _myReturnValue->setBool(_mySubExpression0->eval(theScope)->getInt() < _mySubExpression1->eval(theScope)->getInt());
            return _myReturnValue.getNativePtr();
        }
    private:
        ExpressionPtr _mySubExpression0;
        ExpressionPtr _mySubExpression1;
        ValuePtr _myReturnValue;
    };

    class ConstValue : public Expression {
    public:
        ConstValue(ValuePtr theValue)
            : _myValue(theValue)
        {}
        ValueBase * eval(Scope * theScope) {
            return _myValue.getNativePtr();
        }
    private:
        ValuePtr _myValue;
    };
/*
    bool isTrue(ValuePtr theValue) {
        MAKE_SCOPE_TIMER(isTrue);
        if (theValue) {
            bool * myBool = dom::dynamic_cast_Value<bool>(theValue.get());
            if (myBool) {
                return *myBool;
            }
            int * myInt = dom::dynamic_cast_Value<int>(theValue.get());
            if (myInt) {
                return *myInt != 0;
            }
        }
        return false;
    }
    */
    class ForLoop : public Statement {
    public:
        ForLoop(StatementPtr theInit, ExpressionPtr theCondition, StatementPtr thePostBody, StatementPtr theBody)
            : _myInit(theInit), _myCondition(theCondition), _myPostBody(thePostBody),  _myBody(theBody)
        {}
        void exec(Scope * theScope) {
            _myInit->exec(theScope);
            if (_myBody) {
                for (; _myCondition->eval(theScope)->getBool(); _myPostBody->exec(theScope))
                {
                    _myBody->exec(theScope);
                }
            } else {
                for (; _myCondition->eval(theScope)->getBool(); _myPostBody->exec(theScope));
            }
        }
    private:
        StatementPtr _myInit;
        ExpressionPtr _myCondition;
        StatementPtr _myPostBody;
        StatementPtr _myBody;
    };

    class WhileLoop : public Statement {
    public:
        WhileLoop(ExpressionPtr theCondition, StatementPtr theBody)
            :  _myCondition(theCondition), _myBody(theBody)
        {}
        void exec(Scope * theScope) {
            if (_myBody) {
                while (_myCondition->eval(theScope)->getBool())
                {
                    _myBody->exec(theScope);
                }
            } else {
                while (_myCondition->eval(theScope)->getBool()) {}
            }
        }
    private:
        ExpressionPtr _myCondition;
        StatementPtr _myBody;
    };

    /*
    int toInt(ValuePtr theValue) {
        int * myInt = dom::dynamic_cast_Value<int>(theValue.get());
        if (myInt) {
            return *myInt;
        }
        cerr << "ERROR: toInt: argument not an int" << endl;
        return 0;
    }
*/
    class RepeatN : public Statement {
    public:
        RepeatN(ExpressionPtr theCount, StatementPtr theBody)
            : _myCount(theCount), _myBody(theBody)
        {}
        void exec(Scope * theScope) {
            int n = _myCount->eval(theScope)->getInt();
            for (int i = 0; i < n; ++i) {
                _myBody->exec(theScope);
            }
        }
    private:
        ExpressionPtr _myCount;
        StatementPtr _myBody;
    };

    void increment(int & j) {
        ++j;
    }

    void testLoop() {
        /*
        <int name="i">1</int>
        <assign>
            <lhs>
                <var name="i"/>
            </lhs>
            <rhs>
                <plus>
                    <var name="i"/>
                    <int>1</int>
                </plus>
            </rhs>
        </assign>

        int i = 1;
        i = i + 1;
        function add(int a, int b) {
            return a + b;
        }
        for (int i = 0; i < 10000; i = i + 1) {
            add(2,3);
        }

        */
        cerr << "setup..." << endl;
        Scope myProgram;

        // int x = 1
        myProgram.append(StatementPtr(new Declaration("x",ValuePtr(new Value<int>(0)),
            ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(1)))))));

        // x = x + 1
        myProgram.append(
            StatementPtr(new Assignment(
                ExpressionPtr(new Variable("x")),
                ExpressionPtr(new BinaryExpression(
                    &Op::plus,
                    ExpressionPtr(new Variable("x")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(1)))),
                    ValuePtr(new Value<int>(0))
                ))
                ))
            );

        {
            // function add(int a, int b) {
            //     return a + b;
            // }
            NameValueListPtr myParamList =  NameValueListPtr(new NameValueList);
            myParamList->push_back(NameValue("a",ValuePtr(new Value<int>(0))));
            myParamList->push_back(NameValue("b",ValuePtr(new Value<int>(0))));
            ValuePtr myFunctionValue = ValuePtr(new ScriptFunctionValue(ScriptFunctionPtr(new ScriptFunction(myParamList))));

            ScriptFunctionPtr myFunction = myFunctionValue->getScriptFunction();
            myFunction->append(
                    StatementPtr(new Return(
                        ExpressionPtr(new BinaryExpression(
                            &Op::plus,
                            ExpressionPtr(new Variable("a")),
                            ExpressionPtr(new Variable("b")),
                            ValuePtr(new Value<int>(0))
                        ))
                    ))
                );
            myProgram.append(StatementPtr(new Declaration("add", ValuePtr(new ScriptFunctionValue(ScriptFunctionPtr(0))),
                ExpressionPtr(new ValueExpression(myFunctionValue)))));
        }

        {
            // function product(Matrix4 a, Matrix4 b) {
            //     return a * b;
            // }
            NameValueListPtr myParamList =  NameValueListPtr(new NameValueList);
            myParamList->push_back(NameValue("a",ValuePtr(new Value<asl::Matrix4f>())));
            myParamList->push_back(NameValue("b",ValuePtr(new Value<asl::Matrix4f>())));
            ValuePtr myFunctionValue = ValuePtr(new ScriptFunctionValue(ScriptFunctionPtr(new ScriptFunction(myParamList))));

            ScriptFunctionPtr myFunction = myFunctionValue->getScriptFunction();
            myFunction->append(
                    StatementPtr(new Return(
                        ExpressionPtr(new BinaryExpression(
                            &Op::product,
                            ExpressionPtr(new Variable("a")),
                            ExpressionPtr(new Variable("b")),
                            ValuePtr(new Value<asl::Matrix4f>())
                        ))
                    ))
                );
            myProgram.append(StatementPtr(new Declaration("product", ValuePtr(new ScriptFunctionValue(ScriptFunctionPtr(0))),
                ExpressionPtr(new ValueExpression(myFunctionValue)))));
        }
//--------------------
#if 1
        {
            int n = 5000000;
            myProgram.append(StatementPtr(new TimerStarter("while (i<n) i = add(i,1)")));

            ScopePtr myLoopScope = ScopePtr(new Scope);
            ArgumentListPtr myArguments(new ArgumentList);
            myArguments->append(ExpressionPtr(new Variable("i")));
            myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(1)))));

            myLoopScope->append(StatementPtr(new Assignment(
                    ExpressionPtr(new Variable("i")),
                    ExpressionPtr(new FunctionCall("add", myArguments)))));

            myProgram.append(StatementPtr(new Declaration("i",ValuePtr(new Value<int>(0)),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
                ))
            );
            myProgram.append(StatementPtr(new WhileLoop(
                ExpressionPtr(new BinaryExpression(
                    &Op::less,
                    ExpressionPtr(new Variable("i")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
                myLoopScope
            )));
            myProgram.append(StatementPtr(new TimerStopper("while (i<n) i = add(i,1)",n/1000)));
        }
#endif
//--------------------
#if 1
        {
            int n = 2000000;

            myProgram.append(StatementPtr(new Declaration("m1",ValuePtr(new Value<asl::Matrix4f>()),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(asl::Matrix4f::Identity())))))));

            myProgram.append(StatementPtr(new Declaration("m2",ValuePtr(new Value<asl::Matrix4f>()),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>()))))));

            myProgram.append(StatementPtr(new Declaration("m3",ValuePtr(new Value<asl::Matrix4f>()),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>()))))));

            myProgram.append(StatementPtr(new TimerStarter("repeat(n) c = product(m1,m2)")));

            ScopePtr myLoopScope = ScopePtr(new Scope);
            ArgumentListPtr myArguments(new ArgumentList);
            myArguments->append(ExpressionPtr(new Variable("m1")));
            myArguments->append(ExpressionPtr(new Variable("m2")));

            myLoopScope->append(StatementPtr(new Assignment(
                    ExpressionPtr(new Variable("m3")),
                    ExpressionPtr(new FunctionCall("product", myArguments)))));

            //myProgram.append(StatementPtr(new Declaration("i",ValuePtr(new Value<int>(0)),
            //        ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
            //    ))
            //);
            myLoopScope->append(StatementPtr(new Assignment(
                    ExpressionPtr(new Variable("i")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))));

           myProgram.append(
                StatementPtr(new RepeatN(
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    myLoopScope
                 ))
            );

            myProgram.append(StatementPtr(new TimerStopper("repeat(n) c = product(m1,m2)",n/1000)));
        }
#endif
        //--------------------
        {
            int n = 5000000;
            myProgram.append(StatementPtr(new TimerStarter("for x=x+2")));

            ScopePtr myLoopScope = ScopePtr(new Scope);
#ifdef SLOW_FLEX
            myLoopScope->append(
                StatementPtr(new Assignment(
                    ExpressionPtr(new Variable("x")),
                    ExpressionPtr(new BinaryExpression(
                        &Op::plus,
                        ExpressionPtr(new Variable("x")),
                        ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(2)))),
                        ValuePtr(new Value<int>(0))
                    ))
                ))
            );
#else
           myLoopScope->append(
                StatementPtr(new AssignBinaryIntOpResult(
                    ExpressionPtr(new Variable("x")),
                    &plusOp,
                    ExpressionPtr(new Variable("x")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(2))))
                ))
            );
#endif
            myProgram.append(StatementPtr(new ForLoop(
                StatementPtr(new Declaration("j",ValuePtr(new Value<int>(0)),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
                )),
#if 1
                ExpressionPtr(new Less(
                    ExpressionPtr(new Variable("j")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
                StatementPtr(new Increment(
                    ExpressionPtr(new Variable("j"))
                )),
#else
                ExpressionPtr(new BinaryExpression(
                    &Op::less,
                    ExpressionPtr(new Variable("j")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
                StatementPtr(new UnaryStatement(
                    &Op::increment,
                    ExpressionPtr(new Variable("j"))
                )),
#endif
                myLoopScope
            )));
            myProgram.append(StatementPtr(new TimerStopper("for x=x+2",n/1000)));
        }
//--------------------
        {
            int n = 5000000;
            myProgram.append(StatementPtr(new TimerStarter("for add(2,3)")));

            ScopePtr myLoopScope = ScopePtr(new Scope);
            ArgumentListPtr myArguments(new ArgumentList);
            myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(2)))));
            myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(3)))));
            myLoopScope->append(ExpressionPtr(new FunctionCall("add", myArguments)));

            myProgram.append(StatementPtr(new ForLoop(
                StatementPtr(new Declaration("k",ValuePtr(new Value<int>(0)),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
                )),
                ExpressionPtr(new BinaryExpression(
                    &Op::less,
                    ExpressionPtr(new Variable("k")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
#if 1
                StatementPtr(new Increment(
                    ExpressionPtr(new Variable("k"))
                )),
#else
                StatementPtr(new UnaryStatement(
                    &Op::increment,
                    ExpressionPtr(new Variable("k"))
                )),
#endif
                myLoopScope
            )));
            myProgram.append(StatementPtr(new TimerStopper("for add(2,3)",n/1000)));
        }
//--------------------
        {
            int n = 5000000;
            myProgram.append(StatementPtr(new TimerStarter("while(i<n)i+=1")));

            ScopePtr myLoopScope = ScopePtr(new Scope);
            ArgumentListPtr myArguments(new ArgumentList);
            myArguments->append(ExpressionPtr(new Variable("i")));
            myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(1)))));
            myLoopScope->append(ExpressionPtr(new FunctionCall("add", myArguments)));

            //myProgram.append(StatementPtr(new Declaration("i",ValuePtr(new Value<int>(0)),
            //        ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
            //    ))
            //);
            myProgram.append(StatementPtr(new WhileLoop(

                ExpressionPtr(new BinaryExpression(
                    &Op::less,
                    ExpressionPtr(new Variable("i")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
                StatementPtr(new Increment(
                    ExpressionPtr(new Variable("i"))
                ))
            )));
            myProgram.append(StatementPtr(new TimerStopper("while(i<n)i+=1",n/1000)));
        }
//--------------------
        {
            int n = 8000000;
            myProgram.append(StatementPtr(new TimerStarter("for m.mult(ident)")));

            ScopePtr myLoopScope = ScopePtr(new Scope);

            // Matrix4f m(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
            asl::Matrix4f myMatrix;
            myMatrix.assign(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, asl::UNKNOWN);
            asl::Matrix4f myIdentMatrix;
            myIdentMatrix.makeIdentity();
            myProgram.append(StatementPtr(new Declaration("m",ValuePtr(new Value<asl::Matrix4f>(myMatrix)),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myMatrix)))))));
            myLoopScope->append(
                    StatementPtr(new BinaryStatement(
                        &Op::multiply,
                        ExpressionPtr(new Variable("m")),
                        ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myIdentMatrix))))
                        ))
            );

            myProgram.append(StatementPtr(new ForLoop(
                StatementPtr(new Declaration("l",ValuePtr(new Value<int>(0)),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0))))
                )),
                ExpressionPtr(new BinaryExpression(
                    &Op::less,
                    ExpressionPtr(new Variable("l")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    ValuePtr(new Value<bool>(0))
                )),
#if 1
                StatementPtr(new Increment(
                    ExpressionPtr(new Variable("l"))
                )),
#else
                StatementPtr(new UnaryStatement(
                    &Op::increment,
                    ExpressionPtr(new Variable("l"))
                )),
#endif
                myLoopScope
            )));
            myProgram.append(StatementPtr(new TimerStopper("for m.mult(ident)",n/1000)));
        }
//--------------------
        {
            int n = 80000000;
            myProgram.append(StatementPtr(new TimerStarter("repeat ++o")));
            myProgram.append(StatementPtr(new Declaration("o",ValuePtr(new Value<int>(0)),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))));
            myProgram.append(
                StatementPtr(new RepeatN(
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    StatementPtr(new UnaryStatement(
                        &Op::increment,
                        ExpressionPtr(new Variable("o"))
                    ))
                ))
            );

            myProgram.append(StatementPtr(new TimerStopper("repeat ++o",n/1000)));
        }
//--------------------
        {
            int n = 10000000;
            myProgram.append(StatementPtr(new TimerStarter("repeat op ++p")));
            myProgram.append(StatementPtr(new Declaration("p",ValuePtr(new Value<int>(0)),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))));
            myProgram.append(
                StatementPtr(new RepeatN(
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    StatementPtr(new FastUnaryStatement(
                        &incrementOp,
                        ExpressionPtr(new Variable("p"))
                    ))
                ))
            );

            myProgram.append(StatementPtr(new TimerStopper("repeat op ++p",n/1000)));
        }
   //--------------------
        {
            int n = 80000000;
            myProgram.append(StatementPtr(new TimerStarter("repeat inc ++q")));
            myProgram.append(StatementPtr(new Declaration("q",ValuePtr(new Value<int>(0)),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))));
            myProgram.append(
                StatementPtr(new RepeatN(
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                    StatementPtr(new Increment(
                        ExpressionPtr(new Variable("q"))
                    ))
                ))
            );

            myProgram.append(StatementPtr(new TimerStopper("repeat inc ++q",n/1000)));
        }

//--------------------
        {
            int n = 8000000;
            myProgram.append(StatementPtr(new TimerStarter("repeat m.mult(ident)")));

            ScopePtr myLoopScope = ScopePtr(new Scope);

            // Matrix4f m(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
            asl::Matrix4f myMatrix;
            myMatrix.assign(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, asl::UNKNOWN);
            asl::Matrix4f myIdentMatrix;
            myIdentMatrix.makeIdentity();
            myProgram.append(StatementPtr(new Declaration("mm",ValuePtr(new Value<asl::Matrix4f>(myMatrix)),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myMatrix)))))));
            myLoopScope->append(
                    StatementPtr(new BinaryStatement(
                        &Op::multiply,
                        ExpressionPtr(new Variable("mm")),
                        ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myIdentMatrix))))
                        ))
            );

            myProgram.append(
                    StatementPtr(
                        new RepeatN(
                            ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                            myLoopScope
                        ))
            );
            myProgram.append(StatementPtr(new TimerStopper("repeat m.mult(ident)",n/1000)));
        }
//--------------------
         //--------------------
        cerr << "run..." << endl;
        asl::NanoTime start;
        myProgram.eval(0);
        asl::NanoTime stop;
        double myTime = stop.seconds()-start.seconds();
        cerr << "Time taken = " <<  myTime << endl;

        asl::getDashboard().cycle();
        asl::getDashboard().print(cerr);
        myProgram.dump("myProgram");
//        myLoopScope->dump("myLoopScope");

    };

#if 0
#define FOR_LOOP
#ifdef FOR_LOOP

        ScopePtr myLoopScope = ScopePtr(new Scope);
#define LOOP_SCOPE_PLUS
#ifdef LOOP_SCOPE_PLUS
        myLoopScope->append(
            ExpressionPtr(new Assignment(
                ExpressionPtr(new Variable("x")),
                ExpressionPtr(new BinaryExpression(
                    &Op::plus,
                    ExpressionPtr(new Variable("x")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(2))))
                ))
                ))
            );
#endif
//#define LOOP_SCOPE_CALL
#ifdef LOOP_SCOPE_CALL
        ArgumentListPtr myArguments(new ArgumentList);
        myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(2)))));
        myArguments->append(ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(3)))));
        myLoopScope->append(ExpressionPtr(new FunctionCall("add", myArguments)));
#endif
//#define LOOP_SCOPE_MATMULT
#ifdef LOOP_SCOPE_MATMULT
        // Matrix4f m(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16)
        asl::Matrix4f myMatrix;
        myMatrix.assign(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, asl::Matrix4f::UNKNOWN);
        asl::Matrix4f myIdentMatrix;
        myIdentMatrix.makeIdentity();
        //myProgram.append(ExpressionPtr(new Declaration("m",
        //    ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myMatrix)))))));
        myLoopScope->append(
                ExpressionPtr(new BinaryExpression(
                    &Op::multiply,
                    ExpressionPtr(new Variable("m")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<asl::Matrix4f>(myIdentMatrix))))
                    ))
           );
#endif


        myProgram.append(ExpressionPtr(new ForLoop(
            ExpressionPtr(new Declaration("j",
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))),
            ExpressionPtr(new BinaryExpression(
                &Op::less,
                ExpressionPtr(new Variable("j")),
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))))),
#ifdef PLUS_INCR
            ExpressionPtr(new Assignment(
                ExpressionPtr(new Variable("j")),
                ExpressionPtr(new BinaryExpression(
                    &Op::plus,
                    ExpressionPtr(new Variable("j")),
                    ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(1))))
                ))
                )),
#else
            ExpressionPtr(new UnaryExpression(
                &Op::increment,
                ExpressionPtr(new Variable("j"))
                )),
#endif
            myLoopScope
        )));
#else
        myProgram.append(ExpressionPtr(new Declaration("j",
            ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(0)))))));
        myProgram.append(
            ExpressionPtr(new RepeatN(
                ExpressionPtr(new ConstValue(ValuePtr(new Value<int>(n)))),
                ExpressionPtr(new UnaryExpression(
                    &Op::increment,
                    ExpressionPtr(new Variable("j"))
                ))
            ))
        );
#endif

--------------------
#endif

#if 0
#if 0
std::map<std::string, int> ourValue;

enum OpType { VOID_F_INT_VAR };

struct Instruction {
    char optype;
    void * opfunction;
    int operand1;
    int operand2;
};

typedef void (*Void_IntVar)(int,int *);



function assign(int theValue, int *theVariable) {
    *theVariable = theValue;
}

int program[] = { { ASSIGN, 5000000, "n"}, {IF

    void testStateMachPerf() {
        int pc = 0;
        Instruction * myProgram;
        do {
            Instruction & myInstruction = myProgram[pc];
            switch (myInstruction.optype) {
                case VOID_F_INT_VAR:
                    {
                    typedef void (*Void_IntVar)(int,const char *);
                    MyFunctionPtr myFunction = reinterpret_cast<Void_IntVar>(myInstruction.opfunction);
                    (*myFunction)(myInstruction.operand1,myInstruction.operand2);
                    }
            }
    }
#endif

    enum Operator {
        opPlus, opMinus, opMult, opXor, opAnd, opOr, opReset,
        op0, op1, op2, op3, op4, op5, op6, op7,
        op8, op9, op10, op11, op12, op13, op14, op15,
        op16, op17, op18, op19, op20, op21, op22, op23,
        op24, op25, op26, op27, op28, op29, op30, op31
    };

    int arithSpeed(int n, int myA, Operator myOp, const char * theOpName) {
        asl::NanoTime start;
        int a = myA;
        for (int i = 0; i < n; ++i) {
            switch (myOp) {
                case opAnd: a = a & i;
                break;
                case op0: a = a & i;
                break;
                case op1: a = a & i;
                break;
                case op2: a = a & i;
                break;
                case op3: a = a & i;
                break;
                case op4: a = a & i;
                break;
                case op5: a = a & i;
                break;
                case op6: a = a & i;
                break;
                case op7: a = a & i;
                break;
                case op8: a = a & i;
                break;
                case op9: a = a & i;
                break;
                case op10: a = a & i;
                break;
                case op11: a = a & i;
                break;
                case op12: a = a & i;
                break;
                case op13: a = a & i;
                break;
                case op14: a = a & i;
                break;
                case op15: a = a & i;
                break;
                case op16: a = a & i;
                break;
                case op17: a = a & i;
                break;
                case op18: a = a & i;
                break;
                case op19: a = a & i;
                break;
                case op20: a = a & i;
                break;
                case op21: a = a & i;
                break;
                case op22: a = a & i;
                break;
                case op23: a = a & i;
                break;
                case op24: a = a & i;
                break;
                case op25: a = a & i;
                break;
                case op26: a = a & i;
                break;
                case op27: a = a & i;
                break;
                case op28: a = a & i;
                break;
                case op29: a = a & i;
                break;
                case op30: a = a & i;
                break;
                case op31: a = a & i;
                break;
                case opPlus: a = a + i;
                break;
                case opMinus: a = a - i;
                break;
                case opMult: a = a * i;
                break;
                case opXor: a = a ^ i;
                break;
                case opOr: a = a | i;
                break;
            }
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "arithSpeed "<<theOpName <<" = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
        return a;
    };

    Operator * makeOps(Operator theOp, int theSize) {
        Operator  * myOps = new Operator[theSize];
        for (int i = 0; i < theSize; ++i) {
            myOps[i] = theOp;
        }
        myOps[theSize - 1] = opReset;
        return myOps;
    }

    struct Operation {
        Operator myOperator;
        int * myDestination;
        int * mySrc;
    };
    Operation * makeOps(Operator theOp, int * theDest, int * theSrc, int theSize) {
        Operation  * myOps = new Operation[theSize];
        for (int i = 0; i < theSize; ++i) {
            myOps[i].myOperator = theOp;
            myOps[i].myDestination = theDest;
            myOps[i].mySrc = theSrc;
        }
        myOps[theSize - 1].myOperator = opReset;
        return myOps;
    }

   int aluSpeed(int n, int myA, Operator myOp, const char * theOpName) {
        Operator * myOps = makeOps(myOp, 256);
        asl::NanoTime start;
        int a = myA;
        int pc = 0;
        for (int i = 0; i < n; ++i) {
            switch (myOps[pc]) {
                case opAnd: a = a & i;
                break;
               case op0: a = a + i;
                break;
                case op1: a = a - i;
                break;
                case op2: a = a * i;
                break;
                case op3: a = a | i;
                break;
                case op4: a = a > i;
                break;
                case op5: a = a < i;
                break;
                case op6: a = a && i;
                break;
                case op7: a = a || i;
                break;
                case op8: a = a % i;
                break;
                case op9: a = a / i;
                break;
                case op10: a = a - i;
                break;
                case op11: a = a + i;
                break;
                case op12: a = a * i;
                break;
                case op13: a = a | i;
                break;
                case op14: a = a >= i;
                break;
                case op15: a = a == i;
                break;
                case op16: a = a <= i;
                break;
                case op17: a = a > i;
                break;
                case op18: a = a < i;
                break;
                case op19: a = a * i;
                break;
                case op20: a = a - i;
                break;
                case op21: a = a + i;
                break;
                case op22: a = a % i;
                break;
                case op23: a = a & i;
                break;
                case op24: a = a | i;
                break;
                case op25: a = a + i;
                break;
                case op26: a = a - i;
                break;
                case op27: a = a * i;
                break;
                case op28: a = a / i;
                break;
                case op29: a = a & i;
                break;
                case op30: a = a | i;
                break;
                case op31: a = a && i;
                break;
                case opPlus: a = a + i;
                break;
                case opMinus: a = a - i;
                break;
                case opMult: a = a * i;
                break;
                case opXor: a = a ^ i;
                break;
                case opOr: a = a | i;
                break;
                case opReset: pc = 0;
                break;
            }
            ++pc;
        }
        asl::NanoTime stop;
        delete [] myOps;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "aluSpeed "<<theOpName <<" = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
        return a;
    };

    int procSpeed(int n, int myA, Operator myOp, const char * theOpName) {
        int pc = 0;
        int i = 0;
        Operation * myOps = makeOps(myOp, &myA, &i, 256);
        asl::NanoTime start;
        for (; i < n; ++i) {
            Operation & myOp = myOps[pc];
            int & a = *myOp.myDestination;
            int & b = *myOp.mySrc;

            switch (myOp.myOperator) {
                case opAnd: a = a & b;
                break;
                case op0: a = a + i;
                break;
                case op1: a = a - i;
                break;
                case op2: a = a * i;
                break;
                case op3: a = a | i;
                break;
                case op4: a = a > i;
                break;
                case op5: a = a < i;
                break;
                case op6: a = a && i;
                break;
                case op7: a = a || i;
                break;
                case op8: a = a % i;
                break;
                case op9: a = a / i;
                break;
                case op10: a = a - i;
                break;
                case op11: a = a + i;
                break;
                case op12: a = a * i;
                break;
                case op13: a = a | i;
                break;
                case op14: a = a >= i;
                break;
                case op15: a = a == i;
                break;
                case op16: a = a <= i;
                break;
                case op17: a = a > i;
                break;
                case op18: a = a < i;
                break;
                case op19: a = a * i;
                break;
                case op20: a = a - i;
                break;
                case op21: a = a + i;
                break;
                case op22: a = a % i;
                break;
                case op23: a = a & i;
                break;
                case op24: a = a | i;
                break;
                case op25: a = a + i;
                break;
                case op26: a = a - i;
                break;
                case op27: a = a * i;
                break;
                case op28: a = a / i;
                break;
                case op29: a = a & i;
                break;
                case op30: a = a | i;
                break;
                case op31: a = a && i;
                break;
                case opPlus: a = a + b;
                break;
                case opMinus: a = a - b;
                break;
                case opMult: a = a * b;
                break;
                case opXor: a = a ^ b;
                break;
                case opOr: a = a | b;
                break;
                case opReset: pc = 0;
                break;
            }
            ++pc;
        }
        asl::NanoTime stop;
        delete [] myOps;
        cerr << "a = " << myA << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "procSpeed "<<theOpName <<" = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
        return myA;
    };
#endif
    typedef asl::Ptr<int,asl::SingleThreaded> IntPtr;

    class Base {
    public:
        virtual void incPtr(IntPtr theVal) = 0;
        virtual void inc(int * theVal) = 0;
    };

    class Derived : public Base {
    public:
        virtual void inc(int * theVal) {
            ++(*theVal);
        }
        virtual void incPtr(IntPtr theVal) {
            ++(*theVal);
        }
        static
        void incIndirect(Base * theBase, int * theVal) {
            Base * myBase = theBase;
            myBase->inc(theVal);
        }
        static
        void incIndirectDyn(Base * theBase, int * theVal) {
            Derived * myDerived = dynamic_cast<Derived*>(theBase);
            myDerived->inc(theVal);
        }
    };

    void loopSpeedAsm(int theCount) {

        int a;
        asl::NanoTime start;
#if 0
#if 1 // GCC
        //fastest:
        __asm__
         (
            "push        %esi\n\t"
            "mov         %esi, %0 \n\t"
            "push        %edi  \n\t"
            "mov         %edi, $0FFFFFFFF\n\t"

            "test        %esi, %esi\n\t"
            "je          _ready\n\t"
"_loop:            \n\t"
            "xor         %edi,%esi\n\t"
            "dec         %esi\n\t"
            "je          _ready\n\t"

            "xor         %edi,%esi\n\t"
            "dec         %esi\n\t"
            "je          _ready\n\t"

            "xor         %edi,%esi\n\t"
            "dec         %esi\n\t"
            "jne          _loop \n\t"

"_ready:     \n\t"
            "pop edi\n\t"
            "pop esi\n\t"
            :
            "=&a"(theCount):
            "a"(theCount):
            "cc"
        );
#else
        //fastest:
        __asm {
            push        esi
            mov         esi, theCount
            push        edi
            mov         edi, 0FFFFFFFFh

            test        esi, esi
            je          _ready
_loop:
            xor         edi,esi
            dec         esi
            je          _ready

            xor         edi,esi
            dec         esi
            je          _ready

            xor         edi,esi
            dec         esi
            jne          _loop

_ready:     mov         a, edi
            pop edi
            pop esi
        }
#endif
#endif
#if 0
        __asm {
            push        esi
            mov         esi, theCount
            push        edi
            mov         edi, 0FFFFFFFFh
_loop:
            test        esi, esi
            je          _ready
            jne         _again
_loop2:
            sub         esi, 1
            xor         edi,esi
            test        esi,esi
            jne         _loop
            jmp         _ready
_loop1:
            sub         esi, 1
            xor         edi,esi
            test        esi,esi
            jne         _loop2
            jmp         _ready
_again:
            sub         esi, 1
            xor         edi,esi
            test        esi,esi
            jne         _loop1

_ready:      mov         a, edi
            pop edi
            pop esi
        }
#endif
#if 0
        __asm {
            push        esi
            mov         esi, theCount
            push        edi
            mov         edi, 0FFFFFFFFh

            test        esi, esi
            je          _ready
_loop:
            dec         esi
            xor         edi,esi
            test        esi,esi
            jne         _loop

_ready:      mov         a, edi
            pop edi
            pop esi
        }
#endif
#if 0
        __asm {
            push        ecx
            mov         ecx, theCount
            push        edi
            mov         edi, 0FFFFFFFFh

            test        ecx, ecx
            je          _ready
_loop:
            xor         edi,ecx
            loopne         _loop

_ready:      mov         a, edi
            pop edi
            pop ecx
        }
#endif
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeedAsm Baseline = " <<  myTime << ", mio. per sec.= "<< theCount/myTime/1000000<< endl;
   };
   void loopSpeedAsmMax(int theCount) {

        int a;
        asl::NanoTime start;
#if 0
#if 1 // gcc
        __asm__ (
            "push        %esi  \n\t"
            "mov         %esi, %0 \n\t"
            "push        %edi  \n\t"
            "mov         %edi, $0FFFFFFFF\n\t"

            "test        %esi, %esi\n\t"
            "je          _ready\n\t"
"_loop:\n\t"
            "sub         %esi,1\n\t"
            "je          _ready\n\t"

            "sub         %esi,1\n\t"
            "jne          _loop \n\t"

"_ready:     mov         %0, %edi\n\t"
            "pop %edi\n\t"
            "pop %esi\n\t" :
            "=&a"( theCount ):
            "a"( theCount ):
            "cc"
        );
#else
         __asm {
            push        esi
            mov         esi, theCount
            push        edi
            mov         edi, 0FFFFFFFFh

            test        esi, esi
            je          _ready
_loop:
            sub         esi,1
            je          _ready

            sub         esi,1
            jne          _loop

_ready:     mov         a, edi
            pop edi
            pop esi
        }
#endif
#endif
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeedAsmMax Baseline = " <<  myTime << ", mio. per sec.= "<< theCount/myTime/1000000<< endl;
   };
   void loopSpeedReg(int theCount) {
        register int n = theCount;//1000 * 1000000;
        register int a = 0xffffffff;
        asl::NanoTime start;
        for (register int i = 0; i < n; ++i) {
            a = a ^ i;
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeedReg Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };
    void loopSpeedWhile(int theCount) {
        register int n = theCount;//1000 * 1000000;
        register int a = 0xffffffff;
        asl::NanoTime start;
        while(n--) {
            a = a ^ n;
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeedWhile Baseline = " <<  myTime << ", mio. per sec.= "<< theCount/myTime/1000000<< endl;
   };
    void loopSpeedReg2(int theCount) {
        register int n = theCount;//1000 * 1000000;
        register int a = 0xffffffff;
        asl::NanoTime start;
        for (register int i = 0; i < n; ++i) {
            a ^= i;
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeedReg2 Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };
    void loopSpeed(int n) {
        int a = 0xffffffff;
        asl::NanoTime start;
        for (int i = 0; i < n; ++i) {
            a = a ^ i;
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopSpeed Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };
   void myFunc(int & a, int i) {
        a = a ^ i;
   };
   int myFunc2(int a, int i) {
        return a ^ i;
   };
   void loopFuncPtrSpeed(int n, void (*theFunc)(int&,int)) {
        int a = 0xffffffff;
        asl::NanoTime start;
        for (int i = 0; i < n; ++i) {
            theFunc(a,i);
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopFuncPtrSpeed Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };
   void loopFuncSpeed(int n) {
        int a = 0xffffffff;
        asl::NanoTime start;
        for (int i = 0; i < n; ++i) {
            myFunc(a,i);
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopFuncSpeed Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };
   void loopFunc2Speed(int n) {
        int a = 0xffffffff;
        asl::NanoTime start;
        for (int i = 0; i < n; ++i) {
            a = myFunc2(a,i);
            //cerr << a << ",";
        }
        asl::NanoTime stop;
        cerr << "a = " << a << endl;
        double myTime = stop.seconds()-start.seconds();
        cerr << "loopFunc2Speed Baseline = " <<  myTime << ", mio. per sec.= "<< n/myTime/1000000<< endl;
   };

   void testCPPSpeed() {
        loopSpeedAsmMax(1000 * 1000000);
        loopSpeedAsm(1000 * 1000000);
        loopSpeedWhile(1000 * 1000000);
#if 0
        arithSpeed(10 * 1000000, 0, opPlus, "opPlus");
        arithSpeed(10 * 1000000, 0, opPlus, "opPlus");
        aluSpeed(10 * 1000000, 0, opPlus, "opPlus");
        aluSpeed(10 * 1000000, 0, opOr, "opOr");
        aluSpeed(10 * 1000000, 0, opAnd, "opAnd");

        procSpeed(10 * 1000000, 0, opPlus, "opPlus");
        procSpeed(10 * 1000000, 0, opOr, "opOr");
        procSpeed(10 * 1000000, 0, opAnd, "opAnd");
#endif
        loopSpeedReg2(1000 * 1000000);
        loopSpeedReg(1000 * 1000000);
        loopSpeed(1000 * 1000000);
        loopFuncSpeed(100 * 1000000);
        loopFunc2Speed(100 * 1000000);
        loopFuncPtrSpeed(10 * 1000000, &myFunc);
        {
            asl::NanoTime start;
            for (volatile int i = 0; i < 100 * 1000000; ++i);
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            void (*myFunction)(int&) = &increment;
            asl::NanoTime start;
            for (int i = 0; i < 100 * 1000000; (*myFunction)(i));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop call Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            Base * myBase = new Derived;
            asl::NanoTime start;
            for (int i = 0; i < 100 * 1000000; myBase->inc(&i));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop vfunc call Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            int * ip = new int;
            Base * myBase = new Derived;
            asl::NanoTime start;
            for (*ip = 0; *ip < 100 * 1000000; myBase->inc(ip));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop vfunc cptr call Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            IntPtr IPtr = IntPtr(new int);
            int * ip = IPtr.get();
            Base * myBase = new Derived;
            asl::NanoTime start;
            for (*ip = 0; *ip < 100 * 1000000; myBase->incPtr(IPtr));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop vfunc Ptr call Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            int * ip = new int;
            Base * myBase = new Derived;
            asl::NanoTime start;
            for (*ip = 0; *ip < 100 * 1000000; Derived::incIndirect(myBase,ip));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop vfunc cptr incIndirect Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            int * ip = new int;
            Base * myBase = new Derived;
            asl::NanoTime start;
            for (*ip = 0; *ip < 10 * 1000000; Derived::incIndirectDyn(myBase,ip));
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop vfunc cptr incIndirectDyn Baseline taken = " <<  myTime << ", mio. per sec.= "<< 10/myTime<< endl;
        }
        {
            asl::Matrix4f myMatrix;
            myMatrix.assign(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, asl::UNKNOWN);
            asl::Matrix4f myIdentMatrix;
            myIdentMatrix.makeIdentity();
            asl::NanoTime start;
            for (int i = 0; i < 100 * 1000000; ++i) {
                myMatrix.postMultiply(myIdentMatrix);
            }
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop ident matrix Baseline taken = " <<  myTime << ", mio. per sec.= "<< 100/myTime<< endl;
        }
        {
            asl::Matrix4f myMatrix;
            myMatrix.assign(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16, asl::UNKNOWN);
            asl::Matrix4f myIdentMatrix = myMatrix;
            //myIdentMatrix.makeIdentity();
            asl::NanoTime start;
            for (int i = 0; i < 1 * 1000000; ++i) {
                myMatrix.postMultiply(myIdentMatrix);
                myMatrix = myIdentMatrix;
            }
            asl::NanoTime stop;
            double myTime = stop.seconds()-start.seconds();
            cerr << "Loop full matrix Baseline taken = " <<  myTime << ", mio. per sec.= "<< 1/myTime<< endl;
        }

    }
//////////////////////////////////////////////////////////

    inline int my_add(int a, int b) {
        return a + b;
    };

    void fullMatMult(const float a[4][4], const float b[4][4], float c[4][4]) {
        c[0][0] = a[0][0] * b[0][0] + a[0][1] * b[1][0] + a[0][2] * b[2][0] + a[0][3] * b[3][0];
        c[0][1] = a[0][0] * b[0][1] + a[0][1] * b[1][1] + a[0][2] * b[2][1] + a[0][3] * b[3][1];
        c[0][2] = a[0][0] * b[0][2] + a[0][1] * b[1][2] + a[0][2] * b[2][2] + a[0][3] * b[3][2];
        c[0][3] = a[0][0] * b[0][3] + a[0][1] * b[1][3] + a[0][2] * b[2][3] + a[0][3] * b[3][3];

        c[1][0] = a[1][0] * b[0][0] + a[1][1] * b[1][0] + a[1][2] * b[2][0] + a[1][3] * b[3][0];
        c[1][1] = a[1][0] * b[0][1] + a[1][1] * b[1][1] + a[1][2] * b[2][1] + a[1][3] * b[3][1];
        c[1][2] = a[1][0] * b[0][2] + a[1][1] * b[1][2] + a[1][2] * b[2][2] + a[1][3] * b[3][2];
        c[1][3] = a[1][0] * b[0][3] + a[1][1] * b[1][3] + a[1][2] * b[2][3] + a[1][3] * b[3][3];

        c[2][0] = a[2][0] * b[0][0] + a[2][1] * b[1][0] + a[2][2] * b[2][0] + a[2][3] * b[3][0];
        c[2][1] = a[2][0] * b[0][1] + a[2][1] * b[1][1] + a[2][2] * b[2][1] + a[2][3] * b[3][1];
        c[2][2] = a[2][0] * b[0][2] + a[2][1] * b[1][2] + a[2][2] * b[2][2] + a[2][3] * b[3][2];
        c[2][3] = a[2][0] * b[0][3] + a[2][1] * b[1][3] + a[2][2] * b[2][3] + a[2][3] * b[3][3];

        c[3][0] = a[3][0] * b[0][0] + a[3][1] * b[1][0] + a[3][2] * b[2][0] + a[3][3] * b[3][0];
        c[3][1] = a[3][0] * b[0][1] + a[3][1] * b[1][1] + a[3][2] * b[2][1] + a[3][3] * b[3][1];
        c[3][2] = a[3][0] * b[0][2] + a[3][1] * b[1][2] + a[3][2] * b[2][2] + a[3][3] * b[3][2];
        c[3][3] = a[3][0] * b[0][3] + a[3][1] * b[1][3] + a[3][2] * b[2][3] + a[3][3] * b[3][3];
    }

    void trivialCppBenchmark() {
        cerr << "Pavel's trivial C++ benchmark V.0.1" << endl;

        //###################################
        //# i = i + 1

        int i = 0;
        int n = 2000000000;
        int j = 1;

        asl::NanoTime start;

        for (i = 0; i < n; i+=1) { j^=i;}
        //for (i = 0; i < n; i+=1) {}

        asl::NanoTime stop;
        asl::NanoTime duration = stop - start;

        cerr << j;
        cerr << n << " x {i+=1} took "<<duration.millis() <<"msec. ("<<n/duration.millis()/1e3<<" mio. per sec"<<endl;

        //###################################
        //# my_add(i,1)
        i = 0;
        n = 500000000;

        start.setNow();

        while (i < n) {
            i=my_add(i, 1);
            j^=i;
        }

        stop.setNow();
        duration = stop - start;

        //print(n.toString() + " i = my_add(i,1) took "+ duration +"msec. ("+ n/duration/1e3 +" mio. per sec)");
        cerr << j <<endl;
        cerr << n << " x {i = my_add(i,1)} took "<<duration.millis() <<"msec. ("<<n/duration.millis()/1e3<<" mio. per sec"<<endl;

        //###################################
        //# a = fullMatMult(b, c)
        i = 0;
        n = 50000000;

        float a[4][4] = {{1,0,0,0},{0,1,0,0},{0,0,1,0},{0,0,0,1}};
        float b[4][4] = {{2,0,0,0},{0,2,0,0},{0,0,2,0},{0,0,0,2}};

        start.setNow();

        float c[4][4];
        for (i = 0; i < n; i+=1) {
            fullMatMult(a, b, c);
        }
        stop.setNow();
        duration = stop - start;
        cerr << c[0][0] << endl;
         cerr << n << " x fullMatMult took "<<duration.millis() <<"msec. ("<<n/duration.millis()/1e3<<" mio. per sec"<<endl;
       //print(n.toString() + " j = fullMatMult took "+ duration +"msec. ("+ n/duration * 1000+" per sec)");
    }
///////////////////////////////////////////////////
    bool analyze(const std::string & theText) {
        DyscriptParser myLexer;
        TokenList myTokenList;
        if (myLexer.tokenize(theText,myTokenList)) {
            std::map<TokenId,unsigned int> myStatistic;
            for (int i = 0; i < myTokenList.size();++i) {
                //cerr << myLexer.getTokenName(myTokenList[i]->getId()) << ":" << myTokenList[i]->getSection(theText) << endl;
                myStatistic[myTokenList[i]->getId()]++;
            }
            for (std::map<TokenId,unsigned int>::iterator it = myStatistic.begin();
                it != myStatistic.end(); ++it)
            {
                cerr << myLexer.getTokenName(it->first) << ":" << it->second << endl;
            }
            cerr << "Total file size " <<theText.size()<<", contains "<<myTokenList.size()<<" token"<<endl;
            unsigned int myParsePos = myLexer.parse(theText,myTokenList);
            return true;
        }
        return false;
    }

    bool analyze(const std::vector<std::string> & theFiles) {
        for (int i = 0; i < theFiles.size(); ++i) {
            std::cerr << "analyze: reading file '"<<theFiles[i]<<std::endl;
            std::string myFileText;
            if (asl::readFile(theFiles[i], myFileText)) {
                std::cerr << "analyze: analyzing file '"<<theFiles[i]<<"'"<<std::endl;
                if (!analyze(myFileText)) {
                    return false;
                }
            } else {
                std::cerr<<"analyze: could not read '"<<theFiles[i]<<"'"<<std::endl;
                return false;
            }
        }
        return true;
    }
    void testCode() {
        std::string myProgramText =
            "{"
            "   string myString('hello');"
            "   print(myString)"
            "}";
        analyze(myProgramText);
    }
} //namespace

void testDyScript() {
    dys::testCPPSpeed();
    //dys::testCode();
    dys::testLoop();
}
//===============================================================

// SYMBOL
namespace sym {


class Singleton {
};

class Object {
    virtual Object * clone() = 0;
    virtual Object * get() = 0;
    virtual Object * put() = 0;
};

class Function : Object {
    virtual Object * apply(Object *) = 0;
};

class Symbol : Singleton {
    virtual Symbol * apply(Object *) = 0;
};

class Type : Singleton {
};

class Map : Object {
    virtual Object * add(Object *) = 0;
};

}




//===============================================================





#include <asl/base/Arguments.h>

asl::Arguments ourArguments;

const asl::Arguments::AllowedOption ourAllowedOptions[] = {
    //{"--jsversion", "VERSION"},
    //{"-I", "include path (semicolon-separated)"},
    //{"--no-jswarnings", ""},
    {"--version", ""},
    {"--analyze", ""},
    {"--dyscript", ""},
    {"--cpp", ""},
    {"--cpp-trivial", ""},
    {"", ""}
};

void
printUsage() {
    ourArguments.printUsage();
}

int
main(int argc, char **argv)
{
    ourArguments.addAllowedOptions(ourAllowedOptions);
    ourArguments.parse(argc, argv);

    cerr << ourArguments.getProgramName() << " Copyright (C) 2003-2004 ART+COM" << endl << endl;

    //if (ourArguments.haveOption("-I")) {
        //y60::setIncludePath(ourArguments.getOptionArgument("-I"));
    //}

    //dys::testCode();

    if (ourArguments.haveOption("--cpp")) {
        dys::testCPPSpeed();
        exit(0);
    }
    if (ourArguments.haveOption("--dyscript")) {
        dys::testLoop();
        exit(0);
    }
    if (ourArguments.haveOption("--cpp-trivial")) {
        dys::trivialCppBenchmark();
        exit(0);
    }
/*
    if (ourArguments.haveOption("--no-jswarnings")) {
        //y60::setReportWarnings(false);
    }
*/
    if (ourArguments.haveOption("--version")) {
        cerr << "build on " << __DATE__ << " at " << __TIME__ << endl;
        exit(0);

    }
/*
    if (ourArguments.haveOption("--jsversion")) {
        //y60::setJSVersion(asl::as<int>(ourArguments.getOptionArgument("--jsversion")));
    }
*/
    if (ourArguments.getCount() < 1) {
        printUsage();
        return -1;
    }

    std::vector<std::string> myFilenames;

    unsigned myArgCount = ourArguments.getCount();
    for (unsigned i=0; i<myArgCount; ++i) {
        myFilenames.push_back(ourArguments.getArgument(i));
    }

    if (ourArguments.haveOption("--analyze")) {
        dys::analyze(myFilenames);
    }

    /*
    if (-1 == SDL_Init(SDL_INIT_NOPARACHUTE)) { // prevents SDL from catching fatal signals
        cerr << SDL_GetError() << endl;
        return -1;
    }

    int rv = y60::run(myFilename, myScriptArgs);

    SDL_Quit();
*/
    return 0;
}

