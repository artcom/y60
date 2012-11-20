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
//   $RCSfile: Light.h,v $
//   $Author: pavel $
//   $Revision: 1.5 $
//   $Date: 2004/11/18 16:20:48 $
//
//  Description: A simple scene class.
//
//=============================================================================

// own header
#include "Light.h"

#include <asl/base/Logger.h>

namespace y60 {

    Light::Light(dom::Node & theNode) :
        TransformHierarchyFacade(theNode),
        LightSourceTag::Plug(theNode),
        _myLightSource(),
        _myLightSourceVersion(0)
    {}

    bool
    Light::isHeadLight() const {
        return (getNode().parentNode() && getNode().parentNode()->nodeName() == CAMERA_NODE_NAME);
    }

    const LightSourcePtr
    Light::getLightSource() const {
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


