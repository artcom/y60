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

#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/LightBuilder.h>
#include <y60/scene/WorldBuilder.h>
#include <y60/scene/TransformBuilder.h>
#include <y60/scene/CameraBuilder.h>

#include <c4d_filterdata.h> // needed for SceneSaverData

class ShapeExporter;
class MaterialExporter;
class CameraObject;

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
