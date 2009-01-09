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
** \file xpath/xpath_api.h
**
** \brief XPath API
**
** \author Hendrik Schober
**
** \date 2008-07-15
**
*/

/**********************************************************************************************/

#if !defined(XPATH_API_H)
#define XPATH_API_H

#include "asl_xpath_settings.h"

/**********************************************************************************************/
#include <vector>
/**********************************************************************************************/
#include <asl/dom/Nodes.h>
/**********************************************************************************************/
#include "XPathPath.h"

/**********************************************************************************************/

namespace xpath {

    typedef std::vector<dom::NodePtr>           NodeList;

    void find(const Path&, const dom::NodePtr, dom::NodePtr&);

    dom::NodePtr find(const Path&, const dom::NodePtr);

    ASL_XPATH_EXPORT void findAll(const Path&, const dom::NodePtr, NodeList&);

    NodeList findAll(const Path&, const dom::NodePtr);

    inline 
    dom::NodePtr 
    find(const Path& thePath, const dom::NodePtr theNode)
    {
        dom::NodePtr myResult;
        find(thePath,theNode,myResult);
        return myResult;
    }

    inline
    NodeList
    findAll(const Path& thePath, const dom::NodePtr theNode)
    {
        NodeList myResult;
        findAll(thePath,theNode,myResult);
        return myResult;
    }

}

/**********************************************************************************************/

#endif //!defined(XPATH_API_H)
