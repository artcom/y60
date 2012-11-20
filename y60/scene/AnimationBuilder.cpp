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
//
//   $Id: AnimationBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: AnimationBuilder.cpp,v $
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

// own header
#include "AnimationBuilder.h"

#include <y60/base/DataTypes.h>

#include <asl/dom/Nodes.h>

namespace y60 {
    AnimationBuilder::AnimationBuilder() :
        BuilderBase(ANIMATION_NODE_NAME)
    {
        dom::NodePtr myNode = getNode();
        bool myEnabled = true;

        if (!myNode->hasFacade()) {
            myNode->appendAttribute(NAME_ATTRIB, "");
            myNode->appendAttribute(ANIM_ENABLED_ATTRIB, myEnabled);
            myNode->appendAttribute(ANIM_ATTRIBUTE_ATTRIB, "");
            myNode->appendAttribute(ANIM_PROPERTY_ATTRIB, "");
            myNode->appendAttribute(ANIM_NODEREF_ATTRIB, "");
            myNode->appendAttribute(ANIM_BEGIN_ATTRIB, 0.0f);
            myNode->appendAttribute(ANIM_DURATION_ATTRIB, 0.0f);
            myNode->appendAttribute(ANIM_PAUSE_ATTRIB, 0.0f);
            myNode->appendAttribute(ANIM_COUNT_ATTRIB, 1);
            myNode->appendAttribute(ANIM_DIRECTION_ATTRIB,
                asl::getStringFromEnum(FORWARD, AnimationDirectionString));
        }
    }

    AnimationBuilder::~AnimationBuilder() {
    }

    void
    AnimationBuilder::setEnable(bool theEnable) {
        getNode()->getAttribute(ANIM_ENABLED_ATTRIB)->nodeValueAssign(theEnable);
    }

    void
    AnimationBuilder::setName(const std::string & theName) {
        getNode()->getAttribute(NAME_ATTRIB)->nodeValue(theName);
    }

    void
    AnimationBuilder::setNodeRef(const std::string & theNodeRef) {
        getNode()->getAttribute(ANIM_NODEREF_ATTRIB)->nodeValue(theNodeRef);
    }

    void
    AnimationBuilder::setAttribute(const std::string & theAttribute) {
        getNode()->getAttribute(ANIM_ATTRIBUTE_ATTRIB)->nodeValue(theAttribute);
    }

    void
    AnimationBuilder::setProperty(const std::string & theProperty) {
        getNode()->getAttribute(ANIM_PROPERTY_ATTRIB)->nodeValue(theProperty);
    }

    void
    AnimationBuilder::setBegin(float theBegin) {
        getNode()->getAttribute(ANIM_BEGIN_ATTRIB)->nodeValueAssign(theBegin);
    }

    void
    AnimationBuilder::setDuration(float theDuration) {
        getNode()->getAttribute(ANIM_DURATION_ATTRIB)->nodeValueAssign(theDuration);
    }

    void
    AnimationBuilder::setPause(float thePause) {
        getNode()->getAttribute(ANIM_PAUSE_ATTRIB)->nodeValueAssign(thePause);
    }

    void
    AnimationBuilder::setCount(unsigned theCount) {
        getNode()->getAttribute(ANIM_COUNT_ATTRIB)->nodeValueAssign(theCount);
    }

    void
    AnimationBuilder::setDirection(AnimationDirection theDirection) {
        getNode()->getAttribute(ANIM_DIRECTION_ATTRIB)->nodeValue(
            asl::getStringFromEnum(theDirection, AnimationDirectionString));
    }

    void
    AnimationBuilder::appendValues(const VectorOfString & theValues) {
        (*getNode())(VectorTypeNameTrait<std::string>::name()).appendChild(dom::Text(
            dom::ValuePtr(new dom::ComplexValue<VectorOfString,
            dom::MakeResizeableVector,dom::StringValue>(theValues, 0))));
    }
}
