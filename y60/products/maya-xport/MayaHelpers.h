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
//   $RCSfile: MayaHelpers.h,v $
//   $Author: christian $
//   $Revision: 1.14 $
//   $Date: 2005/04/13 11:24:51 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_xport_helpers_h_
#define _ac_maya_xport_helpers_h_

#include "ExportExceptions.h"

#include <maya/MItDependencyGraph.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MDistance.h>
#include <maya/MAngle.h>
#include <maya/MObject.h>
#include <maya/MTime.h>

#include <string>

class MPlug;
class MPoint;
class MVector;
class MObject;
class MFnDagNode;

void dumpAttributes(const MObject & theObject);

void dumpGraph(MObject & theObject,
               MItDependencyGraph::Direction theDirection = MItDependencyGraph::kDownstream);

void getChildPlugByName(const char * theName, const MPlug & theParent,
                        const MObject & theNode, MPlug & thePlug);

void getConnectedNode(const MPlug & thePlug, MObject & theNode, unsigned theIndex = 0);

void convertToMeter(MPoint & thePoint);
void convertToMeter(MVector & theVector);
void convertToMeter(float & theFloat);
void convertToRad(float & theFloat);
std::string getStrippedTextureFilename(const MPlug & theTexturePlug);
bool isVisible(const MFnDagNode & theDagNode);

template <class T>
bool getCustomAttribute(const MObject & theDagNode, const std::string & theAttributeName, T & myValue) {
    const MFnDependencyNode & myNode = MFnDependencyNode(theDagNode);
    MStatus myStatus;
    MObject myCustomAttribute = myNode.attribute(theAttributeName.c_str(), &myStatus);
    if (myStatus == MS::kSuccess) {
        MPlug myPlug = myNode.findPlug(myCustomAttribute, &myStatus);
        if (!myStatus || !myPlug) {
            throw ExportException(std::string("Cannot get get plug for custom attribute ") + theAttributeName,
                    "SceneExporter::exportCustomAttribute()");
        }

        myPlug.getValue(myValue);
        return true;
    }

    return false;
}

bool
findSkinCluster(const MFnDagNode & theMesh, MObject & theConnectedSkin);

void
checkMStatus(const MStatus & theStatus, const std::string & theWhat, const std::string & theWhere);

void
displayError(const std::string theMessage);

void
displayWarning(const std::string theMessage);

bool
displayProblemSummary();

inline
MTime minimum(const MTime & theOne, const MTime & theOther) {
    return theOne > theOther ? theOther : theOne;
}

inline
MTime maximum(const MTime & theOne, const MTime & theOther) {
    return theOne < theOther ? theOther : theOne;
};

#endif // _ac_maya_xport_helpers_h_
