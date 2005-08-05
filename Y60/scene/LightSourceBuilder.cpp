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
//   $Id: LightSourceBuilder.cpp,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: LightSourceBuilder.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: XML-File-Export Plugin for light sources
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "LightSourceBuilder.h"
#include "LightSource.h"
#include <y60/NodeNames.h>
#include <y60/PropertyNames.h>
#include <y60/property_functions.h>

//TODO: change all setPropertyValue<> stuff to facade access

namespace y60 {
    LightSourceBuilder::LightSourceBuilder(const std::string & theName) : BuilderBase(LIGHTSOURCE_NODE_NAME) {
        dom::NodePtr myNode = getNode();

        if (!myNode->hasFacade()) {
            myNode->appendAttribute(NAME_ATTRIB, theName);
        } else {
            LightSourcePtr myLightSource = getNode()->getFacade<LightSource>();
            myLightSource->set<NameTag>(theName);
        }
 
        (*myNode)(PROPERTY_LIST_NAME);
    }

    LightSourceBuilder::~LightSourceBuilder() {
    }

    void
    LightSourceBuilder::setSpotLight(const float & theCutoff, const float & theExponent) {
        setPropertyValue<float>(getNode(), "float", SPOTLIGHT_CUTOFF_ATTRIB, theCutoff);
        setPropertyValue<float>(getNode(), "float", SPOTLIGHT_EXPONENT_ATTRIB, theExponent);
        (*getNode())[LIGHTSOURCE_TYPE_ATTRIB] =  SPOT_LIGHT;
    }

    void
    LightSourceBuilder::setAmbient(const asl::Vector4f & theAmbient) {
        setPropertyValue<asl::Vector4f>(getNode(), "vector4f", AMBIENT_PROPERTY, theAmbient);
    }

    void
    LightSourceBuilder::setDiffuse(const asl::Vector4f & theDiffuse) {
        setPropertyValue<asl::Vector4f>(getNode(), "vector4f", DIFFUSE_PROPERTY, theDiffuse);
    }

    void
    LightSourceBuilder::setAttenuation(const float & theAttenuation) {
        setPropertyValue<float>(getNode(), "float", ATTENUATION_PROPERTY, theAttenuation);
    }

    void
    LightSourceBuilder::setSpecular(const asl::Vector4f & theSpecular) {
        setPropertyValue<asl::Vector4f>(getNode(), "vector4f", SPECULAR_PROPERTY, theSpecular);
    }

    void
    LightSourceBuilder::setType(const LightSourceType theLightSourceType) {
        dom::NodePtr myNode = getNode();
        if (!myNode->hasFacade()) {
            myNode->appendAttribute(LIGHTSOURCE_TYPE_ATTRIB, asl::getStringFromEnum(theLightSourceType, LightSourceTypeString));
        } else {
            myNode->getFacade<LightSource>()->set<LightSourceTypeTag>(asl::getStringFromEnum(theLightSourceType, LightSourceTypeString));
        }
    }
}
