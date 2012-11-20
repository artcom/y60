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
//   $Id: SceneExporter.cpp,v 1.57 2005/04/29 15:07:34 valentin Exp $
//   $RCSfile: SceneExporter.cpp,v $
//   $Author: valentin $
//   $Revision: 1.57 $
//   $Date: 2005/04/29 15:07:34 $
//
//  Description: This class implements a exporter plugin for Cinema 4D.
//
//=============================================================================

#include "SceneExporter.h"

#include "AnimationExporter.h"
#include "ShapeExporter.h"
#include "CinemaHelpers.h"
#include "res/c4d_symbols.h"
#include "res/description/Facxmlexport.h"
#include "res/description/Facbinaryexport.h"

#include <asl/base/initialization.h>
#include <asl/base/settings.h>
#include <asl/math/linearAlgebra.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>

#include <y60/scene/BodyBuilder.h>
#include <y60/scene/LightSourceBuilder.h>
#include <y60/base/NodeNames.h>

#include <stdio.h>
#include <iostream>
#include <fstream>

#include <c4d.h>

#define LIMIT_LIGHTS

using namespace std;
using namespace y60;

SceneExporter::SceneExporter(bool theBinaryFlag) :
    _myObjectCount(0), _myBinaryFlag(theBinaryFlag)
{}

SceneExporter::~SceneExporter(void) {
}

static long ourProgressCounter = 0;

unsigned
SceneExporter::countObjects(BaseObject *theNode) {
    unsigned myCount = 1;

    BaseObject * myChildNode = theNode->GetDown();
    while (myChildNode) {
        myCount += countObjects(myChildNode);
        myChildNode = myChildNode->GetNext();
    }
    return myCount;
}

void
SceneExporter::exportTransformation(y60::TransformBuilderBase & theBuilder,
                                    BaseObject * theNode,
                                    bool theInverseTransformFlag)
{
    Vector myPos = theNode->GetPos();
    Vector myScale = theNode->GetScale();
    Vector myOrientation = theNode->GetRot();

    // Cinema uses a left-hand coordinate system with Y-up
    if (theInverseTransformFlag) {
        theBuilder.setPosition(asl::Vector3f(-myPos.x, -myPos.y, myPos.z));
        theBuilder.setScale(asl::Vector3f(1.0f / myScale.x, 1.0f / myScale.y, 1.0f / myScale.z));
    }
    else {
        theBuilder.setPosition(asl::Vector3f(myPos.x, myPos.y, -myPos.z));
        theBuilder.setScale(asl::Vector3f(myScale.x, myScale.y, myScale.z));
    }

    BaseTag * myBillboardTag = theNode->GetTag(Tlookatcamera);
    if (myBillboardTag) {
        // billboard
        BaseContainer * myContainer = myBillboardTag->GetDataInstance();
        bool myPitchCamera = 0 != myContainer->GetBool(LOOKATCAMERA_PITCH);
        //GePrint("Billboard " + theNode->GetName());

        if (myPitchCamera) {
            theBuilder.setBillboard(y60::TransformBuilderBase::BB_POINT);
        } else {
            theBuilder.setBillboard(y60::TransformBuilderBase::BB_AXIS);
        }
        theBuilder.setOrientation(asl::Quaternionf(0,0,0,1));
    } else {
        // orientation
        asl::Matrix4f myOrientationMatrix;
        myOrientationMatrix.makeIdentity();
        myOrientationMatrix.rotateZ(-myOrientation.z);
        myOrientationMatrix.rotateX(myOrientation.y); // x and y are swizzeled because myOrientation
        myOrientationMatrix.rotateY(myOrientation.x); // has hpr representation and not carthesian

        // invert transform
        if (theInverseTransformFlag) {
            myOrientationMatrix.invert();
        }

        asl::Vector3f myOrientationVec;
        myOrientationMatrix.getRotation(myOrientationVec, asl::Matrix4f::ROTATION_ORDER_XYZ);
        theBuilder.setOrientation(myOrientationVec);
    }
}

y60::WorldBuilderBasePtr
SceneExporter::writeCamera(y60::WorldBuilderBasePtr theTransformBuilder, CameraObject * theNode) {
    y60::CameraBuilderPtr myCameraBuilder(new y60::CameraBuilder(getString(theNode->GetName())));

    // Keep camera ID for later
    const std::string & myId = theTransformBuilder->appendObject(*myCameraBuilder);
    _myExportedCameras[theNode] = myId;
    //GePrint("Camera '" + theNode->GetName() + "' ID=" + String(myId.c_str()));

    exportTransformation(*myCameraBuilder, theNode);
    myCameraBuilder->setHFov(asl::degFromRad(2.0f *
        (atan(theNode->GetAperture() / (2.0f * theNode->GetFocus())))));

    BaseContainer * myContainer = theNode->GetDataInstance();
    if (myContainer) {
        Real myXOffset = myContainer->GetReal(CAMERAOBJECT_FILM_OFFSET_X);
        Real myYOffset = myContainer->GetReal(CAMERAOBJECT_FILM_OFFSET_Y);
        myCameraBuilder->setShift( -myXOffset, myYOffset );
    }
    return myCameraBuilder;
}

y60::WorldBuilderBasePtr
SceneExporter::writeLight(y60::WorldBuilderBasePtr theTransformBuilder,
                           BaseObject * theNode)
{
    static const char * ourCinemaY60LightsSL[] = {
        "positional",    // LIGHT_TYPE_OMNI
        "positional",    // LIGHT_TYPE_SPOT
        "positional",    // LIGHT_TYPE_SPOTRECT
        "directional",   // LIGHT_TYPE_DISTANT
        "directional",   // LIGHT_TYPE_PARALLEL
        "unsupported",   // LIGHT_TYPE_PARSPOT
        "unsupported",   // LIGHT_TYPE_PARSPOTRECT
        "unsupported",   // LIGHT_TYPE_TUBE
        "unsupported",   // LIGHT_TYPE_AREA
        0
    };

    std::string myLightSourceId;
    // We use the pointer to the node object as unique id, because cinema
    // does not provide this little service
    void * myNodeId = static_cast<void *>(theNode);

    // Check if the light source has been exported before
    if (_myExportedLightSources.find(myNodeId) == _myExportedLightSources.end()) {
        // OpenGL does not handle more than 8 lightsources. So we have to ignore additional
        // ones. For now, we pop up a warning box in these cases.
        // Export Light Source
        y60::SceneBuilder & mySceneBuilder(*_mySceneBuilder);
        y60::LightSourceBuilderPtr myLightSource(new y60::LightSourceBuilder(getString(theNode->GetName())));

        // get the light type
        GeData myGeData;
        theNode->GetParameter(DescID(LIGHT_TYPE), myGeData, 0);
        long myLightType = myGeData.GetLong();
        const std::string myLightTypeStr = asl::getStringFromEnum(myLightType, ourCinemaY60LightsSL);
        myLightSource->setType((y60::LightSourceType)asl::getEnumFromString(myLightTypeStr,
            y60::LightSourceTypeString) );

        // get the light color
        theNode->GetParameter(DescID(LIGHT_COLOR), myGeData, 0);
        Vector myCinemaLightColor = myGeData.GetVector();

        // get the light brightness
        theNode->GetParameter(DescID(LIGHT_BRIGHTNESS), myGeData, 0);
        float myCinemaLightBrightness = myGeData.GetReal();

        if (myLightTypeStr == "positional") {
            // attenuation
            theNode->GetParameter(DescID(LIGHT_DETAILS_FALLOFF), myGeData, 0);
            long myFalloffType = myGeData.GetLong();
            switch (myFalloffType) {
                case LIGHT_DETAILS_FALLOFF_NONE :
                    myLightSource->setAttenuation(0.0f);
                    break;
                case LIGHT_DETAILS_FALLOFF_LINEAR :
                case LIGHT_DETAILS_FALLOFF_INVERSE :
                case LIGHT_DETAILS_FALLOFF_INVERSESQUARE :
                    // case LIGHT_DETAILS_FALLOFF_INVERSECUBIC :
                case LIGHT_DETAILS_FALLOFF_STEP : {
                        // turn on an arbitrary linear attenuation.
                        myLightSource->setAttenuation(1.0f);
                    }
                    break;
            }

            // spot
            if (myLightType == LIGHT_TYPE_SPOT || myLightType == LIGHT_TYPE_SPOTRECT ){
                theNode->GetParameter(DescID(LIGHT_DETAILS_OUTERANGLE), myGeData, 0);
                float myOuterAngle = myGeData.GetReal();
                theNode->GetParameter(DescID(LIGHT_DETAILS_INNERANGLE), myGeData, 0);
                float myInnerAngle = myGeData.GetReal();
                float myExponent = 0;
                if (myOuterAngle) {
                    // we made up this formula. 30 seems ok.
                    myExponent = (1-(myInnerAngle/myOuterAngle)) * 30;
                }

                myLightSource->setSpotLight(asl::degFromRad(myOuterAngle), myExponent);
            }
        }

        asl::Vector4f myLightColor;
        myLightColor[0] = myCinemaLightColor.x * myCinemaLightBrightness;
        myLightColor[1] = myCinemaLightColor.y * myCinemaLightBrightness;
        myLightColor[2] = myCinemaLightColor.z * myCinemaLightBrightness;
        myLightColor[3] = 1;
        myLightSource->setAmbient(myLightColor);
        myLightSource->setDiffuse(myLightColor);
        myLightSource->setSpecular(myLightColor);

        myLightSourceId = mySceneBuilder.appendLightSource(*myLightSource);
        // Is there still room for an additional light source?
        _myExportedLightSources[myNodeId] = myLightSourceId;
    } else {
        // reference the already exported light source
        myLightSourceId = _myExportedLightSources[myNodeId];
    }
    y60::LightBuilderPtr myLightBuilder(new y60::LightBuilder(myLightSourceId, getString(theNode->GetName())));
    theTransformBuilder->appendObject(*myLightBuilder);

    exportTransformation(*myLightBuilder, theNode);
    return myLightBuilder;
}

BaseObject *
SceneExporter::polygonizeObject(BaseObject * theNode)
{
    GePrint("+++ Polygonizing '" + getTreeName(theNode) + "'");

    ModelingCommandData myNodeCommand;
    myNodeCommand.doc = _myDocument;
    myNodeCommand.op = theNode;
    if (!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, myNodeCommand)) {
        throw ExportException("Unable to execute MCOMMAND_CURRENTSTATETOOBJECT", "SceneExporter::polygonizeObject()");
    }

    BaseObject * myPolygonNode = 0;
#if API_VERSION >= 9000
    // result_ex seems to work as well but should not be used
    LONG myNumResults = myNodeCommand.result->GetCount();
    if (myNumResults == 0) {
        throw ExportException("Polygonalization returned 0 objects", "SceneExporter::polygonizeObject()");
    } else if (myNumResults > 1) {
        GePrint("Polygonalization of '" + getTreeName(theNode) + "' returned " + LongToString(myNumResults) + " results");
    }
    myPolygonNode = (BaseObject*)myNodeCommand.result->GetIndex(0);
    if (myPolygonNode != myNodeCommand.result_ex) {
        GePrint("### WARNING : result[0] != result_ex");
    }

    // Free unused results
    for (LONG i = 1; i < myNumResults; ++i) {
        BaseObject * myObject = (BaseObject*)myNodeCommand.result->GetIndex(i);
        BaseObject::Free(myObject);
    }
#else
    myPolygonNode = (BaseObject*)myNodeCommand.result1;
#endif
    if (!myPolygonNode) {
        throw ExportException("Polygonized object is NULL", "SceneExporter::polygonizeObject()");
    }

    return myPolygonNode;
}

y60::WorldBuilderBasePtr
SceneExporter::writeBody(y60::WorldBuilderBasePtr theTransformBuilder,
                         BaseObject * theNode,
                         bool theTransformFlag,
                         bool theForceFrontBackFacing,
                         const string & theNameAppendix)
{
    // We use the pointer to the node object as unique id, because cinema
    // does not provide this little service
    void * myNodeId = static_cast<void *>(theNode);

    // Check if the shape has been exported before
    if (_myExportedShapes.find(myNodeId) == _myExportedShapes.end()) {

        // Polygonize object
        BaseObject * myPolygonNode = 0;
        bool myMustFreePolygonNode = false;
        myPolygonNode = polygonizeObject(theNode);
        myMustFreePolygonNode = true;

        // Export shape, in case of selections this shape
        ShapeExporter myShapeExporter(*_mySceneBuilder, *_myMaterialExporter);
        std::vector<std::string> myShapeIdList = myShapeExporter.writeShape(theNode, myPolygonNode, theForceFrontBackFacing);
        if (myShapeIdList.empty() == false) {
            _myExportedShapes[myNodeId] = myShapeIdList;
        }

        // Free created objects
        if (myMustFreePolygonNode) {
            BaseObject::Free(myPolygonNode);
        }
    }

    /*
     * Create hierarchy of bodies
     */
    y60::BodyBuilderPtr myBodyBuilder;
    if (_myExportedShapes.find(myNodeId) == _myExportedShapes.end()) {
        return myBodyBuilder;
    }

    y60::WorldBuilderBasePtr myParentBuilder = theTransformBuilder;
    bool myTransformFlag = theTransformFlag;
    std::string myBaseName = getString(theNode->GetName()) + theNameAppendix;

    std::vector<std::string> myShapeIdList = _myExportedShapes[myNodeId];
    for (unsigned i = 0; i < myShapeIdList.size(); ++i) {
        std::string myShapeId = myShapeIdList[i];
        myBodyBuilder = y60::BodyBuilderPtr(new y60::BodyBuilder(myShapeId, myBaseName));

        // Append selections
        myParentBuilder->appendObject(*myBodyBuilder);
        if (myTransformFlag) {
            exportTransformation(*myBodyBuilder, theNode);
        }
        myParentBuilder = myBodyBuilder;
        myTransformFlag = false;
        myBaseName += "_";
    }
    return myBodyBuilder;
}

y60::WorldBuilderBasePtr
SceneExporter::writeTransform(y60::WorldBuilderBasePtr theTransformBuilder,
                              BaseObject * theNode)
{
    y60::TransformBuilderPtr myTransformBuilder(new y60::TransformBuilder(getString(theNode->GetName())));
    theTransformBuilder->appendObject(*myTransformBuilder);
    exportTransformation(*myTransformBuilder, theNode, false);
    return myTransformBuilder;
}

y60::WorldBuilderBasePtr
SceneExporter::writeSymmetryTransform(y60::WorldBuilderBasePtr theTransformBuilder,
                                      BaseObject * theNode)
{
    y60::TransformBuilderPtr myTransformBuilder(new y60::TransformBuilder(getString(theNode->GetName())));
    theTransformBuilder->appendObject(*myTransformBuilder);

    GeData myGeData;
    theNode->GetParameter(DescID(SYMMETRYOBJECT_PLANE), myGeData, 0);
    LONG mySymmetryPlane = myGeData.GetLong();
    GePrint("Symmetry plane=" + LongToString(mySymmetryPlane));
    switch (mySymmetryPlane) {
        case SYMMETRYOBJECT_PLANE_XY:
            myTransformBuilder->setScale(asl::Vector3f(1.0f, 1.0, -1.0));
            break;
        case SYMMETRYOBJECT_PLANE_YZ:
            myTransformBuilder->setScale(asl::Vector3f(-1.0f, 1.0, 1.0));
            break;
        case SYMMETRYOBJECT_PLANE_XZ:
            myTransformBuilder->setScale(asl::Vector3f(1.0f, -1.0, 1.0));
            break;
    }

    return myTransformBuilder;
}

void
SceneExporter::writeActiveCamera()
{
    CameraObject * myCamera;
    BaseDraw * myActiveBaseDraw = _myDocument->GetActiveBaseDraw();
    if (!myActiveBaseDraw->HasCameraLink()) {
        myCamera = (CameraObject*) myActiveBaseDraw->GetEditorCamera();
    } else {
        myCamera = (CameraObject*) myActiveBaseDraw->GetSceneCamera(_myDocument);
    }

    if (_myExportedCameras.find(myCamera) == _myExportedCameras.end()) {
        GePrint("Exporting active camera '" + myCamera->GetName() + "'");
        writeCamera(_myWorldBuilder, myCamera);
    }

    CameraMap::const_iterator it = _myExportedCameras.find(myCamera);
    const std::string myId = it->second;
    GePrint("Active camera '" + myCamera->GetName() + "' ID=" + String(myId.c_str()));
    _mySceneBuilder->setInitialCamera(myId);
}

void
SceneExporter::writeObjects(y60::WorldBuilderBasePtr theTransformBuilder,
                            BaseObject * theNode,
                            bool theTransformFlag,
                            bool theForceFrontBackFacing)
{
    // Update progress bar
    ourProgressCounter++;
    if (_myObjectCount && !(ourProgressCounter & 63)) {
        StatusSetBar(Real(ourProgressCounter) / Real(_myObjectCount) * 100.0);
    }

    // Ignore disabled objects
    if (theNode->GetRenderMode() == MODE_OFF) {
        GePrint("Ignoring '" + theNode->GetName() + "'");
        return;
    }

    // Check children for deformation objects
    {
        bool myFoundDeformFlag = false;
        BaseObject * myChild = theNode->GetDown();
        while (myChild && myFoundDeformFlag == false) {
            LONG myNodeType = myChild->GetType();
            switch (myNodeType) {
            case Obend:
            case Obulge:
            case Oexplosion:
            case Oshear:
            case Otaper:
            case Otwist:
            case Owind:
            case Owrap:
                GePrint("Found deformation object '" + myChild->GetName() + "' type=" + LongToString(myNodeType));
                myFoundDeformFlag = true;
                break;
            default:
                break;
            }
            myChild = myChild->GetNext();
        }

        if (myFoundDeformFlag) {
            // Polygonalize node, write-out, return
            BaseObject * myPolygonNode = polygonizeObject(theNode);
            writeObjects(theTransformBuilder, myPolygonNode, theTransformFlag, theForceFrontBackFacing);
            BaseObject::Free(myPolygonNode);
            return;
        }
    }
    y60::WorldBuilderBasePtr myParentBuilder;
    if (writeObject(theTransformBuilder, theNode, myParentBuilder,
                    theTransformFlag, theForceFrontBackFacing)) {
        // Recurse over child nodes
        if (myParentBuilder) {
            BaseObject * myChildNode = theNode->GetDown();
            while (myChildNode) {
                writeObjects(myParentBuilder, myChildNode, true, theForceFrontBackFacing);
                myChildNode = myChildNode->GetNext();
            }
        }
   }
}

bool
SceneExporter::writeObject(y60::WorldBuilderBasePtr theTransformBuilder,
                            BaseObject * & theNode,
                            y60::WorldBuilderBasePtr & theParentBuilder,
                            bool theTransformFlag,
                            bool theForceFrontBackFacing,
                            const std::string & theNameAppendix) {
    const String myTreeName =  getTreeName(theNode);
    std::string myName = getString(theNode->GetName());
    StatusSetText("Exporting '" + myTreeName + "'");
    //LONG myNodeId = theNode->GetNodeID();
    LONG myNodeType = theNode->GetType();

    switch (myNodeType) {
        case Olight:
            {
                theParentBuilder = writeLight(theTransformBuilder, theNode);
            }
            break;
        case Ocamera:
            {
                theParentBuilder = writeCamera(theTransformBuilder, (CameraObject*)theNode);
            }
            break;
        case Onull:
            {
                if (theTransformFlag) {
                    theParentBuilder = writeTransform(theTransformBuilder, theNode);
                }
            }
            break;
        case Oinstance:
            {
                // Find referenced object
                BaseContainer * myContainer    = theNode->GetDataInstance();
                BaseObject    * myLinkedObject = myContainer->GetObjectLink(INSTANCEOBJECT_LINK, _myDocument);
                if (myLinkedObject == 0) {
                    displayMessage("Instance '" + theNode->GetName() + "' references invalid object");
                    return false;
                }

                // Write referenced object hierachy
                if (theTransformFlag) {
                    theParentBuilder = writeTransform(theTransformBuilder, theNode);
                }
                {
                    /* we have already written the transform of the instance so export
                       untransformed linked object only if it is not a transform node */
                    LONG myInstancedNodeType = myLinkedObject->GetType();
                    if (myInstancedNodeType != Onull) {
                        writeObject(theParentBuilder, myLinkedObject,
                                    theParentBuilder, false, theForceFrontBackFacing, "_instance");
                    }
                }

                theNode = myLinkedObject;
            }
            break;
        case Osymmetry:
            {
                // Write transform
                theParentBuilder = writeTransform(theTransformBuilder, theNode);

                // Write symmetry transform & children
                y60::WorldBuilderBasePtr mySymmetryBuilder = writeSymmetryTransform(theParentBuilder, theNode);
                BaseObject * myChildNode = theNode->GetDown();
                while (myChildNode) {
                    writeObjects(mySymmetryBuilder, myChildNode, true, true);
                    myChildNode = myChildNode->GetNext();
                }
            }
            break;
        case Osds:
            {
                // Write transform
                theParentBuilder = writeTransform(theTransformBuilder, theNode);
                BaseObject * myChildNode = theNode->GetDown();
                while (myChildNode) {
                    writeObjects(theParentBuilder, myChildNode, true, theForceFrontBackFacing);
                    myChildNode = myChildNode->GetNext();
                }
                return false;
            }
            break;
        case Obend:
        case Obulge:
        case Oshear:
        case Obackground:
        case Ofloor:
        case Otaper:
        case Otwist:
            GePrint("Unhandled deformation object '" + theNode->GetName() + "' type=" + LongToString(myNodeType));
            break;
        /*
        // Add unsupported types here:
        case xyz:
            throw ExportException(std::string("Unsupported object type: ") +
                                    asl::as_string(theNode->GetType()),
                                    "SceneExporter::writeObjects()");
            break;
        */
        default:
            // Export polygon objects
            theParentBuilder = writeBody(theTransformBuilder, theNode, theTransformFlag, theForceFrontBackFacing, theNameAppendix);
            break;
    }

    if (theParentBuilder && theParentBuilder->getNode()) {
        // export animation if any exists
        AnimationExporter myAnimationExporter(*_mySceneBuilder,_myDocument);
    //    myAnimationExporter.exportGlobal(theNode, theParentBuilder->getId());
        myAnimationExporter.exportCharacter(theNode, theParentBuilder->getId());
    }
    return true;
}

LONG
SceneExporter::Save(PluginSceneSaver * theNode,
                    const Filename & theFileName,
                    BaseDocument * theDocument,
                    LONG theFlags)
{
    LONG myError = FILEERROR_NONE;
    try {
        asl::initialize();
        if (!(theFlags & SCENEFILTER_OBJECTS)) {
            return FILEERROR_NONE;
        }

        // Create a clone to make sure, the exporter does not change the original document
        _myDocument = theDocument; //static_cast<BaseDocument*>(theDocument->GetClone(COPY_DOCUMENT, 0));
        if (!_myDocument) {
            return FILEERROR_MEMORY;
        }

        // Get export flags
        BaseContainer * myBaseContainer = theNode->GetDataInstance();
        bool myInlineTextures = false;
        if (_myBinaryFlag) {
            myInlineTextures = 0 != myBaseContainer->GetBool(AC_Y60_INLINE_TEXTURES_BIN, false);
        } else {
            myInlineTextures = 0 != myBaseContainer->GetBool(AC_Y60_INLINE_TEXTURES, false);
        }
        GePrint("=== Exporting Y60 '" + theFileName.GetString() + "' " + (myInlineTextures ? "with" : "without") + " inlined textures");

        // Polygons count is used for progress bar
        _myObjectCount = 0;
        if (theFlags & SCENEFILTER_PROGRESSALLOWED) {
            BaseObject * myTopLevelNode = _myDocument->GetFirstObject();
            while (myTopLevelNode) {
                _myObjectCount += countObjects(myTopLevelNode);
                myTopLevelNode = myTopLevelNode->GetNext();
            }
        }

        dom::DocumentPtr myDocument(new dom::Document);
        _mySceneBuilder = y60::SceneBuilderPtr(new y60::SceneBuilder(myDocument));
        _myWorldBuilder = y60::WorldBuilderPtr(new y60::WorldBuilder());
        _mySceneBuilder->appendWorld(*_myWorldBuilder);
        _myMaterialExporter = new MaterialExporter(_mySceneBuilder, _myDocument->GetDocumentPath(), myInlineTextures);

        // Export all objects, cameras and materials
        BaseObject * myTopLevelNode = _myDocument->GetFirstObject();
        while (myTopLevelNode) {

            writeObjects(_myWorldBuilder, myTopLevelNode);
            myTopLevelNode = myTopLevelNode->GetNext();
        }

        // Write current scene camera
        writeActiveCamera();

        // Save to file
        StatusSetText("Saving '" + theFileName.GetString() + "'");
        std::string myFileName = getString(theFileName.GetString());
        if (_myBinaryFlag) {
#if 1
            asl::WriteableFile myBlock(myFileName);
            _mySceneBuilder->binarize(myBlock);
#else
            asl::Block myBlock;
            _mySceneBuilder->binarize(myBlock);
            if (!asl::writeFile(myFileName,myBlock)) {
                throw IOError(std::string("Could not open or write binary file '") + myFileName + "'", "Scene::save()");
            }
#endif
        } else {
            std::ofstream myFile(myFileName.c_str());
            if (!myFile) {
                throw OpenFailed(std::string("Can not open file '") + myFileName + "' for writing", "Scene::save()");
            }
            _mySceneBuilder->serialize(myFile);
            if (!myFile) {
                throw WriteFailed(std::string("Could not write text file '") + myFileName + "'", "Scene::save()");
            }
        }
        StatusSetText("Done.");

        // Clear member objects for the next export
        delete _myMaterialExporter;
        ourProgressCounter = 0;
        _myMaterialExporter = 0;
        _myExportedShapes.clear();
        _myExportedLightSources.clear();
        _myExportedCameras.clear();
        _mySceneBuilder = y60::SceneBuilderPtr();
    } catch (const asl::Exception & ex) {
        displayMessage(std::string("Error in SceneExporter::Save(): ") + asl::as_string(ex));
    } catch (...) {
        displayMessage("Error: Unknown exception in SceneExporter::Save()");
    }
    return myError;
}

Bool RegisterAcXmlExporter(void)
{
    GePrint("ART+COM Y60 Exporter - Build " + String(__DATE__));

    // decide by name if the plugin shall be registered - just for user convenience
    String myName = GeLoadString(IDS_Y60_XML_DECL);
    if (!myName.Content())
        return FALSE;

    // be sure to use a unique ID obtained from www.plugincafe.com
    if (!RegisterSceneSaverPlugin(1000001, myName, 0, SceneExporter::Alloc, "Facxmlexport", "x60")) {
        return FALSE;
    }

    myName = GeLoadString(IDS_Y60_BINARY_DECL);
    if (!myName.Content())
        return FALSE;

    if (!RegisterSceneSaverPlugin(1000002, myName, 0, SceneExporter::AllocBinary, "Facbinaryexport", "b60")) {
        return FALSE;
    }

    return TRUE;
}
