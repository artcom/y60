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
//   $Id: SceneBuilder.cpp,v 1.2 2005/04/21 08:59:01 jens Exp $
//   $RCSfile: SceneBuilder.cpp,v $
//   $Author: jens $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 08:59:01 $
//
//
//
//
//=============================================================================

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
#include "WorldBuilder.h"
#include "CameraBuilder.h"
#include "TransformBuilder.h"
#include "Viewport.h"
#include "Facades.h"

#include <y60/CommonTags.h>
#include <y60/Image.h>
#include <y60/NodeNames.h>
#include <y60/Y60xsd.h>

#include <asl/Revision.h>

#include <iostream>

#include <asl/string_functions.h>
#include <dom/Schema.h>
#include <dom/Nodes.h>

namespace y60 {
    SceneBuilder::SceneBuilder(dom::DocumentPtr theDocument) : BuilderBase(SCENE_ROOT_NAME),
        _myDocument(theDocument)
    {
        if (!_myDocument->getSchema()) {
            dom::Document mySchema(ourY60xsd);
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
            getNode()->appendAttribute(REVISION_ATTRIB, asl::ourRevision);
        }

        // Create container nodes
        dom::NodePtr myNode = getNode();
        (*myNode)(WORLD_LIST_NAME);
        (*myNode)(CANVAS_LIST_NAME);
        (*myNode)(MATERIAL_LIST_NAME);
        (*myNode)(LIGHTSOURCE_LIST_NAME);
        (*myNode)(ANIMATION_LIST_NAME);
        (*myNode)(CHARACTER_LIST_NAME);
        (*myNode)(SHAPE_LIST_NAME);
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

    dom::NodePtr SceneBuilder::findImageByFilename_ColorScale_ColorBias(const std::string & theImageName,
                                                                        const asl::Vector4f & theColorScale,
                                                                        const asl::Vector4f & theColorBias)
    {
        dom::NodePtr myImages = getNode()->childNode(IMAGE_LIST_NAME);
        std::vector<dom::NodePtr> myResults;
        myImages->getNodesByAttribute(IMAGE_NODE_NAME, IMAGE_SRC_ATTRIB, theImageName, myResults);
        for(int i = 0; i < myResults.size(); i++) {
            dom::NodePtr myChildNode = myResults[i];
            const asl::Vector4f & myColorScale = myChildNode->getAttributeValue<asl::Vector4f>(IMAGE_COLOR_SCALE_ATTRIB);
            const asl::Vector4f & myColorBias  = myChildNode->getAttributeValue<asl::Vector4f>(IMAGE_COLOR_BIAS_ATTRIB);
            if (asl::almostEqual(myColorScale, theColorScale) &&
                asl::almostEqual(myColorBias, theColorBias) )
            {
                return myChildNode;
            }
        }
        return dom::NodePtr(0);
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


