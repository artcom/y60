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
//   $RCSfile: MaterialExporter.cpp,v $
//   $Author: jens $
//   $Revision: 1.81 $
//   $Date: 2005/04/27 16:29:55 $
//
//  Description: This class implements a polygon exporter plugin for maya.
//
//=============================================================================

#include "MayaHelpers.h"
#include "MaterialExporter.h"
#include "ExportExceptions.h"

#define DEBUG_LEVEL 0

#include <y60/MaterialTypes.h>
#include <y60/DataTypes.h>
#include <y60/SceneBuilder.h>
#include <y60/ShapeBuilder.h>
#include <y60/MaterialBuilder.h>
#include <y60/NodeValueNames.h>
#include <y60/PropertyNames.h>
#include <y60/property_functions.h>

#include <asl/file_functions.h>
#include <asl/string_functions.h>
#include <asl/numeric_functions.h>
#include <asl/Dashboard.h>
#include <asl/Matrix4.h>

#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MColor.h>
#include <maya/MFnDagNode.h>
#include <maya/MFnAttribute.h>
#include <maya/MItDependencyGraph.h>
#include <maya/MFnPhongShader.h>
#include <maya/MFnPhongEShader.h>
//#include <maya/MFn.h>
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
                                    const MayaBlendMode theBlendMode,
                                    float theColorGainAlpha)
{
    MStatus myStatus;


    std::string myMaterialName = theBuilder.getName();
    std::string myTextureName(MFnDependencyNode(theTextureNode).name().asChar());
    DB(AC_PRINT << "dumping texture '" << myTextureName << "'");
    DB(dumpAttributes(theTextureNode));
    DB(
        MPlugArray myConnections;
        MFnDependencyNode(theTextureNode).getConnections(myConnections);
        for(int i = 0;i < myConnections.length(); ++i) {
            AC_PRINT << "CONN " << i << ": " <<myConnections[i].name();
        }
    );

    // texture mapping
    string myTextureMapping = getTextureMappingType(theTextureNode);

    MObject myImageNode = theTextureNode;

    MString myFileName("");
    if (myTextureMapping == y60::TEXCOORD_UV_MAP) {
        _myMaterialUVMappedTexturesMap[myMaterialName].push_back(myTextureName);

    } else { // generative mapping
        MPlug myImagePlug = MFnDependencyNode(theTextureNode).findPlug("image");
        getConnectedNode(myImagePlug, myImageNode);
    }

    MPlug myFilenamePlug = MFnDependencyNode(myImageNode).findPlug("fileTextureName");
    myFilenamePlug.getValue(myFileName);

    std::string myStrippedFileName = findRelativeFilePath(myFileName.asChar());

    // color gain = scale
    Vector4f myColorScale(1,1,1,theColorGainAlpha);
    MPlug myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainR");
    myColorGainPlug.getValue(myColorScale[0]);
    myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainG");
    myColorGainPlug.getValue(myColorScale[1]);
    myColorGainPlug = MFnDependencyNode(theTextureNode).findPlug("colorGainB");
    myColorGainPlug.getValue(myColorScale[2]);

    Vector4f myColorOffset(0,0,0,0);
    MPlug myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetR");
    myColorOffsetPlug.getValue(myColorOffset[0]);
    myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetG");
    myColorOffsetPlug.getValue(myColorOffset[1]);
    myColorOffsetPlug = MFnDependencyNode(theTextureNode).findPlug("colorOffsetB");
    myColorOffsetPlug.getValue(myColorOffset[2]);

    // get custom attributes
    bool myCreateMipmapsFlag = true;
    getCustomAttribute(theTextureNode, "ac_mipmaps", myCreateMipmapsFlag);

    std::string myApplyMode = y60::TEXTURE_APPLY_MODULATE;
    std::string myUsage     = y60::TEXTURE_USAGE_PAINT;
    switch (theBlendMode) {
        case MAYA_BLEND_NONE:
            myApplyMode = y60::TEXTURE_APPLY_REPLACE;
            break;
        case MAYA_BLEND_OVER:
            myApplyMode = y60::TEXTURE_APPLY_DECAL;
            break;
        case MAYA_BLEND_MULTIPLY:
            myApplyMode = y60::TEXTURE_APPLY_MODULATE;
            break;
        case MAYA_BLEND_ADD:
            myApplyMode = y60::TEXTURE_APPLY_ADD;
            break;
        default:
            displayWarning(string("Blendmode: ") + getStringFromEnum(theBlendMode, MayaBlendModesString) + " not yet implemented.");
    }

    AC_DEBUG << "stripped filename=" << myStrippedFileName << " Scale=" << myColorScale << ", Bias=" << myColorOffset;
    string myImageId = theBuilder.createImage(theSceneBuilder,
        MFnDependencyNode(theTextureNode).name().asChar(),
        myStrippedFileName, myUsage, myCreateMipmapsFlag,
        myColorScale, myColorOffset, SINGLE,"");

    // wrap
    std::string myWrapMode  = y60::TEXTURE_WRAP_REPEAT;
    bool myWrapU = false;
    MPlug myWarpUPlug = MFnDependencyNode(myImageNode).findPlug("wrapU");
    if (myWarpUPlug.getValue(myWrapU) != MStatus::kSuccess) {
        AC_WARNING << "No wrapU";
    }

    bool myWrapV = false;
    MPlug myWarpVPlug = MFnDependencyNode(myImageNode).findPlug("wrapV");
    if (myWarpVPlug.getValue(myWrapV) != MStatus::kSuccess) {
        AC_WARNING << "No wrapV";
    }
    if (!myWrapU && !myWrapV) {
        myWrapMode = TEXTURE_WRAP_CLAMP;
    }

    // texture matrix
    asl::Matrix4f myTextureMatrix;
    myTextureMatrix.makeIdentity();

    MObject myPlacementMatrix;
    MPlug myPlacementMatrixPlug = MFnDependencyNode(theTextureNode).findPlug("placementMatrix");
    myPlacementMatrixPlug.getValue(myPlacementMatrix);
    MFnMatrixData myMatrixData(myPlacementMatrix, &myStatus);
    if (myStatus == MStatus::kSuccess) {

        // fetch placement matrix
        MTransformationMatrix myTransform = myMatrixData.transformation();
        MMatrix myMayaMatrix = myTransform.asMatrix();

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
            cout << "+++ maya scale=" << mayaScale[0] << "," << mayaScale[1] << "," << mayaScale[2] << endl;

            MVector mayaTrans = myTransform.translation(MSpace::kWorld);
            cout << "+++ maya trans=" << mayaTrans[0] << "," << mayaTrans[1] << "," << mayaTrans[2] << endl;

            MEulerRotation mayaEuler = myTransform.eulerRotation();
            cout << "+++ maya euler=" << mayaEuler[0] << "," << mayaEuler[1] << "," << mayaEuler[2] << endl;

            /*
             * Calculate texgen matrix
             */
            cout << "***" << endl;
            cout << __DATE__ << " " << __TIME__ << endl;
            cout << "mtx=" << myMatrix << endl;
#if 0
            asl::Matrix4f myMtxInv = myMatrix;
            myMtxInv.invert();
            cout << "inv=" << myMtxInv << endl;
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
            cout << "bbox size=" << myBBoxSize.x << "," << myBBoxSize.y << "," << myBBoxSize.z << endl;

            asl::Matrix4f myNormalizationMatrix;
            myNormalizationMatrix.makeIdentity();
            myNormalizationMatrix.scale(asl::Vector3f(1.0f, -1.0f, 1.0f));
            myNormalizationMatrix.translate(asl::Vector3f(0.5f, 0.5f, 0.5f));

            // fetch parameters from matrix
            asl::Vector3f myScale, myShear, myRotation, myTranslation;
            myMatrix.decompose(myScale, myShear, myRotation, myTranslation);

            myScale[0] *= float(myBBoxSize.x * 0.5);
            myScale[1] *= float(myBBoxSize.y * 0.5);
            myScale[2] *= float(myBBoxSize.z * 0.5f);

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
            cout << "pre-norm=" << myMatrix << endl;
            myMatrix.postMultiply(myNormalizationMatrix);
            cout << "final=" << myMatrix << endl;

            VectorOfVector4f myTexGenParams;
            myTexGenParams.push_back(myMatrix.getColumn(0));
            myTexGenParams.push_back(myMatrix.getColumn(1));
            myTexGenParams.push_back(myMatrix.getColumn(2));
            myTexGenParams.push_back(myMatrix.getColumn(3));
            setPropertyValue<VectorOfVector4f>(theBuilder.getNode(),
                    "vectorofvector4f", "texgenparam0", myTexGenParams);

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

    theBuilder.createTextureNode(myImageId, myApplyMode, myUsage, myWrapMode,
            myTextureMapping, myTextureMatrix, 100, false, 50);
}

string
MaterialExporter::getTextureMappingType(const MObject & theTextureNode) {
    string myResult;
    double myProjectionType = 0;
    MPlug myProjectionTypePlug = MFnDependencyNode(theTextureNode).findPlug("projType");
    myProjectionTypePlug.getValue(myProjectionType);
    unsigned myMappingIndex = unsigned(myProjectionType);
    if (myMappingIndex < 0 || myMappingIndex > 8 ) {
        throw ExportException(string("Sorry, mapping mode not supported: ") + asl::as_string(myMappingIndex), PLUS_FILE_LINE);
    }

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

    std::string myApplyMode = y60::TEXTURE_APPLY_DECAL;
    std::string myUsage     = y60::TEXTURE_USAGE_BUMP;
    std::string myWrapMode  = y60::TEXTURE_WRAP_REPEAT;

    string myImageId = theBuilder.createImage(theSceneBuilder,
        MFnDependencyNode(theBumpNode).name().asChar(),
        myFileName, myUsage, false, asl::Vector4f(1.0f,1.0f,1.0f,1.0f),
        asl::Vector4f(0.0f,0.0f,0.0f,0.0f), SINGLE, "");

    theBuilder.createTextureNode(myImageId, myApplyMode, myUsage, myWrapMode, TEXCOORD_UV_MAP, asl::Matrix4f::Identity(), 100, false, 0);
    theBuilder.needTextureFallback(true);
    std::string myMaterialName = theBuilder.getName();
    std::string myTextureName(MFnDependencyNode(theBumpNode).name().asChar());
    _myMaterialUVMappedTexturesMap[myMaterialName].push_back(myTextureName);
}

void
MaterialExporter::exportEnvCubeTexture(const MObject & theShaderNode,
                                       const MObject & theEnvCubeNode,
                                    y60::MaterialBuilder & theBuilder,
                                    y60::SceneBuilder & theSceneBuilder)
{
    DB(AC_TRACE << "MaterialExporter::exportEnvCubeTexture() - Export texture object: " <<
          MFnDependencyNode(theEnvCubeNode).name().asChar());
    DB(dumpAttributes(theEnvCubeNode));

    MStatus myStatus;

    std::string frontFileName  = getStrippedTextureFilename(getPlug(theEnvCubeNode,"front"));
    std::string rightFileName  = getStrippedTextureFilename(getPlug(theEnvCubeNode,"right"));
    std::string backFileName   = getStrippedTextureFilename(getPlug(theEnvCubeNode,"back"));
    std::string leftFileName   = getStrippedTextureFilename(getPlug(theEnvCubeNode,"left"));
    std::string topFileName    = getStrippedTextureFilename(getPlug(theEnvCubeNode,"top"));
    std::string bottomFileName = getStrippedTextureFilename(getPlug(theEnvCubeNode,"bottom"));

    std::string myApplyMode = y60::TEXTURE_APPLY_MODULATE;

    float myReflectivity = MFnPhongShader(theShaderNode).reflectivity(& myStatus);
    asl::Vector4f myColorScale = asl::Vector4f(1, 1, 1, myReflectivity);

    MColor mySpecularColor = MFnReflectShader(theShaderNode).specularColor(& myStatus);

    theBuilder.appendCubemap(theSceneBuilder,
            MFnDependencyNode(theEnvCubeNode).name().asChar(),
            frontFileName, rightFileName, backFileName,
            leftFileName, topFileName, bottomFileName,
            myApplyMode, myColorScale);

    //NOTE: specular colors are added to the diffuse colors.
    //      no alpha value support for specular colors therefore means
    //      setting the specular alpha to zero.
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
MaterialExporter::exportLayeredTexture(const MFnMesh * theMesh, const MObject & theMultiTextureNode,
                                       y60::MaterialBuilder & theBuilder,
                                       y60::SceneBuilder & theSceneBuilder,
                                       const char* theColorGainPropertyName,
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
    for (int i = myInputPlug.numElements() - 1; i >= 0; --i) {
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

        DB(AC_TRACE << "MaterialExporter::exportLayeredTexture() - alpha: " << myAlpha << ", blendmode: "
            << getStringFromEnum(myBlendMode, MayaBlendModesString));

        // Find connected texture node
        MPlug myColorPlug;
        MObject myTextureNode;
        getChildPlugByName("color", myInputPlug[i], theMultiTextureNode, myColorPlug);
        getConnectedNode(myColorPlug, myTextureNode);

        exportFileTexture(theMesh, myTextureNode, theBuilder, theSceneBuilder,
                 myBlendMode, theColorGainAlpha);
        y60::setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", theColorGainPropertyName, asl::Vector4f(1,1,1,1));
    }
}

bool
MaterialExporter::exportTextures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                 y60::MaterialBuilder & theBuilder,
                                 y60::SceneBuilder & theSceneBuilder,
                                 const std::string & thePlugName,
                                 const char* theColorGainPropertyName,
                                 float theColorGainAlpha)
{
    try {
        return exportMaps(theMesh, theShaderNode,theBuilder, theSceneBuilder,
                thePlugName.c_str(), theColorGainPropertyName, theColorGainAlpha);
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
        return exportMaps(theMesh, theShaderNode,theBuilder, theSceneBuilder, "normalCamera", y60::DIFFUSE_PROPERTY, theColorGainAlpha);
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
                             const char* theColorGainPropertyName,
                             float theColorGainAlpha)
{
    bool hasTextures = false;
    MStatus myStatus;
    DB(AC_TRACE << "MaterialExporter::exportMaps("<<thePlugName<<")");

    DB(dumpAttributes(theShaderNode));
    MPlug myPlug = getPlug(theShaderNode,thePlugName);

    MPlugArray myPlugArray;
    myPlug.connectedTo(myPlugArray, true, false, & myStatus);
    if (myStatus == MS::kFailure) {
        throw ExportException("Could not get destination plug array",
                              "MaterialExporter::getTexture()");
    }

    if (myPlugArray.length() > 1) {
        throw ExportException("Unsupported type of color plug.",
                              "MaterialExporter::getTexture()");
    } else if (myPlugArray.length() == 1) {
        hasTextures = true;

        MObject myTextureNode(myPlugArray[0].node());
        MFn::Type myTextureType = myTextureNode.apiType();
        switch (myTextureType) {
            case MFn::kLayeredTexture:
                exportLayeredTexture(theMesh, myTextureNode, theBuilder, theSceneBuilder,
                                     theColorGainPropertyName, theColorGainAlpha);
                break;
            case MFn::kProjection:
            case MFn::kFileTexture:
                exportFileTexture(theMesh, myTextureNode, theBuilder, theSceneBuilder,
                                  ourDefaultBlendMode, theColorGainAlpha);
                y60::setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", 
                        theColorGainPropertyName, asl::Vector4f(1,1,1,1));
                break;
            case MFn::kBump:
                exportBumpTexture(myTextureNode, theBuilder, theSceneBuilder);
                break;
            case MFn::kEnvCube:
                exportEnvCubeTexture(theShaderNode, myTextureNode, theBuilder, theSceneBuilder);
                break;
            default:
                throw ExportException(std::string("Unsupported type of map plug: ") +
                        myPlugArray[0].node().apiTypeStr(),
                        "MaterialExporter::exportMaps()");
        }

        // check texture alpha, set material transparency if set
        const MFnDependencyNode & myTextureDepNode = MFnDependencyNode(myTextureNode);
        MObject myFileHasAlpha = MFnDependencyNode(myTextureNode).attribute("fileHasAlpha", &myStatus);
        if (myStatus) {
            MPlug myPlug = myTextureDepNode.findPlug(myFileHasAlpha, &myStatus);
            if (myStatus) {
                double myDoubleValue;
                if (!myPlug.isArray() && myPlug.getValue(myDoubleValue)) {
                    AC_DEBUG << thePlugName << " fileHasAlpha=" << myDoubleValue;
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
        if ( ! exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "outColor", y60::SURFACE_COLOR_PROPERTY, myColorGainAlpha)) {
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
    DB(AC_TRACE << "MaterialExporter::exportLambertFeatures()");
    MStatus myStatus;

    try {
        // If no textures were found, we can export the diffuse color value instead
        MColor myTransparency = MFnLambertShader(theShaderNode).transparency(& myStatus);
        DB(AC_TRACE << "myTransp = "<< asl::Vector4f(myTransparency.r, myTransparency.g, myTransparency.b, myTransparency.a) << endl);
       if ( ! exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "color",
            y60::DIFFUSE_PROPERTY, 1.0f - myTransparency.r ))
       {
            MColor myColor = MFnLambertShader(theShaderNode).color(& myStatus);
            if (myStatus == MStatus::kFailure) {
                throw ExportException("Could not get color from node",
                        "MaterialExporter::exportLambertFeatures");
            }

            DB(AC_TRACE << "myColor  = "<< asl::Vector4f(myColor.r, myColor.g, myColor.b, myColor.a) << endl);

            setPropertyValue<asl::Vector4f>(theBuilder.getNode(),
                    "vector4f",
                    y60::DIFFUSE_PROPERTY,
                    asl::Vector4f(myColor.r, myColor.g, myColor.b, float(1.0f - myTransparency.r)));
        }
        exportBumpMaps(theMesh, theShaderNode, theBuilder, theSceneBuilder, 1.0f - myTransparency.r);
    } catch(asl::Exception & ex) {
        ex; // avoid unreferenced variable warning
        DB(AC_TRACE << ex);
        throw;
    }

    MColor myAmbientColor = MFnLambertShader(theShaderNode).ambientColor(& myStatus);
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get color from node",
                "MaterialExporter::exportLambertFeatures");
    }
    //setPropertyValue<asl::Vector4f>(theBuilder.getNode(),"vector4f", y60::DIFFUSE_PROPERTY, asl::Vector4f(1.0, 1.0, 1.0, 1.0));
    setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::AMBIENT_PROPERTY,
            asl::Vector4f(myAmbientColor.r, myAmbientColor.g, myAmbientColor.b, myAmbientColor.a));

    float myGlowFactor = MFnLambertShader(theShaderNode).glowIntensity( & myStatus );
    if (myStatus == MStatus::kFailure) {
        throw ExportException("Could not get glow from node",
                "MaterialExporter::exportLambertFeatures");
    }
    setPropertyValue<float>(theBuilder.getNode(), "float", y60::GLOW_PROPERTY, myGlowFactor);

}

void
MaterialExporter::exportReflectiveFeatures(const MFnMesh * theMesh, const MObject & theShaderNode,
                                        y60::MaterialBuilder & theBuilder,
                                        y60::SceneBuilder & theSceneBuilder)
{
    DB(AC_TRACE << "MaterialExporter::exportReflectiveFeatures()");
    MStatus myStatus;
    if ( ! exportTextures(theMesh, theShaderNode, theBuilder, theSceneBuilder, "reflectedColor", y60::DIFFUSE_PROPERTY, 1.0)) {
        MColor mySpecularColor = MFnReflectShader(theShaderNode).specularColor(& myStatus);
        if (myStatus == MStatus::kFailure) {
            throw ExportException("Could not get specular color from node",
                                "MaterialExporter::exportReflectiveFeatures");
        }

        setPropertyValue<asl::Vector4f>(theBuilder.getNode(), "vector4f", y60::SPECULAR_PROPERTY,
            asl::Vector4f(mySpecularColor.r, mySpecularColor.g, mySpecularColor.b, mySpecularColor.a));
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

    // Convert to 0 - 128 range of OpenGL
    float myShininess = (1.0f-myEccentricity) * 128;

    setPropertyValue<float>(theBuilder.getNode(), "float", y60::SHININESS_PROPERTY, myShininess);
}

std::string
MaterialExporter::createMaterial(const MFnMesh * theMesh, const MObject & theShaderNode,
                                 y60::SceneBuilder & theSceneBuilder, const SpecialFeatures & theSpecialFeatures)
{
    std::string myMaterialName(MFnDependencyNode(theShaderNode).name().asChar());
    y60::MaterialBuilder myMaterialBuilder(myMaterialName, _myInlineTexturesFlag);
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
    if (myMaterialBuilder.getTransparencyFlag() == false) {
        // if no texture has set the transparency flag, do our checks
        myMaterialBuilder.setTransparencyFlag(checkTransparency(theShaderNode));
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
            createLightingFeature(theLightingFeature, y60::PHONG);
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
    // be sure to end with a slash
    if (_myBaseDirectory[_myBaseDirectory.length()-1] != '/') {
        _myBaseDirectory += '/';
    }
    AC_DEBUG << "MaterialExporter::setBaseDirectory _myBaseDirectory=" << _myBaseDirectory << ", was:" << theDirectory;
}

std::string
MaterialExporter::findFile(const std::string & theFileName) {
    if (fileExists(theFileName)) {
        return theFileName;
    } else {
        // If the file does not exist we have to try to find it...
        string myDirectoryPart = theFileName;
        size_t mySlashIndex = myDirectoryPart.rfind('/');
        while (mySlashIndex != string::npos) {
            string myFilePart = theFileName.substr(mySlashIndex + 1, theFileName.length());
            myDirectoryPart = myDirectoryPart.substr(0, mySlashIndex);

            string myTestPath = _myBaseDirectory + myFilePart;
            if (fileExists(myTestPath)) {
                return myTestPath;
            }
            mySlashIndex = myDirectoryPart.rfind('/');
        }

        throw ExportException(string("Could not find file: ") + theFileName, PLUS_FILE_LINE);
    }
}

char convertBackToForwardSlash(char theCharacter) {
    if (theCharacter == '\\') {
        theCharacter = '/';
    }
    return theCharacter;
}

std::string
MaterialExporter::findRelativeFilePath(const std::string & theFileName) {

    // convert backslash to forward slash
    std::string myFileName = theFileName;
    std::transform(theFileName.begin(), theFileName.end(),
                   myFileName.begin(), convertBackToForwardSlash);

    // find file relative to base directory
    string myFoundFileName = findFile(myFileName);
    myFileName = myFoundFileName;
    std::string myBaseName = _myBaseDirectory;
#ifdef WIN32
    // Convert basedir and filename to upper-case before comparing, because windows
    // is case-INsensitive and therefore the comparison needs to be too :( [jb]
    std::transform(myFileName.begin(), myFileName.end(),
                   myFileName.begin(), std::toupper);
    std::transform(myBaseName.begin(), myBaseName.end(),
                   myBaseName.begin(), std::toupper);
#endif

    // Strip the base directory
    size_t myIndex = myFileName.find(myBaseName);
    if (myIndex != std::string::npos) {
        // If the base directory is a part of the filename, just strip it.
        return myFoundFileName.substr(myIndex + myBaseName.length());
    } else {
        // Try to find as much equal directories as possible from BaseDirectory
        bool myEqualFlag = true;
        std::string myBaseRest(myBaseName);
        std::string myFileRest(myFileName);
        while (myEqualFlag) {
            size_t mySlashBaseIndex = myBaseRest.find('/');
            size_t mySlashFilenameIndex = myFileRest.find('/');
            if (mySlashBaseIndex != std::string::npos && mySlashFilenameIndex != std::string::npos) {
                std::string myBaseDir = myBaseRest.substr(0, mySlashBaseIndex);
                std::string myFilenameDir = myFileRest.substr(0, mySlashFilenameIndex);

                if (myBaseDir == myFilenameDir) {
                    myBaseRest = myBaseRest.substr(mySlashBaseIndex+1, myBaseRest.length());
                    myFileRest = myFileRest.substr(mySlashFilenameIndex+1, myFileRest.length());
                } else {
                    myEqualFlag = false;
                }
            } else {
                myEqualFlag = false;
            }
        }

        // Check if we have common root directories
        if (myBaseRest != _myBaseDirectory && myFileRest != myFileName) {
#ifdef WIN32
            // redo the case magic under windows by reconstructing the original path's case [jb]
            myFileRest = myFoundFileName.substr(myFoundFileName.length() - myFileRest.length());
#endif
            // count the directories in the rest of the base name and add some relative '..' to restfilename
            int myResult = std::count(myBaseRest.begin(), myBaseRest.end(), '/');
            for (unsigned i = 0; i < myResult; ++i) {
                myFileRest = std::string("../") + myFileRest;
            }
            AC_DEBUG << "MaterialExporter::findRelativeFilePath stripped filename=" << myFileRest;
            return myFileRest;
        }
        AC_DEBUG << "MaterialExporter::findRelativeFilePath nothing to do.";
        return myFoundFileName;
    }
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

