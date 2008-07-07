//=============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef XPATH_API_H
#define XPATH_API_H

#include <cstdlib>
#include <string>

#include <dom/Nodes.h>

#include "Types.h"
#include "Expression.h"

namespace xpath {

    void f(PathPtr);

    // yields a path pointer to the responsibility of the caller.
    PathPtr xpath_parse(const std::string &instring);

    // OBSOLETE
    inline void xpath_return(PathPtr) {}

    // evaluate path on startingElement into results
    void xpath_evaluate( PathPtr 
                       , const dom::NodePtr startingElement
                       , NodeVector& results );
    void xpath_evaluate( const std::string& 
                       , const dom::NodePtr startingElement
                       , NodeVector& results);

    dom::NodePtr xpath_evaluate1(PathPtr, const dom::NodePtr);

    OrderedNodeSetPtr xpath_evaluateOrderedSet(PathPtr myPath, dom::NodePtr theNode);
    OrderedNodeSetPtr xpath_evaluateOrderedSet(const std::string&, dom::NodePtr);

    NodeSetPtr xpath_evaluateSet(PathPtr, dom::NodePtr);
    NodeSetPtr xpath_evaluateSet(const std::string&, dom::NodePtr);

    NodeVectorPtr xpath_evaluate(PathPtr, dom::NodePtr);
    NodeVectorPtr xpath_evaluate(const std::string&, dom::NodePtr);

    NodeVectorPtr findAll(dom::Node & theRoot, const std::string & theExpression);
    dom::NodePtr find(dom::Node & theRoot, const std::string & theExpression);
};

#endif
