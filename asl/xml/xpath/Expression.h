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
** \file xpath/Expression.h
**
** \brief XPath syntax tree expressions
**
** \author Hendrik Schober
**
** \date 2008-07-14
**
*/

/**********************************************************************************************/

#if !defined(XPATH_PATHEXPRESSION_H)
#define XPATH_PATHEXPRESSION_H

/**********************************************************************************************/
#include <string>

/**********************************************************************************************/
#include "ParseInput.h"
#include "PathParser.h"
#include "Value.h"
#include "Path.h"

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        inline bool isEOF(const ParseInput& pi)
        {
            return pi.eof();
        }

        inline bool isEndOfExpr(ParseInput& pi)
        {
            return pi.eof(true) || pi.peekNextChar()==']';
        }

        inline isQuote(char ch)
        {
            return ch == '\'' || ch == '\"';
        }

        class BinaryExpr : public Predicate::Expression {
        public:
            BinaryExpr()                            : lhs_(), rhs_() {}
            BinaryExpr( const ExpressionPtr lhs
                      , const ExpressionPtr rhs )   : lhs_(lhs), rhs_(rhs) {}
        protected:
            const ExpressionPtr   lhs_;
            const ExpressionPtr   rhs_;
        };

        class OrExpr : public BinaryExpr {
        public:
            OrExpr()                                : BinaryExpr() {}
            OrExpr( const ExpressionPtr lhs
                  , const ExpressionPtr rhs )       : BinaryExpr(lhs,rhs) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        };

        class AndExpr : public BinaryExpr {
        public:
            AndExpr()                               : BinaryExpr() {}
            AndExpr( const ExpressionPtr lhs
                   , const ExpressionPtr rhs )      : BinaryExpr(lhs,rhs) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        };

        class EqualityExpr : public BinaryExpr {
        public:
            enum Operator { Operator_Invalid = 0
                          , Operator_Equal
                          , Operator_Unequal
                          };
            EqualityExpr()                          : BinaryExpr() {}
            EqualityExpr( const ExpressionPtr lhs
                          , const ExpressionPtr rhs
                          , Operator oper )         : BinaryExpr(lhs,rhs)
                                                    , oper_(oper) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Operator              oper_;
        };

        class RelationalExpr : public BinaryExpr {
        public:
            enum Operator { Operator_Invalid = 0
                          , Operator_Less
                          , Operator_Greater
                          , Operator_LessEqual
                          , Operator_GreaterEqual
                          };
            RelationalExpr()                        : BinaryExpr() {}
            RelationalExpr( const ExpressionPtr lhs
                          , const ExpressionPtr rhs
                          , Operator oper )         : BinaryExpr(lhs,rhs)
                                                    , oper_(oper) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Operator              oper_;
        };

        class AdditiveExpr : public BinaryExpr {
        public:
            enum Operator { Operator_Invalid = 0
                          , Operator_Plus
                          , Operator_Minus
                          };
            AdditiveExpr()                          : BinaryExpr(), oper_() {}
            AdditiveExpr( const ExpressionPtr lhs
                        , const ExpressionPtr rhs
                        , Operator oper )           : BinaryExpr(lhs,rhs), oper_(oper) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Operator              oper_;
        };

        class MultiplicativeExpr : public BinaryExpr {
        public:
            enum Operator { Operator_Invalid = 0
                          , Operator_Mul
                          , Operator_Div
                          , Operator_Mod
                          };
            MultiplicativeExpr()                    : BinaryExpr(), oper_() {}
            MultiplicativeExpr( const ExpressionPtr lhs
                              , const ExpressionPtr rhs
                              , Operator oper )     : BinaryExpr(lhs,rhs)
                                                    , oper_(oper) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Operator              oper_;
        };

        class UnaryExpr : public Predicate::Expression {
        public:
            UnaryExpr()                            : expr_(), negative_(false) {}
            UnaryExpr( const ExpressionPtr expr
                     , bool negative = false )     : expr_(expr), negative_(negative) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        protected:
            const ExpressionPtr   expr_;
            bool                  negative_;
        };

        class UnionExpr : public BinaryExpr {
        public:
            UnionExpr()                             : BinaryExpr() {}
            UnionExpr( const ExpressionPtr lhs
                     , const ExpressionPtr rhs )    : BinaryExpr(lhs,rhs) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        };

        class PathExpr : public Predicate::Expression {
        public:
            PathExpr(const Path& path)              : Predicate::Expression(), path_(path) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Path                  path_;
        };

        class FilterExpr : public Predicate::Expression {
        public:
            FilterExpr()                            : expr_(), predicate_() {}
            FilterExpr( const ExpressionPtr expr
                      , const ExpressionPtr pred )  : expr_(expr), predicate_(pred) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            const ExpressionPtr   expr_;
            const ExpressionPtr   predicate_;
        };

        class PrimaryExpr : public Predicate::Expression {
        };

        class VariableRefExpr : public PrimaryExpr {
        public:
            VariableRefExpr()                       : variable_() {}
            VariableRefExpr(const std::string& var) : variable_(var) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            std::string           variable_;
        };

        class LiteralExpr : public PrimaryExpr {
        public:
            LiteralExpr()                           : literal_() {}
            LiteralExpr(const std::string& var)     : literal_(var) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            std::string           literal_;
        };

        class NumberExpr : public PrimaryExpr {
        public:
            NumberExpr()                            : number_() {}
            NumberExpr(const Number num)            : number_(num) {}
            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            Number                number_;
        };

        class FunctionCallExpr : public PrimaryExpr {
        public:
            enum FunctionType { Func_Invalid        = 0x0000
                              // returning node-sets
                              , Func_Id             = Value_NodeSet<<2 | 0x001
                              // returning boolean
                              , Func_StartsWith     = Value_Boolean<<2 | 0x001
                              , Func_Contains
                              , Func_Boolean
                              , Func_Not
                              , Func_True
                              , Func_False
                              , Func_Lang
                              // returning numbers
                              , Func_Last           = Value_Number<<2  | 0x001
                              , Func_Position
                              , Func_Count
                              , Func_StringLength
                              , Func_Number
                              , Func_Sum
                              , Func_Floor
                              , Func_Ceiling
                              , Func_Round
                              // returning strings
                              , Func_LocalName      = Value_String<<2  | 0x001
                              , Func_NamespaceURI
                              , Func_Name
                              , Func_String
                              , Func_Concat
                              , Func_SubstringBefore
                              , Func_SubstringAfter
                              , Func_Substring
                              , Func_NormalizeSpace
                              , Func_Translate
                              };

            typedef std::vector<ExpressionPtr>          Arguments;

            typedef std::vector<ValueType>              ParameterTypeList;

            typedef Object (*FuncImpl)(const Arguments&,const EvaluationContext&);

            struct FunctionInfo {
                bool                implemented;
                FunctionType        functionType;
                ParameterTypeList   parameterTypeList;
                FuncImpl            funcImpl;
                FunctionInfo(bool i, FunctionType ft, ParameterTypeList ptl, FuncImpl fi)
                    : implemented(i), functionType(ft), parameterTypeList(ptl), funcImpl(fi)
                {
                }

                bool operator==(const FunctionInfo& rhs) const
                {
                    return  implemented         == rhs.implemented
                         && functionType        == rhs.functionType
                         && parameterTypeList   == rhs.parameterTypeList
                         && funcImpl            == rhs.funcImpl;
                }
            };

            static FunctionInfo readFunctionType(ParseInput& pi);

            FunctionCallExpr( FunctionType type
                            , const Arguments& args);

            FunctionCallExpr(ParseInput& pi, const Arguments& args);

            virtual Object doEvaluate(const EvaluationContext& evContext) const;
            virtual std::ostream& streamTo(std::ostream& os) const;
        private:
            FunctionType          functionType_;
            Arguments             args_;
        };

        Predicate::ExpressionPtr readExpression(ParseInput& pi);

    }

}

/**********************************************************************************************/

#endif //!defined(XPATH_PATHEXPRESSION_H)
