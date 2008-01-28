//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

#include "Light.h"
#include <asl/Logger.h>

namespace y60 {  

    Light::Light(dom::Node & theNode) : 
        TransformHierarchyFacade(theNode),
        LightSourceTag::Plug(theNode),
        _myLightSource(0),
        _myLightSourceVersion(0)
    {}
                
    bool 
    Light::isHeadLight() const { 
        return (getNode().parentNode() && getNode().parentNode()->nodeName() == CAMERA_NODE_NAME); 
    }
     
    const LightSourcePtr 
    Light::getLightSource() { 
        unsigned long long myCurrentVersion = getVersion<LightSourceTag>();
        if (_myLightSourceVersion != myCurrentVersion) {
            dom::NodePtr myLightSource = getNode().getElementById(get<LightSourceTag>());
            if (!myLightSource) {
                AC_ERROR << getNode();
                AC_ERROR << "Could not find light source with id: " << get<LightSourceTag>();
            } else {
                _myLightSource = myLightSource->getFacade<LightSource>();
                _myLightSourceVersion = myCurrentVersion;
            }
        }
        return _myLightSource; 
    }
}


