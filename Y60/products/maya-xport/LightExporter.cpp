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
//   $RCSfile: LightExporter.cpp,v $
//   $Author: pavel $
//   $Revision: 1.6 $
//   $Date: 2005/04/24 00:41:20 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "LightExporter.h"
#include "ExportExceptions.h"

#include <asl/linearAlgebra.h>
#include <asl/numeric_functions.h>
#include <y60/LightSourceBuilder.h>

#include <maya/MGlobal.h>
#include <maya/MFnSet.h>
#include <maya/MDagPath.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MPlug.h>
#include <maya/MFnAttribute.h>
#include <maya/MFnDagNode.h>
#include <maya/MItMeshPolygon.h>

#include <maya/MFnSpotLight.h>

#include <iostream>

using namespace std;
using namespace y60;

LightExporter::LightExporter(MDagPath theDagPath) : _myInstanceNumber(0) {

    MStatus myStatus;
    _myDagPath = new MDagPath(theDagPath);
    _myLight = new MFnLight( * _myDagPath, & myStatus);

    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not create MFnLight", "LightExporter::LightExporter()");
    }
    //If the shape is instanced then we need to determine which
    //instance this path refers to.
    if (_myDagPath->isInstanced()) {
        _myInstanceNumber = _myDagPath->instanceNumber();
    }
}

LightExporter::~LightExporter() {
    if (_myDagPath != NULL) {
        delete _myDagPath;
    }
}

void
LightExporter::appendToScene(SceneBuilder & theSceneBuilder, std::map<std::string, std::string>& thePathToIdMap) {
    MDagPath firstPath;
    MDagPath::getAPathTo(_myLight->object(), firstPath);
    MStatus status;
    string myPathName = firstPath.fullPathName(&status).asChar();

    // if we already have this lightsource we are done ...
    if (thePathToIdMap.find(myPathName) != thePathToIdMap.end()) {
        return;
    }

    // export common light features
    LightSourceBuilderPtr myLightSource(new LightSourceBuilder(std::string(_myLight->name().asChar())));
    MColor myMColor = _myLight->color();
    asl::Vector4f myColor;
    myColor[0] = myMColor.r;
    myColor[1] = myMColor.g;
    myColor[2] = myMColor.b;
    myColor[3] = myMColor.a;
    float myIntensity = _myLight->intensity();
    myColor *= myIntensity;
    asl::Vector4f noColor(0.0, 0.0, 0.0, 0.0);

    if (_myLight->lightAmbient()) {
        myLightSource->setAmbient(myColor);
    } else {
        myLightSource->setAmbient(noColor);
    }

    if (_myLight->lightDiffuse()) {
        myLightSource->setDiffuse(myColor);
    } else {
        myLightSource->setDiffuse(noColor);
    }

    if (_myLight->lightSpecular()) {
        myLightSource->setSpecular(myColor);
    } else {
        myLightSource->setSpecular(noColor);
    }

    switch (_myLight->object().apiType()) {
        case MFn::kDirectionalLight:
                myLightSource->setType(DIRECTIONAL);
                break;
        case MFn::kAmbientLight:
                myLightSource->setType(AMBIENT);
                break;
        case MFn::kPointLight:
                myLightSource->setType(POSITIONAL);
                break;
        case MFn::kSpotLight:
            {
                MFnSpotLight mySpot(_myLight->object());
                myLightSource->setType(POSITIONAL);
                myLightSource->setSpotLight(float(asl::degFromRad(mySpot.coneAngle())), float(mySpot.dropOff()));
                // XXX: Workaround for a bug in maya. Spotlights claim to have no
                // diffuse or specular component.
                myLightSource->setDiffuse(myColor);
                myLightSource->setSpecular(myColor);

                break;
            }
        default:
            throw ExportException("Unknown lightsource type", PLUS_FILE_LINE);
            break;
    }

    std::string myId = theSceneBuilder.appendLightSource( * myLightSource);
    thePathToIdMap.insert(std::make_pair(myPathName, myId));
}
