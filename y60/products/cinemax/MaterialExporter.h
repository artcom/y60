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
//   $RCSfile: MaterialExporter.h,v $
//   $Author: valentin $
//   $Revision: 1.20 $
//   $Date: 2005/04/29 15:07:34 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_c4d_MaterialExporter_h_
#define _ac_c4d_MaterialExporter_h_

#include "Exceptions.h"

#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/base/DataTypes.h>
#include <y60/base/NodeValueNames.h>

#include <string>
#include <vector>
#include <map>

class Filename;
class TextureTag;
class UVWTag;
class BaseObject;
class BaseChannel;
class String;
class Material;
class PluginShader;
class BaseContainer;

struct ExportedMaterialInfo {
    std::string _myMaterialName;
    std::string _myMaterialId;
    unsigned    _myTextureCount;
    std::vector<LONG> _myTexureMapping;
    ExportedMaterialInfo(): _myMaterialName(""), _myMaterialId(""), _myTextureCount(0) {}
};


typedef std::map<std::string, ExportedMaterialInfo> MaterialInfoMap;

typedef std::vector<std::pair<TextureTag*, UVWTag*> > TextureList;
typedef std::vector<UVWTag*> UVTagList;

class MaterialExporter {
    public:
        MaterialExporter(y60::SceneBuilderPtr theSceneBuilder,
                         const Filename & theDocumentPath, bool theInlineTextures = true);

        ~MaterialExporter();

        ExportedMaterialInfo  initiateExport(BaseObject * theNode, TextureList theTextureList,
                                             y60::SceneBuilder & theSceneBuilder);

        void writeMaterial(const ExportedMaterialInfo & theMaterialId, BaseObject * theNode, TextureList theTextureList,
                           y60::SceneBuilder & theSceneBuilder,
                           const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord);

        void writeMaterial(const ExportedMaterialInfo & theMaterialId, BaseObject * theNode,
                           y60::SceneBuilder & theSceneBuilder,
                           const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord);

    private:
        struct Impl;
        Impl* _myImpl;

        // forbidden
        MaterialExporter(const MaterialExporter&);
        MaterialExporter& operator=(const MaterialExporter&);

        void getColor(BaseChannel * theColorChannel, asl::Vector4f & theColor);
        const std::string getTexturePath(Filename & theDocumentpath, String & theTextureFilename);

        bool exportTexture(Material* theMaterial,
                           y60::MaterialBuilderPtr theMaterialBuilder,
                           y60::SceneBuilder & theSceneBuilder,
                           BaseContainer * theContainer, const y60::TextureUsage & theUsage, TextureTag * theTextureTag,
                           const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord,
                           bool isAlphaChannel = false, bool isEnvMap = false);

        bool exportShader(PluginShader * theShader,
                          y60::MaterialBuilderPtr theMaterialBuilder,
                          Material* theMaterial,
                          y60::SceneBuilder & theSceneBuilder,
                          BaseContainer * theColorContainer,
                          TextureTag * theTextureTag,
                          const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord,
                          bool isAlphaChannel = false, bool isEnvMap = false);

        ExportedMaterialInfo createDefaultMaterial();


};

#endif

