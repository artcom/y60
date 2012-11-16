/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

// own header
#include "BuilderBase.h"


#include <y60/base/NodeNames.h>
#include <y60/base/CommonTags.h>
#include <asl/dom/Nodes.h>

namespace y60 {

    BuilderBase::BuilderBase() {};

    BuilderBase::BuilderBase(const std::string & theNodeName)
        : _myNode(dom::NodePtr(new dom::Element(theNodeName)))
    {}

    BuilderBase::BuilderBase(const dom::NodePtr & theNode) :
        _myNode(theNode)
    {}

    BuilderBase::~BuilderBase() {};

    void
    BuilderBase::reset(const std::string & theNodeName) {
        _myNode = dom::NodePtr(new dom::Element(theNodeName));
    }

    const std::string &
    BuilderBase::getId() const {
        return _myNode->getAttributeString(ID_ATTRIB);
    }

    const std::string &
    BuilderBase::appendNodeWithId(const BuilderBase & theChildBuilder,
            dom::NodePtr theListNode)
    {
        // With assimilate child, the object already has an id.
        if (!(theChildBuilder.getNode()->getAttribute(ID_ATTRIB))) {
            theChildBuilder.getNode()->appendAttribute(ID_ATTRIB, IdTag::getDefault());
        }
        theListNode->appendChild(theChildBuilder.getNode());
        return theChildBuilder.getNode()->getAttributeString(ID_ATTRIB);
    }

    void
    BuilderBase::removeNodeById(dom::NodePtr theListNode, const std::string & theId) {
        for(dom::NodeList::size_type i = 0; i< theListNode->childNodesLength(); i++) {
            dom::NodePtr myChild = theListNode->childNode(i);
            if (myChild->getAttributeString(ID_ATTRIB) == theId) {
                theListNode->removeChild(myChild);
            }
        }
    }

    dom::NodePtr
    BuilderBase::getNode() const {
        return _myNode;
    }
    void
    BuilderBase::setNode(const dom::NodePtr & theNode) {
        _myNode = theNode;
    }
}
