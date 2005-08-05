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
//   $RCSfile: SceneExporter.h,v $
//   $Author: christian $
//   $Revision: 1.33 $
//   $Date: 2005/04/18 16:42:18 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_SceneExporter_h_
#define _ac_maya_SceneExporter_h_

#include "AnimationExporter.h"
#include "ShapeExporter.h"

#include <y60/SceneBuilder.h>
#include <y60/Body.h>
#include <dom/Nodes.h>

#include <maya/MObject.h>
#include <maya/MPxFileTranslator.h>

#include <set>

class MaterialExporter;
class MDagPath;
class MFnDagNode;
class ProgressBar;

namespace y60 {
    class WorldBuilderBase;
    class SceneBuilder;
    class TransformBuilderBase;
}

#if WIN32
#   define DLL_EXPORT __declspec(dllexport)
#else
#   define DLL_EXPORT
#endif

DLL_EXPORT MStatus initializePlugin(MObject theObject);
DLL_EXPORT MStatus uninitializePlugin(MObject theObject);

typedef std::map<y60::BodyPtr, MObject> SkinAndBonesMap;
typedef std::map<std::string, std::string> PathToIdMap;

class SceneExporter : public MPxFileTranslator {
    public:
        SceneExporter();
        virtual ~SceneExporter();

        static void * creator();

        MStatus writer (const MFileObject& theFile,
                        const MString& theOptionsString,
                        MPxFileTranslator::FileAccessMode theMode);

        bool haveWriteMethod () const;
        bool haveReadMethod () const;
        bool canBeOpened () const;

        MString defaultExtension () const;

    private:
        struct ExportOptions {
            ExportOptions() : binaryFlag(false), inlineTexturesFlag(true), enableProgressBar(true) {}
            bool binaryFlag;
            bool inlineTexturesFlag;
            bool enableProgressBar;
        };

        typedef std::set<std::string> StringSet;
        bool isIntermediate(MDagPath & theDagPath);

        ExportOptions parseOptions(const MString & theOptionString);


        void exportAll(y60::SceneBuilder & theSceneBuilder,
                       MaterialExporter & theMaterialExporter,
                       ProgressBar & theProgressBar);
        void exportSelection(y60::SceneBuilder & theSceneBuilder,
                             MaterialExporter & theMaterialExporter,
                             ProgressBar & theProgressBar);
        void exportShapes(y60::SceneBuilder & theSceneBuilder,
                          MaterialExporter & theMaterialExporter,
                          ProgressBar & theProgressBar,
                          std::vector<ShapeExporterPtr> & theExporters);
        void exportSubTree(const MObject& theNode,
                           y60::WorldBuilderBase & theParentTransform,
                           y60::SceneBuilder & theSceneBuilder,
                           const std::string & theParentPath,
                           StringSet * theSelectionSet = 0);

        void exportSceneHierarchy(y60::SceneBuilder & theSceneBuilder,
                                  StringSet * theSelectionSet = 0);

        void exportNonDagGraph(y60::SceneBuilder & theSceneBuilder);

        void exportMesh(MFnDagNode & theDagNode, y60::WorldBuilderBase & theParentTransform,
                        const std::string & theParentPath, StringSet * theSelectionSet);

        const std::string exportCamera(const MFnDagNode & theDagNode, y60::WorldBuilderBase & theParentTransform,
                                       y60::SceneBuilder & theSceneBuilder);

        const std::string exportJoint(const MObject & theObject,
                             y60::WorldBuilderBase & theParentTransform,
                             y60::SceneBuilder & theSceneBuilder,
                             const std::string & theParentPath,
                             StringSet * theSelectionSet);
        const std::string exportTransform(const MObject & theObject,
                             y60::WorldBuilderBase & theParentTransform,
                             y60::SceneBuilder & theSceneBuilder,
                             const std::string & theParentPath,
                             StringSet * theSelectionSet);
        const std::string exportTransformBase(const MObject & theObject,
                             y60::TransformBuilderBase & theTransformBuilder,
                             y60::WorldBuilderBase & theParentTransform,
                             y60::SceneBuilder & theSceneBuilder,
                             const std::string & theParentPath,
                             StringSet * theSelectionSet);

        const std::string exportLight(MFnDagNode & theDagNode, y60::WorldBuilderBase & theParentTransform,
                         const std::string & theParentPath, StringSet * theSelectionSet);

        void traverseChildren(MFnDagNode & theDagNode,
                              y60::WorldBuilderBase & theParentTransform,
                              y60::SceneBuilder & theSceneBuilder,
                              const std::string & theParentPath,
                              StringSet * theSelectionSet);
        void resolveSkinAndBones();

        std::string makeCurrentPath(const MFnDagNode & theDagNode,
                                    const std::string & theParentPath);


        PathToIdMap                        _myIdMap;
        SkinAndBonesMap                    _mySkinAndBonesMap;

        std::string                        _myInitialCamera;
};

#endif // _ac_maya_SceneExporter_h_
