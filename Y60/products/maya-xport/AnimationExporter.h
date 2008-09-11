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

#include <y60/base/iostream_functions.h>
#include <y60/scene/AnimationBuilder.h>
#include <y60/scene/SceneBuilder.h>
#include <asl/base/Dashboard.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnLambertShader.h>
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
                         const std::string & theTypeName,
                         bool thePropertyFlag = false):
                _myY60AttributeName(theY60AttributeName),
                _myTypeName(theTypeName),
                _myConvertFunc(theConvertFunc),
                _myPropertyFlag( thePropertyFlag ) {}
        const std::string   _myY60AttributeName;
        const std::string   _myTypeName;
        ConvertFunc         _myConvertFunc;
        bool                _myPropertyFlag;
};
typedef asl::Ptr<ValueDescription> ValueDescriptionPtr;
typedef std::map<std::string, ValueDescriptionPtr> MayaAttribDescriptionMap;

class AnimationExporter {
    public:
        AnimationExporter(y60::SceneBuilder & theSceneBuilder);
        virtual ~AnimationExporter();

        void exportLambertFeatures(const MFnLambertShader & theDagNode,
                                   const std::string & theNodeId);
        void exportGlobal(const MFnDagNode & theDagNode, const std::string & theNodeId);
        void exportCharacter(const MObject & theNode, std::map<std::string, std::string> & theIdMap);

    private:
        template <class T, class MayaNodeType>
        void exportAnimation(const MayaNodeType & theDagNode,
                             const std::string & theParameterName,
                             const std::string & theNodeId,
                             const std::string & theAttributeName,
                             ConvertFunc theConvertFunc,
                             bool thePropertyFlag = false);
        template <class T, class MayaNodeType>
        bool exportCurve(y60::AnimationBuilder & theAnimBuilder,
                         const MFnAnimCurve & theAnimCurve,
                         const MayaNodeType & theDagNode,
                         const std::string & theParameterName,
                         const std::string & theNodeId,
                         const std::string & theAttributeName,
                         ConvertFunc theConvertFunc,
                         bool thePropertyFlag);
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
                          MTime theDuration,
                          bool thePropertyFlag);
        template <class MayaNodeType>
        MObject getAnimationNode(const MayaNodeType & theDagNode,
                                 const std::string & theParameterName);
                                 
        asl::Vector3f getInitialOrientation(const MFnDagNode & theDagNode);
                                 

        void printAnimationInfo(const MFnAnimCurve & theAnimation);
        y60::SceneBuilder & _mySceneBuilder;
        MayaAttribDescriptionMap _myMayaAttribDescriptionMap;
};

#undef DB

#endif
