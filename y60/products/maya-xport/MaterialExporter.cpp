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

#include "MayaHelpers.h"
#include "MaterialExporter.h"
#include "AnimationExporter.h"
#include "ExportExceptions.h"

#define DEBUG_LEVEL 0

#include <y60/scene/MaterialTypes.h>
#include <y60/base/DataTypes.h>
#include <y60/scene/SceneBuilder.h>
#include <y60/scene/ShapeBuilder.h>
#include <y60/scene/MaterialBuilder.h>
#include <y60/base/NodeValueNames.h>
#include <y60/base/PropertyNames.h>
#include <y60/base/property_functions.h>
#include <y60/image/Image.h>

#include <asl/base/file_functions.h>
#include <asl/base/string_functions.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/Dashboard.h>
#include <asl/math/Matrix4.h>
#include <asl/zip/PackageManager.h>

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MColor.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnAttribute.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnPhongEShader.h>
#include <maya/MFnBlinnShader.h>
#include <maya/MFnMesh.h>
#include <maya/MFnTransform.h>
#include <maya/MFnMatrixData.h>
#include <maya/MQuaternion.h>
#include <maya/MMatrix.h>
#include <maya/MBoundingBox.h>
#include <maya/MEulerRotation.h>

#include <iostream>
#include <algorithm>
#include <cctype> // needed for std::toupper

#define DB(x) //x
#define DB2(x)  //x

using namespace std;
using namespace asl;
using namespace y60;

static const MayaBlendMode ourDefaultBlendMode = MAYA_BLEND_MULTIPLY;

MObject
MaterialExporter::findShader(MObject & theSetNode) {
    MFnDependencyNode myFnNode(theSetNode);
    MPlug myShaderPlug = myFnNode.findPlug("surfaceShader");

    if (myShaderPlug.isNull()) {
        throw ExportException("Could not find shader plug", "MaterialExporter::findShader()");
    }

    MPlugArray myConnectedPlugs;

    // Get all the plugs that are connected as the destination of this
    // surfaceShader plug so we can find the surface shaderNode
    MStatus myStatus;
    myShaderPlug.connectedTo(myConnectedPlugs, true, false, &myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("No connected plug found", "MaterialExporter::findShader()");
    }

    if (myConnectedPlugs.length() != 1) {
        throw ExportException("Error: Shader has more than one or no connected plugs",
                              "MaterialExporter::findShader()");
    }

    return myConnectedPlugs[0].node();
}

MPlug
MaterialExporter::getPlug(const MObject & theShaderNode, const char * theName) {
    // Get the color plug
    MStatus myStatus;
    MPlug myPlug = MFnDependencyNode(theShaderNode).findPlug(theName, &myStatus);
    if (myStatus == MS::kFailure) {
        throw ExportException(std::string("No plug found with name'")+theName+"'", "MaterialExporter::getPlug()");
    }

    return myPlug;
}


void
MaterialExporter::exportFileTexture(const MFnMesh * theMesh, MObject & theTextureNode,
                                    y60::MaterialBuilder & theBuilder,
                                    y60::SceneBuilder & theSceneBuilder,
                                    TextureUsage theUsageMode,
                                    const MayaBlendMode theBlendMode,
                                    float theColorGainAlpha)
{
    MStatus myStatus;

    std::string myMaterialName = theBuilder.getName();
    std::string myTextureName(MFnDependencyNode(theTextureNode).name().asChar());
    DB(
        AC_PRINT << "dumping texture '" << myTextureName << "'";
        dumpAttributes(theTextureNode);
        MPlugArray myConnections;
        MFnDependencyNode(theTextureNode).getConnections(myConnections);
        for(int i = 0;i < myConnections.length(); ++i) {
            AC_PRINT << "CONN " << i << ": " <<myConnections[i].name();
        }
    );

    // occlusion usage
    if (theUsageMode == PAINT) {
        bool myOcclusionMapFlag = false;
        getCustomAttribute(theTextureNode, "ac_occlusion", myOcclusionMapFlag);
        if (myOcclusionMapFlag) {
            AC_DEBUG << "MaterialExporter::exportFileTexture Found ac_occlusion on texture '" << myTextureName << "'";
            theUsageMode = y60::OCCLUSION;
        }
    }

    // texture mapping
    MObject myMayaImageNode = theTextureNode;
    string myTextureMapping = getTextureMappingType(theTextureNode);

    if (myTextureMapping == y60::TEXCOORD_UV_MAP) {
        // only do this if we have a mesh i.e. not for envmap, emissive, etc.
        if (theMesh) {
            _myMaterialUVMappedTexturesMap[myMaterialName].push_back(myTextureName);
        }
    } else {
        // generative mapping
        MPlug myImagePlug = MFnDependencyNode(theTextureNode).findPlug("image");
        getConnectedNode(myImagePlug, myMayaImageNode);
    }

    MPlug myFilenamePlug = MFnDependencyNode(myMayaImageNode).findPlug("fileTextureName");
    MString myFileName;
    myFilenamePlug.getValue(myFileName);
    std::string myRelativeFileName = findRelativeFilePath(myFileName.asChar());

    // color gain = scale
    Vector4f myColorScale(1,1,1,theColorGainAlpha);

    MPlug myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainR");
    myColorGainPlug.getValue(myColorScale[0]);
    myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainG");
    myColorGainPlug.getValue(myColorScale[1]);
    myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainB");
    myColorGainPlug.getValue(myColorScale[2]);

    // UH: for some reason this doesn't work
    //MPlug myAlphaGainPlug = MFnDependencyNode(theTextureNode).findPlug("alphaGain");
    //myAlphaGainPlug.getValue(myColorScale[3]);

    // color offset = bias
    Vector4f myColorBias(0,0,0,0);

    MPlug myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetR");
    myColorOffsetPlug.getValue(myColorBias[0]);
    myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetG");
    myColorOffsetPlug.getValue(myColorBias[1]);
    myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetB");
    myColorOffsetPlug.getValue(myColorBias[2]);

    // ... and neither does this
    //MPlug myAlphaOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("alphaOffset");
    //myAlphaOffsetPlug.getValue(myColorBias[3]);

    // image
    AC_DEBUG << "stripped filename=" << myRelativeFileName << " Scale=" << myColorScale << ", Bias=" << myColorBias;
    dom::NodePtr myImageNode = theBuilder.createImageNode(theSceneBuilder,
        MFnDependencyNode(theTextureNode).name().asChar(),
        myRelativeFileName, theUsageMode, SINGLE);

    // texture matrix
    asl::Matrix4f myTextureMatrix;
    myTextureMatrix.makeIdentity();

    MPlug myPlacementMatrixPlug = MFnDependencyNode(theTextureNode).findPlug("placementMatrix", &myStatus);
    MObject myPlacementMatrix;
    myPlacementMatrixPlug.getValue(myPlacementMatrix);
    MFnMatrixData myMatrixData(myPlacementMatrix, &myStatus);
    if (theMesh && myStatus == MStatus::kSuccess) {

        // fetch placement matrix
        MTransformationMatrix myTransform = myMatrixData.transformation();
        MMatrix myMayaMatrix = myTransform.asMatrix();

        //cout << "maya matrix " << myMayaMatrix << endl;
        asl::Matrix4f myMatrix;
        myMatrix.assign(
            float(myMayaMatrix[0][0]), float(myMayaMatrix[0][1]), float(myMayaMatrix[0][2]), float(myMayaMatrix[0][3]),
            float(myMayaMatrix[1][0]), float(myMayaMatrix[1][1]), float(myMayaMatrix[1][2]), float(myMayaMatrix[1][3]),
            float(myMayaMatrix[2][0]), float(myMayaMatrix[2][1]), float(myMayaMatrix[2][2]), float(myMayaMatrix[2][3]),
            float(myMayaMatrix[3][0]), float(myMayaMatrix[3][1]), float(myMayaMatrix[3][2]), float(myMayaMatrix[3][3]));

        if (myTextureMapping != TEXCOORD_UV_MAP) {

            /*
             * STATE-OF-THE-UNION:
             * - right now rotations don't work with scaling
             * - for some friggin' reason the scaling values we get
             *   from the matrix are same for X&Y
             * - probably the matrix from Maya already contains all this...
             */

            double mayaScale[3];
            myTransform.getScale(mayaScale, MSpace::kWorld);
            //cout << "+++ maya scale=" << mayaScale[0] << "," << mayaScale[1] << "," << mayaScale[2] << endl;

            MVector mayaTrans = myTransform.translation(MSpace::kWorld);
            //cout << "+++ maya trans=" << mayaTrans[0] << "," << mayaTrans[1] << "," << mayaTrans[2] << endl;

            MEulerRotation mayaEuler = myTransform.eulerRotation();
            //cout << "+++ maya euler=" << mayaEuler[0] << "," << mayaEuler[1] << "," << mayaEuler[2] << endl;

            /*
             * Calculate texgen matrix
             */
            //cout << "***" << endl;
            //cout << __DATE__ << " " << __TIME__ << endl;
            //cout << "mtx=" << myMatrix << endl;
#if 0
            asl::Matrix4f myMtxInv = myMatrix;
            myMtxInv.invert();
            //cout << "inv=" << myMtxInv << endl;
#endif

            // object bounding-box for size normalization
            MBoundingBox myBBox = theMesh->boundingBox(&myStatus);
            if (!myStatus) {
                AC_ERROR << "Unable to get bounding box";
                throw ExportException("Unable to get bounding box.", PLUS_FILE_LINE);
            }
            //cout << "bbox="  << myBBox.width() << "x" << myBBox.height() << "x" << myBBox.depth() << endl;
            MPoint myBBoxSize(myBBox.width(), myBBox.height(), myBBox.depth());
            //convertToMeter(myBBoxSize);

            asl::Matrix4f myNormalizationMatrix;
            myNormalizationMatrix.makeIdentity();
            myNormalizationMatrix.scale(asl::Vector3f(1.0f, -1.0f, 1.0f));
            myNormalizationMatrix.translate(asl::Vector3f(0.5f, 0.5f, 0.5f));

            // fetch parameters from matrix
            asl::Vector3f myScale, myShear, myRotation, myTranslation;
            myMatrix.decompose(myScale, myShear, myRotation, myTranslation);

            if( myBBoxSize.x > 0.0) {
                myScale[0] *= float(myBBoxSize.x * 0.5);
            }
            if( myBBoxSize.y > 0.0) {
                myScale[1] *= float(myBBoxSize.y * 0.5);
            }
            if(myBBoxSize.z > 0.0) {
                myScale[2] *= float(myBBoxSize.z * 0.5f);
            }

            myTranslation[0] /= myScale[0] * 2.0f;
            myTranslation[1] /= myScale[1] * 2.0f;
            myTranslation[2] /= myScale[2] * 2.0f;

            // texture scaling
            asl::Matrix4f myScaleMatrix;
            myScaleMatrix.makeScaling(myScale);
            //cout << "scale matrix=" << myScaleMatrix << endl;

            // texture translation
            asl::Matrix4f myPosMatrix;
            myPosMatrix.makeTranslating(myTranslation);
            //cout << "translation matrix=" << myPosMatrix << endl;

            // texture rotation
            asl::Matrix4f myRotMatrix;
            myRotMatrix.makeIdentity();
            //myRotMatrix.translate(asl::Vector3f(0.5f, 0.5f, 0.5f));
            //TODO switch over rotate-order
            myRotMatrix.rotateX(myRotation[0]);
            myRotMatrix.rotateY(myRotation[1]);
            myRotMatrix.rotateZ(myRotation[2]);
            //myRotMatrix.translate(asl::Vector3f(-0.5f, -0.5f, -0.5f));
            //cout << "rotation matrix=" << myRotMatrix << endl;

            // apply
            myMatrix.makeIdentity();
#if 0
            myMatrix.postMultiply(myScaleMatrix);
            myMatrix.postMultiply(myPosMatrix);
            myMatrix.postMultiply(myRotMatrix);
#else
            myMatrix.postMultiply(myRotMatrix);
            myMatrix.postMultiply(myPosMatrix);
            myMatrix.postMultiply(myScaleMatrix);
#endif
            //cout << "pre-norm=" << myMatrix << " rot " << myRotMatrix << " scale " << myScaleMatrix << " pos " << myPosMatrix << endl;
            myMatrix.postMultiply(myNormalizationMatrix);
            //cout << "final=" << myMatrix << endl;

            VectorOfVector4f myTexGenParams;
            myTexGenParams.push_back(myMatrix.getColumn(0));
            myTexGenParams.push_back(myMatrix.getColumn(1));
            myTexGenParams.push_back(myMatrix.getColumn(2));
            myTexGenParams.push_back(myMatrix.getColumn(3));
            string myTexUnitString = string("texgenparam") + asl::as_string(theBuilder.getTextureCount());

            setPropertyValue<VectorOfVector4f>(theBuilder.getNode(),
                    "vectorofvector4f", myTexUnitString.c_str(), myTexGenParams);

        } else {
            /*
             * Use texture placement matrix
             */
            myTextureMatrix = myMatrix;
       }
    } else {
        /*
         * No texture placement matrix, calculate repeat
         */

        // repeat
        double myRepeatU = 1;
        MPlug myRepeatUPlug = MFnDependencyNode(theTextureNode).findPlug("repeatU");
        myRepeatUPlug.getValue(myRepeatU);

        double myRepeatV = 1;
        MPlug myRepeatVPlug = MFnDependencyNode(theTextureNode).findPlug("repeatV");
        myRepeatVPlug.getValue(myRepeatV);

        myTextureMatrix.makeScaling(asl::Vector3f(float(myRepeatU), float(myRepeatV), 1));
    }

    // texture wrap
    TextureWrapMode myWrapMode  = y60::REPEAT;
    bool myWrapU = false;
    MPlug myWarpUPlug = MFnDependencyNode(myMayaImageNode).findPlug("wrapU");
    if (myWarpUPlug.getValue(myWrapU) != MStatus::kSuccess) {
        AC_WARNING << "No wrapU";
    }

    bool myWrapV = false;
    MPlug myWarpVPlug = MFnDependencyNode(myMayaImageNode).findPlug("wrapV");
    if (myWarpVPlug.getValue(myWrapV) != MStatus::kSuccess) {
        AC_WARNING << "No wrapV";
    }
    if (!myWrapU && !myWrapV) {
        myWrapMode = CLAMP;
    }

    // texture mipmap
    bool myCreateMipmapsFlag = true;
    getCustomAttribute(theTextureNode, "ac_mipmaps", myCreateMipmapsFlag);

    // texture
    dom::NodePtr myTextureNode = theBuilder.createTextureNode(theSceneBuilder,
        MFnDependencyNode(theTextureNode).name().asChar(),
        myImageNode->getAttributeString(ID_ATTRIB),
        myWrapMode, myCreateMipmapsFlag,
        myTextureMatrix, "", myColorScale, myColorBias);

    // texture unit apply
    TextureApplyMode myApplyMode = y60::MODULATE;
    switch (theBlendMode) {
        case MAYA_BLEND_NONE:
            myApplyMode = y60::REPLACE;
            break;
        case MAYA_BLEND_OVER:
            myApplyMode = y60::DECAL;
            break;
        case MAYA_BLEND_MULTIPLY:
            myApplyMode = y60::MODULATE;
            break;
        case MAYA_BLEND_ADD:
            myApplyMode = y60::ADD;
            break;
        default:
            displayWarning(string("Blendmode: ") + getStringFromEnum(theBlendMode, MayaBlendModesString) + " not yet implemented.");
    }

    // texture unit
    theBuilder.createTextureUnitNode(myTextureNode->getAttributeString(ID_ATTRIB),
        myApplyMode, theUsageMode, myTextureMapping, asl::Matrix4f::Identity());
}

string
MaterialExporter::getTextureMappingType(const MObject & theTextureNode) {

    MStatus myStatus;
    MPlug myProjectionTypePlug = MFnDependencyNode(theTextureNode).findPlug("projType", &myStatus);
    if (myStatus != MStatus::kSuccess) {
        AC_INFO << "Unable to get 'projType' for node '" << MFnDependencyNode(theTextureNode).name().asChar() << "'";
    }

    double myProjectionType = 0;
    myProjectionTypePlug.getValue(myProjectionType);

    /*
     * Note:
     * - Mayas concentric mode is not supported, instead a spatial mode is authored.
     *   Will fail in Y60!!!
     * - PERSPECTIVE mode will be mapped to FRONTAL.
     */
    static const string myTexMapping[] = {
        TEXCOORD_UV_MAP, TEXCOORD_PLANAR, TEXCOORD_SPHERICAL, TEXCOORD_CYLINDRICAL,
        TEXCOORD_SPHERICAL, TEXCOORD_CUBE, TEXCOORD_CUBE, TEXCOORD_SPATIAL,
        TEXCOORD_FRONTAL
    };
    unsigned myMappingIndex = unsigned(myProjectionType);
    if (myMappingIndex > sizeof(myTexMapping)) {
        throw ExportException(string("Sorry, mapping mode not supported: ") + asl::as_string(myMappingIndex), PLUS_FILE_LINE);
    }

    return myTexMapping[myMappingIndex];
}

void
MaterialExporter::exportBumpTexture(const MObject & theBumpNode,
                                    y60::MaterialBuilder & theBuilder,
                                    y60::SceneBuilder & theSceneBuilder)
{
    DB2(cerr << "MaterialExporter::exportBumpTexture() - name: " << MFnDependencyNode(theBumpNode).name().asChar() << endl);
    DB(dumpAttributes(theBumpNode));
    MPlug myBumpPlug = MFnDependencyNode(theBumpNode).findPlug("bumpValue");

    std::string myFileName = getStrippedTextureFilename(myBumpPlug);

    TextureApplyMode myApplyMode = y60::DECAL;
    TextureUsage myUsage         = y60::BUMP;
    TextureWrapMode myWrapMode   = y60::REPEAT;

    dom::NodePtr myImageNode = theBuilder.createImageNode(theSceneBuilder,
            MFnDependencyNode(theBumpNode).name().asChar(),
            myFileName, myUsage, SINGLE);

    // texture matrix
    asl::Matrix4f myTextureMatrix;
    myTextureMatrix.makeIdentity();

    theBuilder.createTextureNode(theSceneBuilder,
            MFnDependencyNode(theBumpNode).name().asChar(),
            myImageNode->getAttributeString(ID_ATTRIB),
            myWrapMode, false,
            myTextureMatrix);

    theBuilder.needTextureFallback(true);
    std::string myMaterialName = theBuilder.getName();
    std::string myTextureName(MFnDependencyNode(theBumpNode).name().asChar());
    _myMaterialUVMappedTexturesMap[myMaterialName].push_back(myTextureName);
}

void
MaterialExporter::exportEnvCubeTexture(const MObject & theShaderNode,
                                       const MObject & theEnvCubeNode,
                                       y60::MaterialBuilder & theBuilder,
                                       y60::SceneBuilder & theSceneBuilder,
                                       TextureUsage theUsageMode)
{
    AC_DEBUG << "MaterialExporter::exportEnvCubeTexture(" << MFnDependencyNode(theShaderNode).name().asChar() << ", usage=" << theUsageMode << ")";

    std::string frontFileName  = getStrippedTextureFilename(getPlug(theEnvCubeNode,"front"));
    std::string rightFileName  = getStrippedTextureFilename(getPlug(theEnvCubeNode,"right"));
    std::string backFileName   = getStrippedTextureFilename(getPlug(theEnvCubeNode,"back"));
    std::string leftFileName   = getStrippedTextureFilename(getPlug(theEnvCubeNode,"left"));
    std::string topFileName    = getStrippedTextureFilename(getPlug(theEnvCubeNode,"top"));
    std::string bottomFileName = getStrippedTextureFilename(getPlug(theEnvCubeNode,"bottom"));

    TextureApplyMode myApplyMode = y60::MODULATE;
    theBuilder.appendCubemap(theSceneBuilder,
            MFnDependencyNode(theEnvCubeNode).name().asChar(),
            theUsageMode,
            frontFileName, rightFileName, backFileName,
            leftFileName, topFileName, bottomFileName,
            myApplyMode);

    //NOTE: specular colors are added to the diffuse colors.
    //      no alpha value support for specular colors therefore means
    //      setting the specular alpha to zero.
    MStatus myStatus;
    MColor mySpecularColor = MFnReflectShader(theShaderNode).specularColor(& myStatus);
    asl::Vector4f myY60SpecColor(mySpecularColor.r, mySpecularColor.g, mySpecularColor.b, 0); //mySpecularColor.a
    setPropertyValue<asl::Vector4f>(theBuilder.getNode(),
                    "vector4f", y60::SPECULAR_PROPERTY, myY60SpecColor);

/*
    float myShininess = MFnPhongShader(theShaderNode).cosPower(& myStatus);
    // Convert to 0 - 128 range of OpenGL
    myShininess = (myShininess / 100) * 128;
    setPropertyValue<float>(theBuilder.getNode(), "float", y60::SHININESS_PROPERTY, myShininess);
*/
}

void
MaterialExporter::exportLayeredTexture(const MFnMesh * theMesh,
        const MObject & theMultiTextureNode,
        y60::MaterialBuilder & theBuilder,
        y60::SceneBuilder & theSceneBuilder,
        y60::TextureUsage theUsageMode,
        const std::string & theColorGainPropertyName,
        float theColorGainAlpha)
{
    MStatus myStatus;
    MFnDependencyNode myDependencyNode(theMultiTextureNode);
    MPlug myInputPlug = myDependencyNode.findPlug("inputs", & myStatus);

    if (!myStatus) {
        throw ExportException("Could not find inputs plug", "MaterialExporter::exportLayeredTexture()");
    }

    DB(AC_TRACE << "MaterialExporter::exportLayeredTexture() - Number of layers: " <<
          myInputPlug.numElements());
    DB(dumpAttributes(theMultiTextureNode));

    // Iterate over all texture layers
    // Maya shades the first texture last, therefore we reverse the order
    for (int i = static_cast<int>(myInputPlug.numElements()) - 1; i >= 0; --i) {
        // Export alpha and blend mode for the texture
        float myAlpha = 1.0;
        MPlug myAlphaPlug;
        getChildPlugByName("alpha", myInputPlug[i], theMultiTextureNode, myAlphaPlug);

        MObject myAlphaChannelNode;
        MStatus myStatus;
        MPlugArray myPlugArray;
        myAlphaPlug.connectedTo(myPlugArray, true, false, & myStatus);
        unsigned myLength = myPlugArray.length();
        if (myLength <= 0) {
            myAlphaPlug.getValue(myAlpha);
        }

        int myBlendModeValue = 0;
        MPlug myBlendPlug;
        getChildPlugByName("blendMode", myInputPlug[i], theMultiTextureNode, myBlendPlug);
        myBlendPlug.getValue(myBlendModeValue);

        MayaBlendMode myBlendMode = MayaBlendMode(myBlendModeValue);

        // fix bug 476
        // the first (bottom-most) texture should be applied as modulate
        // otherwise we loose lighting effects
        if ((i == static_cast<int>(myInputPlug.numElements() - 1)) && myBlendMode == MAYA_BLEND_NONE) {
            myBlendMode = MAYA_BLEND_MULTIPLY;
        }

        DB(AC_TRACE << "MaterialExporter::exportLayeredTexture() - alpha: " << myAlpha << ", blendmode: "
            << getStringFromEnum(myBlendMode, MayaBlendModesString));

        // Find connected texture node
        MPlug myColorPlug;
        MObject myMayaTextureNode;
        getChildPlugByName("color", myInputPlug[i], theMultiTextureNode, myColorPlug);
        getConnectedNode(myColorPlug, myMayaTextureNode);

        exportFileTexture(theMesh, myMayaTextureNode,
                theBuilder, theSceneBuilder,
                y60::PAINT, myBlendMode,
                theColorGainAlpha);
        y60::setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f",
                        theColorGainPropertyName.c_str(), asl::Vector4f(1,1,1,1));
    }
}

bool
MaterialExporter::exportTextures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                 y60::MaterialBuilder & theBuilder,
                                 y60::SceneBuilder & theSceneBuilder,
                                 const std::string & thePlugName,
                                 TextureUsage theUsageMode,
                                 const std::string & theColorGainPropertyName,
                                 float theColorGainAlpha)
{
    try {
        return exportMaps(theMesh,
                theShaderNode,theBuilder, theSceneBuilder, thePlugName.c_str(),
                theUsageMode, theColorGainPropertyName, theColorGainAlpha);
    } catch (const ExportException & ex) {
        displayError(std::string("Can not export Texture: ") + ex.what() + " at " + ex.where());
        return false;
    }
}
bool
MaterialExporter::exportBumpMaps(const MFnMesh * theMesh, const MObject & theShaderNode,
                                 y60::MaterialBuilder & theBuilder,
                                 y60::SceneBuilder & theSceneBuilder,
                                 float theColorGainAlpha)
{
    try {
        return exportMaps(theMesh,
            theShaderNode,theBuilder, theSceneBuilder, "normalCamera",
            y60::PAINT, y60::DIFFUSE_PROPERTY, theColorGainAlpha);
    } catch (const ExportException & ex) {
        displayError((std::string("Can not export Texture: ") + ex.what() + " at " + ex.where()));
        return false;
    }
}


bool
MaterialExporter::exportMaps(const MFnMesh * theMesh, const MObject & theShaderNode,
                             y60::MaterialBuilder & theBuilder,
                             y60::SceneBuilder & theSceneBuilder,
                             const char * thePlugName,
                             TextureUsage theUsageMode,
                             const std::string & theColorGainPropertyName,
                             float theColorGainAlpha)
{
    AC_DEBUG << "MaterialExporter::exportMaps(shader=" << MFnDependencyNode(theShaderNode).name().asChar() << ", plug=" << thePlugName << ")";

    bool hasTextures = false;
    MStatus myStatus;

    DB(dumpAttributes(theShaderNode));
    MPlug myPlug = getPlug(theShaderNode,thePlugName);

    MPlugArray myPlugArray;
    myPlug.connectedTo(myPlugArray, true, false, & myStatus);
    if (myStatus == MS::kFailure) {
        throw ExportException("Could not get destination plug array",
                              "MaterialExporter::exportMaps()");
    }

    if (myPlugArray.length() > 1) {
        throw ExportException("Unsupported type of color plug.",
                              "MaterialExporter::exportMaps()");
    } else if (myPlugArray.length() == 1) {
        hasTextures = true;

        MObject myMayaTextureNode(myPlugArray[0].node());

        MFn::Type myTextureType = myMayaTextureNode.apiType();
        //AC_DEBUG << "TextureType " << myMayaTextureNode.apiTypeStr();
        switch (myTextureType) {
            case MFn::kLayeredTexture:
                exportLayeredTexture(theMesh, myMayaTextureNode, theBuilder, theSceneBuilder,
                                     theUsageMode, theColorGainPropertyName, theColorGainAlpha);
                break;
            case MFn::kProjection:
            case MFn::kFileTexture:
                exportFileTexture(theMesh, myMayaTextureNode, theBuilder, theSceneBuilder,
                                  theUsageMode, ourDefaultBlendMode, theColorGainAlpha);
                y60::setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f",
                        theColorGainPropertyName.c_str(), asl::Vector4f(1,1,1,1));
                break;
            case MFn::kBump:
                exportBumpTexture(myMayaTextureNode, theBuilder, theSceneBuilder);
                break;
            case MFn::kEnvCube:
                exportEnvCubeTexture(theShaderNode, myMayaTextureNode,
                                     theBuilder, theSceneBuilder,
                                     theUsageMode);
                break;
            default:
                throw ExportException(std::string("Unsupported type of map plug: ") +
                        myPlugArray[0].node().apiTypeStr(), "MaterialExporter::exportMaps()");
        }

        // check texture alpha, set material transparency if set
        const MFnDependencyNode & myTextureDepNode = MFnDependencyNode(myMayaTextureNode);
        MObject myFileHasAlpha = MFnDependencyNode(myMayaTextureNode).attribute("fileHasAlpha", &myStatus);
        if (myStatus) {
            MPlug myPlug = myTextureDepNode.findPlug(myFileHasAlpha, &myStatus);
            if (myStatus) {
                double myDoubleValue;
                if (!myPlug.isArray() && myPlug.getValue(myDoubleValue)) {
                    //AC_DEBUG << thePlugName << " fileHasAlpha=" << myDoubleValue;
                    theBuilder.setTransparencyFlag(myDoubleValue > 0.0);
                }
            }
        }
    }
    DB(AC_TRACE << "MaterialExporter::exportMaps("<<thePlugName<<") ready");

    return hasTextures;
}

void
MaterialExporter::exportUnlitFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                     y60::MaterialBuilder & theBuilder,
                                     y60::SceneBuilder & theSceneBuilder)
{
    DB(AC_TRACE << "MaterialExporter::exportUnlitFeatures()");
    MStatus myStatus;

    try {
        // If no textures were found, we can export the color value instead
        MPlug myTransparencyPlug = MFnDependencyNode(theShaderNode).findPlug("outTransparencyR");
        float myColorGainAlpha;
        myTransparencyPlug.getValue(myColorGainAlpha);
        myColorGainAlpha = 1.0f - myColorGainAlpha;
        if (!exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "outColor",
                            y60::PAINT, y60::SURFACE_COLOR_PROPERTY, myColorGainAlpha)) {
            DB(dumpAttributes(theShaderNode));
            float r, g, b;
            MPlug myColorPlug = MFnDependencyNode(theShaderNode).findPlug("outColorR");
            myColorPlug.getValue(r);
            myColorPlug = MFnDependencyNode(theShaderNode).findPlug("outColorG");
            myColorPlug.getValue(g);
            myColorPlug = MFnDependencyNode(theShaderNode).findPlug("outColorB");
            myColorPlug.getValue(b);
            setPropertyValue<asl::Vector4f>(theBuilder.getNode(),
                    "vector4f",
                    y60::SURFACE_COLOR_PROPERTY, asl::Vector4f(r, g, b, float(myColorGainAlpha)));
        }
    } catch(asl::Exception & ex) {
        ex; // avoid unreferenced variable warning
        DB(AC_TRACE << ex);
        throw;
    }
}

void
MaterialExporter::exportLambertFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                        y60::MaterialBuilder & theBuilder,
                                        y60::SceneBuilder & theSceneBuilder)
{
    AC_DEBUG << "MaterialExporter::exportLambertFeatures(" << MFnDependencyNode(theShaderNode).name().asChar() << ")";
    MStatus myStatus;



    // XXX Material animation tests
    MFnLambertShader myShaderDag( theShaderNode );

    // transparency, alpha
    MColor myTransparency = MFnLambertShader(theShaderNode).transparency(& myStatus);
    float myAlpha = 1.0f - myTransparency.r; // UH: what? alpha from transparency red?!?
    // Problem here is that myAlpha is used for diffuse.a as well as color_scale.a
    // so the final alpha is: diffuse.a * color_scale.a * texture.a

    AnimationExporter myAnimExporter( theSceneBuilder );
    myAnimExporter.exportLambertFeatures( myShaderDag, theBuilder.getId() );

    // ambient color
    MColor myAmbientColor = MFnLambertShader(theShaderNode).ambientColor(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get ambient color from node", "MaterialExporter::exportLambertFeatures");
    }
    setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::AMBIENT_PROPERTY,
            asl::Vector4f(myAmbientColor.r, myAmbientColor.g, myAmbientColor.b, myAmbientColor.a));

    // diffuse color
    MColor myColor = MFnLambertShader(theShaderNode).color(&myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get color from node", "MaterialExporter::exportLambertFeatures");
    }

    // texture
    try {
        if (exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "color",
                           y60::PAINT, y60::DIFFUSE_PROPERTY, 1.0)) {
            // assume color=[1,1,1,1] when using texture
            myColor = MColor(1.0f, 1.0f, 1.0f, 1.0f);
        }
        exportBumpMaps(theMesh, theShaderNode, theBuilder, theSceneBuilder, 1.0);
    } catch(asl::Exception & ex) {
        ex; // avoid unreferenced variable warning
        DB(AC_TRACE << ex);
        throw;
    }

    // calculate diffuse color
    float myDiffuseCoeff = MFnLambertShader(theShaderNode).diffuseCoeff(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get diffuse coefficient from node", "MaterialExporter::exportLambertFeatures");
    }

    myColor = myAmbientColor + (myColor - myAmbientColor) * myDiffuseCoeff;
    asl::Vector4f myDiffuseColor = asl::Vector4f(myColor.r, myColor.g, myColor.b, myAlpha);
    setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::DIFFUSE_PROPERTY, myDiffuseColor);

    // glow
    float myGlowFactor = MFnLambertShader(theShaderNode).glowIntensity( & myStatus );
    if (myStatus == MStatus::kSuccess && myGlowFactor > 0.0f) {
        setPropertyValue<float>(theBuilder.getNode(), "float", y60::GLOW_PROPERTY, myGlowFactor);
    }

    // emissive color
    MColor myEmissiveColor = MFnLambertShader(theShaderNode).incandescence(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get emissive (incandescence) color from node", "MaterialExporter::exportLambertFeatures");
    }
    setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::EMISSIVE_PROPERTY,
            asl::Vector4f(myEmissiveColor.r, myEmissiveColor.g, myEmissiveColor.b, myEmissiveColor.a));
}

void
MaterialExporter::exportReflectiveFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                           y60::MaterialBuilder & theBuilder, y60::SceneBuilder & theSceneBuilder)
{
    MFnReflectShader myReflectShader(theShaderNode);
    AC_DEBUG << "MaterialExporter::exportReflectiveFeatures(" << myReflectShader.name().asChar() << ")";

    MStatus myStatus;
    if (!exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "reflectedColor",
                        y60::PAINT, y60::DIFFUSE_PROPERTY, 1.0)) {
        MColor mySpecularColor = myReflectShader.specularColor(& myStatus);
        if (myStatus == MStatus::kSuccess) {
            setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::SPECULAR_PROPERTY,
                    asl::Vector4f(mySpecularColor.r, mySpecularColor.g, mySpecularColor.b, mySpecularColor.a));
        }
    }

    float myReflectivity = myReflectShader.reflectivity(& myStatus);
    if (myStatus == MStatus::kSuccess && myReflectivity > 0.0f) {
        setPropertyValue<float>(theBuilder.getNode(), "float", "reflectivity", myReflectivity);
    }
}

void
MaterialExporter::exportIncandescenceFeatures(const MFnMesh *, const MObject & theShaderNode,
                                              y60::MaterialBuilder & theBuilder,
                                              y60::SceneBuilder & theSceneBuilder)
{
    AC_DEBUG << "MaterialExporter::exportIncandescenceFeatures(" << MFnDependencyNode(theShaderNode).name().asChar() << ")";
    if (!exportTextures(0, theShaderNode, theBuilder, theSceneBuilder, "incandescence",
                y60::EMISSIVE, y60::DIFFUSE_PROPERTY, 1.0)) {

        /*
        MStatus myStatus;
        MColor myIncandescenceColor = MFnReflectShader(theShaderNode).incandescence(& myStatus);
        if (myStatus == MStatus::kSuccess) {
        }
        */
    }
}

void
MaterialExporter::exportPhongEFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                        y60::MaterialBuilder & theBuilder,
                                        y60::SceneBuilder & theSceneBuilder)
{
    exportReflectiveFeatures(theMesh, theShaderNode, theBuilder, theSceneBuilder);

    MStatus myStatus;
    float myHightlightSize = MFnPhongEShader(theShaderNode).highlightSize(& myStatus);

    float myShininess = asl::maximum(0.0f, 128.0f - (myHightlightSize * 100.0f)); // experimental
    setPropertyValue<float>(theBuilder.getNode(), "float", y60::SHININESS_PROPERTY, myShininess);
}

void
MaterialExporter::exportPhongFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                        y60::MaterialBuilder & theBuilder,
                                        y60::SceneBuilder & theSceneBuilder)
{
    exportReflectiveFeatures(theMesh, theShaderNode, theBuilder, theSceneBuilder);

    MStatus myStatus;
    float myShininess = MFnPhongShader(theShaderNode).cosPower(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get shininess from node",
                              "MaterialExporter::exportPhongFeatures");
    }

    // Convert to 0 - 128 range of OpenGL
    myShininess = (myShininess / 100) * 128;

    setPropertyValue<float>(theBuilder.getNode(), "float", y60::SHININESS_PROPERTY, myShininess);
}

void
MaterialExporter::exportBlinnFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                        y60::MaterialBuilder & theBuilder,
                                        y60::SceneBuilder & theSceneBuilder)
{
    exportReflectiveFeatures(theMesh, theShaderNode, theBuilder, theSceneBuilder);

    MStatus myStatus;
    float myEccentricity = MFnBlinnShader(theShaderNode).eccentricity(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get shininess from node",
                              "MaterialExporter::exportBlinnFeatures");
    }
    float mySpecularRolloff = MFnBlinnShader(theShaderNode).specularRollOff(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get specular rolloff from node",
                              "MaterialExporter::exportBlinnFeatures");
    }

    // experimentally found.
    // if (myEccentricity) {
    //    setPropertyValue<float>(theBuilder.getNode(), "float", "eccentricity", 1/(pow(myEccentricity,2.5f)));
    // } else {
    //    setPropertyValue<float>(theBuilder.getNode(), "float", "eccentricity", 10000000.0f);
    // }
    setPropertyValue<float>(theBuilder.getNode(), "float", "eccentricity", myEccentricity);
    setPropertyValue<float>(theBuilder.getNode(), "float", "specularrolloff", mySpecularRolloff);
}

std::string
MaterialExporter::createMaterial(const MFnMesh * theMesh, const MObject & theShaderNode,
                                 y60::SceneBuilder & theSceneBuilder, const SpecialFeatures & theSpecialFeatures)
{
    std::string myMaterialName(MFnDependencyNode(theShaderNode).name().asChar());
    y60::MaterialBuilder myMaterialBuilder(myMaterialName, _myInlineTexturesFlag);
    Image::allowInlineFlag = _myInlineTexturesFlag;

    std::string myMaterialId = theSceneBuilder.appendMaterial(myMaterialBuilder);

    DB(AC_TRACE << "MaterialExporter::createMaterial() - Exporting material: " << theShaderNode.apiTypeStr()
           << " with shader: " << myMaterialName);

    // Check for custom attributes
    MString myPhysics = "";
    if (getCustomAttribute(theShaderNode, "ac_physics", myPhysics)) {
        cerr << "Found physics in " << myMaterialName << " with value: " << myPhysics.asChar() << endl;
        myMaterialBuilder.addFeature("physics", VectorOfRankedFeature(1, RankedFeature(100, myPhysics.asChar())));
    }

    // Add special features collected in shape exporter
    for (unsigned i = 0; i < theSpecialFeatures.size(); ++i) {
        myMaterialBuilder.addFeature(theSpecialFeatures[i].classname,
               VectorOfRankedFeature(1, RankedFeature(100, theSpecialFeatures[i].values)));
    }

    VectorOfRankedFeature myLightingFeature;
    exportShader(theMesh, theShaderNode, myMaterialBuilder, theSceneBuilder, myLightingFeature);

    myMaterialBuilder.setType(myLightingFeature);
    if (myMaterialBuilder.getTransparencyFlag() == false && checkTransparency(theShaderNode)) {
        // if no texture has set the transparency flag, do our checks
        TargetBuffers myTB = TargetBuffers(((1<<RED_MASK)|(1<<GREEN_MASK)|(1<<BLUE_MASK)|(1<<ALPHA_MASK)|(0<<DEPTH_MASK)));
        setPropertyValue<TargetBuffers>(myMaterialBuilder.getNode(), TARGETBUFFERS_PROPERTY, "targetbuffers", myTB);

        myMaterialBuilder.setTransparencyFlag(true);
    }

    _myMaterialNameMap[myMaterialName] = myMaterialId;

    return myMaterialId;
}

void
MaterialExporter::exportShader(const MFnMesh * theMesh, const MObject & theShaderNode,
                               y60::MaterialBuilder & theMaterialBuilder,
                               y60::SceneBuilder & theSceneBuilder,
                               VectorOfRankedFeature & theLightingFeature)
{
    //dumpAttributes(theShaderNode);

    switch (theShaderNode.apiType()) {
        case MFn::kLambert: {
            createLightingFeature(theLightingFeature, y60::LAMBERT);
            exportLambertFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            break;
        }
        case MFn::kSurfaceShader: {
            createLightingFeature(theLightingFeature, y60::UNLIT);
            exportUnlitFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            break;
        }
        case MFn::kPhongExplorer: {
            createLightingFeature(theLightingFeature, y60::PHONG);
            exportLambertFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            exportPhongEFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            break;
        }
        case MFn::kPhong: {
            createLightingFeature(theLightingFeature, y60::PHONG);
            exportLambertFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            exportPhongFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            break;
        }
        case MFn::kBlinn: {
            createLightingFeature(theLightingFeature, y60::BLINN);
            exportLambertFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            exportBlinnFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
            break;
        }
        case MFn::kLayeredShader: {
            // dumpAttributes(theShaderNode);
            MPlug myInputPlug = getPlug(theShaderNode,"inputs");
            if (!myInputPlug.isArray()) {
                throw ExportException(string("kLayeredShader input plug '")+
                            myInputPlug.name().asChar()+"' is not an array. aborting.",
                            "MaterialExporter::exportShader()");
            }
            for (unsigned i = 0; i < myInputPlug.numElements(); ++i) {
                // Find connected shader node
                MPlug myColorPlug;
                MObject childShaderNode;
                getChildPlugByName("color", myInputPlug[i], theShaderNode, myColorPlug);
                getConnectedNode(myColorPlug, childShaderNode);

                // recurse
                exportShader(theMesh, childShaderNode, theMaterialBuilder, theSceneBuilder, theLightingFeature);
            }
            break;
        }
        default:
            throw ExportException(std::string("Unsupported shader: ") + theShaderNode.apiTypeStr(),
                                  "MaterialExporter::exportShader()");
    }
    exportIncandescenceFeatures(theMesh, theShaderNode, theMaterialBuilder, theSceneBuilder);
}


MObject
MaterialExporter::getMaterial(MIntArray & theIndices, MObjectArray & theShadingGroups,
                              unsigned theFaceIndex)
{
    MAKE_SCOPE_TIMER(MaterialExporter_getMaterial);

    int myMaterialIndex  = theIndices[theFaceIndex];

    if (myMaterialIndex != -1) {
        MAKE_SCOPE_TIMER(MaterialExporter_findShader);
        return findShader(theShadingGroups[myMaterialIndex]);
    }

    throw ExportException("Could not get material name", "MaterialExporter::getMaterial()");
}

std::string
MaterialExporter::createFaceMaterial(const MFnMesh * theMesh, const MObject & theMaterialNode,
                                     y60::SceneBuilder & theSceneBuilder, const SpecialFeatures & theSpecialFeatures)
{
    std::string myMaterialName = std::string(MFnDependencyNode(theMaterialNode).name().asChar());
    StringMap::iterator it = _myMaterialNameMap.find(myMaterialName);
    if (it != _myMaterialNameMap.end()) {
        // If the shader has already been created, return its id
        return it->second;
    } else {
        // If it does not exist yet, we have to create it.
        return createMaterial(theMesh, theMaterialNode, theSceneBuilder, theSpecialFeatures);
    }
}

const VectorOfString &
MaterialExporter::getTextureNames(const std::string & theMaterialName) {
    StringVectorMap::iterator myTextureNames = _myMaterialUVMappedTexturesMap.find(theMaterialName);
    if (myTextureNames != _myMaterialUVMappedTexturesMap.end()) {
        return myTextureNames->second;
    }

    throw ExportException(std::string("Could not find textures for '") + theMaterialName + "'",
            "MaterialExporter::getTextureNames");
}

bool
MaterialExporter::hasUVMappedTextures(const std::string & theMaterialName) {
    return (_myMaterialUVMappedTexturesMap.find(theMaterialName) != _myMaterialUVMappedTexturesMap.end());
}

void
MaterialExporter::setBaseDirectory(const std::string & theDirectory) {
#ifdef WIN32
    if (theDirectory.length() > 1 && theDirectory[1] != ':') {
#else
    if (theDirectory.length() > 0 && theDirectory[0] != '/') {
#endif
        // not an absolute path
        if (theDirectory[0] == '.' && theDirectory[1] == '/') {
            _myBaseDirectory = normalizeDirectory(getCWD(), false) +
                               normalizeDirectory(theDirectory.substr(2,theDirectory.length()) , false);
        } else {
            _myBaseDirectory = normalizeDirectory(getCWD(), false) + '/' + normalizeDirectory(theDirectory, false);
        }
    } else if (theDirectory.empty()) {
         //AC_DEBUG << "MaterialExporter::setBaseDirectory, relative base directory ->"<< getCWD();
        _myBaseDirectory = normalizeDirectory(getCWD(), false);
        _myBaseDirectory += '/';
    } else {
        // absolute path
        _myBaseDirectory = theDirectory;
    }

#ifndef WIN32
    char myRealPath[PATH_MAX];
    if (realpath(_myBaseDirectory.c_str(), myRealPath)) {
        _myBaseDirectory = std::string(myRealPath);
    }
#endif

    // be sure to end with a slash
    if (_myBaseDirectory[_myBaseDirectory.length()-1] != '/') {
        _myBaseDirectory += '/';
    }

        PackageManagerPtr myPackageManager = AppPackageManager::get().getPtr();
        myPackageManager->add(_myBaseDirectory);

    AC_DEBUG << "MaterialExporter::setBaseDirectory _myBaseDirectory=" << _myBaseDirectory << ", was:" << theDirectory;
}

std::string
MaterialExporter::findRelativeFilePath(const std::string & theFileName) {

    AC_TRACE << "MaterialExporter::findRelativeFilePath " << theFileName;

    std::string myFileName = theFileName;
#ifndef WIN32
    // resolve symlinks
    char myRealPath[PATH_MAX];
    if (realpath(myFileName.c_str(), myRealPath)) {
        myFileName = std::string(myRealPath);
    } else {
        AC_WARNING << "MaterialExporter::findRelativeFilePath unable to resolve path '" << myFileName << "'";
    }
#endif

    std::string myRelativeFileName = evaluateRelativePath(_myBaseDirectory, myFileName);
    if (fileExists(myRelativeFileName)) {
        return myRelativeFileName;
    }

    // find filename from base
    std::string myAbsoluteFileName = myRelativeFileName;
    std::size_t mySlashPos = std::string::npos;
    while (mySlashPos != 0) {
        mySlashPos = myAbsoluteFileName.rfind("/", mySlashPos);
        if (mySlashPos == std::string::npos) {
            break;
        }
        myRelativeFileName = myAbsoluteFileName.substr(mySlashPos+1);
        myFileName = _myBaseDirectory + myRelativeFileName;
        if (fileExists(myFileName)) {
            AC_DEBUG << "Found '" << myFileName << "' relative=" << myRelativeFileName;
            return myRelativeFileName;
        }
        if (mySlashPos > 0) {
            mySlashPos--;
        }
    }

    AC_ERROR << "Unable to find '" << myAbsoluteFileName << "' relative to '" << _myBaseDirectory << "'";
    return myAbsoluteFileName;
}

std::string
MaterialExporter::getStrippedTextureFilename(const MPlug & theTexturePlug) {
    MObject myFileNode;
    getConnectedNode(theTexturePlug, myFileNode);
    MString myFileName("");
    MPlug myFilenamePlug = MFnDependencyNode(myFileNode).findPlug("fileTextureName");

    myFilenamePlug.getValue(myFileName);

    return findRelativeFilePath(myFileName.asChar());
}

bool
MaterialExporter::checkAlphaTexture(const MObject & theShaderNode, const std::string thePlugName) {
    MStatus myStatus;

    MPlug myPlug = getPlug(theShaderNode, thePlugName.c_str());

    MPlugArray myPlugArray;
    myPlug.connectedTo(myPlugArray, true, false, & myStatus);
    if (myStatus == MS::kFailure) {
        throw ExportException("Could not get destination plug array",
                "MaterialExporter::checkAlphaTexture()");
    }

    if (myPlugArray.length() > 1) {
        throw ExportException("Unsupported type of color plug.",
                "MaterialExporter::checkAlphaTexture");
    }

    if (myPlugArray.length() > 0) {
        return true;
    }

    return false;
}

bool
MaterialExporter::checkTransparency(const MObject & theShaderNode) {
    bool myTransparencyFlag = false;
    DB(dumpAttributes(theShaderNode));

    MStatus myStatus;
    switch (theShaderNode.apiType()) {
        case MFn::kLambert:
        case MFn::kBlinn:
        case MFn::kPhongExplorer:
        case MFn::kPhong: {
            // TODO: This does not work if a layered shader is connected to the color plug (as in material_test.x60)
            if (checkAlphaTexture(theShaderNode, "transparency")) {
                myTransparencyFlag = true;
            } else {
                MColor myTransparency = MFnLambertShader(theShaderNode).transparency(& myStatus);
                if (myTransparency.r != 0 || myTransparency.g != 0
                    || myTransparency.b != 0 || myTransparency.a < 1.0) {
                    myTransparencyFlag = true;
                }
            }
            break;
        }

        case MFn::kSurfaceShader: {
            // TODO: This does not work if a layered shader is connected to the color plug (as in material_test.x60)
            if (checkAlphaTexture(theShaderNode, "outTransparency")) {
                myTransparencyFlag = true;
            } else {
                MPlug myTransparencyPlugR = MFnDependencyNode(theShaderNode).findPlug("outTransparencyR");
                MPlug myTransparencyPlugG = MFnDependencyNode(theShaderNode).findPlug("outTransparencyG");
                MPlug myTransparencyPlugB = MFnDependencyNode(theShaderNode).findPlug("outTransparencyB");
                double myColorR;
                double myColorB;
                double myColorG;
                myTransparencyPlugR.getValue(myColorR);
                myTransparencyPlugG.getValue(myColorG);
                myTransparencyPlugB.getValue(myColorB);
                if (myColorR != 0 || myColorG != 0 || myColorB != 0) {
                    AC_DEBUG << "outTransparencyRGB true";
                    myTransparencyFlag = true;
                }
            }
            break;
        }
        case MFn::kLayeredShader: {
            myTransparencyFlag = true; // TODO: recurse into shaders
            break;
        }
        default:
            throw ExportException(std::string("Unsupported shader: ") + theShaderNode.apiTypeStr(),
                                  PLUS_FILE_LINE);
    }

    return myTransparencyFlag;
}

