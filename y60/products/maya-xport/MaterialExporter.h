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
//   $Author: jens $
//   $Revision: 1.36 $
//   $Date: 2005/04/21 09:07:31 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#ifndef _ac_maya_MaterialExporter_h_
#define _ac_maya_MaterialExporter_h_

#include <asl/dom/Nodes.h>
#include <y60/base/DataTypes.h>
#include <y60/base/NodeValueNames.h>

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
   MaterialFeature(std::string theClass, std::string theValues) :
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
                        y60::TextureUsage theUsageMode,
                        const std::string & theColorGainPropertyName,
                        float theColorGainAlpha);
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
                            y60::TextureUsage theUsageMode,
                            const std::string & theColorGainPropertyName,
                            float theColorGainAlpha);
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
                               y60::MaterialBuilder & theBuilder,
                               y60::SceneBuilder & theSceneBuilder,
                               y60::TextureUsage theUsageMode,
                               const MayaBlendMode theBlendMode,
                               float theColorGainAlpha);
        void exportEnvCubeTexture(const MObject & theShaderNode, const MObject & theEnvCubeNode,
                                  y60::MaterialBuilder & theBuilder,
                                  y60::SceneBuilder & theSceneBuilder,
                                  y60::TextureUsage theUsageMode);
        void exportLayeredTexture(const MFnMesh * theMesh, const MObject & theTextureNode,
                y60::MaterialBuilder & theBuilder,
                y60::SceneBuilder & theSceneBuilder,
                y60::TextureUsage theUsageMode,
                const std::string & theColorGainPropertyName,
                float theColorGainAlpha);

        void exportLambertFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportReflectiveFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportIncandescenceFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportPhongFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportPhongEFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportBlinnFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);
        void exportUnlitFeatures(const MFnMesh * theMesh, const MObject & theShaderNode, y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder);

        std::string getStrippedTextureFilename(const MPlug & theTexturePlug);
        std::string findRelativeFilePath(const std::string & theFileName);

        bool checkTransparency(const MObject & theShaderNode);
        bool checkAlphaTexture(const MObject & theShaderNode, const std::string thePlugName);

        StringMap        _myMaterialNameMap;
        StringVectorMap  _myMaterialUVMappedTexturesMap;
        std::string      _myBaseDirectory;
        bool             _myInlineTexturesFlag;
};

#endif // _ac_maya_MaterialExporter_h_
