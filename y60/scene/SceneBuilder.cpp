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

// own header
#include "SceneBuilder.h"

#include "ShapeBuilder.h"
#include "CanvasBuilder.h"
#include "ElementBuilder.h"
#include "MaterialBuilder.h"
#include "LightSourceBuilder.h"
#include "LightBuilder.h"
#include "AnimationBuilder.h"
#include "ClipBuilder.h"
#include "CharacterBuilder.h"
#include "ImageBuilder.h"
#include "MovieBuilder.h"
#include "TextureBuilder.h"
#include "WorldBuilder.h"
#include "CameraBuilder.h"
#include "TransformBuilder.h"
#include "Viewport.h"
#include "Facades.h"

#include <y60/base/CommonTags.h>
#include <y60/image/Image.h>
#include <y60/base/NodeNames.h>
#include <y60/base/Y60xsd.h>

#include <iostream>

#include <asl/base/buildinfo.h>
#include <asl/base/string_functions.h>
#include <asl/dom/Schema.h>
#include <asl/dom/Nodes.h>

namespace y60 {
    SceneBuilder::SceneBuilder(dom::DocumentPtr theDocument) : BuilderBase(SCENE_ROOT_NAME),
        _myDocument(theDocument)
    {
        if (!_myDocument->getSchema()) {
            dom::Document mySchema(y60::ourY60xsd);
            _myDocument->addSchema(mySchema,"");
            _myDocument->setValueFactory(asl::Ptr<dom::ValueFactory>(new dom::ValueFactory()));
            dom::registerStandardTypes(*_myDocument->getValueFactory());
            registerSomTypes(*_myDocument->getValueFactory());
        }

        if (!_myDocument->getFacadeFactory()) {
            dom::FacadeFactoryPtr myFacadeFactory = dom::FacadeFactoryPtr(new dom::FacadeFactory());
            registerSceneFacades(myFacadeFactory);
            //myFacadeFactory->registerPrototype("image", dom::FacadePtr(new Image(dom::Node::Prototype)));
            _myDocument->setFacadeFactory(myFacadeFactory);
        }

        dom::NodePtr mySceneNode = theDocument->childNode(SCENE_ROOT_NAME);
        if (mySceneNode) {
            setNode(mySceneNode);
        } else {
            _myDocument->appendChild(getNode());
#if 0 //TODO: requires schema change
            std::string myRevision = asl::build_information::get().executable().history_id();
            if (myRevision.empty()) {
                myRevision = "0";
            }
#else
            int myRevision = 0;
#endif
            getNode()->appendAttribute(REVISION_ATTRIB, myRevision);
        }

        // Create container nodes
        dom::NodePtr myNode = getNode();
        (*myNode)(WORLD_LIST_NAME);
        (*myNode)(RECORD_LIST_NAME);
        (*myNode)(CANVAS_LIST_NAME);
        (*myNode)(MATERIAL_LIST_NAME);
        (*myNode)(LIGHTSOURCE_LIST_NAME);
        (*myNode)(ANIMATION_LIST_NAME);
        (*myNode)(CHARACTER_LIST_NAME);
        (*myNode)(SHAPE_LIST_NAME);
        (*myNode)(TEXTURE_LIST_NAME);
        (*myNode)(IMAGE_LIST_NAME);

        // create the main viewport
        createDefaultViewport();

    }

    SceneBuilder::~SceneBuilder() {
    }

    void
    SceneBuilder::setInitialCamera(const std::string & theCameraId) {
        if (!getMainViewportNode()->hasFacade()) {
            getMainViewportNode()->appendAttribute(CAMERA_ATTRIB, theCameraId);
        } else {
            getMainViewportNode()->getFacade<Viewport>()->set<CameraTag>(theCameraId);
        }

    }

    const std::string &
    SceneBuilder::appendTexture(TextureBuilder & theTexture) {
        return appendNodeWithId(theTexture, getNode()->childNode(TEXTURE_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendShape(ShapeBuilder & theShape) {
        return appendNodeWithId(theShape, getNode()->childNode(SHAPE_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendCanvas(CanvasBuilder & theCanvas) {
        return appendNodeWithId(theCanvas, getNode()->childNode(CANVAS_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendMaterial(MaterialBuilder & theMaterial) {
        return appendNodeWithId(theMaterial, getNode()->childNode(MATERIAL_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendLightSource(LightSourceBuilder & theLightSource) {
        return appendNodeWithId(theLightSource, getNode()->childNode(LIGHTSOURCE_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendWorld(WorldBuilder & theWorld) {
        return appendNodeWithId(theWorld, getNode()->childNode(WORLD_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendAnimation(AnimationBuilder & theAnimation) {
        return appendNodeWithId(theAnimation, getNode()->childNode(ANIMATION_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendImage(ImageBuilder & theImage) {
        return appendNodeWithId(theImage, getNode()->childNode(IMAGE_LIST_NAME));
    }

    const std::string &
    SceneBuilder::appendMovie(MovieBuilder & theMovie) {
        return appendNodeWithId(theMovie, getNode()->childNode(IMAGE_LIST_NAME));
    }

    dom::NodePtr SceneBuilder::findMovieByFilename(const std::string & theMovieName) {
        dom::NodePtr myMovies = getNode()->childNode(IMAGE_LIST_NAME);
        return myMovies->childNodeByAttribute(MOVIE_NODE_NAME, IMAGE_SRC_ATTRIB, theMovieName);
    }

    dom::NodePtr SceneBuilder::findImageByFilename(const std::string & theImageName) {
        dom::NodePtr myImages = getNode()->childNode(IMAGE_LIST_NAME);
        return myImages->childNodeByAttribute(IMAGE_NODE_NAME, IMAGE_SRC_ATTRIB, theImageName);
    }



    const std::string &
    SceneBuilder::appendCharacter(CharacterBuilder & theCharacter) {
        return appendNodeWithId(theCharacter, getNode()->childNode(CHARACTER_LIST_NAME));
    }

    const std::string &
    SceneBuilder::createDefaultViewport() {
        dom::NodePtr myCanvasList = getNode()->childNode(CANVAS_LIST_NAME);
        dom::NodePtr myCanvas = myCanvasList->childNode(CANVAS_NODE_NAME);
        if (!myCanvas) {
            myCanvas = dom::NodePtr(new dom::Element(CANVAS_NODE_NAME));
            myCanvasList->appendChild(myCanvas);
            if (!myCanvas->hasFacade()) {
                myCanvas->appendAttribute(ID_ATTRIB,IdTag::getDefault());
                myCanvas->appendAttribute(NAME_ATTRIB, "MainCanvas");
            }
        }

        dom::NodePtr myViewport = myCanvas->childNode(VIEWPORT_NODE_NAME);
        if (!myViewport) {
            myViewport = dom::NodePtr(new dom::Element(VIEWPORT_NODE_NAME));
            myCanvas->appendChild(myViewport);

            if (!myViewport->hasFacade()) {
                myViewport->appendAttribute(ID_ATTRIB,IdTag::getDefault());
                myViewport->appendAttribute(ORIENTATION_ATTRIB, LANDSCAPE_ORIENTATION);
                myViewport->appendAttribute(NAME_ATTRIB, "MainViewport");
            }
            dom::NodePtr myOverlayList(new dom::Element(OVERLAY_LIST_NAME));
            myViewport->appendChild(myOverlayList);
        }
        return myViewport->getAttributeString(ID_ATTRIB);
    }

    dom::NodePtr
    SceneBuilder::getMainViewportNode() const {
        return getNode()->childNode(CANVAS_LIST_NAME)->childNode(CANVAS_NODE_NAME)->childNode(VIEWPORT_NODE_NAME);
    }

    void
    SceneBuilder::binarize(asl::WriteableStream & theBlock) const {
        _myDocument->binarize(theBlock);
    }

    std::ostream &
    SceneBuilder::serialize(std::ostream & theOutStream) const {
        return theOutStream << *_myDocument;
    }

}


