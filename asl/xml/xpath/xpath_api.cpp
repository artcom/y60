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
#include "Path.h"
#include "PathParser.h"
#include "xpath_api_impl.h"

/**********************************************************************************************/

namespace xpath {

    void find(const Path& path, const dom::NodePtr node, dom::NodePtr& result)
    {
        const detail::LocationStepList& steps = path.getStepList().steps;
        detail::Context context(node.getNativePtr());
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
        detail::Context context(node.getNativePtr());
        result.clear();
        detail::NodePtrVec temp;
        findNodes(context,steps.begin(),steps.end(),temp,false);
        for( detail::NodePtrVec::const_iterator it=temp.begin(); it!=temp.end(); ++it ) {
            result.push_back( (*it)->self().lock() );
        }
    }

}

/**********************************************************************************************/
