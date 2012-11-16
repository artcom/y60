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
//   $RCSfile: LightSource.h,v $
//   $Author: christian $
//   $Revision: 1.9 $
//   $Date: 2004/11/15 10:05:49 $
//
//  Description: This class is the base of all lights.
//
//=============================================================================

#ifndef _ac_scene_LightSource_h_
#define _ac_scene_LightSource_h_

#include "y60_scene_settings.h"

#include <y60/base/DataTypes.h>
#include <y60/base/CommonTags.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/PropertyNames.h>
#include <y60/base/TypeTraits.h>
#include <asl/dom/Facade.h>
#include <asl/dom/PropertyPlug.h>
#include <asl/dom/AttributePlug.h>
#include <asl/dom/ChildNodePlug.h>
#include <asl/base/Logger.h>

#include "PropertyListFacade.h"

#define DEFINE_LIGHT_PROPERTY_TAG(theTagName, theType, thePropertyName, theDefault) \
    DEFINE_PROPERTY_TAG(theTagName,  LightPropertiesFacade, theType, y60::getTypeName<theType>(), thePropertyName,  "properties", theDefault);

namespace y60 {

    //                  TagName             Type           PropertyName                    Default
    DEFINE_ATTRIBUTE_TAG(LightSourceTypeTag, std::string,   LIGHTSOURCE_TYPE_ATTRIB,   "unsupported", Y60_SCENE_DECL);

    DEFINE_LIGHT_PROPERTY_TAG(LightAmbientTag,     asl::Vector4f, AMBIENT_PROPERTY,          asl::Vector4f(0.2f,0.2f,0.2f,1));
    DEFINE_LIGHT_PROPERTY_TAG(LightDiffuseTag,     asl::Vector4f, DIFFUSE_PROPERTY,          asl::Vector4f(0.8f,0.8f,0.8f,1));
    DEFINE_LIGHT_PROPERTY_TAG(LightSpecularTag,    asl::Vector4f, SPECULAR_PROPERTY,         asl::Vector4f(0,0,0,1));
    DEFINE_LIGHT_PROPERTY_TAG(AttenuationTag, float,         ATTENUATION_PROPERTY,      0);
    DEFINE_LIGHT_PROPERTY_TAG(CutOffTag,      float,         SPOTLIGHT_CUTOFF_ATTRIB,   180);
    DEFINE_LIGHT_PROPERTY_TAG(ExponentTag,    float,         SPOTLIGHT_EXPONENT_ATTRIB, 0);


    class LightPropertiesFacade :
        public PropertyListFacade,
        public LightAmbientTag::Plug,
        public LightDiffuseTag::Plug,
        public LightSpecularTag::Plug,
        public AttenuationTag::Plug,
        public CutOffTag::Plug,
        public ExponentTag::Plug
    {
        public:
            LightPropertiesFacade(dom::Node & theNode) :
                PropertyListFacade(theNode),
                LightAmbientTag::Plug(this),
                LightDiffuseTag::Plug(this),
                LightSpecularTag::Plug(this),
                AttenuationTag::Plug(this),
                CutOffTag::Plug(this),
                ExponentTag::Plug(this)
            {}
            IMPLEMENT_FACADE(LightPropertiesFacade);
    };
    typedef asl::Ptr<LightPropertiesFacade, dom::ThreadingModel> LightPropertiesFacadePtr;

    DEFINE_CHILDNODE_TAG(LightPropertiesTag, LightSource, LightPropertiesFacade, "properties");

    class Y60_SCENE_DECL LightSource :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public LightSourceTypeTag::Plug,
        public LightPropertiesTag::Plug
    {
        public:
            LightSource(dom::Node & theNode);
            IMPLEMENT_PARENT_FACADE(LightSource);
            const LightSourceType getType();
        private:
            LightSource();
            void updateProperties();
            LightSourceType    _myType;
    };

    typedef asl::Ptr<LightSource, dom::ThreadingModel> LightSourcePtr;
}

#endif

