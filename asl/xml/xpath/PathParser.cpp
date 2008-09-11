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
** \file xpath/PathParser.cpp
**
** \brief XPath paths parser
**
** \author Hendrik Schober
**
** \date 2008-07-11
**
*/

/**********************************************************************************************/
#include "PathParser.h"

/**********************************************************************************************/
#include <cstring>
#include <cassert>

/**********************************************************************************************/
#include <asl/base/string_functions.h>

/**********************************************************************************************/
#include "ParseInput.h"
#include "Expression.h"

/**********************************************************************************************/

namespace {

    using namespace xpath::detail;

    /******************************************************************************************/

    typedef asl::static_dict_entry_type<Axis::Type>::result_type
                                                  AxisDictEntry;
    const AxisDictEntry axisDictEntries[] = 
        { AxisDictEntry( "ancestor"           , Axis::Type_Ancestor         )
        , AxisDictEntry( "ancestor-or-self"   , Axis::Type_AncestorOrSelf   )
        , AxisDictEntry( "attribute"          , Axis::Type_Attribute        )
        , AxisDictEntry( "child"              , Axis::Type_Child            )
        , AxisDictEntry( "descendant"         , Axis::Type_Descendant       )
        , AxisDictEntry( "descendant-or-self" , Axis::Type_DescendantOrSelf )
        , AxisDictEntry( "following"          , Axis::Type_Following        )
        , AxisDictEntry( "following-sibling"  , Axis::Type_FollowingSibling )
        , AxisDictEntry( "namespace"          , Axis::Type_Namespace        )
        , AxisDictEntry( "parent"             , Axis::Type_Parent           )
        , AxisDictEntry( "preceding"          , Axis::Type_Preceding        )
        , AxisDictEntry( "preceding-sibling"  , Axis::Type_PrecedingSibling )
        , AxisDictEntry( "self"               , Axis::Type_Self             )
        };

    typedef asl::static_dict< Axis::Type
                                , asl_static_array_size(axisDictEntries) >
                                              AxisDict;
    const AxisDict axisDict( (asl::begin(axisDictEntries))
                           , (asl::end  (axisDictEntries)) );

    /******************************************************************************************/

    typedef asl::static_dict_entry_type<NodeTest::Type>::result_type
                                                  NodeTestDictEntry;

    const NodeTestDictEntry nodeTestDictEntries[] = 
        { NodeTestDictEntry( "comment"                , NodeTest::Type_CommentTest               )
        , NodeTestDictEntry( "text"                   , NodeTest::Type_TextTest                  )
        , NodeTestDictEntry( "processing-instruction" , NodeTest::Type_ProcessingInstructionTest )
        , NodeTestDictEntry( "node"                   , NodeTest::Type_NodeTest                  )
        };

    typedef asl::static_dict< NodeTest::Type
                                , asl_static_array_size(nodeTestDictEntries) >
                                              NodeTestDict;
    const NodeTestDict nodeTestDict( (asl::begin(nodeTestDictEntries))
                                   , (asl::end  (nodeTestDictEntries)) );

    /******************************************************************************************/

    Axis::Type readAbbrevAxis(ParseInput& pi)
    {
        static const AxisDictEntry axisAbbrevDictEntries[] = 
            // note: order matters here!
            { AxisDictEntry( ".."   , Axis::Type_Parent           )
            , AxisDictEntry( "."    , Axis::Type_Self             )
            , AxisDictEntry( "//"   , Axis::Type_DescendantOrSelf )
            , AxisDictEntry( "@"    , Axis::Type_Attribute        )
            };

        static const AxisDictEntry* const begin = asl::begin(axisAbbrevDictEntries);
        static const AxisDictEntry* const end   = asl::end  (axisAbbrevDictEntries);

        for( const AxisDictEntry* it=begin; it!=end; ++it ) {
            if( pi.readIf(it->first) ) {
                return it->second;
            }
        }

        return Axis::Type_Invalid;
    }

    /******************************************************************************************/

    inline bool readPathSeparator(ParseInput& pi)
    {
        ParseState ps(pi);
        if( !pi.readIf('/') ) {
            return false;
        }
        if( pi.readIf('/') ) {
            return false;
        }
        ps.commit();
        return true;
    }

    inline bool readDescOrSelfAbbrev(ParseInput& pi)
    {
        return pi.readIf("//");
    }

    bool parse_step( ParseInput& pi, const StepAppender& appender )
    {
        ParseState ps(pi);

        Axis axis(pi);

        const NodeTest nodeTest(pi);

        if( axis.needsNodeTest() ) {
            if( nodeTest.empty() ) {
                pi.setErrorIndex();
                return false;
            }
        } else {
            if( nodeTest.empty() || nodeTest.getType()==NodeTest::Type_NameTest ) {
                appender( LocationStep(axis,NodeTest(NodeTest::Type_NodeTest,"")) );
                if( nodeTest.empty() ) {
                    ps.commit();
                    return true;
                }   
                axis = Axis(Axis::Type_Child);
            }
        }

        const Predicate predicate(pi);
        appender( LocationStep(axis,nodeTest,predicate) );
        ps.commit();
        return true;
    }

}

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        //struct Axis {
            Axis::Type Axis::readAxisType(ParseInput& pi)
            {
                typedef std::pair<AxisDict::const_iterator,ParseInput::const_iterator>
                                                          FindResult;

                const Type abbrev = readAbbrevAxis(pi);
                if( abbrev != Type_Invalid ) {
                    return abbrev;
                }

                const ParseInput::const_iterator oldBegin = pi.begin();
                const FindResult& result = axisDict.lookup(pi.begin(),pi.end());
                if( result.first == axisDict.end() ) {
                    return Type_Child;
                }
                if( result.first->first.size() != result.second-oldBegin ) {
                    return Type_Child;
                }
                pi.forwardTo(result.second);

                pi.readIf("::");

                return result.first->second;
            }

            bool Axis::needsNodeTest() const
            {
                switch( myAxisType_ ) {
                    case Type_Ancestor         : // FALL THROUGH
                    case Type_AncestorOrSelf   : // FALL THROUGH
                    case Type_Descendant       : // FALL THROUGH
                    case Type_DescendantOrSelf : // FALL THROUGH
                    case Type_Following        : // FALL THROUGH
                    case Type_FollowingSibling : // FALL THROUGH
                    case Type_Parent           : // FALL THROUGH
                    case Type_Preceding        : // FALL THROUGH
                    case Type_PrecedingSibling : // FALL THROUGH
                    case Type_Self             : // FALL THROUGH
                    case Type_Root             : // FALL THROUGH
                                                 return false;
                                                 break;

                    case Type_Attribute        : // FALL THROUGH
                    case Type_Child            : // FALL THROUGH
                    case Type_Namespace        : // FALL THROUGH
                                                 return true;
                                                 break;

                    case Type_Invalid          : assert(false); 
                                                 break;
                    default                    : assert(false);
                }
                return false;
            }

            std::ostream& Axis::streamTo(std::ostream& os) const
            {
                if( myAxisType_ == Type_Root ) {
                    os << '/';
                } else {
                    os << toString();
                }
                return os;
            }

            std::string Axis::toString() const
            {
                const AxisDict::const_iterator it = axisDict.rlookup(myAxisType_);
                assert(it!=axisDict.end());
                return it->first;
            }

            int Axis::compare(const Axis& rhs) const
            {
                return std::strcmp( toString().c_str(), rhs.toString().c_str() );
            }

            //};

        //struct NodeTest {
            NodeTest::Type NodeTest::readNodeTestType( ParseInput& pi, std::string& value )
            {
                typedef std::pair<NodeTestDict::const_iterator,ParseInput::index_type>
                                                          FindResult;

                ParseState ps(pi);

                if( pi.readIf('*') ) {
                   value="*";
                   ps.commit();
                   return Type_NameTest;
                }

                const std::string& identifier = pi.readIdentifier();
                if( identifier.empty() ) {
                    return Type_Invalid;
                }

                const FindResult& result = nodeTestDict.lookup(identifier);
                if( result.first==nodeTestDict.end() || result.second!=identifier.size() ) {
                    // could be a name test
                    if( pi.readIf('(') ) { // oops, function call not allowed here
                        return Type_Invalid;
                    }
                    // must be a name test
                    value = identifier;
                    ps.commit();
                    return Type_NameTest;
                }

                if( '(' != pi.peekNextChar(true) ) {
                    value = identifier;
                    ps.commit();
                    return Type_NameTest;
                }
                value = pi.readQuotedText('(',')'); // nested calls not allowed here
                if( !value.empty() && result.first->second!=Type_ProcessingInstructionTest ) {
                    return Type_Invalid;
                }

                ps.commit();
                return result.first->second;
            }

            NodeTest::NodeTest( ParseInput& pi )
                : myNodeTestType_(), myValue_()
            {
                myNodeTestType_ = readNodeTestType(pi,myValue_);
            }

            std::ostream& NodeTest::streamTo(std::ostream& os) const
            {
                if( myNodeTestType_ == Type_NameTest ) {
                    os << myValue_;
                } else {
                    os << toString();
                }
                return os;
            }

            std::string NodeTest::toString() const
            {
                if( myNodeTestType_ == Type_NameTest ) {
                    return myValue_;
                }
                NodeTestDict::const_iterator it = nodeTestDict.rlookup(myNodeTestType_);
                assert(it!=nodeTestDict.end());
                return it->first + "(" + myValue_ + ")";
            }

            int NodeTest::compare(const NodeTest& rhs) const
            {
                return std::strcmp( toString().c_str(), rhs.toString().c_str() );
            }
        //};

        //class Predicate {
        //public:
            //class Expression {
                bool Predicate::Expression::evaluate(const EvaluationContext& evContext) const
                {
                    Object result = doEvaluate(evContext);
                    switch( result.getType() ) {
                    case Value_Number : 
                        return evContext.proximityPos
                            == static_cast<int>(result.as<NumberValue>().getValue());
                    case Value_NodeSet: // FALL THROUGH
                    case Value_Boolean: // FALL THROUGH
                    case Value_String : // FALL THROUGH
                                        break;
                    }
                    return result.as<BooleanValue>().getValue();
                }
            //};

            Predicate::ExpressionPtr Predicate::readExpression( ParseInput& pi)
            {
                ParseState ps(pi);

                Predicate::ExpressionPtr lhs;
                while( pi.readIf('[') ) {
                    Predicate::ExpressionPtr rhs = xpath::detail::readExpression(pi);
                    if( !pi.readIf(']') ) {
                        return Predicate::ExpressionPtr();
                    }
                    if( lhs ) {
                        lhs = ExpressionPtr( new AndExpr(lhs,rhs) );
                    } else {
                        lhs = rhs;
                    }
                }

                ps.commit();
                return lhs;
            }

            std::ostream& Predicate::streamTo(std::ostream& os) const
            {
                if( expression_ ) {
                    expression_->streamTo(os);
                }
                return os;
            }

            std::string Predicate::toString() const
            {
                std::ostringstream oss;
                streamTo(oss);
                return oss.str();
            }

            int Predicate::compare(const Predicate& rhs) const
            {
                // this could be done faster than through streaming!!!
                return std::strcmp( toString().c_str(), rhs.toString().c_str() );
            }
        //};

        //class LocationStep {
        //public:
            std::ostream& LocationStep::streamTo(std::ostream& os) const
            {
                os << myAxis_;
                if( myAxis_.getType() != Axis::Type_Root ) {
                    if( !myNodeTest_.empty() ) {
                        os << "::" << myNodeTest_;
                    }
                    if( !myPredicate_.empty() ) {
                        os << '[' << myPredicate_ << ']';
                    }
                }
                return os;
            }
            
            int LocationStep::compare(const LocationStep& rhs) const
            {
                const int cmpAxis = myAxis_.compare(rhs.myAxis_);
                if( 0 != cmpAxis ) {
                    return cmpAxis;
                }
                const int cmpNodeTest = myNodeTest_.compare(rhs.myNodeTest_);
                if( 0 != cmpNodeTest ) {
                    return cmpNodeTest;
                }
                const int cmpPredicate = myPredicate_.compare(rhs.myPredicate_);
                return cmpPredicate;
            }
        //};

        bool parsePath( ParseInput& pi, const StepAppender& appender )
        {
            if( readDescOrSelfAbbrev(pi) ) {
                appender( LocationStep( Axis(Axis::Type_Root), NodeTest(NodeTest::Type_NodeTest,"") ) );
                appender( LocationStep( Axis(Axis::Type_DescendantOrSelf)
                                      , NodeTest(NodeTest::Type_NodeTest,"") ) );
            } else if( readPathSeparator(pi) ) {
                appender( LocationStep( Axis(Axis::Type_Root), NodeTest(NodeTest::Type_NodeTest,"") ) );
            }

            while( !pi.eof(true) ) {
                if( !parse_step(pi,appender) ) {
                    return false;
                }
                if( readDescOrSelfAbbrev(pi) ) {
                    const LocationStep step( Axis(Axis::Type_DescendantOrSelf)
                                           , NodeTest(NodeTest::Type_NodeTest,"") );
                    appender(step);
                } else if( !readPathSeparator(pi) ) {
                    return pi.eof(true);
                }
            }

            return true;
        }

    }
}

/**********************************************************************************************/
