//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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

#include <y60/DataTypes.h>
#include <y60/CommonTags.h>
#include <y60/NodeValueNames.h>
#include <y60/PropertyNames.h>
#include <y60/TypeTraits.h>
#include <dom/Facade.h>
#include <dom/PropertyPlug.h>
#include <dom/AttributePlug.h>
#include <dom/ChildNodePlug.h>
#include <asl/Logger.h>

#include "PropertyListFacade.h"

#define DEFINE_LIGHT_PROPERTY_TAG(theTagName, theType, thePropertyName, theDefault) \
	DEFINE_PROPERTY_TAG(theTagName,  LightPropertiesFacade, theType, y60::getTypeName<theType>(), thePropertyName,  "properties", theDefault);

namespace y60 {

    //                  TagName             Type           PropertyName                    Default
    DEFINE_ATTRIBUT_TAG(LightSourceTypeTag, std::string,   LIGHTSOURCE_TYPE_ATTRIB,   "unsupported");

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

    class LightSource :
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

