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
** \file xpath/xpath_api.cpp
**
** \brief XPath API
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/
#include "xpath_api.h"

/**********************************************************************************************/
#include <asl/dom/Nodes.h>

/**********************************************************************************************/
#include "XPathPath.h"
#include "PathParser.h"
#include "xpath_api_impl.h"

/**********************************************************************************************/

namespace xpath {

    void find(const Path& path, const dom::NodePtr node, dom::NodePtr& result)
    {
        const detail::LocationStepList& steps = path.getStepList().steps;
        detail::Context context(node.get());
        detail::NodePtrVec temp;
        findNodes(context,steps.begin(),steps.end(),temp,true);
        if( !temp.empty() ) {
            result = temp.front()->self().lock();
        } else {
            result = dom::NodePtr();
        }
    }

    void findAll(const Path& path, const dom::NodePtr node, NodeList& result)
    {
        const detail::LocationStepList& steps = path.getStepList().steps;
        detail::Context context(node.get());
        result.clear();
        detail::NodePtrVec temp;
        findNodes(context,steps.begin(),steps.end(),temp,false);
        for( detail::NodePtrVec::const_iterator it=temp.begin(); it!=temp.end(); ++it ) {
            result.push_back( (*it)->self().lock() );
        }
    }

}

/**********************************************************************************************/
