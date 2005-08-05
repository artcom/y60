//=============================================================================
// Copyright (C) 2000-2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: AnimationExporter.h,v $
//   $Author: pavel $
//   $Revision: 1.20 $
//   $Date: 2005/04/24 00:41:20 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_AnimationExporter_h_
#define _ac_maya_AnimationExporter_h_

#include <y60/iostream_functions.h>
#include <y60/AnimationBuilder.h>
#include <y60/SceneBuilder.h>
#include <asl/Dashboard.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnCamera.h>

#include <string>
#include <vector>
#include <set>
#include <map>

class MFnDagNode;
class MObject;

#undef DB
#define DB(x) // x

namespace y60 {
    class SceneBuilder;
}
class SceneExporter;

typedef void (*ConvertFunc)(float &);
typedef asl::Ptr<MFnAnimCurve> MFnAnimCurvePtr;

struct RotationContainer {
    MFnAnimCurvePtr myRotationX;
    MFnAnimCurvePtr myRotationY;
    MFnAnimCurvePtr myRotationZ;
    asl::Vector3f myInitialEuler;
    RotationContainer(asl::Vector3f theInitialEuler) : myRotationX(0), myRotationY(0), myRotationZ(0),
                          myInitialEuler(theInitialEuler) {}
                          
    RotationContainer() : myRotationX(0), myRotationY(0), myRotationZ(0),
                          myInitialEuler(asl::Vector3f(0,0,0)) {}
};

typedef std::map<std::string, RotationContainer> RotationMap;


struct ValueDescription {
    public:
        ValueDescription(const std::string & theY60AttributeName,
                         ConvertFunc theConvertFunc,
                         const std::string & theTypeName):
                _myY60AttributeName(theY60AttributeName),
                _myTypeName(theTypeName),
                _myConvertFunc(theConvertFunc) {}
        const std::string   _myY60AttributeName;
        const std::string   _myTypeName;
        ConvertFunc         _myConvertFunc;
};
typedef asl::Ptr<ValueDescription> ValueDescriptionPtr;
typedef std::map<std::string, ValueDescriptionPtr> MayaAttribDescriptionMap;

class AnimationExporter {
    public:
        AnimationExporter(y60::SceneBuilder & theSceneBuilder);
        virtual ~AnimationExporter();

        void exportGlobal(const MFnDagNode & theDagNode, const std::string & theNodeId);
        void exportCharacter(const MObject & theNode, std::map<std::string, std::string> & theIdMap);

    private:
        template <class T>
        void exportAnimation(const MFnDagNode & theDagNode,
                             const std::string & theParameterName,
                             const std::string & theNodeId,
                             const std::string & theAttributeName,
                             ConvertFunc theConvertFunc);
        template <class T>
        bool exportCurve(y60::AnimationBuilder & theAnimBuilder,
                         const MFnAnimCurve & theAnimCurve,
                         const MFnDagNode & theDagNode,
                         const std::string & theParameterName,
                         const std::string & theNodeId,
                         const std::string & theAttributeName,
                         ConvertFunc theConvertFunc);
        void
        exportRotatingAnimation(const MFnDagNode & theDagNode, 
                         const std::string & theNodeId);
        void
        exportQuaternionAnimation(y60::AnimationBuilder & theAnimBuilder,
                                  MFnAnimCurvePtr theAnimCurveX,
                                  MFnAnimCurvePtr theAnimCurveY,
                                  MFnAnimCurvePtr theAnimCurveZ,
                                  MTime & theStartTime,
                                  MTime & theEndTime,
                                  const asl::Vector3f & theInitialEuler,
                                  const std::string & theNodeId,
                                  const std::string & theAttributeName,
                                  const MFnAnimCurve::InfinityType & thePostInfinity,
                                  const std::string & theAnimCurveName);
        bool
        exportCurve(y60::AnimationBuilder & theAnimBuilder,
                    const MObject & theAnimNodeX,
                    const MObject & theAnimNodeY,
                    const MObject & theAnimNodeZ,
                    const asl::Vector3f & theInitialEuler,
                    const std::string & theNodeId,
                    const std::string & theAttributeName);
        template <class T>
        void
        exportSampledData(y60::AnimationBuilder & theAnimBuilder,
                          const std::vector<T> & theValues,
                          const std::string & theCurveName,
                          const MFnAnimCurve::InfinityType & thePostInfinity,
                          const std::string & theNodeId,
                          const std::string & theAttributeName,
                          MTime theStartTime,
                          MTime theDuration);
        MObject getAnimationNode(const MFnDagNode & theDagNode,
                                 const std::string & theParameterName);
                                 
        asl::Vector3f getInitialOrientation(const MFnDagNode & theDagNode);
                                 

        void printAnimationInfo(const MFnAnimCurve & theAnimation);
        y60::SceneBuilder & _mySceneBuilder;
        MayaAttribDescriptionMap _myMayaAttribDescriptionMap;
};

template <class T>
bool
AnimationExporter::exportCurve(y60::AnimationBuilder & theAnimBuilder,
                               const MFnAnimCurve & theAnimCurve,
                               const MFnDagNode & theDagNode,
                               const std::string & theParameterName,
                               const std::string & theNodeId,
                               const std::string & theAttributeName,
                               ConvertFunc theConvertFunc)
{
    std::string myAnimCurveName = theAnimCurve.name().asChar();
    // We reject animations with empty names, because we do not know where they come from
    // TODO, find what is happening here!
    if (myAnimCurveName == "") {
        return false;
    }

    DB(AC_TRACE << "Exporting animation curve: " << myAnimCurveName << std::endl);
    
    MTime myEndTime    = theAnimCurve.time(theAnimCurve.numKeys()-1);
    MTime myStartTime  = theAnimCurve.time(0);
    MTime myDuration   = myEndTime - myStartTime;

    DB(AC_TRACE << " start: " << myStartTime << ", end " << myEndTime << std::endl);
        
    MTime mySampleTime = myStartTime;
    // mySampleTime.setUnit(ourSamplingUnit);
    mySampleTime.setUnit(MTime::uiUnit());
    std::vector<T> myValues;
    unsigned mySampleCount = static_cast<unsigned>((myEndTime - myStartTime).value());
    myValues.reserve(mySampleCount);

    // Focal length animation needs special treatment, because it is not
    // trivial to convert focal length to hfov
    if (theParameterName == "focalLength") {
        MAKE_SCOPE_TIMER(exportAnimation_loop_focalLength);
        MFnCamera myCamera(theDagNode.object());
        double myOriginalFocalLength = myCamera.focalLength();
        for (; mySampleTime <= myEndTime; mySampleTime++) {
            double myFocalLength = theAnimCurve.evaluate(mySampleTime);

            // Set the animated focal length
            myCamera.setFocalLength(myFocalLength);

            // Retrieve hfov from camera
            float myValue = float(asl::degFromRad(myCamera.horizontalFieldOfView()));
            myValues.push_back(myValue);
        }
        myCamera.setFocalLength(myOriginalFocalLength);
    } else {
        MAKE_SCOPE_TIMER(exportAnimation_loop);
        for (; mySampleTime <= myEndTime; mySampleTime++) {
            START_TIMER(exportAnimation_loop_evaluate);
            float myValue = float(theAnimCurve.evaluate(mySampleTime));
            DB(AC_TRACE << "====== Sample @ " << mySampleTime << " is " << myValue << std::endl);
            STOP_TIMER(exportAnimation_loop_evaluate);
            START_TIMER(exportAnimation_theConvertFunc);
            theConvertFunc(myValue);
            STOP_TIMER(exportAnimation_theConvertFunc);
            MAKE_SCOPE_TIMER(exportAnimation_loop_push_back);
            myValues.push_back(static_cast<T>(myValue));
        }
    }
    exportSampledData(theAnimBuilder, myValues, theAnimCurve.name().asChar(), 
            theAnimCurve.postInfinityType(), theNodeId, theAttributeName, 
            myStartTime, myDuration);
    return true;
}

template <class T>
void
AnimationExporter::exportSampledData(y60::AnimationBuilder & theAnimBuilder,
                                     const std::vector<T> & theValues,
                                     const std::string & theCurveName,
                                     const MFnAnimCurve::InfinityType & thePostInfinity,
                                     const std::string & theNodeId,
                                     const std::string & theAttributeName,
                                     MTime theStartTime,
                                     MTime theDuration) 
{
    START_TIMER(exportAnimation_myAnimationBuilder);
    theAnimBuilder.setName(theCurveName);
    theAnimBuilder.setNodeRef(theNodeId);
    theAnimBuilder.setAttribute(theAttributeName);
    theAnimBuilder.setDuration(float(theDuration.as(MTime::kSeconds)));
    STOP_TIMER(exportAnimation_myAnimationBuilder);

    theAnimBuilder.setBegin(float(theStartTime.as(MTime::kSeconds)));

    START_TIMER(exportAnimation_myAnimationBuilder_setCount);
    // MFnAnimCurve::InfinityType myPostInfinity = theAnimCurve.postInfinityType();
    if (thePostInfinity == MFnAnimCurve::kCycle) {
        theAnimBuilder.setCount(0);
    } else if (thePostInfinity == MFnAnimCurve::kOscillate) {
        theAnimBuilder.setDirection(y60::PONG);
        theAnimBuilder.setCount(0);
    }
    STOP_TIMER(exportAnimation_myAnimationBuilder_setCount);

    START_TIMER(exportAnimation_appendValues);
    theAnimBuilder.setBegin(float(theStartTime.as(MTime::kSeconds)));
    theAnimBuilder.appendValues(theValues);
    STOP_TIMER(exportAnimation_appendValues);
}


template <class T>
void
AnimationExporter::exportAnimation(const MFnDagNode & theDagNode,
                                   const std::string & theParameterName,
                                   const std::string & theNodeId,
                                   const std::string & theAttributeName,
                                   ConvertFunc theConvertFunc)
{
    MAKE_SCOPE_TIMER(AnimationExporter_exportAnimation);
    MObject myNode = getAnimationNode(theDagNode, theParameterName);
    if (!myNode.isNull()) {
        DB(AC_TRACE << "animation node found for " << theParameterName << endl);
        MStatus myStatus;
        MFnAnimCurve myAnimCurve(myNode, &myStatus);
        if (myStatus == MStatus::kFailure) {
            return;
        }
        y60::AnimationBuilder myAnimationBuilder;
        std::string myId = _mySceneBuilder.appendAnimation(myAnimationBuilder);
        bool success = exportCurve<T>(myAnimationBuilder, myAnimCurve, theDagNode,
                       theParameterName, theNodeId, theAttributeName, theConvertFunc);
        if (!success) {
            _mySceneBuilder.removeNodeById(_mySceneBuilder.getNode()->childNode(y60::ANIMATION_LIST_NAME), myId);
        }
    }
}
#undef DB

#endif
