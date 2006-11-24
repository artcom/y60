//=============================================================================
// Copyright (C) 2003-2006 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifdef WIN32
#define NOMINMAX
// Note: GLH_EXT_SINGLE_FILE must be defined only in one object file
// it makes the header file to define the function pointer variables
// never set it in a .h file or any other file that shall be linked
// with this object file
// If you use GLH_EXT_SINGLE_FILE 1 make sure that glh_extensions.h
// and glh_genext.h has not been included from another include file
// already without GLH_EXT_SINGLE_FILE set
//
#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>
#else
//
#endif

#include "ShaderLibrary.h"

#include <dom/Nodes.h>

#include <y60/NodeNames.h>
#include <y60/PropertyNames.h>
#include <y60/DataTypes.h>
#include <y60/Body.h>
#include <y60/Viewport.h>
#include <y60/Camera.h>

#include <y60/property_functions.h>
#include <y60/glExtensions.h>
#include <y60/GLUtils.h>

//#include <asl/file_functions.h>
#include <asl/string_functions.h>
#include <asl/Logger.h>


using namespace std;
using namespace asl;
using namespace dom;

#define DB(x) // x
// profiling

#ifdef PROFILING_LEVEL_NORMAL
#define DBP(x)  x
#else
#define DBP(x) // x
#endif

// more profiling
#ifdef PROFILING_LEVEL_FULL
#define DBP2(x)  x
#else
#define DBP2(x) // x
#endif

namespace y60 {

    static const GLenum ourTexGenCoord[] = {
        GL_S, GL_T, GL_R, GL_Q
    };
    static const GLenum ourTexGenToken[] = {
        GL_TEXTURE_GEN_S, GL_TEXTURE_GEN_T, GL_TEXTURE_GEN_R, GL_TEXTURE_GEN_Q
    };

    GLShader::GLShader(const dom::NodePtr theNode) :
        _myType(FIXED_FUNCTION_MATERIAL)
    {
        _myId              = theNode->getAttributeString(ID_ATTRIB);
        _myName            = theNode->getAttributeString(NAME_ATTRIB);
        _myCost            = theNode->getAttributeValue<float>(COST_ATTRIB);
        _myDefaultTextures = theNode->childNode("textures");
        loadFeatureSet(theNode->childNode("supports"));
    }

    GLShader::~GLShader()  {
        unload();
    }

    const string &
    GLShader::getId() const {
        return _myId;
    }

    const string &
    GLShader::getName() const {
        return _myName;
    }

    const MaterialType &
    GLShader::getType() const {
        return _myType;
    }

    GLShader::ShaderMatch
    GLShader::matches(const string & theFeatureClass,
                      const VectorOfString & theRequirementList) const
    {
        const VectorOfString * myFeaturesLeft = _myFeatureSet.getFeatures(theFeatureClass);
        if (myFeaturesLeft) {
            if ((*myFeaturesLeft == theRequirementList)) {
                return FULL_MATCH;
            }
            // ignore requirement => WILDCARD_MATCH
            //if (theRequirementList.size() == 0) {
            //    return WILDCARD_MATCH;
            //}
            if (myFeaturesLeft->size() != theRequirementList.size() ) {
                return NO_MATCH;
            }
            // check if we have any wildcards to ignore
            for (int myIndex = 0; myIndex < myFeaturesLeft->size(); myIndex++) {
                if ((*myFeaturesLeft)[myIndex] != theRequirementList[myIndex] &&
                    (*myFeaturesLeft)[myIndex] != "*" ) {
                        return NO_MATCH;
                    }
            }
            return WILDCARD_MATCH;
        } else {
            return NO_MATCH;
        }
    }

    void
    GLShader::loadFeatureSet(const dom::NodePtr theShaderNode) {
        _myFeatureSet.load(theShaderNode);
    }

    unsigned
    GLShader::getPropertyNodeCount() const {
        return _myPropertyNodes.size();
    }

    const dom::NodePtr
    GLShader::getDefaultTextures() {
        return _myDefaultTextures;
    }

    const dom::NodePtr
    GLShader::getPropertyNode(unsigned theIndex) {
        return _myPropertyNodes[theIndex];
    }

    void GLShader::load(IShaderLibrary & theShaderLibrary) {
    }

    void GLShader::unload() {
    }

    void
    GLShader::activateGroup1(MaterialBase & theMaterial, const Viewport & theViewport) {
        MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial.getChild<MaterialPropertiesTag>();

        // color,depth mask
        const TargetBuffers & myMasks = myMaterialPropFacade->get<TargetBuffersTag>();
        // glDepthMask(myMasks[y60::DEPTH_MASK]); Depth-writes is now part of RenderState, so set it in Renderer
        glColorMask(myMasks[y60::RED_MASK], myMasks[y60::GREEN_MASK],
                myMasks[y60::BLUE_MASK], myMasks[y60::ALPHA_MASK]);

        // line smooth, line width
        if (myMaterialPropFacade->get<LineSmoothTag>()) {
            glEnable(GL_LINE_SMOOTH);
        } else {
            glDisable(GL_LINE_SMOOTH);
        }

        float myLineWidth = myMaterialPropFacade->get<LineWidthTag>();
        glLineWidth(myLineWidth);

        const asl::Vector3f & myPointSizeParams = myMaterialPropFacade->get<PointSizeTag>();
        glPointSize(myPointSizeParams[0]);
        glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, myPointSizeParams[1]);
        glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, myPointSizeParams[2]);

        const VectorOfBlendFunction & myBlendFunction = myMaterialPropFacade->get<BlendFunctionTag>();
        if (myBlendFunction.size() == 2) {
#if 0
            // we do NOT want this version since then a non-glowing material (myGlow=0)
            // does not overwrite a glowing material behind it.
            float myGlow = myMaterialPropFacade->get<GlowTag>();
            if (myGlow > 0.0f && theViewport.get<ViewportDrawGlowTag>()) {
                glBlendFuncSeparate(asGLBlendFunction(mySrcFunc), asGLBlendFunction(myDstFunc), 
                                    GL_CONSTANT_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glBlendColor(1.0f,1.0f,1.0f, myGlow);
            } else {
                glBlendFunc(asGLBlendFunction(mySrcFunc), asGLBlendFunction(myDstFunc)); 
            }
#else
            if (theViewport.get<ViewportDrawGlowTag>()) {
                glBlendFuncSeparate(asGLBlendFunction(myBlendFunction[0]), asGLBlendFunction(myBlendFunction[1]), 
                                    GL_CONSTANT_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                float myGlow = myMaterialPropFacade->get<GlowTag>();
                glBlendColor(1.0f,1.0f,1.0f, myGlow);
            } else {
                glBlendFunc(asGLBlendFunction(myBlendFunction[0]), asGLBlendFunction(myBlendFunction[1])); 
            }
#endif
        } else {
            throw ShaderException(string("Blendfunction for material '") + theMaterial.get<NameTag>() + " has "
                    + asl::as_string(myBlendFunction.size()) + " elements. Expected two.", PLUS_FILE_LINE);
        }

        if (glBlendEquation != Missing_glBlendEquation) {
            const BlendEquation & myBlendEquation = myMaterialPropFacade->get<BlendEquationTag>();
            GLenum myEquation = asGLBlendEquation(myBlendEquation);
            glBlendEquation(myEquation);
        }

    }

    void
    GLShader::activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {

        //AC_DEBUG << "GLShader::activate " << theMaterial.get<NameTag>();

        theMaterial.updateParams();

        MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial.getChild<MaterialPropertiesTag>();


        // line stipple
        dom::NodePtr myLineStippleProp = myMaterialPropFacade->getProperty(LINESTIPPLE_PROPERTY);
        if (myLineStippleProp) {
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, myLineStippleProp->nodeValueAs<unsigned int>());
        }


        dom::NodePtr myPointAttenuationProp = myMaterialPropFacade->getProperty(POINTATTENUATION_PROPERTY);
        if (myPointAttenuationProp) {
            glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB,
                                  myPointAttenuationProp->nodeValueAs<asl::Vector3f>().begin());
        }


    }

    void
    GLShader::enableTextures(const y60::MaterialBase & theMaterial) {

        unsigned myTextureCount = theMaterial.getTextureCount();
        // AC_TRACE << "GLShader::enableTextures " << theMaterial.get<NameTag>() << " count=" << myTextureCount;

        glMatrixMode(GL_TEXTURE);
        bool alreadyHasSpriteTexture = false;
        for (unsigned i = 0; i < myTextureCount; ++i) {

            const y60::Texture & myTexture = theMaterial.getTexture(i);
            bool hasMipMaps = myTexture.getImage()->get<ImageMipmapTag>();

            GLenum myTextureType = 0;
            switch (myTexture.getImage()->getType()) {
            case SINGLE:
                myTextureType = myTexture.getImage()->get<ImageDepthTag>() > 1 ? GL_TEXTURE_3D : GL_TEXTURE_2D;
                break;
            case CUBEMAP:
                myTextureType = GL_TEXTURE_CUBE_MAP_ARB;
                break;
            default:
                throw ShaderException(string("Invalid image type in material '") + theMaterial.get<NameTag>() + "'", PLUS_FILE_LINE);
                break;
            }
            GLenum myTexUnit = asGLTextureRegister(i);
            AC_DEBUG << "GLShader::enableTextures material=" << theMaterial.get<NameTag>() << " unit=" << hex << myTexUnit << dec << " texid=" << myTexture.getId() << " wrap=" << myTexture.getWrapMode();

            glActiveTexture(myTexUnit);
            glBindTexture(myTextureType, myTexture.getId());

            // load texture matrix
            asl::Matrix4f myMatrix = myTexture.getImage()->get<ImageMatrixTag>();
            myMatrix.postMultiply(myTexture.get<TextureMatrixTag>());
            //AC_TRACE << "TM " << myTexture.get<TextureMatrixTag>();
            //AC_TRACE << "MM " << myMatrix;
            glLoadMatrixf(static_cast<const GLfloat *>(myMatrix.getData()));

            // [DS] this isn't necessary. Filtering and wrapmode are stored
            // as part of the texture object. Instead we should set it once
            // at creation time to make the texture usable even if it is not
            // rendered at all. Some kind of dirty notification would be necessary.
            glTexParameteri(myTextureType, GL_TEXTURE_WRAP_S,
                    asGLTextureWrapmode(myTexture.getWrapMode()));
            CHECK_OGL_ERROR;
            glTexParameteri(myTextureType, GL_TEXTURE_WRAP_T,
                    asGLTextureWrapmode(myTexture.getWrapMode()));
            CHECK_OGL_ERROR;
            if (myTextureType == GL_TEXTURE_3D) {
                glTexParameteri(myTextureType, GL_TEXTURE_WRAP_R,
                        asGLTextureWrapmode(myTexture.getWrapMode()));
                CHECK_OGL_ERROR;
            }

            glTexParameteri(myTextureType, GL_TEXTURE_MAG_FILTER,
                    asGLTextureSampleFilter(myTexture.getMagFilter(), false));
            CHECK_OGL_ERROR;
            // only minification can use mipmaps
            glTexParameteri(myTextureType, GL_TEXTURE_MIN_FILTER,
                    asGLTextureSampleFilter(myTexture.getMinFilter(), hasMipMaps));
            CHECK_OGL_ERROR;

            if (myTexture.get<TextureSpriteTag>()) {
                if (!alreadyHasSpriteTexture) {
                    glEnable(GL_POINT_SPRITE_ARB);
                    CHECK_OGL_ERROR;
                    if (glPointParameterfARB) {
                        glPointParameterfARB(GL_POINT_SPRITE_R_MODE_NV, GL_S);
                        CHECK_OGL_ERROR;
                    }
                } else {
                    glDisable(GL_POINT_SPRITE_ARB);
                }
                glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
                CHECK_OGL_ERROR;
                alreadyHasSpriteTexture = true;
            } else {
                glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE );
                CHECK_OGL_ERROR;
            }
        }
        glMatrixMode(GL_MODELVIEW);
    }

    void
    GLShader::disableTextures(const y60::MaterialBase & theMaterial) {
        // nothing to do
    }

    void
    GLShader::deactivate(const MaterialBase & theMaterial) {
        //AC_DEBUG << "GLShader::deactivate " << theMaterial.get<NameTag>();
        if (theMaterial.hasTexGen()) {
            // disable texture coordinate generation
            for (unsigned myTexUnit = 0; myTexUnit < theMaterial.getTexGenModes().size(); ++myTexUnit) {
                glActiveTexture(asGLTextureRegister(myTexUnit));
                glDisable(GL_TEXTURE_GEN_S);
                glDisable(GL_TEXTURE_GEN_T);
                glDisable(GL_TEXTURE_GEN_R);
                glDisable(GL_TEXTURE_GEN_Q);
            }
        }
    }

    void
    GLShader::loadShaderProperties(const dom::NodePtr theShaderNode, ShaderDescription & theShader) {
        dom::NodePtr myPropertyListNode = theShaderNode->childNode(PROPERTY_LIST_NAME);
        if (myPropertyListNode) {
            _myPropertyNodes.push_back(myPropertyListNode);
        }
    }

    void
    GLShader::loadParameters(const dom::NodePtr theParameterListNode, ShaderDescription & theShader) {
        theShader._myVertexParameters.clear();

        dom::NodePtr myParameterListNode = theParameterListNode->childNode(PARAMETER_LIST_NAME);
        int myParameterCount = myParameterListNode->childNodesLength();
        for (unsigned i = 0; i < myParameterCount; ++i) {
            dom::NodePtr myParameterNode = myParameterListNode->childNode(i);
            if (myParameterNode->nodeType() != dom::Node::COMMENT_NODE) {
                const string & myPropertyName = myParameterNode->getAttributeString("name");
                const string & myPropertyTypeString = myParameterNode->nodeName();
                TypeId myPropertyType = TypeId(asl::getEnumFromString(myPropertyTypeString, TypeIdStrings));

                VertexDataRole myPropertyRole =
                    VertexDataRole(asl::getEnumFromString(myPropertyName, VertexDataRoleString));

                string  myParameterFunctionString = VERTEXPARAM_PASS_THROUGH;
                if (myParameterNode->getAttribute(FUNCTION_ATTRIB) ) {
                    myParameterFunctionString = myParameterNode->getAttributeString(FUNCTION_ATTRIB);
                }

                ParameterFunction myParameterFunction = ParameterFunction(
                        asl::getEnumFromString(myParameterFunctionString, ParameterFunctionStrings));

                GLRegister myRegister;
                if (myParameterNode->getAttribute(REGISTER_ATTRIB)) {
                    const string & myRegisterString = myParameterNode->getAttributeString("register");
                    myRegister = GLRegister(asl::getEnumFromString(myRegisterString, GLRegisterString));
                } else {
                    myRegister = GLRegister(asl::getEnumFromString(myPropertyName, GLRegisterString));
                }

                string myDefaultValue = "";
                dom::NodePtr myTextNode = myParameterNode->childNode("#text");
                if (myTextNode) {
                    myDefaultValue = myTextNode->nodeValue();
                }
                theShader._myVertexParameters.push_back(MaterialParameter(myPropertyName, myPropertyRole, myRegister,
                                                                           myPropertyType, myParameterFunction,
                                                                           myDefaultValue));
                theShader._myVertexRegisterFlags.set(myRegister);
            }
        }
    }

    void
    GLShader::bindBodyParams(const MaterialBase & theMaterial,
            const Viewport & theViewport,
            const LightVector & theLights,
            const Body & theBody,
            const Camera & theCamera)
    {
        //AC_DEBUG << "GLShader.bindBodyParams " << theMaterial.get<NameTag>();
        DBP2(MAKE_SCOPE_TIMER(GLShader_bindBodyParams));
        if (theMaterial.hasTexGen()) {

            bool mustRestoreMatrix = false;
            MaterialBase::TexGenModeList myTexGenModes = theMaterial.getTexGenModes();
            MaterialBase::TexGenParamsList myTexGenParams = theMaterial.getTexGenParams();

            // enable texture coordinate generation
            for (unsigned myTexUnit = 0; myTexUnit < myTexGenModes.size(); ++myTexUnit) {

                glActiveTexture(asGLTextureRegister(myTexUnit));

                MaterialBase::TexGenMode myModes = myTexGenModes[myTexUnit];
                MaterialBase::TexGenParams myParams = myTexGenParams[myTexUnit];

                for (unsigned i = 0; i < myModes.size(); ++i) {

                    //AC_DEBUG << "unit=" << myTexUnit << " mode=" << myModes[i] << " params=" << myParams[i];

                    if (myModes[i] == NONE) {
                        continue;
                    }

                    // set texgen plane params
                    if (myModes[i] == EYE_LINEAR) {

                        // push modelview matrix
                        if (mustRestoreMatrix == false) {
                            glPushMatrix();
#if 1
                            glLoadIdentity();
#else
                            // UH: should we load the camera global matrix instead of identity?
                            glLoadMatrixf(static_cast<const GLfloat *>(theCamera.get<GlobalMatrixTag>().getData()));
#endif
                            mustRestoreMatrix = true;
                        }

                        glTexGenfv(ourTexGenCoord[i], GL_EYE_PLANE, &(*(myParams[i]).begin()));
                    } else if (myModes[i] == OBJECT_LINEAR) {
                        glTexGenfv(ourTexGenCoord[i], GL_OBJECT_PLANE, &(*(myParams[i]).begin()));
                    }

                    // set texgen mode
                    glTexGeni(ourTexGenCoord[i], GL_TEXTURE_GEN_MODE, asGLTexCoordMode(myModes[i]));
                    glEnable(ourTexGenToken[i]);
                }
                CHECK_OGL_ERROR;
            }

            if (mustRestoreMatrix) {
                glPopMatrix();
            }
        }
    }

    const ShaderFeatureSet &
    GLShader::getFeatureSet() const {
        return _myFeatureSet;
    }

    const VectorOfString *
    GLShader::getFeatures(const string & theFeatureClass) const {
        return _myFeatureSet.getFeatures(theFeatureClass);
    }

    bool GLShader::hasFeature(const string & theFeatureClass) const {
        return _myFeatureSet.hasFeature(theFeatureClass);
    }

    float
    GLShader::getCosts() const {
        return _myCost;
    }

    unsigned
    GLShader::getMaxTextureUnits() const {
        int myMaxTexUnits;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &myMaxTexUnits);
        if (myMaxTexUnits < 0) { // should not happen
            myMaxTexUnits = 0;
        }
        return unsigned(myMaxTexUnits);
    }
} // namespace y60

