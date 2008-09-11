//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: WorldBuilderBase.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: WorldBuilderBase.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "WorldBuilderBase.h"
#include "Facades.h"
#include <y60/base/NodeNames.h>
#include <asl/dom/Nodes.h>

namespace y60 {

    WorldBuilderBase::WorldBuilderBase(const std::string & theNodeName)
        : BuilderBase(theNodeName), _myAssimilateFlag(false)
    {
        if (!getNode()->hasFacade()) {
            bool myVisible = true;
            getNode()->appendAttribute(VISIBLE_ATTRIB, myVisible);
        } else {
            WorldFacadePtr myWorld = getNode()->getFacade<WorldFacade>();
            myWorld->set<VisibleTag>(true);
        }
    }

    WorldBuilderBase::WorldBuilderBase(const dom::NodePtr & theNode) :
        BuilderBase(theNode)
    {}

    const std::string &
    WorldBuilderBase::appendObject(WorldBuilderBase & theBuilder) {
        return appendNodeWithId(theBuilder, getNode());
    }

    void
    WorldBuilderBase::setVisiblity(bool theVisibleFlag) {
        getNode()->getAttribute(VISIBLE_ATTRIB)->nodeValueAssign(theVisibleFlag);
    }

    bool
    WorldBuilderBase::getVisiblity() const {
        return getNode()->getAttributeValue<bool>(VISIBLE_ATTRIB);
    }
}
