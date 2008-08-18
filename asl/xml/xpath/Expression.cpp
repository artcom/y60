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
** \file xpath/Expression.cpp
**
** \brief XPath syntax tree expressions
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/
#include "Expression.h"

/**********************************************************************************************/
#include <cfloat>
#include <sstream>

/**********************************************************************************************/
#include <asl/string_functions.h>

/**********************************************************************************************/
#include "Path.h"
#include "ParseInput.h"
#include "PathParser.h"
#include "xpath_api_impl.h"

/**********************************************************************************************/

namespace {

    using namespace xpath::detail;

    typedef Predicate::ExpressionPtr                  ExpressionPtr;

    /******************************************************************************************/

    template< typename Func_ >
    inline ExpressionPtr tryRead(ParseInput& pi, Func_ func)
    {
        ParseState ps(pi);
        const ExpressionPtr expr = func(pi);
        if( ps.hasRead() ) {
            ps.commit();
            return expr;
        }
        return ExpressionPtr();
    }

    inline MultiplicativeExpr::Operator readMultOperator(ParseInput& pi)
    {
        if( pi.readIf('*') ) {
            return MultiplicativeExpr::Operator_Mul;
        }
        if( pi.readIf("mod") ) {
            return MultiplicativeExpr::Operator_Mod;
        }
        if( pi.readIf("div") ) {
            return MultiplicativeExpr::Operator_Div;
        }
        pi.setErrorIndex();
        return MultiplicativeExpr::Operator_Invalid;
    }

    inline AdditiveExpr::Operator readAddOperator(ParseInput& pi)
    {
        if( pi.readIf('+') ) {
            return AdditiveExpr::Operator_Plus;
        }
        if( pi.readIf('-') ) {
            return AdditiveExpr::Operator_Minus;
        }
        pi.setErrorIndex();
        return AdditiveExpr::Operator_Invalid;
    }
    
    inline RelationalExpr::Operator readRelOperator(ParseInput& pi)
    {
        if( pi.readIf('<') ) {
            return RelationalExpr::Operator_Less;
        }
        if( pi.readIf('>') ) {
            return RelationalExpr::Operator_Greater;
        }
        if( pi.readIf("<=") ) {
            return RelationalExpr::Operator_LessEqual;
        }
        if( pi.readIf(">=") ) {
            return RelationalExpr::Operator_GreaterEqual;
        }
        pi.setErrorIndex();
        return RelationalExpr::Operator_Invalid;
    }

    inline EqualityExpr::Operator readEquOperator(ParseInput& pi)
    {
        if( pi.readIf('=') ) {
            return EqualityExpr::Operator_Equal;
        }
        if( pi.readIf("!=") ) {
            return EqualityExpr::Operator_Unequal;
        }
        pi.setErrorIndex();
        return EqualityExpr::Operator_Invalid;
    }

    inline bool readUnionOperator(ParseInput& pi)
    {
        return pi.readIf('|');
    }

    /******************************************************************************************/

    ExpressionPtr readNumberExpr(ParseInput& pi)
    {
        std::string numstr;
        while( asl::is_digit(pi.peekNextChar()) ) {
            numstr += pi.getNextChar();
        }
        if( pi.readIf('.') ) {
            numstr += '.';
            while( asl::is_digit(pi.peekNextChar()) ) {
                numstr += pi.getNextChar();
            }
        }
        std::istringstream iss(numstr);
        Number num;
        iss >> num;
        assert( iss && iss.eof() );
        return ExpressionPtr( new NumberExpr(num) );
    }

    ExpressionPtr readLiteralExpr(ParseInput& pi)
    {
        const std::string& lit = pi.readQuotedText(pi.peekNextChar(),pi.peekNextChar());
        return ExpressionPtr( new LiteralExpr(lit) );
    }

    ExpressionPtr readFunctionCallExpr(ParseInput& pi)
    {
        //[16]    FunctionCall                    ::= FunctionName '(' ( Argument ( ',' Argument )* )? ')'
        //[17]    Argument                        ::= Expr
        const FunctionCallExpr::FunctionInfo functionInfo = FunctionCallExpr::readFunctionType(pi);
        if( functionInfo.functionType != FunctionCallExpr::Func_Invalid ) {
            ParseState ps(pi);
            if( pi.readIf('(') ) {
                FunctionCallExpr::Arguments arguments;
                // read arguments
                while( !pi.readIf(')') ) {
                    if( isEndOfExpr(pi) ) {
                        pi.setErrorIndex();
                        throw xpath::SyntaxError( "Function call missing ')'", PLUS_FILE_LINE );
                    }
                    ExpressionPtr expr = readExpression(pi);
                    if( expr ) {
                        arguments.push_back(expr);
                        if( pi.readIf(')') ) {
                            break;
                        }
                        if( !pi.readIf(',') ) {
                            pi.setErrorIndex();
                            return ExpressionPtr();
                        }
                    } else {
                        if( !arguments.empty() ) {
                            pi.setErrorIndex();
                            return ExpressionPtr();
                        }
                    }
                }
                ps.commit();
                return ExpressionPtr( new FunctionCallExpr(functionInfo.functionType,arguments) );
            }
        }
        pi.setErrorIndex();
        return ExpressionPtr();
    }

    ExpressionPtr readVariableReferenceExpr(ParseInput& pi)
    {
        ParseState ps(pi);
        if( !pi.readIf('$') ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }
        const std::string& identifier = pi.readIdentifier();
        if( identifier.empty() ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }
        ps.commit();
        return ExpressionPtr( new VariableRefExpr(identifier) );
    }

    ExpressionPtr readPrimaryExpr(ParseInput& pi)
    {
        //[15]    PrimaryExpr                     ::= '(' Expr ')'
        if( '(' == pi.peekNextChar(true) ) {
            ParseState ps(pi);
            if( const ExpressionPtr expr = tryRead(pi,&readExpression) ) {
                if( isEndOfExpr(pi) || pi.readIf(')') ) {
                    ps.commit();
                    return expr;
                }
            }
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        //                                          | VariableReference
        if( '$' == pi.peekNextChar(true) ) {
            return readVariableReferenceExpr(pi);
        }

        //                                          | Number
        if( asl::is_digit(pi.peekNextChar(true)) ) {
            return readNumberExpr(pi);
        }

        //                                          | Literal
        if( isQuote(pi.peekNextChar(true)) ) {
            return readLiteralExpr(pi);
        }

        //                                          | FunctionCall
        if( const ExpressionPtr expr = tryRead(pi,&readFunctionCallExpr) ) {
            return expr;
        }

        pi.setErrorIndex();
        return ExpressionPtr();
    }

    ExpressionPtr readFilterExpr(ParseInput& pi)
    {
        //[20]    FilterExpr                      ::= PrimaryExpr
        ExpressionPtr expr = readPrimaryExpr(pi);
        if( !expr ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }
        ExpressionPtr predicate;
        //                                          | FilterExpr Predicate
        if( '['==pi.peekNextChar() ) {
            predicate = readExpression(pi);
        }
        return ExpressionPtr( new FilterExpr(expr,predicate) );
    }

    ExpressionPtr readPathExpr(ParseInput& pi)
    {
        //[19]    PathExpr                        ::= LocationPath
        ParseState ps(pi);
        xpath::Path::StepList stepList;
        parsePath(pi,stepList.steps);
        if( ps.hasRead() ) {
            xpath::Path path(stepList);
            ps.commit();
            return ExpressionPtr( new PathExpr(path) );
        }
        //                                          | FilterExpr
        ExpressionPtr expr = readFilterExpr(pi);
        if( !expr ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }
        if( '/' != pi.peekNextChar(true) ) {
            ps.commit();
            return expr;
        }
        //                                          | FilterExpr '/' RelativeLocationPath !!!
        //                                          | FilterExpr '//' RelativeLocationPath !!!
        pi.setErrorIndex();
        throw xpath::NotImplemented( "This is not implemented (see http://www.w3.org/TR/1999/REC-xpath-19991116#NT-PathExpr, production #3 and #4)", PLUS_FILE_LINE );
    }

    ExpressionPtr readUnionExpr(ParseInput& pi)
    {
       //[18]    UnionExpr                       ::= PathExpr
        ExpressionPtr lhs = tryRead(pi,&readPathExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | UnionExpr '|' PathExpr
        while( readUnionOperator(pi) ) {
            if( ExpressionPtr rhs = tryRead(pi,&readPathExpr) ) {
                lhs = ExpressionPtr( new UnionExpr(lhs,rhs) );
            } else {
                return lhs;
            }
        }
        ps.commit();
        return lhs;

    }

    ExpressionPtr readUnaryExpr(ParseInput& pi)
    {
        //[27]    UnaryExpr                       ::= '-' UnaryExpr
        if( pi.readIf('-') ) {
            return ExpressionPtr( new UnaryExpr(readUnaryExpr(pi),true) );
        }
        //                                          | UnionExpr
        return readUnionExpr(pi);
    }

    ExpressionPtr readMultiplicativeExpr(ParseInput& pi)
    {
        //[26]    MultiplicativeExpr              ::= UnaryExpr
        ExpressionPtr lhs = tryRead(pi,&readUnaryExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | MultiplicativeExpr MultiplyOperator UnaryExpr
        //                                          | MultiplicativeExpr 'div' UnaryExpr
        //                                          | MultiplicativeExpr 'mod' UnaryExpr
        while( MultiplicativeExpr::Operator op = readMultOperator(pi) ) {
            if( ExpressionPtr rhs = tryRead(pi,&readUnaryExpr) ) {
                lhs = ExpressionPtr( new MultiplicativeExpr(lhs,rhs,op) );
            } else {
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    ExpressionPtr readAdditiveExpr(ParseInput& pi)
    {
        //[25]    AdditiveExpr                    ::= MultiplicativeExpr
        //                                          | AdditiveExpr '+' MultiplicativeExpr
        //                                          | AdditiveExpr '-' MultiplicativeExpr
        ExpressionPtr lhs = tryRead(pi,&readMultiplicativeExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | AdditiveExpr '+' MultiplicativeExpr
        //                                          | AdditiveExpr '-' MultiplicativeExpr
        while( AdditiveExpr::Operator op = readAddOperator(pi) ) {
            if( ExpressionPtr rhs = tryRead(pi,&readUnaryExpr) ) {
                lhs = ExpressionPtr( new AdditiveExpr(lhs,rhs,op) );
            } else {
                pi.setErrorIndex();
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    ExpressionPtr readRelationalExpr(ParseInput& pi)
    {
        //[24]    RelationalExpr                  ::= AdditiveExpr
        ExpressionPtr lhs = tryRead(pi,&readAdditiveExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | RelationalExpr '<' AdditiveExpr
        //                                          | RelationalExpr '>' AdditiveExpr
        //                                          | RelationalExpr '<=' AdditiveExpr
        //                                          | RelationalExpr '>=' AdditiveExpr
        while( RelationalExpr::Operator op = readRelOperator(pi) ) {
            if( ExpressionPtr rhs = tryRead(pi,&readAdditiveExpr) ) {
                lhs = ExpressionPtr( new RelationalExpr(lhs,rhs,op) );
            } else {
                pi.setErrorIndex();
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    ExpressionPtr readEqualityExpr(ParseInput& pi)
    {
        //[23]    EqualityExpr                    ::= RelationalExpr
        ExpressionPtr lhs = tryRead(pi,&readRelationalExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | EqualityExpr '=' RelationalExpr
        //                                          | EqualityExpr '!=' RelationalExpr
        while( EqualityExpr::Operator op = readEquOperator(pi) ) {
            if( ExpressionPtr rhs = tryRead(pi,&readRelationalExpr) ) {
                lhs = ExpressionPtr( new EqualityExpr(lhs,rhs,op) );
            } else {
                pi.setErrorIndex();
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    ExpressionPtr readAndExpr(ParseInput& pi)
    {
        //[22]    AndExpr                         ::= EqualityExpr
        ExpressionPtr lhs = tryRead(pi,&readEqualityExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | AndExpr 'and' EqualityExpr
        while( pi.readIf("and") ) {
            if( ExpressionPtr rhs = tryRead(pi,&readEqualityExpr) ) {
                lhs = ExpressionPtr( new AndExpr(lhs,rhs) );
            } else {
                pi.setErrorIndex();
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    ExpressionPtr readOrExpr(ParseInput& pi)
    {
        //[21]    OrExpr                          ::= AndExpr
        ExpressionPtr lhs = tryRead(pi,&readAndExpr);
        if( !lhs ) {
            pi.setErrorIndex();
            return ExpressionPtr();
        }

        ParseState ps(pi);
        //                                          | OrExpr 'or' AndExpr
        while( pi.readIf("or") ) {
            if( ExpressionPtr rhs = tryRead(pi,&readAndExpr) ) {
                lhs = ExpressionPtr( new OrExpr(lhs,rhs) );
            } else {
                pi.setErrorIndex();
                return ExpressionPtr();
            }
        }
        ps.commit();
        return lhs;
    }

    /******************************************************************************************/

    inline FunctionCallExpr::ExpressionPtr getArg( const FunctionCallExpr::Arguments& args
                                                 , std::size_t                        idx )
    {
        if( args.size() < idx ) {
            throw xpath::SyntaxError( "Invalid number of function call arguments", PLUS_FILE_LINE );
        }
        return args[idx-1];
    }

    template< typename Result >
    inline Result getArgAs( const FunctionCallExpr::Arguments&  args
                          , std::size_t                         idx
                          , const Predicate::EvaluationContext& evContext )
    {
        FunctionCallExpr::ExpressionPtr expr = getArg(args,idx);
        const Object& obj = expr->doEvaluate(evContext);
        return obj.as<Result>();
    }

    inline String::size_type findSubString(const StringValue& str, const StringValue& substr)
    {
        return str.getValue().find(substr.getValue());
    }

    inline char readNextNormalizedChar(String& result, const String& str, String::size_type& idx)
    {
        assert(idx<str.size());

        if( asl::is_space(str[idx]) ) {
            do {
                if( ++idx == str.size() ) {
                    return '\0';
                }
            } while( asl::is_space(str[idx]) );
            return ' ';
        } else {
            return str[idx++];
        }
    }

    Object executeFunc_Id             (const FunctionCallExpr::Arguments&     , const Predicate::EvaluationContext&          )
    {
        throw xpath::NotImplemented( "Function 'id()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_StartsWith     (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        const String::size_type idx = findSubString( getArgAs<StringValue>(args,1,evContext)
                                                   , getArgAs<StringValue>(args,2,evContext) );
        return Object( 0 == idx );
    }

    Object executeFunc_Contains       (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        const String::size_type idx = findSubString( getArgAs<StringValue>(args,1,evContext)
                                                   , getArgAs<StringValue>(args,2,evContext) );
        return Object( String::npos != idx );
    }

    Object executeFunc_Boolean        (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( getArg(args,1)->doEvaluate(evContext).as<BooleanValue>() );
    }

    Object executeFunc_Not            (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( !( getArg(args,1)->doEvaluate(evContext).as<BooleanValue>().getValue() ) );
    }

    Object executeFunc_True           (const FunctionCallExpr::Arguments&     , const Predicate::EvaluationContext&          )
    {
        return Object( true );
    }

    Object executeFunc_False          (const FunctionCallExpr::Arguments&     , const Predicate::EvaluationContext&          )
    {
        return Object( false );
    }

    Object executeFunc_Lang           (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'lang()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_Last           (const FunctionCallExpr::Arguments&     , const Predicate::EvaluationContext& evContext)
    {
        return Object( static_cast<Number>(evContext.nodeSetSize) );
    }

    Object executeFunc_Position       (const FunctionCallExpr::Arguments&     , const Predicate::EvaluationContext& evContext)
    {
        return Object( static_cast<Number>(evContext.proximityPos) );
    }

    Object executeFunc_Count          (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( static_cast<Number>(getArg(args,1)->doEvaluate(evContext).as<NodeSetValue>().getValue().size()) );
    }

    Object executeFunc_StringLength   (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( static_cast<Number>(getArg(args,1)->doEvaluate(evContext).as<StringValue>().getValue().size()) );
    }

    Object executeFunc_Number         (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( getArg(args,1)->doEvaluate(evContext).as<NumberValue>().getValue() );
    }

    Object executeFunc_Sum            (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'sum()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_Floor          (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( std::floor( getArg(args,1)->doEvaluate(evContext).as<NumberValue>().getValue() ) );
    }

    Object executeFunc_Ceiling        (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( std::ceil ( getArg(args,1)->doEvaluate(evContext).as<NumberValue>().getValue() ) );
    }

    Object executeFunc_Round          (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( std::floor( getArg(args,1)->doEvaluate(evContext).as<NumberValue>().getValue() + .5 ) );
    }

    Object executeFunc_LocalName      (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'local-name()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_NamespaceURI   (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'namespace-uri()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_Name           (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'name()' is not implemented", PLUS_FILE_LINE );
    }

    Object executeFunc_String         (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( getArg(args,1)->doEvaluate(evContext).as<StringValue>() );
    }

    Object executeFunc_Concat         (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        String result;
        result += getArgAs<StringValue>(args,1,evContext).getValue();
        result += getArgAs<StringValue>(args,2,evContext).getValue();
        for( FunctionCallExpr::Arguments::size_type idx=3; idx<=args.size(); ++idx ) {
            result += getArgAs<StringValue>(args,idx,evContext).getValue();
        }
        return Object( result );
    }

    Object executeFunc_SubstringBefore(const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        const StringValue&    str = getArgAs<StringValue>(args,1,evContext);
        const StringValue& substr = getArgAs<StringValue>(args,2,evContext);
        const String::size_type idx = findSubString(str,substr);
        return Object( str.getValue().substr(0,idx) );
    }

    Object executeFunc_SubstringAfter (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        const StringValue&    str = getArgAs<StringValue>(args,1,evContext);
        const StringValue& substr = getArgAs<StringValue>(args,2,evContext);
        const String::size_type idx = findSubString(str,substr);
        return Object( str.getValue().substr(idx+substr.getValue().size()) );
    }

    Object executeFunc_Substring      (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        return Object( getArgAs<StringValue>(args,1,evContext).getValue().substr( static_cast<std::size_t>(getArgAs<NumberValue>(args,2,evContext).getValue())
                                                                                , static_cast<std::size_t>(getArgAs<NumberValue>(args,3,evContext).getValue()) ) );
    }

    Object executeFunc_NormalizeSpace (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        const String str = getArgAs<StringValue>(args,1,evContext).getValue();
        String result;
        if( !str.empty() ) {
            String::size_type idx = 0;
            const char ch = readNextNormalizedChar(result,str,idx);
            if( ch != '\0' && ch != ' ' ) {
                result += ch;
            }
            while( idx < str.size()  ) {
                const char ch = readNextNormalizedChar(result,str,idx);
                if( ch != '\0') {
                    result += ch;
                }
            }
        }
        return Object( result );
    }

    Object executeFunc_Translate      (const FunctionCallExpr::Arguments& args, const Predicate::EvaluationContext& evContext)
    {
        throw xpath::NotImplemented( "Function 'translate()' is not implemented", PLUS_FILE_LINE );
        /*
        The translate function returns the first argument string with occurrences of characters 
        in the second argument string replaced by the character at the corresponding position 
        in the third argument string. For example, translate("bar","abc","ABC") returns the 
        string "BAr". If there is a character in the second argument string with no character 
        at a corresponding position in the third argument string (because the second argument 
        string is longer than the third argument string), then occurrences of that character in 
        the first argument string are removed. For example, translate("--aaa--","abc-","ABC") 
        returns "AAA". If a character occurs more than once in the second argument string, then 
        the first occurrence determines the replacement character. If the third argument string 
        is longer than the second argument string, then excess characters are ignored.
        */
    }

    const ValueType ValueType_Non       = static_cast<ValueType>(0);
    const ValueType ValueType_Any       = static_cast<ValueType>(-1);
    const ValueType ValueType_Ellipsis  = static_cast<ValueType>(-2);

    inline
    FunctionCallExpr::ParameterTypeList
    buildParameterTypeList( ValueType vt1 = ValueType_Non
                          , ValueType vt2 = ValueType_Non
                          , ValueType vt3 = ValueType_Non
                          , ValueType vt4 = ValueType_Non )
    {
        FunctionCallExpr::ParameterTypeList result;
        if( vt1 != ValueType_Non ) result.push_back(vt1); else return result;
        if( vt2 != ValueType_Non ) result.push_back(vt2); else return result;
        if( vt3 != ValueType_Non ) result.push_back(vt3); else return result;
        if( vt4 != ValueType_Non ) result.push_back(vt4); 
        return result;
    }

    typedef asl::static_dict_entry_type<FunctionCallExpr::FunctionInfo>::result_type
                                                  FuncDictEntry;

    const FuncDictEntry funcDictEntries[] = 
        { FuncDictEntry( "id"               , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_Id              , buildParameterTypeList( ValueType_Any                                  ), executeFunc_Id              ) )
        , FuncDictEntry( "starts-with"      , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_StartsWith      , buildParameterTypeList( Value_String, Value_String                     ), executeFunc_StartsWith      ) )
        , FuncDictEntry( "contains"         , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Contains        , buildParameterTypeList( Value_String, Value_String                     ), executeFunc_Contains        ) )
        , FuncDictEntry( "boolean"          , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Boolean         , buildParameterTypeList( ValueType_Any                                  ), executeFunc_Boolean         ) )
        , FuncDictEntry( "not"              , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Not             , buildParameterTypeList( ValueType_Any                                  ), executeFunc_Not             ) )
        , FuncDictEntry( "true"             , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_True            , buildParameterTypeList(                                                ), executeFunc_True            ) )
        , FuncDictEntry( "false"            , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_False           , buildParameterTypeList(                                                ), executeFunc_False           ) )
        , FuncDictEntry( "lang"             , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_Lang            , buildParameterTypeList( Value_String                                   ), executeFunc_Lang            ) )
        , FuncDictEntry( "last"             , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Last            , buildParameterTypeList(                                                ), executeFunc_Last            ) )
        , FuncDictEntry( "position"         , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Position        , buildParameterTypeList(                                                ), executeFunc_Position        ) )
        , FuncDictEntry( "count"            , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Count           , buildParameterTypeList( Value_NodeSet                                  ), executeFunc_Count           ) )
        , FuncDictEntry( "string-length"    , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_StringLength    , buildParameterTypeList( Value_String                                   ), executeFunc_StringLength    ) )
        , FuncDictEntry( "number"           , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Number          , buildParameterTypeList( ValueType_Any                                  ), executeFunc_Number          ) )
        , FuncDictEntry( "sum"              , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_Sum             , buildParameterTypeList( Value_NodeSet                                  ), executeFunc_Sum             ) )
        , FuncDictEntry( "floor"            , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Floor           , buildParameterTypeList( Value_Number                                   ), executeFunc_Floor           ) )
        , FuncDictEntry( "ceiling"          , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Ceiling         , buildParameterTypeList( Value_Number                                   ), executeFunc_Ceiling         ) )
        , FuncDictEntry( "round"            , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Round           , buildParameterTypeList( Value_Number                                   ), executeFunc_Round           ) )
        , FuncDictEntry( "local-name"       , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_LocalName       , buildParameterTypeList( Value_NodeSet                                  ), executeFunc_LocalName       ) )
        , FuncDictEntry( "namespace-uri"    , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_NamespaceURI    , buildParameterTypeList( Value_NodeSet                                  ), executeFunc_NamespaceURI    ) )
        , FuncDictEntry( "name"             , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_Name            , buildParameterTypeList( Value_NodeSet                                  ), executeFunc_Name            ) )
        , FuncDictEntry( "string"           , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_String          , buildParameterTypeList( ValueType_Any                                  ), executeFunc_String          ) )
        , FuncDictEntry( "concat"           , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Concat          , buildParameterTypeList( Value_String, Value_String, ValueType_Ellipsis ), executeFunc_Concat          ) )
        , FuncDictEntry( "substring-before" , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_SubstringBefore , buildParameterTypeList( Value_String, Value_String                     ), executeFunc_SubstringBefore ) )
        , FuncDictEntry( "substring-after"  , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_SubstringAfter  , buildParameterTypeList( Value_String, Value_String                     ), executeFunc_SubstringAfter  ) )
        , FuncDictEntry( "substring"        , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_Substring       , buildParameterTypeList( Value_String, Value_Number, Value_Number       ), executeFunc_Substring       ) )
        , FuncDictEntry( "normalize-space"  , FunctionCallExpr::FunctionInfo( true , FunctionCallExpr::Func_NormalizeSpace  , buildParameterTypeList( Value_String                                   ), executeFunc_NormalizeSpace  ) )
        , FuncDictEntry( "translate"        , FunctionCallExpr::FunctionInfo( false, FunctionCallExpr::Func_Translate       , buildParameterTypeList( Value_String, Value_String, Value_String       ), executeFunc_Translate       ) )
        };

    typedef asl::static_dict<FunctionCallExpr::FunctionInfo,asl_static_array_size(funcDictEntries)>
                                              FuncDict;
    const FuncDict funcDict( asl::begin(funcDictEntries)
                           , asl::end  (funcDictEntries) );

    typedef std::map<FunctionCallExpr::FunctionType,FuncDict::const_iterator>
                                                        Ft2FdiMap;

    inline Ft2FdiMap buildFt2FdiMap()
    {
        Ft2FdiMap result;
        for( FuncDict::const_iterator it=funcDict.begin(); it!=funcDict.end(); ++it ) {
            result.insert( Ft2FdiMap::value_type(it->second.functionType,it) );
        }
        return result;
    }

    inline const Ft2FdiMap& getFt2FdiMap()
    {
        static const Ft2FdiMap& ft2FdiMap = buildFt2FdiMap();
        return ft2FdiMap;
    }

    inline FuncDict::const_iterator getFunctionInfo(FunctionCallExpr::FunctionType type)
    {
        const Ft2FdiMap& ft2FdiMap = getFt2FdiMap(); 
        const Ft2FdiMap::const_iterator itFT = ft2FdiMap .find(type);
        assert(itFT != ft2FdiMap.end());
        const FuncDict::const_iterator itFD = itFT->second;
        const FunctionCallExpr::FunctionInfo& functionInfo = itFD->second;
        assert(type==functionInfo.functionType);
        return itFD;
    }

    inline void checkFuncParameterList( const std::string&                         funcName
                                      , const FunctionCallExpr::Arguments&         args
                                      , const FunctionCallExpr::ParameterTypeList& argTypes )
    {
        if( args.size() != argTypes.size() ) {
            if( !argTypes.empty() ) {
                if( argTypes.back() == ValueType_Ellipsis ) {
                    return;
                }
            }
            throw xpath::SyntaxError( std::string("Invalid number of arguments for function '")
                                    + funcName
                                    + "()'", PLUS_FILE_LINE );
        }
    }

    /******************************************************************************************/

    enum ComparisonOperation { BooleanOperation
                             , EqualityOperation
                             , RelationalOperation };

    template< int Int >
    struct Int2Type {
        enum { result = Int };
    };

    template< int LhsValueType
            , int RhsValueType >
    struct Comparison;

    template< int LhsValueType, int RhsValueType >
    struct Comparison {
        template< template<typename> class Comparator >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<BooleanOperation>)
        {
            return Comparator<xpath::detail::Boolean>()( lhs.as<BooleanValue>().getValue(), rhs.as<BooleanValue>().getValue() );
        }

        template< template<typename> class Comparator >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<EqualityOperation>)
        {
            if( lhs.getType()==Value_Boolean || rhs.getType()==Value_Boolean ) {
                return Comparator<xpath::detail::Boolean>()( lhs.as<BooleanValue>().getValue(), rhs.as<BooleanValue>().getValue() );
            }
            if( lhs.getType()==Value_Number || rhs.getType()==Value_Number ) {
                return Comparator<Number>()( lhs.as<NumberValue>().getValue(), rhs.as<NumberValue>().getValue() );
            }
            if( lhs.getType()==Value_String || rhs.getType()==Value_String ) {
                return Comparator<String>()( lhs.as<StringValue>().getValue(), rhs.as<StringValue>().getValue() );
            }
            assert(false);
            return true;
        }

        template< template<typename> class Comparator >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<RelationalOperation>)
        {
            return Comparator<Number>()( lhs.as<NumberValue>().getValue(), rhs.as<NumberValue>().getValue() );
        }
    };

    template< int RhsValueType >
    struct Comparison< Value_NodeSet, RhsValueType > {
        template< template<typename> class Comparator, int Operation >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<Operation>)
        {
            const NodeSet lhsNS = lhs.as<NodeSetValue>().getValue();
            for( NodeSet::const_iterator itL=lhsNS.begin(); itL!=lhsNS.end(); ++itL ) {
                if( Comparison<Value_String,RhsValueType>::template compare<Comparator>(Object(getStringValue(*itL)),rhs,Int2Type<Operation>()) ) {
                    return true;
                }
            }
            return false;
        }
    };

    template< int LhsValueType >
    struct Comparison< LhsValueType, Value_NodeSet > {
        template< template<typename> class Comparator, int Operation >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<Operation>)
        {
            const NodeSet rhsNS = rhs.as<NodeSetValue>().getValue();
            for( NodeSet::const_iterator itR=rhsNS.begin(); itR!=rhsNS.end(); ++itR ) {
                if( Comparison<LhsValueType,Value_String>::template compare<Comparator>(lhs,Object(getStringValue(*itR)),Int2Type<Operation>()) ) {
                    return true;
                }
            }
            return false;
        }
    };

    template<>
    struct Comparison< Value_NodeSet, Value_NodeSet > {
        template< template<typename> class Comparator, int Operation >
        static bool compare(const Object& lhs, const Object& rhs, Int2Type<Operation>)
        {
            const NodeSet lhsNS = lhs.as<NodeSetValue>().getValue();
            const NodeSet rhsNS = rhs.as<NodeSetValue>().getValue();
            for( NodeSet::const_iterator itL=lhsNS.begin(); itL!=lhsNS.end(); ++itL ) {
                for( NodeSet::const_iterator itR=rhsNS.begin(); itR!=rhsNS.end(); ++itR ) {
                    if( Comparison<Value_String,Value_String>::compare<Comparator>(Object(getStringValue(*itL)),Object(getStringValue(*itR)),Int2Type<Operation>()) ) {
                        return true;
                    }
                }
            }
            return false;
        }
    };

    template< int Operation
            , int LhsValueType
            , int RhsValueType
            , template<typename> class Comparator >
    inline bool doCompareRhs( const Object& lhs, const Object& rhs )
    {
        return Comparison<LhsValueType,RhsValueType>::template compare<Comparator>(lhs,rhs,Int2Type<Operation>());
    }

    template< int Operation
            , int LhsValueType
            , template<typename> class Comparator >
    inline bool doCompareLhs( const Object& lhs, const Object& rhs )
    {
        switch( rhs.getType() ) {
            case Value_NodeSet : return doCompareRhs<Operation,LhsValueType,Value_NodeSet,Comparator>(lhs,rhs);
            case Value_Boolean : return doCompareRhs<Operation,LhsValueType,Value_Boolean,Comparator>(lhs,rhs);
            case Value_Number  : return doCompareRhs<Operation,LhsValueType,Value_Number ,Comparator>(lhs,rhs);
            case Value_String  : return doCompareRhs<Operation,LhsValueType,Value_String ,Comparator>(lhs,rhs);
            default : assert(false);
        }
        return true;
    }

    template< int Operation
            , template<typename> class Comparator >
    inline bool compare( const Object& lhs, const Object& rhs )
    {
        switch( lhs.getType() ) {
            case Value_NodeSet : return doCompareLhs<Operation,Value_NodeSet,Comparator>(lhs,rhs);
            case Value_Boolean : return doCompareLhs<Operation,Value_Boolean,Comparator>(lhs,rhs);
            case Value_Number  : return doCompareLhs<Operation,Value_Number ,Comparator>(lhs,rhs);
            case Value_String  : return doCompareLhs<Operation,Value_String ,Comparator>(lhs,rhs);
            default : assert(false);
        }
        return true;
    }

}

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        //class BinaryExpr : public Predicate::Expression {
        //public:
        //};

        //class OrExpr : public BinaryExpr {
        //public:
            std::ostream& OrExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                os << " or ";
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object OrExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                return Object(compare<BooleanOperation,std::logical_or>(lhsValue,rhsValue));
            }
        //};

        //class AndExpr : public BinaryExpr {
        //public:
            std::ostream& AndExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                os << " and ";
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object AndExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                return Object(compare<BooleanOperation,std::logical_and>(lhsValue,rhsValue));
            }
        //};

        //class EqualityExpr : public BinaryExpr {
        //public:
            std::ostream& EqualityExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                switch( oper_ ) {
                    case Operator_Equal   : os << '='; break;
                    case Operator_Unequal : os << "!="; break;
                    default               : assert(false);
                }
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object EqualityExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                switch( oper_ ) {
                    case Operator_Equal   : return Object(compare<EqualityOperation,std::equal_to    >(lhsValue,rhsValue));
                    case Operator_Unequal : return Object(compare<EqualityOperation,std::not_equal_to>(lhsValue,rhsValue));
                    default               : assert(false);
                }
                return Object(false);
            }
        //};

        //class RelationalExpr : public BinaryExpr {
        //public:
            std::ostream& RelationalExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                switch( oper_ ) {
                    case Operator_Less         : os << '<'; break;
                    case Operator_Greater      : os << '>'; break;
                    case Operator_LessEqual    : os << "<="; break;
                    case Operator_GreaterEqual : os << ">="; break;
                    default                    : assert(false);
                }
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object RelationalExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                switch( oper_ ) {
                    case Operator_Less         : return Object(compare<RelationalOperation,std::less         >(lhsValue,rhsValue));
                    case Operator_Greater      : return Object(compare<RelationalOperation,std::greater      >(lhsValue,rhsValue));
                    case Operator_LessEqual    : return Object(compare<RelationalOperation,std::less_equal   >(lhsValue,rhsValue));
                    case Operator_GreaterEqual : return Object(compare<RelationalOperation,std::greater_equal>(lhsValue,rhsValue));
                    default                    : assert(false);
                }
                return Object(false);
            }
        //};

        //class AdditiveExpr : public BinaryExpr {
        //public:
            std::ostream& AdditiveExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                switch( oper_ ) {
                    case Operator_Plus  : os << '+'; break;
                    case Operator_Minus : os << '-'; break;
                    default             : assert(false);
                }
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object AdditiveExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                if( lhsValue.getType() != Value_Number ) {
                    throw SyntaxError( "Additive expression implies number value", PLUS_FILE_LINE );
                }
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                if( rhsValue.getType() != Value_Number ) {
                    throw SyntaxError( "Additive expression implies number value", PLUS_FILE_LINE );
                }
                switch( oper_ ) {
                    case Operator_Plus  : return Object(lhsValue.as<NumberValue>().getValue() + rhsValue.as<NumberValue>().getValue());
                    case Operator_Minus : return Object(lhsValue.as<NumberValue>().getValue() - rhsValue.as<NumberValue>().getValue());
                    default             : assert(false);
                }
                return Object(false);
            }
        //};

        //class MultiplicativeExpr : public BinaryExpr {
        //public:
            std::ostream& MultiplicativeExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                switch( oper_ ) {
                    case Operator_Mul : os << " * "; break;
                    case Operator_Div : os << " div " ; break;
                    case Operator_Mod : os << " mod " ; break;
                    default           : assert(false);
                }
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object MultiplicativeExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                if( lhsValue.getType() != Value_Number ) {
                    throw SyntaxError( "Multiplicative expression implies number value", PLUS_FILE_LINE );
                }
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                if( rhsValue.getType() != Value_Number ) {
                    throw SyntaxError( "Multiplicative expression implies number value", PLUS_FILE_LINE );
                }
                switch( oper_ ) {
                    case Operator_Mul : return Object(lhsValue.as<NumberValue>().getValue() * rhsValue.as<NumberValue>().getValue());
                    case Operator_Div : return Object(lhsValue.as<NumberValue>().getValue() / rhsValue.as<NumberValue>().getValue());
                    case Operator_Mod : return Object(NumberValue( static_cast<long int>(lhsValue.as<NumberValue>().getValue())
                                                                 % static_cast<long int>(rhsValue.as<NumberValue>().getValue()) ));
                    default           : assert(false);
                }
                return Object(false);
            }
        //};

        //class UnaryExpr : public Predicate::Expression {
        //public:
            std::ostream& UnaryExpr::streamTo(std::ostream& os) const
            {
                if( negative_ ) {
                    os << '-';
                }
                expr_->streamTo(os);
                return os;
            }
        //protected:
            Object UnaryExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& tmp = expr_->doEvaluate(evContext);
                if( !negative_ ) {
                    return tmp;
                }
                if( tmp.getType() != Value_Number ) {
                    throw SyntaxError( "Unary minus implies number value", PLUS_FILE_LINE );
                }
                return Object(-tmp.as<NumberValue>().getValue());
            }
        //};

        //class UnionExpr : public BinaryExpr {
        //public:
            std::ostream& UnionExpr::streamTo(std::ostream& os) const
            {
                lhs_->streamTo(os);
                os << " | ";
                rhs_->streamTo(os);
                return os;
            }
        //protected:
            Object UnionExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object& lhsValue = lhs_->doEvaluate(evContext);
                if( lhsValue.getType() != Value_NodeSet ) {
                    throw SyntaxError( "Union expression implies node set expression", PLUS_FILE_LINE );
                }
                const Object& rhsValue = rhs_->doEvaluate(evContext);
                if( rhsValue.getType() != Value_NodeSet ) {
                    throw SyntaxError( "Union expression implies node set expression", PLUS_FILE_LINE );
                }
                NodeSet result( lhsValue.as<NodeSetValue>().getValue() );
                result.insert( rhsValue.as<NodeSetValue>().getValue().begin()
                             , rhsValue.as<NodeSetValue>().getValue().end  () );
                return Object(result);
            }
        //};

        //class PathExpr : public Predicate::Expression {
        //public:
            std::ostream& PathExpr::streamTo(std::ostream& os) const
            {
                path_.streamTo(os);
                return os;
            }
        //protected:
            Object PathExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                NodePtrVec nodePtrVec;
                Context subcontext(evContext.contextNode);
                findNodes(path_,subcontext,nodePtrVec,false);
                const NodeSet nodeSet( nodePtrVec.begin(), nodePtrVec.end() );
                return Object(nodeSet);
            }
        //};

        //class FilterExpr : public Predicate::Expression {
        //public:
            std::ostream& FilterExpr::streamTo(std::ostream& os) const
            {
                os << *expr_;
                if( predicate_ ) {
                    os << predicate_;
                }
                return os;
            }
        //protected:
            Object FilterExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const Object v = expr_->doEvaluate(evContext);
                if( !predicate_ ) {
                    return v;
                }
                if( !v.getType() == Value_NodeSet ) {
                    throw SyntaxError( "Predicate implies node set expression", PLUS_FILE_LINE );
                }
                const NodeSet nodeSet = v.as<NodeSetValue>().getValue();
                NodeSet result;
                std::size_t idx = 0;
                for( NodeSet::const_iterator it=nodeSet.begin(); it!=nodeSet.end(); ++it ) {
                    const EvaluationContext subEvContext( *it, nodeSet.size(), ++idx );
                    if( predicate_->evaluate(subEvContext) ) {
                        result.insert(*it);
                    }
                }
                return Object(result);
            }
        //};

        //class PrimaryExpr : public Predicate::Expression {
        //};

        //class VariableRefExpr : public PrimaryExpr {
        //public:
            std::ostream& VariableRefExpr::streamTo(std::ostream& os) const
            {
                os << variable_;
                return os;
            }
        //protected:
            Object VariableRefExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                throw NotImplemented( "XPath variable references not implemented", PLUS_FILE_LINE );
            }
        //};

        //class LiteralExpr : public PrimaryExpr {
        //public:
            std::ostream& LiteralExpr::streamTo(std::ostream& os) const
            {
                os << '\"' << literal_ << '\"';
                return os;
            }
        //protected:
            Object LiteralExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                return Object(literal_);
            }
        //};

        //class NumberExpr : public PrimaryExpr {
        //public:
            std::ostream& NumberExpr::streamTo(std::ostream& os) const
            {
                os << number_;
                return os;
            }
        //protected:
            Object NumberExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                return Object(number_);
            }
        //};

        //class FunctionCallExpr : public PrimaryExpr {
        //public:
            FunctionCallExpr::FunctionInfo FunctionCallExpr::readFunctionType(ParseInput& pi)
            {
                typedef std::pair<FuncDict::const_iterator,ParseInput::const_iterator>
                                                          FindResult;
                const FindResult& result = funcDict.lookup(pi.begin(),pi.end());
                if( result.first == funcDict.end() ) {
                    pi.setErrorIndex();
                    return FunctionInfo(false,Func_Invalid,ParameterTypeList(),NULL);
                }
                if( !result.first->second.implemented ) {
                    pi.setErrorIndex();
                    throw NotImplemented( std::string("Function '") 
                                        + result.first->first
                                        + "()' is not implemented", PLUS_FILE_LINE );
                }
                pi.forwardTo(result.second);
                assert(result.first->second.implemented);
                return result.first->second;
            }

            FunctionCallExpr::FunctionCallExpr( FunctionType type
                                              , const Arguments& args)
                : functionType_(type), args_(args)
            {
                const FuncDict::const_iterator itFD = getFunctionInfo(functionType_);
                checkFuncParameterList( itFD->first, args_, itFD->second.parameterTypeList );
            }

            FunctionCallExpr::FunctionCallExpr(ParseInput& pi, const Arguments& args)
                : args_(args)
            {
                const FunctionInfo& functionInfo = readFunctionType(pi);
                const FuncDict::const_iterator itFD = getFunctionInfo(functionType_);
                checkFuncParameterList( itFD->first, args_, functionInfo.parameterTypeList );
                functionType_ = functionInfo.functionType;
            }

            std::ostream& FunctionCallExpr::streamTo(std::ostream& os) const
            {
                const FuncDict::const_iterator itFD = getFunctionInfo(functionType_);
                      Arguments::const_iterator begin = args_.begin();
                const Arguments::const_iterator end   = args_.end  ();
                os << itFD->first << '(';
                if( begin != end ) {
                    for(;;) {
                        os << **begin;
                        ++begin;
                        if( begin == end ) {
                            break;
                        }
                        os << ',';
                    }
                }
                os << ')';
                return os;
            }
        //protected:
            Object FunctionCallExpr::doEvaluate(const EvaluationContext& evContext) const
            {
                const FuncDict::const_iterator itFD = getFunctionInfo(functionType_);
                const FunctionInfo& functionInfo = itFD->second;
                assert(functionInfo.implemented);
                return (functionInfo.funcImpl)(args_,evContext);
            }
        //private:
        //};

        ExpressionPtr readExpression(ParseInput& pi)
        {
            return readOrExpr(pi);
        }

    }

}

/**********************************************************************************************/
