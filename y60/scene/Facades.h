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

#ifndef _scene_Facades_h_
#define _scene_Facades_h_

#include "y60_scene_settings.h"

#include "TransformHierarchyFacade.h"
#include "LodFacade.h"
#include "Camera.h"
#include "Body.h"
#include "MaterialBase.h"
#include "Light.h"
#include "LightSource.h"
#include "Canvas.h"
#include "Viewport.h"
#include "Overlay.h"
#include "Shape.h"
#include "IncludeFacade.h"
#include "Geometry.h"
#include "Scene.h"
#include "TextureUnit.h"
#include "Texture.h"
#include "Record.h"
#include "Primitive.h"
#include "Transform.h"
#include "World.h"
#include "Joint.h"

#include <y60/image/Image.h>
#include <y60/video/Movie.h>
#include <y60/video/Capture.h>

#include <asl/dom/Nodes.h>
#include <asl/math/Matrix4.h>

namespace y60 {

    inline void registerSceneFacades(dom::FacadeFactoryPtr theFactory) {
        theFactory->registerPrototype(SCENE_ROOT_NAME, dom::FacadePtr(new Scene(dom::Node::Prototype)));
        theFactory->registerPrototype(WORLD_NODE_NAME, dom::FacadePtr(new World(dom::Node::Prototype)));
        theFactory->registerPrototype(BODY_NODE_NAME, dom::FacadePtr(new Body(dom::Node::Prototype)));
        theFactory->registerPrototype(TRANSFORM_NODE_NAME, dom::FacadePtr(new TransformFacade(dom::Node::Prototype)));
        theFactory->registerPrototype(LOD_NODE_NAME, dom::FacadePtr(new LodFacade(dom::Node::Prototype)));
        theFactory->registerPrototype(LIGHT_NODE_NAME, dom::FacadePtr(new Light(dom::Node::Prototype)));
        theFactory->registerPrototype(LIGHTSOURCE_NODE_NAME, dom::FacadePtr(new LightSource(dom::Node::Prototype)));
        theFactory->registerPrototype(PROPERTY_LIST_NAME, dom::FacadePtr(new LightPropertiesFacade(dom::Node::Prototype)),
                LIGHTSOURCE_NODE_NAME);
        theFactory->registerPrototype(CAMERA_NODE_NAME, dom::FacadePtr(new Camera(dom::Node::Prototype)));
        theFactory->registerPrototype(PROJECTOR_NODE_NAME, dom::FacadePtr(new Projector(dom::Node::Prototype)));
        theFactory->registerPrototype(JOINT_NODE_NAME, dom::FacadePtr(new JointFacade(dom::Node::Prototype)));
        theFactory->registerPrototype(CANVAS_NODE_NAME, dom::FacadePtr(new Canvas(dom::Node::Prototype)));
        theFactory->registerPrototype(VIEWPORT_NODE_NAME, dom::FacadePtr(new Viewport(dom::Node::Prototype)));
        theFactory->registerPrototype(OVERLAY_NODE_NAME, dom::FacadePtr(new Overlay(dom::Node::Prototype)));
        theFactory->registerPrototype(IMAGE_NODE_NAME, dom::FacadePtr(new Image(dom::Node::Prototype)));
        theFactory->registerPrototype(MOVIE_NODE_NAME, dom::FacadePtr(new Movie(dom::Node::Prototype)));
        theFactory->registerPrototype(CAPTURE_NODE_NAME, dom::FacadePtr(new Capture(dom::Node::Prototype)));
        theFactory->registerPrototype(TEXTURE_NODE_NAME, dom::FacadePtr(new Texture(dom::Node::Prototype)));
        theFactory->registerPrototype(TEXTUREUNIT_NODE_NAME, dom::FacadePtr(new TextureUnit(dom::Node::Prototype)));
        theFactory->registerPrototype(SHAPE_NODE_NAME, dom::FacadePtr(new Shape(dom::Node::Prototype)));
        theFactory->registerPrototype(INCLUDE_NODE_NAME, dom::FacadePtr(new IncludeFacade(dom::Node::Prototype)));
        theFactory->registerPrototype(MATERIAL_NODE_NAME, dom::FacadePtr(new MaterialBase(dom::Node::Prototype)));
        theFactory->registerPrototype(PROPERTY_LIST_NAME, dom::FacadePtr(new MaterialPropertiesFacade(dom::Node::Prototype)),
                MATERIAL_NODE_NAME);
        theFactory->registerPrototype(REQUIRES_LIST_NAME, dom::FacadePtr(new MaterialRequirementFacade(dom::Node::Prototype)),
                MATERIAL_NODE_NAME);
        theFactory->registerPrototype(RECORD_NODE_NAME, dom::FacadePtr(new RecordFacade(dom::Node::Prototype)));
        theFactory->registerPrototype(ELEMENTS_NODE_NAME, dom::FacadePtr(new Primitive(dom::Node::Prototype)));

        //=== Analytic Geometry Nodes =========================================
        theFactory->registerPrototype(PLANE_NODE_NAME, dom::FacadePtr(new Plane(dom::Node::Prototype)));
        theFactory->registerPrototype(POINT_NODE_NAME, dom::FacadePtr(new Point(dom::Node::Prototype)));
        theFactory->registerPrototype(VECTOR_NODE_NAME, dom::FacadePtr(new Vector(dom::Node::Prototype)));
    }

}

#endif
