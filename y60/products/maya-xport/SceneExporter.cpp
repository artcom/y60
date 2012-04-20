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

#include "SceneExporter.h"
#include "ShapeExporter.h"
#include "CurveExporter.h"
#include "ExportExceptions.h"
#include "MaterialExporter.h"
#include "LightExporter.h"
#include "MayaHelpers.h"
#include "ProgressBar.h"

#include <asl/dom/Nodes.h>
#include <asl/base/buildinfo.h>
#include <asl/base/Dashboard.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/MappedBlock.h>

#include <y60/scene/SceneBuilder.h>
#include <y60/scene/LightBuilder.h>
#include <y60/scene/TransformBuilder.h>

#include <y60/scene/CameraBuilder.h>
#include <y60/scene/WorldBuilder.h>
#include <y60/scene/JointBuilder.h>

#include <maya/MStatus.h>
#include <maya/MPoint.h>
#include <maya/MString.h>
#include <maya/MObject.h>
#include <maya/MGlobal.h>
#include <maya/MItDag.h>
#include <maya/MDagPath.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MFnPlugin.h>
#include <maya/MFnCamera.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MFnTransform.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnSkinCluster.h>
#include <maya/MItDependencyNodes.h>

#include <iostream>
#include <fstream>

#define DB(x) // x

const char * ourMayaName = "Y60";

using namespace std;
using namespace asl;
using namespace y60;
using namespace dom;

#if MAYA_API_VERSION < 700
    #error "Y60 Maya exporter not yet released for this Maya version"
#endif
#ifdef LINUX
#if MAYA_API_VERSION < 800 && __GNUC__ == 3 && __GNUC_MINOR__ != 3
    #error "gcc 3.3.x must be used to create Maya 7.0 plugins"
#elif MAYA_API_VERSION == 800 && __GNUC__ == 4 && __GNUC_MINOR__ != 0
    #error "gcc 4.0.x must be used to create Maya 8.0 plugins"
#endif
#endif

SceneExporter::SceneExporter() : _myInitialCamera("c0") {
}


SceneExporter::~SceneExporter() {
}

void *
SceneExporter::creator() {
    return new SceneExporter();
}

MString
SceneExporter::defaultExtension() const {
    return MString("");
}

MStatus
initializePlugin(MObject theObject) {
    DB(AC_TRACE << "SceneExporter::initializePlugin()" << endl);
    MStatus myStatus;
    std::string myRevision = asl::build_information::get().executable().history_id().substr(0,6);
    if (myRevision.empty()) {
        myRevision = "0";
    }
    MFnPlugin myPlugin(theObject, "ART+COM XML exporter plugin", myRevision.c_str(), "Any");

    // Register the translator with the system
    //
    myStatus =  myPlugin.registerFileTranslator(ourMayaName,
                                              "",
                                              SceneExporter::creator,
                                              "acY60XmlExportOptions",
                                              "binary=0;inlineTextures=0",
                                              true);
    if (!myStatus) {
        myStatus.perror("registerFileTranslator");
        return myStatus;
    }
    return myStatus;
}

MStatus
uninitializePlugin(MObject theObject) {
    DB(AC_TRACE << "SceneExporter::uninitializePlugin()" << endl);
    MStatus   myStatus;
    MFnPlugin myPlugin( theObject );

    myStatus =  myPlugin.deregisterFileTranslator(ourMayaName);
    if (!myStatus) {
        myStatus.perror("deregisterFileTranslator");
        return myStatus;
    }

    return myStatus;
}

SceneExporter::ExportOptions
SceneExporter::parseOptions(const MString & theOptionString) {
    ExportOptions myExportOptions;
    DB(AC_TRACE << "*** Options: " << theOptionString.asChar() << endl);
    if (theOptionString.length() > 0) {
        MStringArray myOptionList;
        MStringArray myOption;
        theOptionString.split(';', myOptionList);
        for (unsigned i = 0; i < myOptionList.length(); ++i) {
            myOption.clear();
            myOptionList[i].split('=', myOption);
            if (myOption[0] == MString("binary")) {
                myExportOptions.binaryFlag = 0 != myOption[1].asInt();
            } else if (myOption[0] == MString("inlineTextures")) {
                myExportOptions.inlineTexturesFlag = 0 != myOption[1].asInt();
            } else if (myOption[0] == MString("progressBar")) {
                myExportOptions.enableProgressBar = 0 != myOption[1].asInt();
            }
        }
    }

    return myExportOptions;
}

MStatus
SceneExporter::writer(const MFileObject & theFile,
                      const MString & theOptionString,
                      MPxFileTranslator::FileAccessMode theMode)
{
    MStatus myStatus = MS::kSuccess;
    {
        MAKE_SCOPE_TIMER(SceneExporter_writer);

        ExportOptions myOptions = parseOptions(theOptionString);
        ProgressBar myProgressBar(myOptions.enableProgressBar);
#if MAYA_API_VERSION == 800
        MString myFileName = theFile.resolvedFullName();
#else
        MString myFileName = theFile.fullName();
#endif

        try {
            dom::DocumentPtr myDocument(new dom::Document);
            SceneBuilder     mySceneBuilder(myDocument);
            MaterialExporter myMaterialExporter(myOptions.inlineTexturesFlag);

            myMaterialExporter.setBaseDirectory(asl::getDirectoryPart(std::string(myFileName.asChar())));
            clearCurveMaterials();

            //check which objects are to be exported, and invoke the corresponding
            //methods; only 'export all' and 'export selection' are allowed

            if (MPxFileTranslator::kExportAccessMode == theMode) {
                exportAll(mySceneBuilder, myMaterialExporter, myProgressBar);
            } else if (MPxFileTranslator::kExportActiveAccessMode == theMode) {
                exportSelection(mySceneBuilder, myMaterialExporter, myProgressBar);
            } else {
                throw ExportException("Invalid export access mode", "SceneExporter::writer()");
            }

            // Add correct file extension
            int myLastDot = myFileName.rindex('.');
            if (myLastDot >= 0) {
                MString myExtension = myFileName.substring(myLastDot+1,myFileName.length()-1);
                if (myExtension == "mb" || myExtension == "x60" || myExtension == "b60") {
                    // strip extension
                    myFileName = myFileName.substring(0, myLastDot-1);
                }
            }
            if (myOptions.binaryFlag) {
                myFileName = myFileName + ".b60";
            } else {
                myFileName = myFileName + ".x60";
            }

            //AC_PRINT << "write filename=" << myFileName << " cwd=" << getCWD();
            myProgressBar.setStatus("Writing file to disk");
            if (myOptions.binaryFlag) {
                MAKE_SCOPE_TIMER(writer_binary);
#ifdef USE_MEM_COLLECT_BLOCK_OUT
                asl::Block myBlock;
#else
#ifdef USE_MAPPED_BLOCK_OUT
                asl::MappedBlock myBlock(myFileName.asChar());
#else
                asl::WriteableFile myBlock(myFileName.asChar());
#endif
#endif
                dom::Document myDocument;
                myDocument.appendChild(mySceneBuilder.getNode());
                myDocument.binarize(myBlock);
#ifdef USE_MEM_COLLECT_BLOCK_OUT
                if (!asl::writeFile(myFileName.asChar(), myBlock)) {
                    throw IOError(std::string("Could not write binary file '") + myFileName.asChar() + "'", "SceneExporter::writer()");
                }
#endif
            } else {
                MAKE_SCOPE_TIMER(writer_ascii);
                std::ofstream myFile(myFileName.asChar());
                if (!myFile) {
                    throw OpenFailed(std::string("Can not open file '") + myFileName.asChar() + "' for writing", "SceneExporter::writer()");
                }
                myFile << mySceneBuilder;
                if (!myFile) {
                    throw WriteFailed(std::string("Could not write text file '") + myFileName.asChar() + "'", "SceneExporter::writer()");
                }
            }

            asl::getDashboard().cycle();
            asl::getDashboard().print(std::cerr);
            asl::getDashboard().reset();
        } catch (ExportCancelled) {
            // Ignore cancle button exceptions
        } catch (const asl::Exception & ex) {
            displayError(string("Exception while writing file: ") + myFileName.asChar() + ":\n" + asl::as_string(ex));
            myStatus = MStatus::kFailure;
        } catch (const std::exception & ex) {
            displayError(string("Standard exception while writing file: ") + myFileName.asChar() + ":\n" +
                               ex.what());
            myStatus = MStatus::kFailure;
        } catch (...) {
            displayError(string("Unknown Exception while writing file: ") + myFileName.asChar());
            myStatus = MStatus::kFailure;
        }
        _myIdMap.clear();
        _mySkinAndBonesMap.clear();

        if (myStatus) {
            bool myProblemsReported = displayProblemSummary();
            if (myProblemsReported) {
                MGlobal::displayInfo("### There were problems during export. Please check log.");
            } else {
                //cerr << "Export to " << myFileName.asChar() << " successful!" << endl;
                MGlobal::displayInfo("Export to " + myFileName + " successful!");
            }
        }
    }

    return myStatus;
}

bool
SceneExporter::haveWriteMethod() const {
    return true;
}

bool
SceneExporter::haveReadMethod() const {
    return false;
}

bool
SceneExporter::canBeOpened() const {
    return true;
}

const string
SceneExporter::exportJoint(const MObject & theObject, WorldBuilderBase & theParentTransform,
        SceneBuilder & theSceneBuilder, const string & theParentPath,
        StringSet * theSelectionSet)
{
    MAKE_SCOPE_TIMER(SceneExporter_exportJoint);
    MStatus myStatus;
    MFnIkJoint myJoint(theObject, &myStatus);
    checkMStatus(myStatus, "Could not cast a DagNode to MFnIKJoint.", PLUS_FILE_LINE);

    JointBuilder myJointBuilder(std::string(myJoint.name().asChar()));

    const string & myId = exportTransformBase(theObject, myJointBuilder, theParentTransform, theSceneBuilder,
        theParentPath, theSelectionSet);

    MTransformationMatrix::RotationOrder myOrder = MTransformationMatrix::kXYZ;
    double  myJointOrientation[3];
    myStatus = myJoint.getOrientation(myJointOrientation, myOrder);
    checkMStatus(myStatus, "Could not get joint orientation.", PLUS_FILE_LINE);
    myJointBuilder.setJointOrientation(asl::Vector3f(float(myJointOrientation[0]),
                float(myJointOrientation[1]), float(myJointOrientation[2])));

    return myId;
}

const string
SceneExporter::exportTransform(const MObject & theObject, WorldBuilderBase & theParentTransform,
        SceneBuilder & theSceneBuilder, const string & theParentPath,
        StringSet * theSelectionSet)
{
    MAKE_SCOPE_TIMER(SceneExporter_exportTransform);
    MStatus myStatus;
    MFnTransform myTransform(theObject, &myStatus);
    checkMStatus(myStatus, "Could not cast a DagNode to MFnTransform.", PLUS_FILE_LINE);

    TransformBuilder myTransformBuilder(std::string(myTransform.name().asChar()));

    return exportTransformBase(theObject, myTransformBuilder, theParentTransform, theSceneBuilder,
        theParentPath, theSelectionSet);
}

const string
SceneExporter::exportTransformBase(const MObject & theObject, TransformBuilderBase & theTransformBuilder,
        WorldBuilderBase & theParentTransform, SceneBuilder & theSceneBuilder, const string & theParentPath,
        StringSet * theSelectionSet)
{
    MStatus myStatus;
    MFnTransform myTransform(theObject, &myStatus);
    if (!myStatus) {
        throw ExportException("Could not cast a DagNode to MFnTransform.", PLUS_FILE_LINE);
    }

    MDagPath myPath;
    MDagPath::getAPathTo(theObject, myPath);
    string myPathName = myPath.fullPathName().asChar();
    //string myTransformId = IDGenerator::get().getNextID(theTransformBuilder.getIdPrefix());
    //_myIdMap.insert(std::make_pair(myPathName, myTransformId));

    string myTransformId = theParentTransform.appendNodeWithId(theTransformBuilder,
                                        theParentTransform.getNode());
    _myIdMap.insert(std::make_pair(myPathName, myTransformId));

    string myCurrentPath = makeCurrentPath(myTransform, theParentPath);
    DB(AC_TRACE << "SceneExporter::exportTransform(): child count = " << myTransform.childCount() << endl; );
    for (unsigned int i=0; i< myTransform.childCount(); i++) {
        MAKE_SCOPE_TIMER(exportTransform_childs_exportSubTree);
        exportSubTree(MObject(myTransform.child(i)), theTransformBuilder, theSceneBuilder,
                myCurrentPath, theSelectionSet);
    }

    if (theTransformBuilder.getNode()->childNodesLength() == 1 && !theTransformBuilder.getAssimilateFlag()) {
        const string & myNodeName = theTransformBuilder.getNode()->childNode(0)->nodeName();

        if (myNodeName != TRANSFORM_NODE_NAME && myNodeName != JOINT_NODE_NAME) {
            MAKE_SCOPE_TIMER(exportTransform_assimilation);
            string myOldId = theTransformBuilder.getNode()->getAttribute(ID_ATTRIB)->nodeValue();
            // Maya meshes don't contain transforms - they have a parent transform node instead.
            // So we take the child node and put it into the graph one level up.
            // All further operations are applied on the former child node
            theParentTransform.getNode()->removeChild(theTransformBuilder.getNode());
            theTransformBuilder.setNode(theTransformBuilder.getNode()->childNode(0));
            theParentTransform.setAssimilateFlag();
            myTransformId = theParentTransform.appendObject(theTransformBuilder);

            // replace items with assimilated child's id with new id
            PathToIdMap::iterator myIterBegin = _myIdMap.begin();
            PathToIdMap::iterator myIterEnd   = _myIdMap.end();
            for (;myIterBegin != myIterEnd; ++myIterBegin) {
                if (myIterBegin->second == myOldId) {
                    myIterBegin->second = myTransformId;
                }
            }

        }
    }

    MVector myTranslation;
    MPoint  myPivot;
    MVector myPivotTranslation;
    MVector myScalePivotTranslation;
    double  myScale[3];
    asl::Quaternionf myRotation;

    int myBillboard = 0;
    if (getCustomAttribute(theObject, "ac_billboard", myBillboard)) {
        theTransformBuilder.setBillboard(static_cast<TransformBuilderBase::BillboardType>(myBillboard));
    }

    myStatus = myTransform.getScale(myScale);
    if (myStatus) {
        // MTransformationMatrix::RotationOrder myOrder = MTransformationMatrix::kXYZ;
        // myStatus = myTransform.getRotation(myRotation, myOrder);
        double x,y,z,w;
        myStatus = myTransform.getRotationQuaternion(x,y,z,w, MSpace::kTransform);
        myRotation = asl::Quaternionf(float(x),float(y),float(z),float(-w));
    }
    if (myStatus) {
        myTranslation = myTransform.translation(MSpace::kTransform, &myStatus);
    }
    if (myStatus) {
        myPivot = myTransform.rotatePivot(MSpace::kTransform, &myStatus);
    }
    if (myStatus) {
        myPivotTranslation = myTransform.rotatePivotTranslation(MSpace::kTransform, &myStatus);
    }
    if (myStatus) {
        myScalePivotTranslation = myTransform.scalePivotTranslation(MSpace::kTransform, &myStatus);
    }

    if (myStatus != MS::kSuccess) {
        throw ExportException("Can't get scale/position/rotation/pivot/pivot-translation/scale-pivot-translation from node",
                "SceneExporter::exportTransform()");
    }

    {
        MAKE_SCOPE_TIMER(exportTransform_calc_transform);
        MPoint myPosition(myTranslation.x, myTranslation.y, myTranslation.z);
        convertToMeter(myPosition);
        convertToMeter(myPivot);
        convertToMeter(myPivotTranslation);

        // Add the pivot translation to the position, because it is just a correction factor that will not be used
        // outside maya.
        theTransformBuilder.setPosition(asl::Vector3f(float(myPivotTranslation[0]), float(myPivotTranslation[1]), float(myPivotTranslation[2])) +
            asl::Vector3f(float(myPosition.x), float(myPosition.y), float(myPosition.z)));

        theTransformBuilder.setOrientation(myRotation);
        theTransformBuilder.setScale(asl::Vector3f(float(myScale[0]), float(myScale[1]), float(myScale[2])));

        // if transform is child of a joint, do not export pivots
        if (!dynamic_cast<JointBuilder *>(&theParentTransform)) {
            theTransformBuilder.setPivot(asl::Vector3f(float(myPivot.x), float(myPivot.y), float(myPivot.z)));
        }

        // Do not set visiblity to true, if the assimilated child already set it to false
        // or if the assimilated child is a camera
        if (theTransformBuilder.getVisiblity() == true &&
                theTransformBuilder.getNode()->nodeName() != CAMERA_NODE_NAME)
        {
            theTransformBuilder.setVisiblity(isVisible(myTransform));
        }
    }
    return myTransformId;
}

void
SceneExporter::exportMesh(MFnDagNode & theDagNode,
                          WorldBuilderBase & theParentTransform,
                          const string & theParentPath,
                          StringSet * theSelectionSet)
{
    if (theDagNode.isIntermediateObject()) {
        return;
    }

    TransformBuilder* myParentTransform =
        dynamic_cast<TransformBuilder*>(&theParentTransform);
    if (!myParentTransform) {
        throw ExportException("The world may only contain transform nodes.",
                "SceneExporter::exportMesh()");
    }
    MDagPath myPath;
    MDagPath::getAPathTo(theDagNode.object(), myPath);
    string myCurrentPath = makeCurrentPath(theDagNode, theParentPath);
    if ( ! theSelectionSet || theSelectionSet->find(myCurrentPath) != theSelectionSet->end()) {
        string myPathName = myPath.fullPathName().asChar();
        string::size_type myDelimiter = theParentPath.rfind("|");
        string myBodyName = theParentPath.substr(myDelimiter + 1);
        BodyPtr myBody;

        PathToIdMap::iterator myMapIt = _myIdMap.find(myPathName);
        if (myMapIt != _myIdMap.end()) {
            std::string myShapeId = myMapIt->second;
            if ( ! myShapeId.empty()) {
                DB(AC_TRACE << "Append Body: " << myBodyName << " for shape " << myShapeId << endl;)
                myBody = Body::create(theParentTransform.getNode(), myShapeId);
                myBody->set<NameTag>(myBodyName);
                myBody->set<VisibleTag>(isVisible(theDagNode));
            } else {
                throw ExportException(std::string("Shape with path '") + myPathName + "' has empty id.",
                                      PLUS_FILE_LINE);
            }
        } else {
            throw ExportException(std::string("Can not find a shape with path '") + myPathName + "'",
                                  PLUS_FILE_LINE);
        }
        // determine if a skin cluster is connected (for skin & bones)
        MObject mySkinClusterNode;
        if (findSkinCluster(theDagNode, mySkinClusterNode)) {
            DB(AC_TRACE << "Found kSkinClusterFilter" << endl;);
            _mySkinAndBonesMap.insert(std::make_pair(myBody, mySkinClusterNode));
        }
    }
}

void
SceneExporter::resolveSkinAndBones() {
    MStatus myStatus;

    SkinAndBonesMap::iterator it=_mySkinAndBonesMap.begin();
    for (;it != _mySkinAndBonesMap.end(); ++it) {
        //dom::NodePtr myNode = it->first;
        BodyPtr myBody = it->first;
        MFnSkinCluster mySkinCluster(it->second);

        //BodyBuilder myBodyBuilder(myNode);

        MPlug myMatrixPlugs = mySkinCluster.findPlug("matrix");
        if (!myMatrixPlugs.isArray()) {
            throw ExportException(string("No joints connected to skin matrix plug '")+
                    myMatrixPlugs.name().asChar()+"' is not an array. aborting.",
                    "SceneExporter::resolveSkinAndBones()");
        }
        for (unsigned int i = 0; i < myMatrixPlugs.numElements(); ++i) {
            MPlugArray myWorldMatrixPlugs;
            myMatrixPlugs[i].connectedTo(myWorldMatrixPlugs, true, false, & myStatus);
            if (!myStatus) {
                throw ExportException(string("Could not get connected joint in skin '") +
                        mySkinCluster.name().asChar()+"' Plug:'"+myMatrixPlugs[i].name().asChar()+"'",
                        PLUS_FILE_LINE);
            }
            MFnDagNode myJoint(myWorldMatrixPlugs[0].node());
            MDagPath myPath;
            MDagPath::getAPathTo(myJoint.object(), myPath);
            string myJointPathName = myPath.fullPathName().asChar();

            PathToIdMap::iterator myMapIt = _myIdMap.find(myJointPathName);
            if (myMapIt != _myIdMap.end()) {
                std::string myJointId = myMapIt->second;
                if ( ! myJointId.empty()) {

                    //myBodyBuilder.appendJointReference(myJointId);
                    // We have to make a copy because a get returns const references.
                    vector<string> myJointIds = myBody->get<SkeletonTag>();
                    myJointIds.push_back(myJointId);
                    myBody->set<SkeletonTag>(myJointIds);
                } else {
                    throw ExportException(std::string("body with path '") + myJointPathName + "' has empty id.",
                            PLUS_FILE_LINE);
                }
            } else {
                throw ExportException(std::string("Can not find a body with path '") + myJointPathName + "'",
                        PLUS_FILE_LINE);
            }
        }
    }
}

const string
SceneExporter::exportLight(MFnDagNode & theDagNode,
                          WorldBuilderBase & theParentTransform,
                          const string & theParentPath,
                          StringSet * theSelectionSet)
{
    string myId = "";
    TransformBuilder* myParentTransform =
        dynamic_cast<TransformBuilder*>(&theParentTransform);
    if (!myParentTransform) {
        throw ExportException("The world may only contain transform nodes.",
                "SceneExporter::exportLight()");
    }
    MDagPath myPath;
    MDagPath::getAPathTo(theDagNode.object(), myPath);
    string myCurrentPath = makeCurrentPath(theDagNode, theParentPath);
    if ( ! theSelectionSet || theSelectionSet->find(myCurrentPath) != theSelectionSet->end()) {
        string myPathName = myPath.fullPathName().asChar();
        string::size_type myDelimiter = theParentPath.rfind("|");
        string myLightName = theParentPath.substr(myDelimiter + 1);
        LightBuilder myLightBuilder(_myIdMap[myPathName], myLightName);

        myId = theParentTransform.appendObject(myLightBuilder);
        myLightBuilder.setVisiblity(isVisible(theDagNode));
    }

    return myId;
}

void
SceneExporter::traverseChildren(MFnDagNode & theDagNode,
                                WorldBuilderBase & theParentTransform,
                                SceneBuilder & theSceneBuilder,
                                const std::string & theParentPath,
                                StringSet * theSelectionSet)
{
    string myCurrentPath = makeCurrentPath(theDagNode, theParentPath);
    DB(AC_TRACE << "SceneExporter::traverseChildren(): child count = " << theDagNode.childCount() << endl;);
    for (unsigned int i=0; i< theDagNode.childCount(); i++) {
        exportSubTree(theDagNode.child(i), theParentTransform, theSceneBuilder,
                myCurrentPath, theSelectionSet);
    }

}

string
SceneExporter::makeCurrentPath(const MFnDagNode & theDagNode, const string & theParentPath) {
    if (theDagNode.object().apiType() != MFn::kWorld) {
        return theParentPath + "|" + theDagNode.name().asChar();
    }
    return theParentPath;
}

const string
SceneExporter::exportCamera(const MFnDagNode & theDagNode, WorldBuilderBase & theParentTransform,
    SceneBuilder & theSceneBuilder)
{
    string myCameraId = "";
    MFnCamera myMCamera(theDagNode.object());
    std::string myCameraName = std::string(theDagNode.name().asChar());

    // Only export visible cameras, except for the default perspective camera
    if (isVisible(theDagNode) || myCameraName == "perspShape") {
        CameraBuilder myCameraBuilder(myCameraName);

        float myHFov = float(asl::degFromRad(myMCamera.horizontalFieldOfView()));
        myCameraBuilder.setHFov(myHFov);
        myCameraId = theParentTransform.appendObject(myCameraBuilder);

        // just get the default perspective camera
        if (myCameraName == "perspShape") {
            _myInitialCamera = myCameraId;
        }
    }
    return myCameraId;
}

void
SceneExporter::exportSubTree(const MObject & theNode,
                             WorldBuilderBase & theParentTransform,
                             SceneBuilder & theSceneBuilder,
                             const string & theParentPath,
                             std::set<string> * theSelectionSet)
{
    MAKE_SCOPE_TIMER(SceneExporter_exportSubTree);

    dom::NodePtr curNode;
    MStatus myStatus;
    MFnDagNode myDagNode(theNode, &myStatus);

    string myCurrentPath;
    {
        MAKE_SCOPE_TIMER(SceneExporter_makeCurrentPath);
        myCurrentPath = makeCurrentPath(myDagNode, theParentPath);
    }
    string myId = "";

    DB(AC_TRACE << "exportSubTree type = "<< theNode.apiTypeStr() << endl);

    switch (theNode.apiType()) {
        case MFn::kTransform:
        case MFn::kLookAt:
            myId = exportTransform(theNode, theParentTransform, theSceneBuilder,
                                   theParentPath, theSelectionSet);
            break;
        case MFn::kJoint:
            myId = exportJoint(theNode, theParentTransform, theSceneBuilder,
                               theParentPath, theSelectionSet);
            break;
        case MFn::kMesh:
            exportMesh(myDagNode, theParentTransform, theParentPath, theSelectionSet);
            break;
        case MFn::kNurbsCurve:
            exportCurve(myDagNode, theParentTransform, theSceneBuilder);
            break;
        case MFn::kCamera:
            myId = exportCamera(myDagNode, theParentTransform, theSceneBuilder);
            break;
        case MFn::kSpotLight:
        case MFn::kPointLight:
        case MFn::kDirectionalLight:
        case MFn::kAmbientLight:
            myId = exportLight(myDagNode, theParentTransform, theParentPath, theSelectionSet);
            break;
        case MFn::kWorld:
        default:
        {
            MAKE_SCOPE_TIMER(SceneExporter_case_kWorld_default);
            traverseChildren(myDagNode, theParentTransform, theSceneBuilder,
                theParentPath, theSelectionSet);
        }
    }

    if (myId.size()) {
        MAKE_SCOPE_TIMER(SceneExporter_animationSearch);
        AnimationExporter myAnimationExporter(theSceneBuilder);
        myAnimationExporter.exportGlobal(myDagNode, myId);
    }
}

void
SceneExporter::exportNonDagGraph(SceneBuilder & theSceneBuilder) {
    // Gather the rest of the dependency nodes
    MItDependencyNodes myNodeIt;
    for (; !myNodeIt.isDone(); myNodeIt.next()) {
        MObject myNode = myNodeIt.item();
        if (myNode.isNull()) {
            continue;
        }
        if (myNode.hasFn (MFn::kCharacter)) {
            DB(AC_TRACE << "exportCharacter() node: " << endl;)
            AnimationExporter myAnimationExporter(theSceneBuilder);
            myAnimationExporter.exportCharacter(myNode, _myIdMap);
        }
    }
}

void
SceneExporter::exportSceneHierarchy(SceneBuilder & theSceneBuilder,
                                    set<string> * theSelectionSet)
{
    DB(AC_TRACE << "SceneExporter::exportSceneHierarchy()" << endl);
    MAKE_SCOPE_TIMER(SceneExporter_exportSceneHierarchy);

    // Get the root node.
    MStatus myStatus;
    MItDag myRootIt(MItDag::kDepthFirst, MFn::kInvalid, & myStatus);
    MObject myWorld = myRootIt.item();
    WorldBuilder myWorldBuilder;
    theSceneBuilder.appendWorld(myWorldBuilder);
    exportSubTree(myWorld, myWorldBuilder, theSceneBuilder, string(""), theSelectionSet);
    resolveSkinAndBones();
    theSceneBuilder.setInitialCamera(_myInitialCamera);
}

void
SceneExporter::exportShapes(SceneBuilder & theSceneBuilder,
                         MaterialExporter & theMaterialExporter,
                         ProgressBar & theProgressBar,
                         vector<ShapeExporterPtr> & theExporters)
{
    MAKE_SCOPE_TIMER(SceneExporter_exportShapes);
    theProgressBar.setStatus("Exporting Meshes");

    unsigned myVertexCount = 0;
    for (unsigned i = 0; i < theExporters.size(); ++i) {
        if (!theExporters[i]->isInstanced()) {
            myVertexCount += theExporters[i]->getVertexCount();
        }
    }

    cerr << "Total vertex count: " << myVertexCount << endl;
    theProgressBar.setSize(myVertexCount);

    unsigned myCounter = 0;

    for (unsigned i = 0; i < theExporters.size(); ++i) {
        theExporters[i]->appendToScene(theSceneBuilder, theMaterialExporter, _myIdMap);
        if (!theExporters[i]->isInstanced()) {
            theProgressBar.advance(theExporters[i]->getVertexCount());
            myCounter += theExporters[i]->getVertexCount();
        }
    }
}

void
SceneExporter::exportAll(SceneBuilder & theSceneBuilder,
                         MaterialExporter & theMaterialExporter,
                         ProgressBar & theProgressBar)
{
    DB(AC_TRACE << "SceneExporter::exportAll()" << endl);
    MAKE_SCOPE_TIMER(SceneExporter_exportAll);

    MStatus myStatus;

    // create an iterator for only the mesh components of the DAG
    MItDag itDag(MItDag::kDepthFirst, MFn::kMesh, & myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get Dag iterator", "SceneExporter::exportAll()");
    }

    vector<ShapeExporterPtr> myShapeExporters;
    {
        MAKE_SCOPE_TIMER(collectMeshes);
        for(;!itDag.isDone(); itDag.next()) {
            //get the current DAG path
            MDagPath myDagPath;
            if (itDag.getPath(myDagPath) == MStatus::kFailure) {
                throw ExportException("Could not get Dag path", "SceneExporter::exportAll()");
            }

            //if this node is visible, then process the poly mesh it represents
            if (!isIntermediate(myDagPath)) {
                myShapeExporters.push_back(ShapeExporterPtr(new ShapeExporter(myDagPath)));
            }
        }
    }
    exportShapes(theSceneBuilder, theMaterialExporter, theProgressBar, myShapeExporters);

    theProgressBar.setStatus("Exporting Lights");
    MItDag myLightIt(MItDag::kDepthFirst, MFn::kLight, & myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get DAG iterator", "SceneExporter::exportAll()");
    }

    for (; ! myLightIt.isDone(); myLightIt.next()) {
        MAKE_SCOPE_TIMER(SceneExporter_exportLights);
        MDagPath myDagPath;
        if (myLightIt.getPath(myDagPath) == MStatus::kFailure) {
            throw ExportException("Could not get DAG path", "SceneExporter::exportAll()");
        }

        if (!isIntermediate(myDagPath)) {
            LightExporter myLightExporter(myDagPath);
            myLightExporter.appendToScene(theSceneBuilder, _myIdMap);
        }
    }

    theProgressBar.setStatus("Export scene hierarchy");
    exportSceneHierarchy(theSceneBuilder);

    // the enhanced animation nodes do not reside in the dag
    // so we iterate in an extra pass ;-)
    theProgressBar.setStatus("Exporting characters");
    exportNonDagGraph(theSceneBuilder);
}

void
SceneExporter::exportSelection(SceneBuilder & theSceneBuilder,
                               MaterialExporter & theMaterialExporter,
                               ProgressBar & theProgressBar)
{
    MStatus myStatus;

    //create an iterator for the selected mesh components of the DAG
    MSelectionList mySelectionList;
    if (MStatus::kFailure == MGlobal::getActiveSelectionList(mySelectionList)) {
        throw ExportException("Could not get active selection list",
                              "SceneExporter::exportSelection()");
    }

    MItSelectionList itSelectionList(mySelectionList, MFn::kMesh, & myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get selection list iterator",
                              "SceneExporter::exportSelection()");
    }

    // collect shapes
    set<string> mySelectionSet;
    vector<ShapeExporterPtr> myShapeExporters;
    for (itSelectionList.reset(); !itSelectionList.isDone(); itSelectionList.next()) {
        theProgressBar.advance();
        MDagPath myDagPath;

        //get the current dag path and process the poly mesh on it
        if (MStatus::kFailure == itSelectionList.getDagPath(myDagPath)) {
            throw ExportException("Could not get Dag path", "SceneExporter::exportSelection()");
        }

        myShapeExporters.push_back(ShapeExporterPtr(new ShapeExporter(myDagPath)));

        mySelectionSet.insert(string(myDagPath.fullPathName().asChar()));
    }

    exportShapes(theSceneBuilder, theMaterialExporter, theProgressBar, myShapeExporters);

    exportSceneHierarchy(theSceneBuilder, & mySelectionSet);
}

bool
SceneExporter::isIntermediate(MDagPath & theDagPath) {
    MFnDagNode myDagNode(theDagPath);
    return myDagNode.isIntermediateObject();
}



