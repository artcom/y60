//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "JSModellingFunctions.h"
#include "JScppUtils.h"

#include "JSNode.h"
#include "JSVector.h"
#include "JSBox.h"
#include "JSMatrix.h"

#include "JSScene.h"
#include <y60/modelling_functions.h>
#include <y60/Body.h>

#include <iostream>

using namespace std;
using namespace asl;

using namespace y60;

namespace jslib {

JS_STATIC_DLL_CALLBACK(JSBool)
CreateTransform(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a new transform node and adds it to a given parent");
        DOC_PARAM("theParentNode", "A node in the world", DOC_TYPE_NODE);
        DOC_RVAL("The new transform node", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 1);

        dom::NodePtr  myParentNode(0);
        if (!convertFrom(cx, argv[0], myParentNode)) {
            JS_ReportError(cx,"CreateTransform: argument 1 is not a node");
            return JS_FALSE;
        }

        dom::NodePtr myResult = createTransform(myParentNode);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateCanvas(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a new canvas");
        DOC_PARAM("theScene", "The scene to create the canvas inside", DOC_TYPE_SCENE);
        DOC_PARAM("theName", "Name of the canvas", DOC_TYPE_STRING);
        DOC_RVAL("The new canvas node", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 2);

        y60::ScenePtr mySceneNode(0);
        convertFrom(cx, argv[0], mySceneNode);
        string myCanvasName;
        convertFrom(cx, argv[1], myCanvasName);

        dom::NodePtr myResult = createCanvas(mySceneNode, myCanvasName);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateBody(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a new body");
        DOC_PARAM("theParentNode", "A node inside the world, to append the body to", DOC_TYPE_NODE);
        DOC_PARAM("theShapeId", "The id of the shape connected to the body", DOC_TYPE_STRING);
        DOC_RVAL("The new body node", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 2);

        dom::NodePtr  myParentNode(0);
        convertFrom(cx, argv[0], myParentNode);
        string myShapeId;
        convertFrom(cx, argv[1], myShapeId);

        dom::NodePtr myResult = createBody(myParentNode, myShapeId);
        
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateQuad(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a quad");
        DOC_PARAM("theScene", "", DOC_TYPE_OBJECT);
        DOC_PARAM("theMaterialId", "", DOC_TYPE_STRING);
        DOC_PARAM("theTopLeftCorner", "", DOC_TYPE_VECTOR3F);
        DOC_PARAM("theBottomRightCorner", "", DOC_TYPE_VECTOR3F);
        DOC_RVAL("The quad shape node", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 4);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);
        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);
        Vector3f myTopLeftCorner;
        convertFrom(cx, argv[2], myTopLeftCorner);
        Vector3f myBottomRightCorner;
        convertFrom(cx, argv[3], myBottomRightCorner);

        dom::NodePtr myResult = createQuad(myScene, myMaterialId,
                                           myTopLeftCorner, myBottomRightCorner);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateSurface2DFromContour(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a surface from a unclosed countour, defined by a VectorOfVector2f");
        DOC_PARAM("theScene", "", DOC_TYPE_OBJECT);
        DOC_PARAM("theMaterialId", "", DOC_TYPE_STRING);
        DOC_PARAM("theContour", "", DOC_TYPE_VECTOROFVECTOR2F);
        DOC_PARAM("theShapeName", "", DOC_TYPE_STRING);
        DOC_PARAM_OPT("theEqualThreshold", "", DOC_TYPE_FLOAT, 1e-5);
        DOC_RVAL("The quad shape node", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 4, 5);
        dom::NodePtr myResult;
        if (argc == 4 || argc == 5) {
            y60::ScenePtr myScene(0);
            convertFrom(cx, argv[0], myScene);
            string myMaterialId;
            convertFrom(cx, argv[1], myMaterialId);
            VectorOfVector2f myContour;
            convertFrom(cx, argv[2], myContour);
            string myShapeName;
            convertFrom(cx, argv[3], myShapeName);
            if (argc == 4) {
                myResult = createSurface2DFromContour(myScene, myMaterialId,
                                                      myContour, myShapeName);
            } else {
                float myThreshold;
                convertFrom(cx, argv[4], myThreshold);
                myResult = createSurface2DFromContour(myScene, myMaterialId,
                                                      myContour, myShapeName, myThreshold);
            }
        }        
        
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateCrosshair(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a new crosshair shape");
        DOC_PARAM("theScene", "The scene to append the crosshair to", DOC_TYPE_SCENE);
        DOC_PARAM("theMaterialId", "The id of the material for the crosshair shape", DOC_TYPE_STRING);
        DOC_PARAM("theInnerRadius", "Inner radius", DOC_TYPE_FLOAT);
        DOC_PARAM("theHairLength", "Crosshair length", DOC_TYPE_FLOAT);
        DOC_PARAM("theName", "Name of the shape", DOC_TYPE_STRING);
        DOC_RVAL("The new crosshair shape", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        float myInnerRadius;
        convertFrom(cx, argv[2], myInnerRadius);

        float myHairLength;
        convertFrom(cx, argv[3], myHairLength);

        string myName;
        convertFrom(cx, argv[4], myName);

        dom::NodePtr myResult = createCrosshair(myScene, myMaterialId, myInnerRadius,
                                                myHairLength, myName);
        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateDistanceMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Create a distance markup shape");
        DOC_PARAM("theScene", "The scene to create the distance inside", DOC_TYPE_SCENE);
        DOC_PARAM("theMaterialId", "The id of the material used for the shapes", DOC_TYPE_STRING);
        DOC_PARAM("thePositions", "Array of positions", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_PARAM_OPT("theName", "Name for the quadstack shape", DOC_TYPE_STRING, "");
        DOC_RVAL("The new created distance shape", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 3, 4);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        std::vector<asl::Vector3f> myPositions;
        convertFrom(cx, argv[2], myPositions);

        string myName;
        if (argc > 3) {
            convertFrom(cx, argv[3], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 3:
                myResult = createDistanceMarkup(myScene, myMaterialId, myPositions);
                break;
            case 4:
                myResult = createDistanceMarkup(myScene, myMaterialId,
                                                myPositions, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateAngleMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Create an angle markup shape");
        DOC_PARAM("theScene", "The scene to create the angle inside", DOC_TYPE_SCENE);
        DOC_PARAM("theMaterialId", "The id of the material used for the shapes", DOC_TYPE_STRING);
        DOC_PARAM("theApex", "", DOC_TYPE_VECTOR3F);
        DOC_PARAM("thePointA", "", DOC_TYPE_VECTOR3F);
        DOC_PARAM("thePointA", "", DOC_TYPE_VECTOR3F);
        DOC_PARAM_OPT("theName", "Name for the quadstack shape", DOC_TYPE_STRING, "");
        DOC_RVAL("The new created angle shape", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 6, 7);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        asl::Vector3f myApex;
        convertFrom(cx, argv[2], myApex);

        asl::Vector3f myPointA;
        convertFrom(cx, argv[3], myPointA);

        asl::Vector3f myPointB;
        convertFrom(cx, argv[4], myPointB);

        bool myOuterAngleFlag;
        convertFrom(cx, argv[5], myOuterAngleFlag);

        string myName;
        if (argc > 6) {
            convertFrom(cx, argv[6], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 6:
                myResult = createAngleMarkup(myScene, myMaterialId, myApex, myPointA,
                                             myPointB, myOuterAngleFlag);
                break;
            case 7:
                myResult = createAngleMarkup(myScene, myMaterialId, myApex, myPointA,
                                             myPointB, myOuterAngleFlag, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

static JSBool CreateStrip(const std::string &theType, JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) 
{
    try {
        ensureParamCount(argc, 3, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myMaterialId;
        convertFrom(cx, argv[1], myMaterialId);

        std::vector<asl::Vector3f> myPositions;
        convertFrom(cx, argv[2], myPositions);

        std::vector<asl::Vector2f> myTexCoords;
        if (argc > 3) {
            convertFrom(cx, argv[3], myTexCoords);
        }

        std::vector<asl::Vector4f> myColors;
        if (argc > 4) {
            convertFrom(cx, argv[4], myColors);
        }
        dom::NodePtr myResult = createStrip(theType, myScene, myMaterialId, myPositions, myTexCoords, myColors);
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateLineStrip(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) 
{
    DOC_BEGIN("Create a linestrip shape");
    DOC_PARAM("theScene", "The scene to create the strip in", DOC_TYPE_SCENE);
    DOC_PARAM("theMaterialId", "Shape material id.", DOC_TYPE_STRING);
    DOC_PARAM("thePositions", "Array of positions", DOC_TYPE_VECTOROFVECTOR3F);
    DOC_PARAM_OPT("theTexCoords", "Texture Coordinates", DOC_TYPE_VECTOROFVECTOR2F, "");
    DOC_PARAM_OPT("theColors", "Vertexcolors", DOC_TYPE_VECTOROFVECTOR4F, "");
    DOC_RVAL("The new created linestrip shape", DOC_TYPE_NODE);
    DOC_END;
    return CreateStrip(PRIMITIVE_TYPE_LINE_STRIP, cx, obj, argc, argv, rval);
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateQuadStrip(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) 
{
    DOC_BEGIN("Create a quadstrip shape");
    DOC_PARAM("theScene", "The scene to create the strip in", DOC_TYPE_SCENE);
    DOC_PARAM("theMaterialId", "Shape material id.", DOC_TYPE_STRING);
    DOC_PARAM("thePositions", "Array of positions", DOC_TYPE_VECTOROFVECTOR3F);
    DOC_PARAM_OPT("theTexCoords", "Texture Coordinates", DOC_TYPE_VECTOROFVECTOR2F, "");
    DOC_PARAM_OPT("theColors", "Vertexcolors", DOC_TYPE_VECTOROFVECTOR4F, "");
    DOC_RVAL("The new created quadstrip shape", DOC_TYPE_NODE);
    DOC_END;
    return CreateStrip(PRIMITIVE_TYPE_QUAD_STRIP, cx, obj, argc, argv, rval);
}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateTriangleStrip(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) 
{
    DOC_BEGIN("Create a trianglestrip shape");
    DOC_PARAM("theScene", "The scene to create the strip in", DOC_TYPE_SCENE);
    DOC_PARAM("theMaterialId", "Shape material id.", DOC_TYPE_STRING);
    DOC_PARAM("thePositions", "Array of positions", DOC_TYPE_VECTOROFVECTOR3F);
    DOC_PARAM_OPT("theTexCoords", "Texture Coordinates", DOC_TYPE_VECTOROFVECTOR2F, "");
    DOC_PARAM_OPT("theColors", "Vertexcolors", DOC_TYPE_VECTOROFVECTOR4F, "");
    DOC_RVAL("The new created trianglestrip shape", DOC_TYPE_NODE);
    DOC_END;
    return CreateStrip(PRIMITIVE_TYPE_TRIANGLE_STRIP, cx, obj, argc, argv, rval);
}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateTriangleMeshMarkup(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Create a triangle mesh markup shape");
        DOC_PARAM("theScene", "The scene to create the shape inside", DOC_TYPE_SCENE);
        DOC_PARAM("theLineMaterialId", "The id of the material used for the lines", DOC_TYPE_STRING);
        DOC_PARAM("theAreaMaterialId", "The id of the material used for the areas", DOC_TYPE_STRING);
        DOC_PARAM("thePositions", "Array of positions", DOC_TYPE_VECTOROFVECTOR3F);
        DOC_PARAM_OPT("theName", "Name for the quadstack shape", DOC_TYPE_STRING, "");
        DOC_RVAL("The new created shape", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 4, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        string myLineMaterialId;
        convertFrom(cx, argv[1], myLineMaterialId);

        string myAreaMaterialId;
        convertFrom(cx, argv[2], myAreaMaterialId);

        std::vector<asl::Vector3f> myPositions;
        convertFrom(cx, argv[3], myPositions);

        string myName;
        if (argc > 4) {
            convertFrom(cx, argv[4], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 4:
                myResult = createTriangleMeshMarkup(myScene, myLineMaterialId, myAreaMaterialId,
                                             myPositions);
                break;
            case 5:
                myResult = createTriangleMeshMarkup(myScene, myLineMaterialId, myAreaMaterialId,
                                             myPositions, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateImage(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an image inside the scene");
    DOC_PARAM("theScene", "The scene to create the material inside", DOC_TYPE_SCENE);    
    DOC_PARAM("theImageSource", "Path to image file", DOC_TYPE_NODE);
    DOC_RESET;
    DOC_PARAM("theScene", "The scene to create the material inside", DOC_TYPE_SCENE);    
    DOC_PARAM("theWidth", "Image width", DOC_TYPE_INTEGER);
    DOC_PARAM("theHeight", "Image height", DOC_TYPE_INTEGER);
    DOC_PARAM("thePixelEncoding", "Pixel encoding", DOC_TYPE_STRING);
    DOC_RVAL("The new image", DOC_TYPE_NODE)
    DOC_END;
    try {

        ensureParamCount(argc, 2, 4);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        if (argc == 2) {
            std::string myImageSrc;
            if (!convertFrom(cx, argv[1], myImageSrc)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #2 must be a string (File path)");
                return JS_FALSE;
            }

            dom::NodePtr myResult = myScene->getImagesRoot()->appendChild(
                dom::NodePtr(new dom::Element("image")));
            myResult->appendAttribute(IMAGE_SRC_ATTRIB, myImageSrc);
            *rval = as_jsval(cx, myResult);
        } else if (argc ==4) {
            unsigned myWidth;
            if (!convertFrom(cx, argv[1], myWidth)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #2 must be a int (imagewidth)");
                return JS_FALSE;
            }
            unsigned myHeight;
            if (!convertFrom(cx, argv[2], myHeight)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #3 must be a int (imageheight)");
                return JS_FALSE;
            }
            std::string myPixelEncoding;
            if (!convertFrom(cx, argv[3], myPixelEncoding)) {
                JS_ReportError(cx, "JSScene::createImage(): argument #4 must be a string (pixelencoding)");
                return JS_FALSE;
            }
            myPixelEncoding = asl::toUpperCase(myPixelEncoding);
            dom::NodePtr myResult = myScene->getImagesRoot()->appendChild(
                dom::NodePtr(new dom::Element("image")));
            y60::ImagePtr myImage = myResult->getFacade<y60::Image>();
            myImage->createRaster(myWidth, myHeight, 1,
                PixelEncoding(getEnumFromString(myPixelEncoding, PixelEncodingString)));
            memset(myImage->getRasterPtr()->pixels().begin(), 0, myImage->getRasterPtr()->pixels().size());
            *rval = as_jsval(cx, myResult);

        } else {
            throw asl::Exception(string("Not enough arguments, must be 2 (scene,filename) or 4(scene,width,height,encoding)."));
        }
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateColorMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an untextured and unlit colored material.");
    DOC_PARAM("theScene", "The scene to create the material inside", DOC_TYPE_SCENE);    
    DOC_PARAM_OPT("theColor", "", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
    DOC_RVAL("theMaterialNode", DOC_TYPE_NODE)
    DOC_END;
    try {
        ensureParamCount(argc, 1, 2);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        dom::NodePtr myResult;
        if (argc == 1) {
            myResult = y60::createColorMaterial(myScene);
        } else {
            asl::Vector4f myColor;
            convertFrom(cx, argv[1], myColor);
            myResult = y60::createColorMaterial(myScene, myColor);
        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateLambertMaterial(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Creates an untextured lambert shaded materail.");
    DOC_PARAM("theScene", "The scene to create the material inside", DOC_TYPE_SCENE);
    DOC_PARAM_OPT("theDiffuseColor", "", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
    DOC_PARAM_OPT("theAmbientColor", "", DOC_TYPE_VECTOR4F, "[0,0,0,1]");
    DOC_RVAL("theMaterialNode", DOC_TYPE_NODE)
    DOC_END;
    try {
      ensureParamCount(argc, 1, 3);
      
      y60::ScenePtr myScene(0);
      convertFrom(cx, argv[0], myScene);

        dom::NodePtr myResult;
        if (argc == 1) {
            myResult = y60::createLambertMaterial(myScene);
        } else {
            asl::Vector4f myDiffuseColor;
            convertFrom(cx, argv[1], myDiffuseColor);
            if (argc == 2) {
                myResult = y60::createLambertMaterial(myScene, myDiffuseColor);
            } else {
                asl::Vector4f myAmbientColor;
                convertFrom(cx, argv[2], myAmbientColor);
                myResult = y60::createLambertMaterial(myScene, myDiffuseColor, myAmbientColor);
            }
        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}


JS_STATIC_DLL_CALLBACK(JSBool)
CreateUnlitTexturedMaterial(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Create an unlit textured material");
        DOC_PARAM("theScene", "The scene to create the material inside", DOC_TYPE_SCENE);
        DOC_PARAM_OPT("theTextureFilename", "Path of the file to use for as texture", DOC_TYPE_STRING, "");
        DOC_PARAM_OPT("theName", "Material name", DOC_TYPE_STRING, "ColorMaterial");
        DOC_PARAM_OPT("theTransparencyFlag", "Does the texture contain transparent pixels", DOC_TYPE_BOOLEAN, false);
        DOC_PARAM_OPT("theSpriteFlag", "Use the material as sprite (for particles)", DOC_TYPE_BOOLEAN, false);
        DOC_PARAM_OPT("theColor", "Surfacecolor of the material", DOC_TYPE_VECTOR4F, "[1,1,1,1]");
        DOC_PARAM_OPT("theDepth", "The Depth of the texture (for 3D-Textures)", DOC_TYPE_INTEGER, 1);
        DOC_RESET;
        DOC_PARAM_OPT("theImageNode", "Image node to use for as texture", DOC_TYPE_NODE, 0);
        DOC_PARAM_OPT("theName", "Material name", DOC_TYPE_STRING, "ColorMaterial");
        DOC_PARAM_OPT("theTransparencyFlag", "Does the texture contain transparent pixels", DOC_TYPE_BOOLEAN, false);
        DOC_PARAM_OPT("theSpriteFlag", "Use the material as sprite (for particles)", DOC_TYPE_BOOLEAN, false);
        DOC_PARAM_OPT("theColor", "Surfacecolor of the material", DOC_TYPE_VECTOR4F, "[1,1,1,1]");

        DOC_RVAL("The new created material", DOC_TYPE_NODE);
        DOC_END;

        ensureParamCount(argc, 1, 7);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        dom::NodePtr myResult;

        dom::NodePtr myImageNode(0);
        string myTextureFilename;
        if (argc > 1) {
          if (!convertFrom(cx, argv[1], myImageNode)) {
            convertFrom(cx, argv[1], myTextureFilename);
          }
        }
        string myName;
        if (argc > 2) {
            convertFrom(cx, argv[2], myName);
        }
        bool myTransparencyFlag;
        if (argc > 3) {
            convertFrom(cx, argv[3], myTransparencyFlag);
        }
        bool mySpriteFlag;
        if (argc > 4) {
            convertFrom(cx, argv[4], mySpriteFlag);
        }
        Vector4f myColor;
        if (argc > 5) {
            convertFrom(cx, argv[5], myColor);
        }
        unsigned myDepth;
        if (argc > 6) {
            convertFrom(cx, argv[6], myDepth);
            if (myImageNode) {
              AC_WARNING << "Ignoring depth value, because image node already has one.";
            }
        }

        switch (argc) {
        case 1:
          myResult = createUnlitTexturedMaterial(myScene,"");
          break;
        case 2:
          if (myImageNode) {
            myResult = createUnlitTexturedMaterial(myScene,myImageNode);
          } else {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename);
          }
          break;
        case 3:
          if (myImageNode) {
            myResult = createUnlitTexturedMaterial(myScene,myImageNode, myName);
          } else {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,myName);
          }
          break;
        case 4:
          if (myImageNode) {
            myResult = createUnlitTexturedMaterial(myScene,myImageNode,myName,myTransparencyFlag);
          } else {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                                                   myName,myTransparencyFlag);
          }
          break;
        case 5:
          if (myImageNode) {
          myResult = createUnlitTexturedMaterial(myScene,myImageNode,
                                                 myName,myTransparencyFlag,mySpriteFlag);
          } else {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                                                 myName,myTransparencyFlag,mySpriteFlag);
          }
          break;
        case 6:
          if (myImageNode) {
            myResult = createUnlitTexturedMaterial(myScene,myImageNode,
                                                   myName,myTransparencyFlag,mySpriteFlag, myColor);
          } else {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                                                 myName,myTransparencyFlag,mySpriteFlag, myDepth, myColor);
          }
          break;
        case 7:
          if (!myImageNode) {
            myResult = createUnlitTexturedMaterial(myScene,myTextureFilename,
                                                   myName,myTransparencyFlag,mySpriteFlag,myDepth, myColor );
          }
          break;
        }

        *rval = as_jsval(cx, myResult);
        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateQuadStack(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a stack of quads");
        DOC_PARAM("theScene", "The scene to create the quadstack inside", DOC_TYPE_SCENE);
        DOC_PARAM("theDimensions", "I am not quite sure, how this works. Try asking David or Thomas.", DOC_TYPE_VECTOR3I);
        DOC_PARAM("theSize", "The size of the quad shape", DOC_TYPE_FLOAT);
        DOC_PARAM("theMaterialId", "The id of the material used for the quad shapes", DOC_TYPE_STRING);
        DOC_PARAM_OPT("theName", "Name for the quadstack shape", DOC_TYPE_STRING, "");
        DOC_RVAL("The new created quadstack shape", DOC_TYPE_NODE);
        DOC_END;
        ensureParamCount(argc, 4, 5);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        asl::Vector3i myDimensions;
        convertFrom(cx, argv[1], myDimensions);

        float mySize;
        convertFrom(cx, argv[2], mySize);

        string myMaterialId;
        convertFrom(cx, argv[3], myMaterialId);

        string myName;
        if (argc > 4) {
            convertFrom(cx, argv[4], myName);
        }

        dom::NodePtr myResult;
        switch (argc) {
            case 4:
                myResult = createQuadStack(myScene, myDimensions, mySize,
                                             myMaterialId);
                break;
            case 5:
                myResult = createQuadStack(myScene, myDimensions, mySize,
                                             myMaterialId, myName);
                break;
        }
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}

JS_STATIC_DLL_CALLBACK(JSBool)
CreateVoxelProxyGeometry(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Creates a proxy geometry for a volume renderer");
        DOC_END;
        ensureParamCount(argc, 7);

        y60::ScenePtr myScene(0);
        convertFrom(cx, argv[0], myScene);

        asl::Box3f myVoxelBox;
        convertFrom(cx, argv[1], myVoxelBox);

        Matrix4f myModel;
        convertFrom(cx, argv[2], myModel);

        Matrix4f myCamera;
        convertFrom(cx, argv[3], myCamera);

        Vector3i myVolumeSize;
        convertFrom(cx, argv[4], myVolumeSize);

        float mySampleRate;
        convertFrom(cx, argv[5], mySampleRate);

        string myMaterialId;
        convertFrom(cx, argv[6], myMaterialId);

        string myName;
        convertFrom(cx, argv[7], myName);

        dom::NodePtr myResult;
        myResult = createVoxelProxyGeometry(myScene, myVoxelBox, myModel, myCamera,
                                            myVolumeSize, mySampleRate, myMaterialId, myName);
        *rval = as_jsval(cx, myResult);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;

}

JS_STATIC_DLL_CALLBACK(JSBool)
SetAlpha(JSContext * cx, JSObject * obj, uintN argc, jsval *argv, jsval *rval) {
    try {
        DOC_BEGIN("Set body/shape alpha.");
        DOC_PARAM("theNode", "The node to operate on, can be a body or a shape.", DOC_TYPE_NODE);
        DOC_PARAM("theAlpha", "Alpha value to set.", DOC_TYPE_FLOAT);
        DOC_END;

        ensureParamCount(argc, 2);

        dom::NodePtr myNode;
        float myAlpha;

        convertFrom(cx, argv[0], myNode);
        convertFrom(cx, argv[1], myAlpha);

        y60::setAlpha(&*myNode, myAlpha);

        return JS_TRUE;

    } HANDLE_CPP_EXCEPTION;
}



static JSBool
toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Returns string representation fo the modelling functions");
    DOC_RVAL("theString", DOC_TYPE_STRING);
    DOC_END;
    std::string myStringRep = std::string("ModellingFunctions@") + as_string(obj);
*rval = as_jsval(cx, myStringRep);
    return JS_TRUE;
}

JSFunctionSpec *
JSModellingFunctions::Functions() {
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"toString",             toString,                0},
        {0}
    };
    return myFunctions;
}

JSFunctionSpec *
JSModellingFunctions::StaticFunctions() {
    static JSFunctionSpec myFunctions[] = {
        // name                         native                       nargs
        {"createTransform",             CreateTransform,             1},
        {"createSurface2DFromContour",  CreateSurface2DFromContour,  4},
        {"createBody",                  CreateBody,                  2},
        {"createCanvas",                CreateCanvas,                2},
        {"createQuad",                  CreateQuad,                  2},
        {"createCrosshair",             CreateCrosshair,             5},
        {"createDistanceMarkup",        CreateDistanceMarkup,        5},
        {"createAngleMarkup",           CreateAngleMarkup,           6},
        {"createTriangleMeshMarkup",    CreateTriangleMeshMarkup,    5},
        {"createLineStrip",             CreateLineStrip,             4},
        {"createQuadStrip",             CreateQuadStrip,             4},
        {"createTriangleStrip",         CreateTriangleStrip,         4},
        {"createQuadStack",             CreateQuadStack,             5},
        {"createImage",                 CreateImage,                 4},
        {"createLambertMaterial",       CreateLambertMaterial,       3},
        {"createColorMaterial",         CreateColorMaterial,         3},
        {"createUnlitTexturedMaterial", CreateUnlitTexturedMaterial, 7},
        {"createVoxelProxyGeometry",    CreateVoxelProxyGeometry,    7},
        {"setAlpha",                    SetAlpha,                    2},
        {0}
    };
    return myFunctions;
}

JSPropertySpec *
JSModellingFunctions::Properties() {
    static JSPropertySpec myProperties[] = {
        {0}
    };
    return myProperties;
}

JSConstIntPropertySpec *
JSModellingFunctions::ConstIntProperties() {
    static JSConstIntPropertySpec myProperties[] = {{0}};
    return myProperties;
}

JSPropertySpec *
JSModellingFunctions::StaticProperties() {
    static JSPropertySpec myProperties[] = {{0}};
    return myProperties;
}

// getproperty handling
JSBool
JSModellingFunctions::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_FALSE;
}

// setproperty handling
JSBool
JSModellingFunctions::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
    return JS_FALSE;
}

JSBool
JSModellingFunctions::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("This object is static. You do not need to construct it.");
    DOC_END;
    AC_TRACE << "Constructor start " << endl;
    AC_TRACE << "Constructor argc =" << argc << endl;
    if (JSA_GetClass(cx,obj) != Base::Class()) {
        JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
        return JS_FALSE;
    }

    JSModellingFunctions *myNewObject = 0;
    OWNERPTR myNewNative = OWNERPTR(new bool);
    myNewObject = new JSModellingFunctions(myNewNative, &(*myNewNative));

     if (myNewObject) {
        JS_SetPrivate(cx,obj,myNewObject);
        return JS_TRUE;
    }
    JS_ReportError(cx,"JSAudio::Constructor: bad parameters");
    return JS_FALSE;
}

void
JSModellingFunctions::addClassProperties(JSContext * cx, JSObject * theClassProto) {
    JSA_AddFunctions(cx, theClassProto, Functions());
}

JSObject *
JSModellingFunctions::initClass(JSContext *cx, JSObject *theGlobalObject) {
    JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor,
        Properties(), Functions(), 0, 0, StaticFunctions());
    DOC_CREATE(JSModellingFunctions);
    return myClass;
}

bool convertFrom(JSContext *cx, jsval theValue, DummyT *& theDummy) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSModellingFunctions::NATIVE >::Class()) {
                theDummy = &(*JSClassTraits<JSModellingFunctions::NATIVE>::getNativeOwner(cx,myArgument));
                return true;
            }
        }
    }
    return false;
}


bool convertFrom(JSContext *cx, jsval theValue, asl::Ptr<DummyT> & theDummy) {
    if (JSVAL_IS_OBJECT(theValue)) {
        JSObject * myArgument;
        if (JS_ValueToObject(cx, theValue, &myArgument)) {
            if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSModellingFunctions::NATIVE >::Class()) {
                theDummy = JSClassTraits<JSModellingFunctions::NATIVE>::getNativeOwner(cx,myArgument);
                return true;
            }
        }
    }
    return false;
}

jsval as_jsval(JSContext *cx, JSModellingFunctions::OWNERPTR & theOwner) {
    JSObject * myReturnObject = JSModellingFunctions::Construct(cx, theOwner, &(*theOwner));
    return OBJECT_TO_JSVAL(myReturnObject);
}

}
