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

#include <asl/math/linearAlgebra.h>
#include <asl/math/numeric_functions.h>
#include <y60/scene/LightSourceBuilder.h>

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
