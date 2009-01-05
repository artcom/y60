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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#ifndef _ParticleSystem_h_
#define _ParticleSystem_h_

#include <asl/base/PlugInBase.h>

#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/scene/Scene.h>
#include <y60/jsbase/JSNode.h>
#include <y60/jsbase/JSVector.h>
#include <y60/scene/MaterialBase.h>
#include <y60/modelling/modelling_functions.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/jslib/AbstractRenderWindow.h>
#include <y60/scene/Shape.h>

#include <iostream>
#include <vector>


class ParticleSystem {
public:
    ParticleSystem(const y60::ScenePtr & theScene);
    ~ParticleSystem();

    void create( const dom::NodePtr & theParentNode,
                 unsigned theParticleCount=1000,
                 const std::string & theTextureName="1.png",
                 const asl::Vector3f & theInitialDirection=asl::Vector3f(0.0f, 1.0f, 0.0f),
                 const asl::Vector2f & theScattering=asl::Vector2f(0.0f, 45.0f),
                 const asl::Vector2f & theSpeedRange=asl::Vector2f(10.0, 20.0) );

    void remove();
        
    dom::NodePtr getMaterialNode() const {
        return _myMaterialNode;
    }
    dom::NodePtr getBodyNode() const {
        return _myBodyNode;
    }
    dom::NodePtr getTextureNode() const {
        return _myTextureNode;
    }
    dom::NodePtr getImageNode() const {
        return _myImageNode;
    }
    dom::NodePtr getAnimationNode() const {
        return _myAnimationNode;
    }

private:
    unsigned _myParticleCount;
    std::string _myTextureName;
    asl::Vector3f _myInitialDirection;
    asl::Vector2f _mySpeedRange;
    
    dom::NodePtr  _myParticleVertexDataPtr;
    
    y60::ScenePtr _myScene;
    dom::NodePtr  _myShapeNode;
    dom::NodePtr  _myBodyNode;
    dom::NodePtr  _myMaterialNode;
    dom::NodePtr  _myTextureNode;
    dom::NodePtr  _myImageNode;
    dom::NodePtr  _myAnimationNode;
    dom::NodePtr  _myParentNode;
    
    asl::Vector3f _myPointSize;
};

#endif // _ParticleSystem_h_
