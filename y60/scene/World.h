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

#ifndef _scene_World_h_
#define _scene_World_h_

#include "y60_scene_settings.h"

#include "TransformHierarchyFacade.h"
#include "Light.h"

namespace y60 {

    //                  theTagName           theType           theAttributeName               theDefault
    DEFINE_ATTRIBUTE_TAG(SkyBoxMaterialTag,   std::string,      SKYBOX_MATERIAL_ATTRIB,   "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(LodScaleTag,         float,            LODSCALE_ATTRIB,          1, Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(FogModeTag,          std::string,      FOGMODE_ATTRIB,           "", Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(FogColorTag,         asl::Vector4f,    FOGCOLOR_ATTRIB,          asl::Vector4f(0,0,0,0), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(FogRangeTag,         asl::Vector2f,    FOGRANGE_ATTRIB,          asl::Vector2f(0, 1), Y60_SCENE_DECL);
    DEFINE_ATTRIBUTE_TAG(FogDensityTag,       float,            FOGDENSITY_ATTRIB,        1, Y60_SCENE_DECL);

    class World :
        public TransformHierarchyFacade,
        public SkyBoxMaterialTag::Plug,
        public LodScaleTag::Plug,
        public FogModeTag::Plug,
        public FogColorTag::Plug,
        public FogRangeTag::Plug,
        public FogDensityTag::Plug
    {
        public:
            World(dom::Node & theNode) :
                TransformHierarchyFacade(theNode),
                SkyBoxMaterialTag::Plug(theNode),
                LodScaleTag::Plug(theNode),
                FogModeTag::Plug(theNode),
                FogColorTag::Plug(theNode),
                FogRangeTag::Plug(theNode),
                FogDensityTag::Plug(theNode)
        {}

        IMPLEMENT_FACADE(World);

        const LightVector & getLights() const {
            return _myLights;
        }

        void updateLights();

        LightVector _myLights;
    };

    typedef asl::Ptr<World, dom::ThreadingModel> WorldPtr;

}

#endif
