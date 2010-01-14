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
** \file xpath/xpath_api_impl.h
**
** \brief XPath API implementation
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/

#if !defined(XPATH_API_IMPL_H)
#define XPATH_API_IMPL_H

#include "asl_xpath_settings.h"

/**********************************************************************************************/
#include "xpath_api.h"

/**********************************************************************************************/
#include <vector>

/**********************************************************************************************/
#include <asl/dom/Nodes.h>

/**********************************************************************************************/

/**********************************************************************************************/

namespace xpath {

    namespace detail {

        /**************************************************************************************/
        /* basic stuff                                                                        */

        typedef std::vector<const dom::Node*>               NodePtrVec;

        struct Context {
            const dom::Node*            contextNode;
            NodePtrVec                  currentNodes;
            Context( const dom::Node* node )              : contextNode(node), currentNodes() {}
            Context( const dom::Node* node
                , const NodePtrVec& curr )             : contextNode(node), currentNodes(curr) {}
        };

        /**************************************************************************************/
        /* finding nodes for specific axisses                                                 */

        inline void findChildNodes(Context& context)
        {
            const dom::NodeList& children = context.contextNode->childNodes();
            const std::size_t size = children.size();
            for( std::size_t idx=0; idx<size; ++idx ) {
                const dom::Node* child = children.item(idx).get();
                context.currentNodes.push_back(child);
            }
        }

        inline void findAttributeNodes(Context& context)
        {
            const dom::NodeList& attributes = context.contextNode->attributes();
            const std::size_t size = attributes.size();
            for( std::size_t idx=0; idx<size; ++idx ) {
                const dom::Node* attribute = attributes.item(idx).get();
                context.currentNodes.push_back(attribute);
            }
        }

        inline void findChildNodesRecursivly(Context& context)
        {
            const dom::NodeList& children = context.contextNode->childNodes();
            const std::size_t size = children.size();
            for( std::size_t idx=0; idx<size; ++idx ) {
                const dom::Node* child = children.item(idx).get();
                context.currentNodes.push_back(child);
                Context childcontext(child);
                findChildNodesRecursivly(childcontext);
                context.currentNodes.insert( context.currentNodes.end()
                                        , childcontext.currentNodes.begin()
                                        , childcontext.currentNodes.end  () );
            }
        }

        inline void findParentNodesRecursivly(Context& context)
        {
            const dom::Node* parent = context.contextNode->parentNode();
            if( parent ) {
                context.currentNodes.push_back(parent);
                Context parentcontext(parent);
                findParentNodesRecursivly(context);
                context.currentNodes.insert( context.currentNodes.end()
                                        , parentcontext.currentNodes.begin()
                                        , parentcontext.currentNodes.end  () );
            }
        }

        inline void findFollowingSiblingNodes(Context& context)
        {
            while( const dom::Node* const sibling = context.contextNode->nextSibling().get() ) {
                context.currentNodes.push_back(sibling);
                Context siblingcontext(sibling);
                findChildNodesRecursivly(context);
                context.currentNodes.insert( context.currentNodes.end()
                                        , siblingcontext.currentNodes.begin()
                                        , siblingcontext.currentNodes.end  () );
            }
        }

        inline void findFollowingNodes(Context& context)
        {
            findChildNodesRecursivly(context);
            findFollowingSiblingNodes(context);
            while( const dom::Node* const parent = context.contextNode->parentNode() ) {
                Context parentcontext(parent);
                findFollowingSiblingNodes(parentcontext);
                context.currentNodes.insert( context.currentNodes.end()
                                        , parentcontext.currentNodes.begin()
                                        , parentcontext.currentNodes.end  () );
            }
        }

        inline void findPrecedingSiblingNodes(Context& context)
        {
            while( const dom::Node* const sibling = context.contextNode->previousSibling().get() ) {
                context.currentNodes.push_back(sibling);
                Context siblingcontext(sibling);
                findChildNodesRecursivly(context);
                context.currentNodes.insert( context.currentNodes.begin()
                                        , siblingcontext.currentNodes.begin()
                                        , siblingcontext.currentNodes.end  () );
            }
        }

        inline void findPrecedingNodes(Context& context)
        {
            findChildNodesRecursivly(context);
            findPrecedingSiblingNodes(context);
            while( const dom::Node* const parent = context.contextNode->parentNode() ) {
                Context parentcontext(parent);
                findPrecedingSiblingNodes(parentcontext);
                context.currentNodes.insert( context.currentNodes.end()
                                        , parentcontext.currentNodes.begin()
                                        , parentcontext.currentNodes.end  () );
            }
        }

        inline void findAttributeChildNodes(Context& context)
        {
            findAttributeNodes(context);
        }

        /**************************************************************************************/
        /* finding nodes for a step                                                           */

        inline NodePtrVec findAxisNodes(Context& context, const Axis& axis)
        {
            Context subcontext(context.contextNode);
            switch( axis.getType() ) {
                case Axis::Type_Root             :  subcontext.currentNodes.push_back( subcontext.contextNode->getDocumentNode() );
                                                    break;

                case Axis::Type_AncestorOrSelf   :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                    // FALL THROUGH
                case Axis::Type_Ancestor         :  findParentNodesRecursivly(subcontext);
                                                    break;

                case Axis::Type_DescendantOrSelf :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                    // FALL THROUGH
                case Axis::Type_Descendant       :  findChildNodesRecursivly(subcontext);
                                                    break;

                case Axis::Type_Following        :  findFollowingNodes(subcontext);
                                                    break;
                case Axis::Type_FollowingSibling :  findFollowingSiblingNodes(subcontext);
                                                    break;
                case Axis::Type_Preceding        :  findPrecedingNodes(subcontext);
                                                    break;
                case Axis::Type_PrecedingSibling :  findPrecedingSiblingNodes(subcontext);
                                                    break;

                case Axis::Type_Child            :  findChildNodes(subcontext);
                                                    break;
                case Axis::Type_Parent           :  subcontext.currentNodes.push_back( subcontext.contextNode->parentNode() );
                                                    break;
                case Axis::Type_Self             :  subcontext.currentNodes.push_back( subcontext.contextNode );
                                                    break;
                case Axis::Type_Namespace        :  throw ParseError( "\"namespace\" axis not implemented!", PLUS_FILE_LINE );
                                                    break;
                case Axis::Type_Attribute        :  findAttributeChildNodes(subcontext);
                                                    break;

                case Axis::Type_Invalid          :  // FALL THROUGH
                default                          :  assert(false);
            }
            return subcontext.currentNodes;
        }

        inline const NodePtrVec findNodeTestNodes(Context& context, const NodeTest& nodeTest)
        {
            switch( nodeTest.getType() ) {
                case NodeTest::Type_NodeTest :  if( nodeTest.getValue().empty() ) {
                                                    return context.currentNodes;
                                                }
                                                break;
                case NodeTest::Type_NameTest :  if( nodeTest.getValue() == "*" ) {
                                                    return context.currentNodes;
                                                }
                                                break;
                default:
//TODO:  check warning: enumeration value 'Type_Invalid' not handled in switch
//TODO:  check warning: enumeration value 'Type_CommentTest' not handled in switch
//TODO:  check warning:  enumeration value 'Type_TextTest' not handled in switch
//TODO:  check warning: enumeration value 'Type_ProcessingInstructionTest' not handled in switch
                                                break;
            }
            NodePtrVec currentNodes;
            for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
                switch( nodeTest.getType() ) {
                    case NodeTest::Type_CommentTest               :
                        if( (*it)->nodeType() == dom::Node::COMMENT_NODE ) {
                            currentNodes.push_back(*it);
                        }
                        break;
                    case NodeTest::Type_TextTest                  :
                        if( (*it)->nodeType() == dom::Node::TEXT_NODE ) {
                            currentNodes.push_back(*it);
                        }
                        break;
            	    case NodeTest::Type_ProcessingInstructionTest :
                        if( (*it)->nodeType() == dom::Node::PROCESSING_INSTRUCTION_NODE ) {
                            if( !nodeTest.getValue().empty() ) {
                                if( nodeTest.getValue()!=(*it)->nodeValueAs<std::string>() ) {
                                }
                            }
                            currentNodes.push_back(*it);
                        }
                        break;
                    case NodeTest::Type_NodeTest                  :
                        if( (*it)->nodeType() == dom::Node::ELEMENT_NODE ) {
                            currentNodes.push_back(*it);
                        }
                        break;
                    case NodeTest::Type_NameTest                  :
                        if( (*it)->nodeName() == nodeTest.getValue() ) {
                            currentNodes.push_back(*it);
                        }
                        break;
                    case NodeTest::Type_Invalid                   : // FALL THROUGH
                    default                                       : assert(false);
                }
            }
            return currentNodes;
        }

        inline const NodePtrVec findPredicateNodes(Context& context, const Predicate& predicate)
        {
            if( predicate.empty() ) {
                return context.currentNodes;
            }
            NodePtrVec currentNodes;
            for( NodePtrVec::size_type idx=0; idx<context.currentNodes.size(); ++idx ) {
                const Predicate::EvaluationContext evContext( context.currentNodes[idx]
                                                            , context.currentNodes.size()
                                                            , idx+1 );
                if( predicate.evaluate(evContext) ) {
                    currentNodes.push_back(context.currentNodes[idx]);
                }
            }
            return currentNodes;
        }

        inline void findStepNodes( Context& context, const LocationStep& step )
        {
            Context subcontext(context.contextNode);
            subcontext.currentNodes = findAxisNodes     (subcontext,step.getAxis()     );
            subcontext.currentNodes = findNodeTestNodes (subcontext,step.getNodeTest() );
            subcontext.currentNodes = findPredicateNodes(subcontext,step.getPredicate());
            context.currentNodes.insert( context.currentNodes.end()
                                       , subcontext.currentNodes.begin()
                                       , subcontext.currentNodes.end  () );
        }

        /**************************************************************************************/
        /* finding nodes for a step list                                                      */

        template< typename RanIt >
        inline void findNodes( Context& context
                           , RanIt begin, RanIt end
                           , NodePtrVec& result
                           , bool stopOnFirstHit )
        {
            if( begin == end ) {
                return;
            }
            findStepNodes(context,*begin);
            ++begin;
            if( begin != end ) {
                for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
                    Context childcontext(*it);
                    findNodes(childcontext,begin,end,result,stopOnFirstHit);
                    if( stopOnFirstHit && !result.empty() ) {
                        return;
                    }
                }
            } else {
                for( NodePtrVec::const_iterator it=context.currentNodes.begin(); it!=context.currentNodes.end(); ++it ) {
                    const NodePtrVec::const_iterator found = std::find(result.begin(),result.end(),*it);
                    if( found == result.end() ) {
                        result.push_back(*it);
                    }
                }
            }
        }

        /**************************************************************************************/

        inline void findNodes( const Path& path
                             , Context& context
                             , NodePtrVec& result
                             , bool stopOnFirstHit )
        {
            const detail::LocationStepList& steps = path.getStepList().steps;
            findNodes( context, steps.begin(), steps.end(), result, stopOnFirstHit );
        }

        /**************************************************************************************/

    }
}

/**********************************************************************************************/

#endif //!defined(XPATH_API_IMPL_H)

