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
//   $RCSfile: MayaHelpers.cpp,v $
//   $Author: christian $
//   $Revision: 1.21 $
//   $Date: 2005/04/13 17:23:23 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "MayaHelpers.h"
#include "ExportExceptions.h"

#include <asl/base/string_functions.h>

#include <maya/MGlobal.h>
#include <maya/MPlug.h>
#include <maya/MObject.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnAttribute.h>
#include <maya/MItDependencyGraph.h>
#include <iostream>

using namespace std;

void
checkMStatus(const MStatus & theStatus, const std::string & theWhat, const std::string & theWhere) {
    if (!theStatus) {
        throw MayaException(theWhat, theWhere);
    }
}

bool
isVisible(const MFnDagNode & theDagNode) {
    MStatus myStatus;
    MPlug myVisibilityPlug = theDagNode.findPlug("visibility", & myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not find visibility plug", "SceneExporter::isVisible()");
    }

    bool isVisible;
    myStatus = myVisibilityPlug.getValue(isVisible);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get value from visibility plug",
                              "SceneExporter::isVisible()");
    }

    return isVisible;
}



void
dumpAttributes(const MObject & theObject) {
    const MFnDependencyNode & myNode = MFnDependencyNode(theObject);
    MStatus myStatus;

    cerr << "--- Attributes: --- " << endl;
    for (unsigned int i = 0; i < myNode.attributeCount(&myStatus); ++i) {
        MObject myAttribute = myNode.attribute(i, &myStatus);
        if (!myStatus) {
            cerr << "#### attribute Error" << endl;
            continue;
        }
        MString myName = MFnAttribute(myAttribute,&myStatus).name();
        if (!myStatus) {
            cerr << "#### attribute name Error" << endl;
            continue;
        }
        MPlug myPlug = myNode.findPlug(myAttribute,&myStatus);
        if (!myStatus) {
            cerr << "#### plug find Error" << endl;
            continue;
        }
        if (!myPlug) {
            cerr << "#### plug  Error" << endl;
            continue;
        }
         //MString myPartialPlugName = myPlug.partialName(true,true,true,true,true,true,&myStatus);
        cerr << "Attribute " << i << " name = '" << myName.asChar() << "', plug name = '" << myPlug.name().asChar()
             << "'";
             //<< "', partialname='" << myPartialPlugName << "'";

        // Not quite nice but rare
        MObject myObject;
        double  myDoubleValue;
        char    myCharValue;
        MString myStringValue;

        if (!myPlug.isArray() ) {
            cerr << ", apiType='" << myAttribute.apiTypeStr() << "'";
            if (myPlug.getValue(myStringValue)) {
                cerr << ", type='MString', value='" << myStringValue.asChar() << "'";
            } else if (myPlug.getValue(myDoubleValue)) {
                cerr << ", type='double' value='" << myDoubleValue << "'";
            } else if (myPlug.getValue(myCharValue)) {
                cerr << ", type='char' value='" << myCharValue << "'";
            } else if (myPlug.getValue(myObject)) {
                cerr << ", MObject type='" << myObject.apiTypeStr() << "'";
            }
            cerr << endl;
        } else {
            cerr << endl;

            MPlugArray myPlugs;
            myPlug.connectedTo(myPlugs, true, false, & myStatus);
            if (!myStatus) {
                cerr << "#### Error: Could not find connected plugs" << endl;
            }

            for (unsigned j = 0; j < myPlugs.length(); ++j) {
                cerr << " - Connected to " << myPlugs[j].name().asChar() << endl;
            }

            if (myPlug.isArray()) {
                cerr << myPlug.name().asChar() << " is array with "
                    << myPlug.numElements() << " elements" << endl;
                for (unsigned j = 0; j < myPlug.numElements(); ++j) {
                    if (myPlug[j].name() != "" ) {
                        cerr << " --- " << myPlug[j].name().asChar() << " num children: "
                            << myPlug[j].numChildren() << endl;
                    }
                    for (unsigned k = 0; k < myPlug[j].numChildren(); ++k) {
                        MPlug mySubPlug = myPlug[j].child(k);
                        cerr << " ------ " << mySubPlug.name().asChar();

                        double mySubPlugValue;
                        if (mySubPlug.getValue(mySubPlugValue)) {
                            cerr << " value: " << mySubPlugValue;
                        }
                        cerr << endl;
                    }
                }
            }
        }
    }
}

void
dumpGraph(MObject & theObject, MItDependencyGraph::Direction theDirection) {
    MStatus myStatus;
	MItDependencyGraph itDG(theObject, MFn::kInvalid,
								theDirection,
								MItDependencyGraph::kBreadthFirst,
								MItDependencyGraph::kNodeLevel,
								&myStatus);
    while (!itDG.isDone()) {
        cerr << "------- Attributes of Node type '" << itDG.thisNode().apiTypeStr() << "' --------" << endl;
        dumpAttributes(itDG.thisNode());
        itDG.next();
    }
}

void
getChildPlugByName(const char * theName, const MPlug & theParent,
                                        const MObject & theNode, MPlug & thePlug)
{
    MStatus myStatus;
    MObject myAttribute = MFnDependencyNode(theNode).attribute(theName, & myStatus);

    if (!myStatus) {
        throw ExportException(std::string("Could not get '") + theName + "' attribute",
                              "getChildPlugByName()");
    }

    thePlug = theParent.child(myAttribute, & myStatus);

    if (!myStatus) {
        throw ExportException(std::string("Could not get '") + theName + "' plug",
                              "getChildPlugByName()");
    }
}

void
getConnectedNode(const MPlug & thePlug, MObject & theNode, unsigned theIndex) {
    MStatus myStatus;
    MPlugArray myPlugArray;
    thePlug.connectedTo(myPlugArray, true, false, & myStatus);

    if (!myStatus) {
        throw ExportException("Could not get connected plugs",
                "getConnectedNode()");
    }
    unsigned myLength = myPlugArray.length();
    if (myLength <= theIndex) {
        throw ExportException(std::string("Could not find connected node with index ") +
                asl::as_string(theIndex), "getConnectedNode()");
    }

    theNode = myPlugArray[theIndex].node();
}

void convertToMeter(MPoint & thePoint) {
    // remember UI-Unitsetting
    MDistance::Unit myUIUnit = MDistance::uiUnit();
    // set UI-unit to target unit
    MDistance::setUIUnit(MDistance::kMeters);

    // convert point to setting
    MDistance::internalToUI(thePoint.x);
    thePoint.x /= 100.0;
    MDistance::internalToUI(thePoint.y);
    thePoint.y /= 100.0;
    MDistance::internalToUI(thePoint.z);
    thePoint.z /= 100.0;
    // reset to former unit
    MDistance::setUIUnit(myUIUnit);
}

void convertToMeter(MVector & theVector) {
    MDistance::Unit myUIUnit = MDistance::uiUnit();
    MDistance::setUIUnit(MDistance::kMeters);
    MDistance::internalToUI(theVector.x);
    MDistance::internalToUI(theVector.y);
    MDistance::internalToUI(theVector.z);
    theVector.x /= 100.0;
    theVector.y /= 100.0;
    theVector.z /= 100.0;
    MDistance::setUIUnit(myUIUnit);
}

void convertToMeter(float & theFloat) {
    // remember UI-Unitsetting
    MDistance::Unit myUIUnit = MDistance::uiUnit();
    // set UI-unit to target unit
    MDistance::setUIUnit(MDistance::kMeters);

    // convert float to setting
    MDistance::internalToUI(theFloat);
    theFloat /= 100.0;

    // reset to former unit
    MDistance::setUIUnit(myUIUnit);
}

void convertToRad(float & theFloat) {
    MAngle::Unit myUIUnit = MAngle::uiUnit();
    MAngle::setUIUnit(MAngle::kRadians);

    // convert float to setting
    MAngle::internalToUI(theFloat);

    // reset to former unit
    MAngle::setUIUnit(myUIUnit);
}

bool
findSkinCluster(const MFnDagNode & theMesh, MObject & theConnectedSkin) {
    MStatus myStatus;

    MObject myObject(theMesh.object());
    MItDependencyGraph myDGIt(myObject, MFn::kSkinClusterFilter, MItDependencyGraph::kUpstream,
        MItDependencyGraph::kDepthFirst, MItDependencyGraph::kNodeLevel, &myStatus);

    if (myStatus == MS::kInvalidParameter) {
        displayWarning("Invalid parameter for MItDependencyGraph in findSkinCluster()");
    }

    if (myStatus == MS::kFailure || myDGIt.isDone()) {
        return false;
    }

    myDGIt.disablePruningOnFilter();

    theConnectedSkin = myDGIt.thisNode();

    myDGIt.next();
    if (!myDGIt.isDone()) {
        displayWarning(string("Mesh ") + theMesh.name().asChar() + " has more then one skincluster" +
            " ignoring all, but the first.");
    }

    return true;
}

vector<string> ourWarnings;
vector<string> ourErrors;

void
displayError(const std::string theMessage) {
    string myError = string("### ERROR ") + theMessage;
    cerr << myError << endl;
    MGlobal::displayInfo(MString(myError.c_str()));
    ourErrors.push_back(theMessage);
}

void
displayWarning(const std::string theMessage) {
    string myWarning = string("### WARNING ") + theMessage;
    cerr << myWarning << endl;
    MGlobal::displayWarning(MString(myWarning.c_str()));
    ourWarnings.push_back(theMessage);
}

// This can only be called once per export, because we want the errors/warnings to be cleared for the
// next export.
bool
displayProblemSummary() {
    if (ourWarnings.size() || ourErrors.size()) {
        cerr << "+------------------------------------   Summary of Problems   -------------------------------------" << endl;
        if (ourWarnings.size()) {
            cerr << "| WARNINGS:" << endl;
            for (unsigned i = 0; i < ourWarnings.size(); ++i) {
                cerr << "| " << ourWarnings[i] << endl;
            }
        }
        if (ourErrors.size()) {
            cerr << "| ERRORS:" << endl;
            for (unsigned i = 0; i < ourErrors.size(); ++i) {
                cerr << "| " << ourErrors[i] << endl;
            }
        }
        cerr << "+--------------------------------------------------------------------------------------------------" << endl;

        ourWarnings.clear();
        ourErrors.clear();
        return true;
    } else {
        return false;
    }
}

