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

// own header
#include "Scene.h"

#include "IShader.h"
#include "Primitive.h"
#include "Facades.h"
#include "Body.h"
#include "ISceneDecoder.h"
#include "TextureManager.h"
#include "SceneOptimizer.h"

#include <y60/base/NodeNames.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/DataTypes.h>
#include <y60/base/Y60xsd.h>

#include "SceneBuilder.h"
#include "WorldBuilder.h"
#include "CameraBuilder.h"

#include <y60/base/DecoderManager.h>

#include <asl/math/intersection.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>
#include <asl/base/Time.h>
#include <asl/base/Dashboard.h>
#include <asl/base/Logger.h>
#include <asl/base/Stream.h>

#include <asl/dom/Schema.h>
#include <asl/xpath/xpath_api.h>
#include <iostream>
#include <fstream>
#include <set>

using namespace asl;
using namespace std;
using namespace dom;

#define DB(x) //x
#define DONT_USE_MAPPED_BLOCK_IO

namespace y60 {

    Scene::Scene(dom::Node & theNode) :
        Facade(theNode),
        _myTextureManager(TextureManager::create()),
        _myPreviousDomVersion(0)
    {
        // Add the y60 decoder by default
        DecoderManager::get().addDecoder(IDecoderPtr(new Y60Decoder()));
        AC_TRACE << "Scene CTOR for " << this;
    }

    Scene::~Scene() {}

    //////////////////////////////////////////////////////////////////////////////////////
    // Scene saving
    //////////////////////////////////////////////////////////////////////////////////////

    void
    Scene::saveBinary(const std::string & theFilename) {
#ifdef DONT_USE_MAPPED_BLOCK_IO
        asl::Block theBlock;
#else
        asl::WriteableFile theBlock(theFilename);
        if (!theBlock) {
            throw IOError(std::string("Could map the new writable binary file '") + theFilename + "'", "Scene::saveBinary()");
        }
#endif
        _mySceneDom->binarize(theBlock);
#ifdef DONT_USE_MAPPED_BLOCK_IO
        if (!asl::writeFile(theFilename,theBlock)) {
            throw IOError(std::string("Could not open or write binary file '") + theFilename + "'", "Scene::saveBinary()");
        }
#endif
    }
    void
    Scene::saveAsText(const std::string & theFilename, bool theGreedyFlag) {
        std::ofstream myFile(theFilename.c_str(), std::ios_base::trunc | std::ios_base::out);
        if (!myFile) {
            throw OpenFailed(std::string("Can not open file '") + theFilename + "' for writing", "Scene::saveAsText()");
        }
        _mySceneDom->print(myFile, "", theGreedyFlag);
        if (!myFile) {
            throw WriteFailed(std::string("Could not write text file '") + theFilename + "'", "Scene::saveAsText()");
        }
    }

   void
    Scene::saveBinaryWithCatalog(const std::string & theFilename, const std::string & theCatalogFilenname) {
        if (theFilename == theCatalogFilenname) {
#ifdef DONT_USE_MAPPED_BLOCK_IO
            asl::Block theBlock;
#else
            asl::WriteableFile theBlock(theFilename);
            if (!theBlock) {
                throw IOError(std::string("Could map the new writable binary file '") + theFilename + "'", "Scene::saveBinary()");
            }
#endif
            _mySceneDom->binarize(theBlock, theBlock);
#ifdef DONT_USE_MAPPED_BLOCK_IO
            if (!asl::writeFile(theFilename,theBlock)) {
                throw IOError(std::string("Could not open or write binary file '") + theFilename + "'", "Scene::saveBinary()");
            }
#endif
        } else {
#ifdef DONT_USE_MAPPED_BLOCK_IO
            asl::Block theBlock;
            asl::Block theCatalogBlock;
#else
            asl::WriteableFile theBlock(theFilename);
            if (!theBlock) {
                throw IOError(std::string("Could map the new writable binary file '") + theFilename + "'", "Scene::saveBinary()");
            }
            asl::WriteableFile theCatalogBlock(theCatalogFilenname);
            if (!theCatalogBlock) {
                throw IOError(std::string("Could map the new writable binary catalog file '") + theCatalogFilenname + "'", "Scene::saveBinary()");
            }
#endif
            _mySceneDom->binarize(theBlock, theCatalogBlock);
#ifdef DONT_USE_MAPPED_BLOCK_IO
            if (!asl::writeFile(theFilename,theBlock)) {
                throw IOError(std::string("Could not open or write binary file '") + theFilename + "'", "Scene::saveBinary()");
            }
            if (!asl::writeFile(theFilename,theCatalogBlock)) {
                throw IOError(std::string("Could not open or write binary catalog file '") + theCatalogFilenname + "'", "Scene::saveBinary()");
            }
#endif
         }
    }

    void
    Scene::save(const std::string & theFilename, bool theBinaryFlag, bool theGreedyFlag) {
        AC_INFO << "Saving scene file '" << theFilename << "' ... ";
        asl::Time saveStart;
        if (theBinaryFlag) {
            saveBinary(theFilename);
        } else {
            saveAsText(theFilename, theGreedyFlag);
        }
        asl::Time saveEnd;
        double mySaveTime = saveEnd - saveStart;
        unsigned long myFileSize = asl::getFileSize(theFilename);
        AC_INFO << "Saved File Size is " << myFileSize << " bytes";
        AC_INFO << "Save time " << mySaveTime << " sec, " << long(myFileSize/mySaveTime/1024)<<" kb/sec";
    }

    void
    Scene::saveWithCatalog(const std::string & theFilename, const std::string & theCatalogFilenname, bool theBinaryFlag) {
        AC_INFO << "Saving scene file with '" << theFilename << "' with catalog '"<<theCatalogFilenname<<"'... ";
        asl::Time saveStart;
        if (theBinaryFlag) {
            saveBinaryWithCatalog(theFilename,theCatalogFilenname);
        } else {
            AC_ERROR << "non-binary saving with catalog not yet implemented";
        }
        asl::Time saveEnd;
        double mySaveTime = saveEnd - saveStart;
        unsigned long myFileSize = asl::getFileSize(theFilename);
        AC_INFO << "Saved File Size is " << myFileSize << " bytes";
        AC_INFO << "Save time " << mySaveTime << " sec, " << long(myFileSize/mySaveTime/1024)<<" kb/sec";
    }


    void
    Scene::saveSchema(const std::string & theFilename, int theSchemaIndex, bool theBinaryFlag) {
        AC_INFO << "Saveing schema file '" << theFilename << "' ... ";
        std::ofstream myFile(theFilename.c_str());
        if (!myFile) {
            throw OpenFailed(std::string("Can not open file '") + theFilename + "' for writing", "Scene::saveSchema()");
        }
        if (theBinaryFlag) {
            asl::Block theBlock;
            _mySceneDom->getSchema()->childNode(theSchemaIndex)->binarize(theBlock);
            myFile.write(theBlock.strbegin(),theBlock.size());
        } else {
            myFile << *_mySceneDom->getSchema()->childNode(theSchemaIndex);
        }
        if (!myFile) {
            throw WriteFailed(std::string("Could not write file '") + theFilename + "' for writing", "Scene::saveSchema()");
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Scene loading and updating
    //////////////////////////////////////////////////////////////////////////////////////

    void
    Scene::clear() {
        _mySceneDom = dom::DocumentPtr(new dom::Document());
        _myPreviousDomVersion = 0;
    }

    void
    Scene::setupEmptyDocument(dom::Document & theDocument, const std::string & theSchemaFilename) {
        theDocument = Document();
        asl::Ptr<ValueFactory> myFactory = asl::Ptr<ValueFactory>(new ValueFactory());
        registerStandardTypes(*myFactory);
        registerSomTypes(*myFactory);
        theDocument.setValueFactory(myFactory);
        FacadeFactoryPtr myFacadeFactory = FacadeFactoryPtr(new dom::FacadeFactory());
        registerSceneFacades(myFacadeFactory);
        theDocument.setFacadeFactory(myFacadeFactory);
        if (theSchemaFilename.size()) {
            std::string myFile;
            if (asl::readFile(theSchemaFilename, myFile)) {
                Document mySchema(myFile);
                theDocument.addSchema(mySchema,"");
            } else {
                throw IOError(std::string("Can not open or read schema file '") + theSchemaFilename + "'", PLUS_FILE_LINE);
            }
        }
        if (!theDocument.getSchema() || !theDocument.getSchema()->childNodes().length()) {
            Document mySchema(y60::ourY60xsd);
            theDocument.addSchema(mySchema,"");
        }
    }

    ScenePtr
    Scene::load(const std::string & theFilename, PackageManagerPtr thePackageManager,
            const IProgressNotifierPtr & theNotifier,
            bool useSchema, bool loadLazy)
    {
        //_myTextureManager->setPackageManager(thePackageManager);
        if (!theFilename.empty() && theFilename != "null") {
            asl::Ptr<ReadableStreamHandle> mySource = thePackageManager->readStream(theFilename);
            if (!mySource) {
                throw IOError(std::string("Can not open or read scene file '") + theFilename + "'", PLUS_FILE_LINE);
            }
            try {
                return load(mySource, theFilename, theNotifier, useSchema, loadLazy);
            } catch (asl::Exception & ex) {
                ex.appendWhat(string("while opening '")+theFilename+"'");
                throw ex;
            }
        }
        return load(asl::Ptr<asl::ReadableStreamHandle>(), "empty scene", theNotifier, useSchema, loadLazy);
    }

    ScenePtr
    Scene::load(asl::Ptr<asl::ReadableStreamHandle> theSource, const std::string & theFilename,
               const IProgressNotifierPtr & theNotifier,
               bool useSchema, bool loadLazy)
    {

        AC_DEBUG << "Scene::load: loadLazy="<<loadLazy;
        asl::Time loadStart;

        dom::DocumentPtr mySceneDom(new Document());
        if (useSchema) {
            setupEmptyDocument(*mySceneDom, "");
        }

        if (!DecoderManager::get().findDecoder<ISceneDecoder>("dummy.x60", asl::Ptr<asl::ReadableStreamHandle>())) {
            DecoderManager::get().addDecoder(IDecoderPtr(new Y60Decoder()));
        }

        asl::Ptr<ISceneDecoder> myDecoder =
            DecoderManager::get().findDecoder<ISceneDecoder>(theFilename, theSource);
        if (myDecoder) {
            myDecoder->setLazy(loadLazy);
            myDecoder->setProgressNotifier(theNotifier);
            myDecoder->decodeScene(theSource, mySceneDom);
            myDecoder->setProgressNotifier(IProgressNotifierPtr());
        } else {
            throw SceneException(std::string("Could not determine file type of '") +theFilename + "' (bad magic number)", PLUS_FILE_LINE);
        }
        //_myPreviousDomVersion = 0;
        //_mySceneBuilder = SceneBuilderPtr(new SceneBuilder(_mySceneDom));
        //getNode() = _mySceneDom->childNode(SCENE_ROOT_NAME);

        asl::Time loadEnd;
        AC_INFO << "  Total load time: " << (loadEnd - loadStart) << " sec";

        ScenePtr myNewScene(mySceneDom->firstChild()->getFacade<Scene>());
        myNewScene->setSceneDom(mySceneDom);
        myNewScene->setSceneBuilder(SceneBuilderPtr(new SceneBuilder(mySceneDom)));
        return myNewScene;
    }

    void
    Scene::setSceneDom(dom::DocumentPtr theDocument) {
        _mySceneDom = theDocument;
    }

    ScenePtr
    Scene::createStubs(asl::PackageManagerPtr thePackageManager) {
        dom::DocumentPtr mySceneDom(new Document());
        setupEmptyDocument(*mySceneDom, "");
        SceneBuilderPtr mySceneBuilder(SceneBuilderPtr(new SceneBuilder(mySceneDom)));
        //getNode() = _mySceneDom->childNode(SCENE_ROOT_NAME);

        WorldBuilder myWorldBuilder;
        mySceneBuilder->appendWorld(myWorldBuilder);

        CameraBuilder myCameraBuilder("myCamera");
        const std::string & myCameraId = myWorldBuilder.appendObject(myCameraBuilder);

        mySceneBuilder->getMainViewportNode()->getFacade<Viewport>()->set<CameraTag>(myCameraId);
        ScenePtr myNewScene = mySceneDom->firstChild()->getFacade<Scene>();
        myNewScene->setSceneDom(mySceneDom);
        myNewScene->setup();
        myNewScene->setSceneBuilder(mySceneBuilder);
        return myNewScene;
    }

    IShaderLibraryPtr
    Scene::getShaderLibrary() const {
        if (_myTextureManager->getResourceManager()) {
            return _myTextureManager->getResourceManager()->getShaderLibrary();
        }
        return IShaderLibraryPtr();
    }

    void
    Scene::setupShaderLibrary() {
        if (_myTextureManager->getResourceManager()) {
            if (!_myTextureManager->getResourceManager()->getShaderLibrary()) {
                // If no shaderlibrary has been set, yet, add a default library.
                //_myTextureManager->getResourceManager()->loadShaderLibrary();
            }
        } else {
            AC_WARNING << "Scene::setupShaderLibrary(): No resource manager found.";
        }
    }

    void
    Scene::setup() {
        asl::Time setupStart;
        AC_INFO << "Loading textures...";
        _myTextureManager->setTextureList(getTexturesRoot());

        setupShaderLibrary();
        AC_INFO << "Loading world...";
        update(ANIMATIONS_LOAD|SHAPES|WORLD);
        asl::Time setupEnd;
        AC_INFO << "Total setup time: " << (setupEnd-setupStart) << " sec";
    }

    void
    Scene::optimize(dom::NodePtr theRootNode) {
        SceneOptimizer myOptimizer(*this);
        if (theRootNode) {
            myOptimizer.run(theRootNode);
        } else {
            myOptimizer.run();
        }
        update(SHAPES|WORLD);
    }

    void
    Scene::loadAnimations(NodePtr theWorld) {
        NodePtr myAnimationNodes = getNode().childNode(ANIMATION_LIST_NAME);
        _myAnimationManager.loadGlobals(myAnimationNodes, theWorld);

        NodePtr myCharacterNodes = getNode().childNode(CHARACTER_LIST_NAME);
        _myAnimationManager.loadCharacters(myCharacterNodes, theWorld);
    }

    void
    Scene::updateAllModified() {
        //AC_WARNING << "updateAllModified() disabled by pavel";
        //return;

        dom::Node & mySceneElement = getNode();
        //printChangedNodes(mySceneElement, _myPreviousDomVersion, 0);
        dom::NodePtr myWorldListElement = mySceneElement.childNode(WORLD_LIST_NAME);
        dom::NodePtr myWorldElement = myWorldListElement ? myWorldListElement->childNode(WORLD_NODE_NAME) : dom::NodePtr();
        dom::NodePtr myLightsElement = mySceneElement.childNode(LIGHTSOURCE_LIST_NAME);
        dom::NodePtr myShapesElement = mySceneElement.childNode(SHAPE_LIST_NAME);
        dom::NodePtr myCanvasesElement = mySceneElement.childNode(CANVAS_LIST_NAME);
        dom::NodePtr myMaterialsElement = mySceneElement.childNode(MATERIAL_LIST_NAME);
        dom::NodePtr myAnimationsElement = mySceneElement.childNode(ANIMATION_LIST_NAME);

        short myUpdateFlags = 0;
        if (myMaterialsElement->nodeVersion() > _myPreviousDomVersion) {
            myUpdateFlags|=Scene::MATERIALS;
            DB(AC_TRACE << " MATERIALS");
        }
        if (myShapesElement->nodeVersion() > _myPreviousDomVersion) {
            myUpdateFlags|=Scene::SHAPES;
            DB(AC_TRACE << " SHAPES");
        }
        if (myWorldElement->nodeVersion() > _myPreviousDomVersion) {
            myUpdateFlags|=Scene::WORLD;
            DB(AC_TRACE << " WORLD");
        }
        if (myCanvasesElement->nodeVersion() > _myPreviousDomVersion) {
            DB(AC_TRACE << " CANVASES");
        }
        if (myAnimationsElement->nodeVersion() > _myPreviousDomVersion) {
            myUpdateFlags|=Scene::ANIMATIONS;
            DB(AC_TRACE << " ANIMATIONS");
        }
        update(myUpdateFlags);

        _myPreviousDomVersion = _mySceneDom->nodeVersion();
    }

    void
    Scene::update(unsigned short theUpdateFlags) {
        //AC_WARNING << "update() disabled by pavel";
        //return;
        if (theUpdateFlags & ANIMATIONS_LOAD) {
            AC_TRACE << "updateAnimationsReload";
            MAKE_SCOPE_TIMER(updateAnimationsReload);
            NodePtr myWorlds = getWorldsRoot();
            for(unsigned i = 0; i < myWorlds->childNodesLength("world"); i++) {
                loadAnimations(myWorlds->childNode("world", i));
            }
        } else if (theUpdateFlags & ANIMATIONS) {
            AC_TRACE << "updateAnimations";
            MAKE_SCOPE_TIMER(updateAnimations);
            _myAnimationManager.update();
        }
        if (theUpdateFlags & WORLD) {
            AC_TRACE << "updateWorld";
            MAKE_SCOPE_TIMER(updateWorld);
            Matrix4f myInitialMatrix;
            myInitialMatrix.makeIdentity();

            NodePtr myWorlds = getWorldsRoot();
            for(unsigned i = 0; i < myWorlds->childNodesLength("world"); i++) {
                NodePtr myWorldNode = myWorlds->childNode("world", i);
                WorldPtr myWorldFacade = myWorldNode->getFacade<World>();
                myWorldFacade->updateLights();
                updateTransformHierachy(myWorldNode);
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Scene traversal
    //////////////////////////////////////////////////////////////////////////////////////

    NodePtr
    Scene::getTexturesRoot() {
        return getNode().childNode(TEXTURE_LIST_NAME);
    }

    const NodePtr
    Scene::getTexturesRoot() const {
        return getNode().childNode(TEXTURE_LIST_NAME);
    }

    NodePtr
    Scene::getImagesRoot() {
        return getNode().childNode(IMAGE_LIST_NAME);
    }

    const NodePtr
    Scene::getImagesRoot() const {
        return getNode().childNode(IMAGE_LIST_NAME);
    }

    NodePtr
    Scene::getWorldsRoot() {
        return getNode().childNode(WORLD_LIST_NAME);
    }

    const NodePtr
    Scene::getWorldsRoot() const {
        return getNode().childNode(WORLD_LIST_NAME);
    }

    NodePtr
    Scene::getWorldRoot() {
        //AC_WARNING << "getWorldRoot is deprecated";
        return getNode().childNode(WORLD_LIST_NAME)->childNode(WORLD_NODE_NAME);
    }

    const NodePtr
    Scene::getWorldRoot() const {
        //AC_WARNING << "getWorldRoot is deprecated";
        return getNode().childNode(WORLD_LIST_NAME)->childNode(WORLD_NODE_NAME);
    }

    NodePtr
    Scene::getShapesRoot() {
        return getNode().childNode(SHAPE_LIST_NAME);
    }

    const NodePtr
    Scene::getShapesRoot() const {
        return getNode().childNode(SHAPE_LIST_NAME);
    }

    NodePtr
    Scene::getCanvasRoot() {
        return getNode().childNode(CANVAS_LIST_NAME);
    }

    const NodePtr
    Scene::getCanvasRoot() const {
        return getNode().childNode(CANVAS_LIST_NAME);
    }

    NodePtr
    Scene::getRecordsRoot() {
        return getNode().childNode(RECORD_LIST_NAME);
    }

    const NodePtr
    Scene::getRecordsRoot() const {
        return getNode().childNode(RECORD_LIST_NAME);
    }

    NodePtr
    Scene::getMaterialsRoot() {
        return getNode().childNode(MATERIAL_LIST_NAME);
    }

    const NodePtr
    Scene::getMaterialsRoot() const {
        return getNode().childNode(MATERIAL_LIST_NAME);
    }

    float
    Scene::getWorldSize(const NodePtr & theActiveCamera) const {
        AC_TRACE << "getWorldSize()";
        CameraPtr myCameraFacade = theActiveCamera->getFacade<Camera>();
        WorldPtr myWorldFacade = myCameraFacade->getWorld()->getFacade<World>();
        asl::Box3f myWorldBox = myWorldFacade->get<BoundingBoxTag>();

        asl::Matrix4f myCameraMatrix = myCameraFacade->get<GlobalMatrixTag>();
        asl::Vector3f myCameraPos = myCameraMatrix.getTranslation();
        myWorldBox.extendBy(myCameraPos);
        return asl::distance(myWorldBox[0], myWorldBox[1]);
    }
    void
    Scene::updateTransformHierachy(NodePtr theNode) {

        // TODO: This should be possibly done in a lazy fashion during rendering
        std::vector<IncludeFacadePtr> myIncludes = theNode->getAllFacades<IncludeFacade>(INCLUDE_NODE_NAME);
        AC_TRACE << "includes = "<< myIncludes.size();
        for (unsigned i = 0; i < myIncludes.size();++i) {
            AC_TRACE << "includes["<<i<<"] = "<< myIncludes[i]->getNode();
            IncludeFacadePtr & myInclude = myIncludes[i];
            Node & myIncludeNode = myInclude->getNode();

            // Check if src has a new revision
            if (!myInclude->isUpToDate()) {
                myInclude->setUpToDate();
                if (myIncludeNode.childNodesLength()) {
                    while (myIncludeNode.childNodesLength()) {
                        myIncludeNode.removeChild(myIncludeNode.firstChild());
                    }
                    collectGarbage();
                }
                if (myInclude->get<IncludeSrcTag>() != "") {
                    import(myInclude->get<IncludeSrcTag>(), AppPackageManager::get().getPtr(), myIncludeNode.self().lock());
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Camera handling
    ///////////////////////////////////////////////////////////////////////////////////////////

    void
    Scene::collectCameras(NodeList & theCameras) const {
        NodePtr myStartNode = getNode().childNode(WORLD_LIST_NAME)->childNode(WORLD_NODE_NAME);
        collectCameras(theCameras, myStartNode);
    }

    void
    Scene::collectCameras(NodeList & theCameras, NodePtr theNode) const {
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChildNode = theNode->childNode(i);
            if (myChildNode->nodeName() == CAMERA_NODE_NAME) {
                theCameras.append(myChildNode);
            }
            collectCameras(theCameras, myChildNode);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    // Picking 
    ///////////////////////////////////////////////////////////////////////////////////////////
    
    dom::NodePtr
    Scene::pickBody(const unsigned int theX, const unsigned int theY, const dom::NodePtr theCanvas) const {
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        ViewportPtr myViewport = myCanvas->getViewportAt(theX, theY);
        if (myViewport) {
            return myViewport->pickBody(theX, theY);
        } else {
            return dom::NodePtr(0);
        }
    }

    dom::NodePtr 
    Scene::pickBody(const unsigned int theX, const unsigned int theY) const {
        const dom::NodePtr myCanvas = getCanvasRoot()->childNode(CANVAS_NODE_NAME); 
        return pickBody(theX, theY, myCanvas);
    }

    dom::NodePtr 
    Scene::pickBodyBySweepingSphereFromBodies(const unsigned int theX,
                    const unsigned int theY,
                    const float theSphereRadius,
                    const dom::NodePtr theCanvas) const
    {
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        ViewportPtr myViewport = myCanvas->getViewportAt(theX, theY);
        if (myViewport) {
            return myViewport->pickBodyBySweepingSphereFromBodies(theX, theY, theSphereRadius);
        } else {
            return NodePtr(0);
        }
    }

    dom::NodePtr 
    Scene::pickBodyBySweepingSphereFromBodies(const unsigned int theX,
                            const unsigned int theY,
                            const float theSphereRadius) const
    {
        const dom::NodePtr myCanvas = getCanvasRoot()->childNode(CANVAS_NODE_NAME); 
        return pickBodyBySweepingSphereFromBodies(theX, theY, theSphereRadius, myCanvas);
    }

    void
    Scene::getPickedBodyInformation(const unsigned int theX,
                             const unsigned int theY,
                             y60::IntersectionInfo & theInformation,
                             const dom::NodePtr theCanvas) const {
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        ViewportPtr myViewport = myCanvas->getViewportAt(theX, theY);
        if (myViewport) {
            myViewport->getPickedBodyInformation(theX, theY, theInformation);
        }
    }

    void
    Scene::getPickedBodyInformation(const unsigned int theX,
                             const unsigned int theY,
                             y60::IntersectionInfo & theInformation) const {
        const dom::NodePtr myCanvas = getCanvasRoot()->childNode(CANVAS_NODE_NAME);
        getPickedBodyInformation(theX, theY, theInformation, myCanvas);
    }

    void
    Scene::getPickedBodiesInformation(const unsigned int theX,
                             const unsigned int theY,
                             y60::IntersectionInfoVector & theInformation,
                             const dom::NodePtr theCanvas) const {
        CanvasPtr myCanvas = theCanvas->getFacade<Canvas>();
        ViewportPtr myViewport = myCanvas->getViewportAt(theX, theY);
        if (myViewport) {
            myViewport->getPickedBodiesInformation(theX, theY, theInformation);
        }
    }

    void
    Scene::getPickedBodiesInformation(const unsigned int theX,
                             const unsigned int theY,
                             y60::IntersectionInfoVector & theInformation) const {
        const dom::NodePtr myCanvas = getCanvasRoot()->childNode(CANVAS_NODE_NAME);
        getPickedBodiesInformation(theX, theY, theInformation, myCanvas);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    // Intersection / Collision detection
    ///////////////////////////////////////////////////////////////////////////////////////////

    template <class PROBE>
    struct IntersectBodyVisitor {
        const PROBE & _myStick;
        IntersectionInfoVector & _myList;

        IntersectBodyVisitor(const PROBE & theStick, IntersectionInfoVector & theList) :
            _myStick(theStick), _myList(theList) {
        }
        virtual ~IntersectBodyVisitor() {}

        virtual bool hitBoundingBox(const asl::Box3f & myBoundingBox, bool isInsensible, bool isLeafNode) {
            return (!isInsensible && asl::intersection(_myStick, myBoundingBox));
        }

        bool visit(NodePtr theNode, y60::ShapePtr theShape, const asl::Matrix4f & theTransformation) {
            asl::Matrix4f myInverseTransformation = asl::inverse(theTransformation);
            const PROBE myShapeSpaceStick = _myStick * myInverseTransformation;
            // supply a vector to store the intersecting primitives
            asl::Ptr<Primitive::IntersectionList> myPrimitiveIntersections =
                asl::Ptr<Primitive::IntersectionList>(new Primitive::IntersectionList);

            // try shape
            if (theShape->intersect(myShapeSpaceStick, *myPrimitiveIntersections)) {
                // store intersection information
                _myList.push_back(y60::IntersectionInfo( theNode, theShape, theTransformation,
                        myInverseTransformation, myPrimitiveIntersections));
                return true;
            }
            return false;
        }

    };

    // By now we only check intersection on bb level (VS)
    // TODO extend the box intersection visit by getting real contact with the primitives
    template<>
    struct IntersectBodyVisitor<class asl::Box3<float> > {
        const asl::Box3<float> & _myBox;
        IntersectionInfoVector & _myList;

        IntersectBodyVisitor(const asl::Box3<float> & theBox, IntersectionInfoVector & theList) :
            _myBox(theBox), _myList(theList)
        {}
        virtual ~IntersectBodyVisitor() {}

        virtual bool hitBoundingBox(const asl::Box3f & myBoundingBox, bool isInsensible, bool isLeafNode) {
            return (!isInsensible && _myBox.touches(myBoundingBox));
        }

        bool visit(NodePtr theNode, y60::ShapePtr theShape, const asl::Matrix4f & theTransformation) {
            // will only be called if the bb test suceeds, so just add the stuff to the intersection info pool
            asl::Matrix4f myInverseTransformation = asl::inverse(theTransformation);
            asl::Ptr<Primitive::IntersectionList> myPrimitiveIntersections =
                asl::Ptr<Primitive::IntersectionList>(new Primitive::IntersectionList);


            _myList.push_back(y60::IntersectionInfo( theNode, theShape, theTransformation,
                        myInverseTransformation, myPrimitiveIntersections));

            return true;
        }
    };

    struct IntersectBoundingBoxCenterVisitor : public IntersectBodyVisitor< asl::Box3<float> > {
        IntersectBoundingBoxCenterVisitor(const asl::Box3<float> & theBox, IntersectionInfoVector & theList) :
            IntersectBodyVisitor<asl::Box3<float> >( theBox, theList )
        { }

        virtual bool hitBoundingBox(const asl::Box3f & myBoundingBox, bool isInsensible, bool isLeafNode) {
            //return (!isInsensible && _myBox.contains( myBoundingBox.getCenter()));
            if ( isLeafNode ) {
                bool myContainsFlag = _myBox.contains( myBoundingBox.getCenter());
                return (!isInsensible && myContainsFlag);
            } else {
                return (!isInsensible && _myBox.touches(myBoundingBox));
            }
        }
    };

    bool
    Scene::intersectWorld(const asl::LineSegment<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_LineSegment);
        IntersectBodyVisitor<asl::LineSegment<float> > myVisitor(theStick, theIntersections);
        return visitBodys(myVisitor, getWorldRoot(), theIntersectInvisibleBodysFlag );
    }
    bool
    Scene::intersectWorld(const Ray<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Ray);
        IntersectBodyVisitor<Ray<float> > myVisitor(theStick, theIntersections);
        return visitBodys(myVisitor, getWorldRoot(), theIntersectInvisibleBodysFlag);
    }
    bool
    Scene::intersectWorld(const asl::Line<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Line);
        IntersectBodyVisitor<asl::Line<float> > myVisitor (theStick, theIntersections);
        return visitBodys(myVisitor, getWorldRoot() , theIntersectInvisibleBodysFlag);
    }

    bool
    Scene::intersectBodyCenters(dom::NodePtr theRootNode, const asl::Box3<float> & theBox,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Box3f_center);
        IntersectBoundingBoxCenterVisitor myVisitor(theBox, theIntersections);
        return visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag);
    }
    bool
    Scene::intersectBodies(dom::NodePtr theRootNode, const asl::Box3<float> & theBox,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Box3f);
        IntersectBodyVisitor<asl::Box3<float> > myVisitor(theBox, theIntersections);
        return visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag);
    }
    bool
    Scene::intersectBodies(dom::NodePtr theRootNode, const asl::LineSegment<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_LineSegment);
        IntersectBodyVisitor<asl::LineSegment<float> > myVisitor(theStick, theIntersections);
        return visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag);
    }
    bool
    Scene::intersectBodies(dom::NodePtr theRootNode, const Ray<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Ray);
        IntersectBodyVisitor<Ray<float> > myVisitor(theStick, theIntersections);
        return visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag);
    }
    bool
    Scene::intersectBodies(dom::NodePtr theRootNode, const asl::Line<float> & theStick,
                          IntersectionInfoVector & theIntersections,
                          bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(Scene_intersect_Line);
        IntersectBodyVisitor<asl::Line<float> > myVisitor (theStick, theIntersections);
        return visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag );
    }
    template <class VISITOR>
    bool
    Scene::visitBodys(VISITOR & theVisitor,
                      NodePtr theNode,
                      bool theIntersectInvisibleBodysFlag)
    {
        bool myResult = false;

        TransformHierarchyFacadePtr myTransform = theNode->Node::tryGetFacade<y60::TransformHierarchyFacade>();
        if (!myTransform) {
            return myResult;
        }
        bool isInsensible = myTransform->TransformHierarchyFacade::get<y60::InsensibleTag>();
        bool isVisibleVisitFlag = theIntersectInvisibleBodysFlag || myTransform->TransformHierarchyFacade::get<y60::VisibleTag>();
        Box3f myBoundingBox = myTransform->TransformHierarchyFacade::get<BoundingBoxTag>();
        bool isLeafNode = theNode->childNodesLength() == 0;
        if (isVisibleVisitFlag && theVisitor.hitBoundingBox(myBoundingBox, isInsensible, isLeafNode) ) {
            // check bounding box hierarchy first
            if (theNode->nodeName() == BODY_NODE_NAME) {
                // prepare check for intersection with body geometry
                y60::ShapePtr myShape;
                dom::NodePtr myShapeNode = theNode->getElementById(theNode->Node::getFacade<Body>()->Body::get<ShapeTag>());
                if (myShapeNode) {
                    myShape = myShapeNode->Node::getFacade<Shape>();
                }
                // we need the inverse global matrix to transform the stick into shape space
                //const asl::Matrix4f & myTransformation = theNode->Node::getAttributeValue<asl::Matrix4f>(GLOBAL_MATRIX_ATTRIB);
                const asl::Matrix4f & myTransformation = myTransform->TransformHierarchyFacade::get<y60::GlobalMatrixTag>();

                if (theVisitor.visit(theNode, myShape, myTransformation)) {
                    myResult = true;
                }

            }
            unsigned myChildCount = theNode->childNodesLength();
            for (unsigned i = 0; i < myChildCount; ++i) {
                if (visitBodys(theVisitor, theNode->childNode(i), theIntersectInvisibleBodysFlag)) {
                    myResult = true;
                }
            }
        }
        return myResult;
    }

    struct SweepSphereAllContactsVisitor {
        const asl::Sphere<float> & _mySphere;
        const Vector3<float> & _myMotion;
        CollisionInfoVector & _myList;

        SweepSphereAllContactsVisitor(const asl::Sphere<float> & theSphere, const Vector3f & theMotion,
            CollisionInfoVector & theList) :
            _mySphere(theSphere), _myMotion(theMotion), _myList(theList) {
        }

        bool hitBoundingBox(const asl::Box3f & myBoundingBox, bool isInsensible, bool isLeafNode) {
            if (isInsensible) {
                return false;
            }
            // TODO: Capsule / Box intersection.
            // we approximate the capsule with a box for now
            Point3f myEndCenter = _mySphere.center+_myMotion;
            float minX = minimum(_mySphere.center[0], myEndCenter[0]) - _mySphere.radius;
            float minY = minimum(_mySphere.center[1], myEndCenter[1]) - _mySphere.radius;
            float minZ = minimum(_mySphere.center[2], myEndCenter[2]) - _mySphere.radius;
            float maxX = maximum(_mySphere.center[0], myEndCenter[0]) + _mySphere.radius;
            float maxY = maximum(_mySphere.center[1], myEndCenter[1]) + _mySphere.radius;
            float maxZ = maximum(_mySphere.center[2], myEndCenter[2]) + _mySphere.radius;
            Box3f myCapsuleBox(Point3f(minX, minY, minZ), Point3f(maxX, maxY, maxZ));
            return myCapsuleBox.touches(myBoundingBox);
        }

        bool visit(NodePtr theNode, y60::ShapePtr theShape, const asl::Matrix4f & theTransformation) {
            // supply a vector to store the collisions
            asl::Ptr<Primitive::SphereContactsList> myContacts =
                asl::Ptr<Primitive::SphereContactsList>(new Primitive::SphereContactsList);

            // try shape
            if (theShape->collide(_mySphere, _myMotion, theTransformation, *myContacts)) {
                // store collision information
                _myList.push_back(y60::CollisionInfo());
                y60::CollisionInfo & myCollision = _myList.back();
                myCollision._myBody = theNode;
                myCollision._myShape = theShape;
                myCollision._myTransformation = theTransformation;
                myCollision._myInverseTransformation = asl::inverse(theTransformation);
                myCollision._myPrimitiveSphereContacts = myContacts;
                return true;
            }
            return false;
        }
    };

    struct SweepSphereFirstContactVisitor {
        const asl::Sphere<float> & _mySphere;
        const Vector3<float> & _myMotion;
        CollisionInfo & _myCollision;
        asl::SweptSphereContact<float> _myFirstContact;

        SweepSphereFirstContactVisitor(const asl::Sphere<float> & theSphere, const Vector3f & theMotion,
            CollisionInfo & theCollision) :
            _mySphere(theSphere), _myMotion(theMotion), _myCollision(theCollision) {
        }

        bool hitBoundingBox(const asl::Box3f & myBoundingBox, bool isInsensible, bool isLeafNode) {
            // TODO: Capsule / Box intersection.
            // we approximate the capsule with a box for now
            Point3f myEndCenter = _mySphere.center+_myMotion;
            float minX = minimum(_mySphere.center[0], myEndCenter[0]) - _mySphere.radius;
            float minY = minimum(_mySphere.center[1], myEndCenter[1]) - _mySphere.radius;
            float minZ = minimum(_mySphere.center[2], myEndCenter[2]) - _mySphere.radius;
            float maxX = maximum(_mySphere.center[0], myEndCenter[0]) + _mySphere.radius;
            float maxY = maximum(_mySphere.center[1], myEndCenter[1]) + _mySphere.radius;
            float maxZ = maximum(_mySphere.center[2], myEndCenter[2]) + _mySphere.radius;
            Box3f myCapsuleBox(Point3f(minX, minY, minZ), Point3f(maxX, maxY, maxZ));
            bool hitBox = !isInsensible && myCapsuleBox.intersects(myBoundingBox);
            return hitBox;
        }

        bool visit(NodePtr theNode, y60::ShapePtr theShape, const asl::Matrix4f & theTransformation) {
            asl::Ptr<Primitive::SphereContactsList> myContact =
                asl::Ptr<Primitive::SphereContactsList>(new Primitive::SphereContactsList(1));

            // try shape
            if (theShape->collide(_mySphere, _myMotion, theTransformation, _myFirstContact, (*myContact)[0]))  {
                // store collision information
                _myCollision._myBody = theNode;
                _myCollision._myShape = theShape;
                _myCollision._myTransformation = theTransformation;
                _myCollision._myInverseTransformation = asl::inverse(theTransformation);
                _myCollision._myPrimitiveSphereContacts = myContact;
                _myFirstContact = (*myContact)[0]._myMinContact;
                return true;
            }
            return false;
        }
    };

    bool
    Scene::collideWithWorld(const asl::Sphere<float> & theSphere,
                                const Vector3<float> & theMotion,
                                CollisionInfoVector & theCollisions,
                                bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(collideWithWorld);
        return collideWithBodies(getWorldRoot(), theSphere, theMotion, theCollisions, theIntersectInvisibleBodysFlag);
    }

    bool
    Scene::collideWithWorld(const asl::Sphere<float> & theSphere,
                                const Vector3<float> & theMotion,
                                CollisionInfo & theCollision,
                                bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(collideWithWorldFirstOnly);
        return collideWithBodies( getWorldRoot(), theSphere, theMotion, theCollision, theIntersectInvisibleBodysFlag);
    }

    bool
    Scene::collideWithBodies(NodePtr theRootNode,
                             const asl::Sphere<float> & theSphere,
                             const Vector3<float> & theMotion,
                             CollisionInfoVector & theCollisions,
                            bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(collideWithBodies);
        SweepSphereAllContactsVisitor myVisitor(theSphere, theMotion, theCollisions);
        bool rc = visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag);
        return rc;
    }

    bool
    Scene::collideWithBodies(NodePtr theRootNode, const asl::Sphere<float> & theSphere,
                                const Vector3<float> & theMotion,
                                CollisionInfo & theCollision,
                                bool theIntersectInvisibleBodysFlag)
    {
        MAKE_SCOPE_TIMER(collideWithWorldFirstOnly);
        SweepSphereFirstContactVisitor myVisitor(theSphere, theMotion, theCollision);
        bool rc = visitBodys(myVisitor, theRootNode, theIntersectInvisibleBodysFlag );
        return rc;
    }

    void
    Scene::import(const std::string & theFilename, asl::PackageManagerPtr thePackageManager,
                  dom::NodePtr theRoot /*= dom::NodePtr(0)*/)
    {
        AC_INFO << "Importing scene: " << theFilename;
        if (!theRoot) {
            theRoot = getWorldRoot();
        }

        asl::Ptr<ReadableStreamHandle> mySource = thePackageManager->readStream(theFilename);
        if (!mySource) {
            AC_ERROR << "Could not open include file '" << theFilename + "'. So I will ignore it.";
            return;
        }

        DocumentPtr mySceneDom(new dom::Document());
        setupEmptyDocument(*mySceneDom, "");
        asl::Ptr<ISceneDecoder> myDecoder =
            DecoderManager::get().findDecoder<ISceneDecoder>(theFilename, mySource);
        if (myDecoder) {
            myDecoder->decodeScene(mySource, mySceneDom);
        } else {
            AC_ERROR << "Unknown extension '" << getExtension(theFilename) + "' - ignoring included file: " << theFilename;
            return;
        }

        // (1) Make all ids unique and fix the respective references
        map<string, string> myOldToNewIdMap;
        createUniqueIds(getSceneDom(), mySceneDom, myOldToNewIdMap, removeExtension(getFilenamePart(theFilename)));
        updateReferences(mySceneDom, myOldToNewIdMap);

        // (2) Append all nodes from the new dom into the old dom
        NodePtr myOldScene = getSceneDom()->childNode(SCENE_ROOT_NAME);
        NodePtr myNewScene = mySceneDom->childNode(SCENE_ROOT_NAME);

        for (unsigned i = 0; i < myOldScene->childNodesLength(); ++i) {
            NodePtr myOldChild = myOldScene->childNode(i);
            if (myOldChild->nodeType() == dom::Node::ELEMENT_NODE &&
                myOldChild->nodeName() != CANVAS_LIST_NAME)
            {
                AC_TRACE << "myOldChild = "<<myOldChild->nodeName();
                for (unsigned j = 0; j < myNewScene->childNodesLength(); ++j) {
                    NodePtr myNewChild = myNewScene->childNode(j);
                    AC_TRACE << "myNewChild = "<<myNewChild->nodeName();
                    if (myNewChild->nodeName() == myOldChild->nodeName()) {
                        while (myNewChild->childNodesLength() != 0) {
                            AC_TRACE << "myNewChild.firstChild = "<<myNewChild->firstChild()->nodeName();
                            if (myNewChild->nodeName() == WORLD_LIST_NAME) {
                                theRoot->appendChild(myNewChild->firstChild());
                            } else {
                                myOldChild->appendChild(myNewChild->firstChild());
                            }
                        }
                    }
                }
            }
        }

        setup();
    }

    void
    Scene::updateReferences(dom::NodePtr theNode, map<string, string> & theOldToNewIdMap) {
        for (unsigned i = 0; i < theNode->attributes().length(); ++i) {
            NodePtr myAttribute = theNode->attributes().item(i);

            // Ignore empty id-ref fields which are sometimes used (e.g. in skyboxmaterial-attribute)
            if (myAttribute->nodeValueTypeName() == "IDREF" && myAttribute->nodeValue().size()) {
                map<string, string>::iterator myNewId = theOldToNewIdMap.find(myAttribute->nodeValue());
                if (myNewId != theOldToNewIdMap.end()) {
                    myAttribute->nodeValue(myNewId->second);
                } else {
                    throw InvalidIdReference(string("Attribute '") + myAttribute->nodeName() +
                        "' with type IDREF points to unknown id: " + myAttribute->nodeValue(), PLUS_FILE_LINE);
                }
            }

            // Special case for skeleton nodes
            if (myAttribute->nodeName() == "skeleton" && myAttribute->nodeValue().size()) {
                dom::Node::WritableValue<VectorOfString> mySkeletonLock(myAttribute);
                VectorOfString & myJointIds = mySkeletonLock.get();
                for (unsigned i = 0; i < myJointIds.size(); ++i) {
                    map<string, string>::iterator myNewId = theOldToNewIdMap.find(myJointIds[i]);
                    if (myNewId != theOldToNewIdMap.end()) {
                        myJointIds[i] = myNewId->second;
                    } else {
                        throw InvalidIdReference(string("Skeleton attribute '") + myAttribute->nodeValue() +
                            "' points to unknown id: " + myJointIds[i], PLUS_FILE_LINE);
                    }
                }
            }
        }

        // Recurse into child nodes
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
                updateReferences(myChild, theOldToNewIdMap);
            }
        }
    }

    void
    Scene::createUniqueIds(dom::NodePtr theDocument, dom::NodePtr theNode,
        map<string, string> & theOldToNewIdMap, const string & thePrefix)
    {
        NodePtr myIdAttribute = theNode->getAttribute(ID_ATTRIB);
        if (myIdAttribute) {
            // Create a new unique id
            //string myNewId = thePrefix + "::" + IdTag::get().getDefault();
            string myNewId = IdTag::get().getDefault();

            // Change the current id to the new id
            theOldToNewIdMap[myIdAttribute->nodeValue()] = myNewId;
            myIdAttribute->nodeValue(myNewId);
        }

        // Recurse into child nodes
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            if (myChild->nodeType() == dom::Node::ELEMENT_NODE) {
                createUniqueIds(theDocument, myChild, theOldToNewIdMap, thePrefix);
            }
        }
    }

    void
    Scene::collectGarbage() {
        AC_TRACE << "Scene::collectGarbage";

        // (pre) Remove unused world nodes
        // (1) Iterate over all animation and character nodes and delete those that have idrefs which point to nowhere
        //     and collect all IDREFs
        // (2) Iterate over all canvases nodes and delete those that have idrefs which point to nowhere
        //     and collect all IDREFs
        // (3) Recurse through world and collect IDREFs
        // (4) Iterate over shape list and remove all shapes with ids not in IDREF-list
        //     for all others, collect IDREFs
        // (5) Iterate over material list and remove all materials with ids not in IDREF-list
        //     for all other, collect IDREFs
        // (6) Iterate over image list and remove all images with ids not in IDREF-list

        Node & mySceneNode = getNode();
        std::set<string> myReferences;
        removeDangelingNodes(mySceneNode.childNode(CANVAS_LIST_NAME), _mySceneDom);
        removeDangelingNodes(mySceneNode.childNode(CHARACTER_LIST_NAME), _mySceneDom);
        removeDangelingNodes(mySceneNode.childNode(ANIMATION_LIST_NAME), _mySceneDom);

        collectReferences(mySceneNode.childNode(CANVAS_LIST_NAME), myReferences);
        NodePtr myWorlds = getWorldsRoot();
        for(unsigned i = 0; i < myWorlds->childNodesLength("world"); i++) {
            collectReferences(myWorlds->childNode("world", i), myReferences);
        }
        removeUnreferencedNodes(getNode().childNode(LIGHTSOURCE_LIST_NAME), myReferences);
        removeUnreferencedNodes(getShapesRoot(), myReferences);

        collectReferences(getShapesRoot(), myReferences);
        removeUnreferencedNodes(getMaterialsRoot(), myReferences);

        collectReferences(getMaterialsRoot(), myReferences);
        collectReferences(getCanvasRoot(), myReferences);
        removeUnreferencedNodes(getTexturesRoot(), myReferences);
        collectReferences(getTexturesRoot(), myReferences);
        removeUnreferencedNodes(getImagesRoot(), myReferences);
    }

    void
    Scene::collectReferences(NodePtr theNode, std::set<string> & theReferences) {
        for (unsigned i = 0; i < theNode->attributes().length(); ++i) {
            NodePtr myAttribute = theNode->attributes().item(i);
            if (myAttribute->nodeValueTypeName() == "IDREF") {
                theReferences.insert(myAttribute->nodeValue());
            }
        }
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            collectReferences(theNode->childNode(i), theReferences);
        }
    }

    void
    Scene::removeDangelingNodes(NodePtr theNode, NodePtr theDocument) {
        vector<NodePtr> myNodesToRemove;
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            if (isDangelingNode(myChild, theDocument)) {
                myNodesToRemove.push_back(myChild);
            }
        }
        for (unsigned i = 0; i < myNodesToRemove.size(); ++i) {
            theNode->removeChild(myNodesToRemove[i]);
        }
    }

    bool
    Scene::isDangelingNode(NodePtr theNode, NodePtr theDocument) {
        for (unsigned i = 0; i < theNode->attributes().length(); ++i) {
            NodePtr myAttribute = theNode->attributes().item(i);
            if (myAttribute->nodeValueTypeName() == "IDREF") {
                if (myAttribute->nodeValue() != "" &&
                    !theDocument->getElementById(myAttribute->nodeValue()))
                {
                    return true;
                }
            }
        }

        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            if (isDangelingNode(theNode->childNode(i), theDocument)) {
                return true;
            }
        }
        return false;
    }

    void
    Scene::removeUnreferencedNodes(NodePtr theNode, const std::set<string> & theReferences) {
        vector<NodePtr> myNodesToRemove;
        for (unsigned i = 0; i < theNode->childNodesLength(); ++i) {
            NodePtr myChild = theNode->childNode(i);
            NodePtr myId = myChild->getAttribute(ID_ATTRIB);
            if (myId) {
                if (theReferences.find(myId->nodeValue()) == theReferences.end()) {
                    myNodesToRemove.push_back(myChild);
                    AC_INFO << "Scene::removeUnreferencedNodes is removing node with id: " << myId->nodeValue() << ", name="<<(*myChild)[NAME_ATTRIB].nodeValue();
                }
            }
        }
        for (unsigned i = 0; i < myNodesToRemove.size(); ++i) {
            theNode->removeChild(myNodesToRemove[i]);
        }
    }

    void
    Scene::registerResourceManager(ResourceManager* theResourceManager) {
        // We only have to update the Scene if this is the first ResourceManager.
        if (1 == _myTextureManager->registerResourceManager(theResourceManager)) {
            //clearShapes();
            setup();
        }
    }

    void
    Scene::deregisterResourceManager() {
        AC_DEBUG << "deregisterResourceManager";
        if (_myTextureManager->registerResourceManager(0) == 0) {
            // XXX ?
            //clearShapes();
            //setup();
        }
    }

    const IResourceManager *
    Scene::getResourceManager() const {
        return _myTextureManager->getResourceManager();
    }

    IResourceManager *
    Scene::getResourceManager() {
        return _myTextureManager->getResourceManager();
    }
}

