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
//   $RCSfile: CurveExporter.cpp,v $
//   $Author: pavel $
//   $Revision: 1.10 $
//   $Date: 2005/04/24 00:41:20 $
//
//
//=============================================================================

#include "CurveExporter.h"
#include "MayaHelpers.h"

#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/ElementBuilder.h>
#include <y60/base/PropertyNames.h>
#include <y60/scene/WorldBuilderBase.h>
#include <y60/base/property_functions.h>
#include <y60/scene/Body.h>
#include <y60/base/DataTypes.h>

#include <asl/base/string_functions.h>
#include <asl/base/Dashboard.h>
#include <asl/dom/Nodes.h>

#include <maya/MStatus.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnNurbsCurve.h>

#include <map>
#include <list>
#include <algorithm>

#define DB(x) // x

using namespace std;
using namespace y60;

typedef map<string, string> MaterialColorMap;
MaterialColorMap ourMaterialMap;

void clearCurveMaterials() {
    ourMaterialMap.clear();
}

struct Knot {
    Knot(const MFnNurbsCurve & theCurve, double theParam) :
        param(theParam)
    {
        theCurve.getPointAtParam(theParam, point);
    }

    Knot() : param(0) {}

    MPoint point;
    double param;

    bool operator<(const Knot& that) {
        return this->param < that.param;
    }

    bool operator==(const Knot& that) {
        return this->param == that.param;
    }
};

asl::Vector4f
getLineColor(MFnDagNode & theDagNode) {
    float myR = 1.0;
    float myG = 1.0;
    float myB = 1.0;
    float myA = 1.0;

    MObject myParent = theDagNode.parent(0);
    getCustomAttribute(myParent, "ac_linecolor_r", myR);
    getCustomAttribute(myParent, "ac_linecolor_g", myG);
    getCustomAttribute(myParent, "ac_linecolor_b", myB);
    getCustomAttribute(myParent, "ac_linecolor_alpha", myA);

    return asl::Vector4f(myR, myG, myB, myA);
}

bool
refinementRequired(const MFnNurbsCurve & theCurve,
                   const Knot & k1,
                   const Knot & k2,
                   Knot & theNewKnot,
                   double theTolerance)
{
    theNewKnot.param = (k1.param + k2.param) / 2.0;
    MPoint myInterpolatedPoint = (k1.point + k2.point) / 2.0;

    theCurve.getPointAtParam(theNewKnot.param, theNewKnot.point);

    double myError = (myInterpolatedPoint - theNewKnot.point).length();
    return (myError > theTolerance);
}

void
refine(const MFnNurbsCurve & theCurve, list<Knot> & theKnotList,
    const Knot & k1, const Knot & k2, double theTolerance)
{
    /*cerr << "Refine k1: " << k1.param << " k2: " << k2.param <<
        " knotlist length: " << theKnotList.size() << endl;
       */
    Knot myNewKnot;
    bool myRefinementFlag = refinementRequired(theCurve,
        k1, k2, myNewKnot, theTolerance);

    //cerr << "refinement required: " << myRefinementFlag << endl;

    if (myRefinementFlag) {
        theKnotList.push_back(myNewKnot);
        refine(theCurve, theKnotList, k1, myNewKnot, theTolerance);
        refine(theCurve, theKnotList, myNewKnot, k2, theTolerance);
    }
}

void
exportCurve(MFnDagNode & theDagNode, WorldBuilderBase & theParentTransform, SceneBuilder & theSceneBuilder) {
    MAKE_SCOPE_TIMER(CurveExporter_exportCurve);

    if (theDagNode.isIntermediateObject()) {
        return;
    }

    string myCurveName = string(theDagNode.name().asChar());
    DB(AC_TRACE << "Exporting curve: " << myCurveName << endl);

    MFnNurbsCurve myMCurve(theDagNode.object());

    DB(
        cerr << "Number of cvs: " << myMCurve.numCVs() << endl;
        cerr << "Number of spans: " << myMCurve.numSpans() << endl;
        cerr << "Number of knots: " << myMCurve.numKnots() << endl;
        cerr << "Degree: " << myMCurve.degree() << endl;
        cerr << "Length: " << myMCurve.length() << endl;

        for (double d = 0.0; d <= 1.0; d += 0.1) {
            MPoint myPoint;
            myMCurve.getPointAtParam(d, myPoint);
            cerr << "Point at param " << d << ": " << myPoint.x << ", " << myPoint.y << ", " << myPoint.z << endl;
        }

        int myCvCount = myMCurve.numCVs();
        for (int i = 0; i < myCvCount; ++i) {
            MPoint myPoint;
            myMCurve.getCV(i, myPoint);
            cerr << "CV " << i << ": " << myPoint.x << ", " << myPoint.y << ", " << myPoint.z << endl;
        }

        int myKnotCount = myMCurve.numKnots();
        for (int i = 0; i < myKnotCount; ++i) {
            double knotParam;
            knotParam = myMCurve.knot(i);

            MPoint myPoint;
            myMCurve.getPointAtParam(knotParam, myPoint);
            cerr << "Point at knot# " << i << ": " << myPoint.x << ", " << myPoint.y << ", " << myPoint.z << endl;

        }
    )

    double myTolerance = myMCurve.length() * 0.001;

    list<Knot> myKnots;
    Knot myKnot(myMCurve, myMCurve.knot(0));
    myKnots.push_back(myKnot);

    int myNumberOfKnots = myMCurve.numKnots();
    for (int i = 1; i < myNumberOfKnots; ++i) {
        Knot myNextKnot(myMCurve, myMCurve.knot(i));
        refine(myMCurve, myKnots, myKnot, myNextKnot, myTolerance);

        myKnots.push_back(myNextKnot);
        myKnot = myNextKnot;
    }

    myKnots.sort();

    // Create unlit material for curve
    string myMaterialId = "";
    asl::Vector4f myLineColor = getLineColor(theDagNode);
    float myLineWidth = 1.0;
    MString myBlendFunction = "[src_alpha, one_minus_src_alpha]";
    getCustomAttribute(theDagNode.parent(0), "ac_linewidth", myLineWidth);
    getCustomAttribute(theDagNode.parent(0), "ac_blendfunc", myBlendFunction);

    string myMaterialKey = asl::as_string(myLineColor) + asl::as_string(myLineWidth) + myBlendFunction.asChar();

    MaterialColorMap::iterator myIt = ourMaterialMap.find(myMaterialKey);
    if (myIt == ourMaterialMap.end()) {
        MaterialBuilder myMaterialBuilder(string("M") + myCurveName, false);
        myMaterialId = theSceneBuilder.appendMaterial(myMaterialBuilder);
        VectorOfRankedFeature myLightingFeature;
        createLightingFeature(myLightingFeature, UNLIT);
        myMaterialBuilder.setType(myLightingFeature);
        myMaterialBuilder.setTransparencyFlag(myLineColor[3] != 1);

        setPropertyValue<asl::Vector4f>(myMaterialBuilder.getNode(), "vector4f",
                        SURFACE_COLOR_PROPERTY, myLineColor);
        setPropertyValue<float>(myMaterialBuilder.getNode(), "float",
                        LINEWIDTH_PROPERTY, myLineWidth);
        setPropertyValue<VectorOfBlendFunction>(myMaterialBuilder.getNode(), "vectorofblendfunction",
                        BLENDFUNCTION_PROPERTY, asl::as<VectorOfBlendFunction>(myBlendFunction.asChar()));
        ourMaterialMap[myMaterialKey] = myMaterialId;
    } else {
        myMaterialId = myIt->second;
    }

    // Build curve shape
    ShapeBuilder myShapeBuilder(myCurveName);
    string myShapeId = theSceneBuilder.appendShape(myShapeBuilder);

    unsigned myVertexCount = myMCurve.numKnots();
    if (myVertexCount == 0) {
        throw ExportException(std::string("No points found in curve") + myCurveName, "CurveExporter::exportCurve()");
    }

    myShapeBuilder.createVertexDataBin<asl::Vector3f>(POSITION_ROLE, myVertexCount);
    myShapeBuilder.createVertexDataBin<asl::Vector3f>(NORMAL_ROLE, 1);
    myShapeBuilder.appendVertexData(NORMAL_ROLE, asl::Vector3f(1, 0, 0));

    ElementBuilder myElementBuilder(PrimitiveTypeStrings[LINE_STRIP], myMaterialId);
    myElementBuilder.createIndex(POSITION_ROLE, POSITIONS, myVertexCount);
    myElementBuilder.createIndex(NORMAL_ROLE, NORMALS, 1);

    list<Knot>::iterator myEnd = myKnots.end();
    unsigned myCounter = 0;
    for (list<Knot>::iterator it = myKnots.begin(); it != myEnd; ++it) {
        MPoint & myPoint = it->point;
        convertToMeter(myPoint);
        myShapeBuilder.appendVertexData(POSITION_ROLE, asl::Vector3f(float(myPoint.x), float(myPoint.y), float(myPoint.z)));
        myElementBuilder.appendIndex(POSITIONS, myCounter);
        myElementBuilder.appendIndex(NORMALS, 0);
        ++myCounter;
    }

    myShapeBuilder.appendElements(myElementBuilder);

    // Build body
    BodyPtr myBody = Body::create(theParentTransform.getNode(), myShapeId);
    myBody->set<NameTag>(myCurveName);
    myBody->set<VisibleTag>(isVisible(theDagNode));
}
