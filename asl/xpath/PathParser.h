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
** \file xpath/PathParser.h
**
** \brief XPath paths parser
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/

#if !defined(XPATH_PATHPARSER_H)
#define XPATH_PATHPARSER_H

/**********************************************************************************************/
#include "Path.h"

/**********************************************************************************************/
#include <string>
#include <vector>
#include <iterator>
#include <ostream>

/**********************************************************************************************/
#include <asl/base/Ptr.h>
#include <asl/base/static_dict.h>
#include <asl/dom/Nodes.h>

/**********************************************************************************************/
#include "Value.h"
#include "ParseInput.h"

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        class Axis {
        public:
            enum Type { Type_Invalid
                      , Type_Ancestor
                      , Type_AncestorOrSelf
                      , Type_Attribute
                      , Type_Child
                      , Type_Descendant
                      , Type_DescendantOrSelf
                      , Type_Following
                      , Type_FollowingSibling
                      , Type_Namespace
                      , Type_Parent
                      , Type_Preceding
                      , Type_PrecedingSibling
                      , Type_Self
                      , Type_Root
                      };
            Type readAxisType(ParseInput& pi);

            Axis()                                  : myAxisType_(Type_Child) {}
            Axis( Type t )                          : myAxisType_(t) {}
            Axis( ParseInput& pi )                  : myAxisType_(readAxisType(pi)) {}

            bool isRoot() const                       {return getType()==Type_Root;}

            bool needsNodeTest() const;

            Type getType() const                      {return myAxisType_;}

            std::ostream& streamTo(std::ostream& os) const;

            std::string toString() const;

            int compare(const Axis& rhs) const;

        private:
            Type                  myAxisType_;
        };

        class NodeTest {
        public:
            enum Type { Type_Invalid
                      , Type_NameTest
                      , Type_CommentTest
                      , Type_TextTest
                      , Type_ProcessingInstructionTest // note: might have a literal
                      , Type_NodeTest
                      };

            static Type readNodeTestType( ParseInput& pi
                                        , std::string& lit);

            NodeTest()                              : myNodeTestType_(Type_NameTest), myValue_() {}
            NodeTest(Type t, const std::string& v)  : myNodeTestType_(t), myValue_(v) {}
            NodeTest( ParseInput& pi );

            Type getType() const                      {return myNodeTestType_;}

            const std::string& getValue() const       {return myValue_;}

            bool empty() const                        {return getType()==Type_Invalid || (getType()==Type_NameTest && getValue().empty());}

            std::ostream& streamTo(std::ostream& os) const;

            std::string toString() const;

            int compare(const NodeTest& rhs) const;

        private:
            Type                  myNodeTestType_;
            std::string           myValue_;
        };

        class Predicate {
        public:
            class Expression {
            public:
                typedef asl::Ptr<Expression>            ExpressionPtr;

                struct EvaluationContext {
                    const dom::Node*contextNode;
                    std::size_t     nodeSetSize;
                    std::size_t     proximityPos;
                    EvaluationContext(const dom::Node* cn, std::size_t nss, std::size_t pp)
                        : contextNode(cn), nodeSetSize(nss), proximityPos(pp) {}
                };

                virtual ~Expression()                   {}

                bool evaluate(const EvaluationContext& evContext) const;

                virtual std::ostream& streamTo(std::ostream& os) const = 0;

                virtual Object doEvaluate(const EvaluationContext& evContext) const = 0;
            };

            typedef Expression::ExpressionPtr           ExpressionPtr;
            typedef Expression::EvaluationContext       EvaluationContext;

            static ExpressionPtr readExpression( ParseInput& pi);

            Predicate()                               : expression_() {}

            Predicate(ParseInput& pi)                 : expression_(readExpression(pi)) {}

            bool empty() const                        {return !expression_;}

            std::ostream& streamTo(std::ostream& os) const;

            std::string toString() const;

            int compare(const Predicate& rhs) const;

            bool evaluate(const EvaluationContext& evContext) const
                                                      {return expression_ ? expression_->evaluate(evContext) : true;}

        private:
            ExpressionPtr         expression_;
        };

        class LocationStep {
        public:
            LocationStep()
                : myAxis_(), myNodeTest_(), myPredicate_() {}

            LocationStep( const Axis       axis
                        , const NodeTest&  nodeTest
                        , const Predicate& predicate = Predicate() )
                : myAxis_(axis), myNodeTest_(nodeTest), myPredicate_(predicate) {}

            bool isRoot() const                       {return myAxis_.isRoot();}

            std::ostream& streamTo(std::ostream& os) const;
            
            std::string toString() const;

            int compare(const LocationStep& rhs) const;

            const Axis     & getAxis     () const               {return myAxis_;}
            const NodeTest & getNodeTest () const               {return myNodeTest_;}
            const Predicate& getPredicate() const               {return myPredicate_;}

        private:
            Axis                  myAxis_;
            NodeTest              myNodeTest_;
            Predicate             myPredicate_;
        };

        inline
        std::ostream& operator<<(std::ostream& os, const Axis& axis)
        {axis.streamTo(os);return os;}

        inline
        std::ostream& operator<<(std::ostream& os, const NodeTest& nodeTest)
        {nodeTest.streamTo(os);return os;}

        inline
        std::ostream& operator<<(std::ostream& os, const Predicate& predicate)
        {predicate.streamTo(os);return os;}

        inline
        std::ostream& operator<<(std::ostream& os, const Predicate::Expression& expr)
        {expr.streamTo(os);return os;}

        inline
        std::ostream& operator<<(std::ostream& os, const LocationStep& step)
        {step.streamTo(os);return os;}

        typedef std::vector<LocationStep>             LocationStepList;
        
        struct StepAppender {
            virtual void operator()(const LocationStep&) const = 0;
        };

        struct ListStepAppender : public StepAppender {
            ListStepAppender(LocationStepList& locationSteps)
                : locationSteps_(locationSteps) {}
            virtual void operator()(const LocationStep& step) const
            {locationSteps_.push_back(step);}
            LocationStepList& locationSteps_;
        };

        bool parsePath( ParseInput& pi, const StepAppender& appender );

        inline bool parsePath( ParseInput& pi, LocationStepList& locationSteps )
        {
            return parsePath( pi, ListStepAppender(locationSteps) );
        }

    }

    struct Path::StepList {
        detail::LocationStepList steps;
    };

}

/**********************************************************************************************/

#endif //!defined(XPATH_PATHPARSER_H)

