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
//   $RCSfile: MaterialExporter.h,v $
//   $Author: jens $
//   $Revision: 1.36 $
//   $Date: 2005/04/21 09:07:31 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_MaterialExporter_h_
#define _ac_maya_MaterialExporter_h_

#include <dom/Nodes.h>
#include <y60/DataTypes.h>

#include <maya/MStatus.h>
#include <maya/MPlug.h>
#include <maya/MObject.h>
#include <maya/MFnMesh.h>

#include <vector>
#include <map>

namespace y60 {
    class MaterialBuilder;
    class SceneBuilder;
}

struct MaterialFeature {
    MaterialFeature::MaterialFeature(std::string theClass, std::string theValues) :
        classname(theClass), values(theValues)
    {}

    std::string classname;
    std::string values;
};

typedef std::vector<MaterialFeature> SpecialFeatures;

typedef std::map<std::string, std::string>   StringMap;
typedef std::map<std::string, y60::VectorOfString>  StringVectorMap;

class MFnDependencyNode;

enum MayaBlendMode {
    MAYA_BLEND_NONE = 0,
    MAYA_BLEND_OVER,
    MAYA_BLEND_IN,
    MAYA_BLEND_OUT,
    MAYA_BLEND_ADD,
    MAYA_BLEND_SUBSTRACT,
    MAYA_BLEND_MULTIPLY,
    MAYA_BLEND_DIFFERENCE,
    MAYA_BLEND_LIGHTEN,
    MAYA_BLEND_DARKEN,
    MAYA_BLEND_SATURATE,
    MAYA_BLEND_DESATURATE,
    MAYA_BLEND_ILLUMINATE
};

static const char * MayaBlendModesString[] = {
    "None",
    "Over",
    "In",
    "Out",
    "Add",
    "Subtract",
    "Multiply",
    "Difference",
    "Lighten",
    "Darken",
    "Saturate",
    "Desaturate",
    "Illuminate",
    0
};

class MaterialExporter {
    public:
        MaterialExporter(bool theInlineTexturesFlag = true) : _myInlineTexturesFlag(theInlineTexturesFlag) {}
        virtual ~MaterialExporter() {}
        std::string createFaceMaterial(const MFnMesh * theMesh, const MObject & theMaterialNode, y60::SceneBuilder & theSceneBuilder,
            const SpecialFeatures & theSpecialFeatures);

        MObject getMaterial(MIntArray & theIndices, MObjectArray & theShadingGroups, unsigned theFaceIndex);
        const y60::VectorOfString & getTextureNames(const std::string & theMaterialName);
        bool hasUVMappedTextures(const std::string & theMaterialName);
        void setBaseDirectory(const std::string & theDirectory);

    private:
		std::string getTextureMappingType(const MObject & theTextureNode);
        std::string createMaterial(const MFnMesh * theMesh, const MObject & theShaderNode, y60::SceneBuilder & theSceneBuilder,
            const SpecialFeatures & theSpecialFeatures);
        MPlug getPlug(const MObject & theShaderNode,const char * theName);
        MObject findShader(MObject & theSetNode);
        bool exportMaps(const MFnMesh * theMesh, const MObject & theShaderNode,
                        y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder,
                        const char * thePlugName,
                        const char* theColorGainPropertyName, float theColorGainAlpha);
#if 0
        void exportVertexParameters(y60::MaterialBuilder & theBuilder);
#endif
		void exportShader(const MFnMesh * theMesh, const MObject & theShaderNode,
							   y60::MaterialBuilder & theMaterialBuilder,
                               y60::SceneBuilder & theSceneBuilder,
							   y60::VectorOfRankedFeature & theLightingFeature);
        bool exportTextures(const MFnMesh * theMesh, const MObject & theShaderNode,
                            y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder,
                            const std::string & thePlugName,
                            const char* theColorGainPropertyName, float theColorGainAlpha);
        bool exportBumpMaps(const MFnMesh * theMesh, const MObject & theShaderNode,
                            y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder,
                            float theColorGainAlpha);

        void exportBumpTexture(const MObject & theBumpNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
#if 0        
        void exportProjectedTexture(const MObject & theProjectionNode,
                                    y60::MaterialBuilder & theBuilder,
                                    y60::SceneBuilder & theSceneBuilder,
                                    const float theAlpha, const MayaBlendMode theBlendMode,
                                    const char* theColorGainPropertyName, float theColorGainAlpha);
#endif        
        void exportFileTexture(const MFnMesh * theMesh, MObject & theTextureNode,
                               y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder,
                               const MayaBlendMode theBlendMode,
                               float theColorGainAlpha);
		void exportEnvCubeTexture(const MObject & theShaderNode,
			                      const MObject & theEnvCubeNode,
                                    y60::MaterialBuilder & theBuilder,
                                    y60::SceneBuilder & theSceneBuilder);
        void exportLayeredTexture(const MFnMesh * theMesh, const MObject & theTextureNode,
                                  y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder,
                                  const char* theColorGainPropertyName, float theColorGainAlpha);

        void exportLambertFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportReflectiveFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportPhongFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportPhongEFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportBlinnFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportUnlitFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);

		std::string getStrippedTextureFilename(const MPlug & theTexturePlug);
        void stripBaseDir(std::string & theFileName);

        bool checkTransparency(const MObject & theShaderNode);
        bool checkAlphaTexture(const MObject & theShaderNode, const std::string thePlugName);
        /*
        void calcColorScaleAndBiasFromAlpha(const float theAlpha,
                                            const MayaBlendMode theBlendMode,
                                            asl::Vector4f &theColorScale,
                                            asl::Vector4f &theColorBias);
        */

        StringMap        _myMaterialNameMap;
        StringVectorMap  _myMaterialUVMappedTexturesMap;
        std::string      _myBaseDirectory;
        bool             _myInlineTexturesFlag;
};

#endif // _ac_maya_MaterialExporter_h_
