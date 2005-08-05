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
//   $RCSfile: Light.h,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2004/11/18 16:20:48 $
//
//  Description: A simple scene class.
//
//=============================================================================

#ifndef _ac_scene_Light_h_
#define _ac_scene_Light_h_

#include "TransformHierarchyFacade.h"
#include "LightSource.h"

#include <vector>

namespace y60 {
    //                  theTagName      theType      theAttributeName          theDefault
    DEFINE_ATTRIBUT_TAG(LightSourceTag, std::string, LIGHT_SOURCE_ATTRIB, "");

    class Light : 
        public TransformHierarchyFacade,
        public LightSourceTag::Plug
    {
        public:
            Light(dom::Node & theNode);
            
            IMPLEMENT_FACADE(Light);

            bool isHeadLight() const;
            const LightSourcePtr getLightSource();
        private:
            LightSourcePtr _myLightSource;
            unsigned long long _myLightSourceVersion;
    };

    typedef asl::Ptr<Light, dom::ThreadingModel> LightPtr;
    typedef std::vector<LightPtr> LightVector;
}

#endif 

