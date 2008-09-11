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

#include "c4d_include.h"

#include <y60/scene/MaterialBuilder.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/base/DataTypes.h>
#include <y60/base/NodeValueNames.h>

#include <string>
#include <vector>
#include <map>

struct ExportedMaterialInfo {
    std::string _myMaterialName;
    std::string _myMaterialId;
    unsigned    _myTextureCount;
	std::vector<LONG> _myTexureMapping;
	ExportedMaterialInfo(): _myMaterialName(""), _myMaterialId(""), _myTextureCount(0) {}
};

struct MaterialTextureTagCombo {
	TextureTag * _myTextureTag;
	Material *   _myMaterial;
};

typedef std::map<std::string, ExportedMaterialInfo> MaterialInfoMap;

typedef std::vector<std::pair<TextureTag*, UVWTag*> > TextureList;
typedef std::vector<UVWTag*> UVTagList;
typedef std::vector<MaterialTextureTagCombo> MaterialList;
class MaterialExporter {
	public:
    	MaterialExporter(y60::SceneBuilderPtr theSceneBuilder,
    	                 const Filename & theDocumentPath, bool theInlineTextures = true);
	    ~MaterialExporter() {};

		ExportedMaterialInfo  initiateExport(BaseObject * theNode, TextureList theTextureList,
                                             y60::SceneBuilder & theSceneBuilder);

		void writeMaterial(const ExportedMaterialInfo & theMaterialId, BaseObject * theNode, TextureList theTextureList,
			               y60::SceneBuilder & theSceneBuilder,
						   const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord);

		void writeMaterial(const ExportedMaterialInfo & theMaterialId, BaseObject * theNode,
			               y60::SceneBuilder & theSceneBuilder,
						   const asl::Vector3f & theMinCoord, const asl::Vector3f & myMaxCoord);

    private:
        y60::SceneBuilderPtr  _mySceneBuilder;

	    MaterialInfoMap         _myMaterialMap;
		ExportedMaterialInfo    _myDefaultMaterialInfo;
		y60::MaterialBuilderPtr _myMaterialBuilder;
		y60::MaterialBuilderPtr _myDefaultMaterialBuilder;
		MaterialList            _myMaterials;

	    Filename             _myDocumentPath;
        bool                 _myInlineTextures;
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

