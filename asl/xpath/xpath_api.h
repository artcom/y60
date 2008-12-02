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

/**********************************************************************************************/
#include <vector>
/**********************************************************************************************/
#include <asl/dom/Nodes.h>
/**********************************************************************************************/
#include "Path.h"

/**********************************************************************************************/

namespace xpath {

    typedef std::vector<dom::NodePtr>           NodeList;

    void find(const Path&, const dom::NodePtr, dom::NodePtr&);

    dom::NodePtr find(const Path&, const dom::NodePtr);

    void findAll(const Path&, const dom::NodePtr, NodeList&);

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
