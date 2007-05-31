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
//   $RCSfile: SceneExporter.h,v $
//   $Author: ulrich $
//   $Revision: 1.25 $
//   $Date: 2005/04/29 08:46:12 $
//
//  Description: This class implements a exporter plugin for Cinema 4D.
//
//=============================================================================

#ifndef _ac_c4d_SceneExporter_h_
#define _ac_c4d_SceneExporter_h_


#include <stddef.h>
#include <string>

#include <y60/SceneBuilder.h>
#include <y60/ShapeBuilder.h>
#include <y60/LightBuilder.h>
#include <y60/WorldBuilder.h>
#include <y60/TransformBuilder.h>
#include <y60/CameraBuilder.h>

#include "c4d_include.h"

class ShapeExporter;
class MaterialExporter;

class SceneExporter : public SceneSaverData {
	public:
        DEFINE_NESTED_EXCEPTION(SceneExporter,Exception,asl::Exception);
        DEFINE_NESTED_EXCEPTION(SceneExporter,IOError,Exception);
        DEFINE_NESTED_EXCEPTION(SceneExporter,OpenFailed,IOError);
        DEFINE_NESTED_EXCEPTION(SceneExporter,ReadFailed,IOError);
        DEFINE_NESTED_EXCEPTION(SceneExporter,WriteFailed,IOError);

        SceneExporter(bool theBinaryFlag);
	    ~SceneExporter(void);

		virtual LONG Save(PluginSceneSaver * theNode, const Filename & theFileName,
                          BaseDocument * theDocument, LONG theFlags);

		static NodeData * Alloc(void) {
		    return gNew SceneExporter(false);
		}

		static NodeData * AllocBinary(void) {
		    return gNew SceneExporter(true);
		}

    private:
        unsigned countObjects(BaseObject * theNode);

        BaseObject * polygonizeObject(BaseObject * theNode);

        bool writeObject(y60::WorldBuilderBasePtr theTransformBuilder,
  	                     BaseObject * & theNode,
                         y60::WorldBuilderBasePtr & theParentBuilder,
                         bool theTransformFlag = true,
                         bool theForceFrontBackFacing = false,
                         const std::string & theNameAppendix = "");

        void writeObjects(y60::WorldBuilderBasePtr theTransformBuilder,
	                      BaseObject * theNode,
                          bool theTransformFlag = true,
                          bool theForceFrontBackFacing = false);

        y60::WorldBuilderBasePtr writeBody(y60::WorldBuilderBasePtr theTransformBuilder,
                                           BaseObject * theNode,
                                           bool theTransformFlag = true,
                                           bool theForceFrontBackFacing = false,
                                           const std::string & theNameAppendix = "");

        y60::WorldBuilderBasePtr writeLight(y60::WorldBuilderBasePtr theTransformBuilder,
                                            BaseObject * theNode);

        y60::WorldBuilderBasePtr writeCamera(y60::WorldBuilderBasePtr theTransformBuilder,
                                             CameraObject * theNode);

        y60::WorldBuilderBasePtr writeTransform(y60::WorldBuilderBasePtr theTransformBuilder,
                                                BaseObject * theNode);

        y60::WorldBuilderBasePtr writeSymmetryTransform(y60::WorldBuilderBasePtr theTransformBuilder,
                                                        BaseObject * theNode);

        void writeActiveCamera();

        void exportTransformation(y60::TransformBuilderBase & theBuilder,
                                  BaseObject * theNode, bool theInverseTransformFlag = false);

        BaseDocument     *  _myDocument;
        y60::SceneBuilderPtr _mySceneBuilder;
        y60::WorldBuilderPtr _myWorldBuilder;
        MaterialExporter *   _myMaterialExporter;

        typedef std::map<void*, std::vector<std::string> > ShapeMap;
        ShapeMap  _myExportedShapes;

        typedef std::map<void*, std::string> LightMap;
        LightMap  _myExportedLightSources;

        typedef std::map<BaseObject*, std::string> CameraMap;
        CameraMap _myExportedCameras;

	    unsigned  _myObjectCount;
	    bool      _myBinaryFlag;
};

#endif
