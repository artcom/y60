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

#include "AnimationBuilder.h"
#include <y60/DataTypes.h>

#include <dom/Nodes.h>

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
