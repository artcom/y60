//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
