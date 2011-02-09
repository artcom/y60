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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

//own header
#include "GLShader.h"


#include "ShaderLibrary.h"

#include <asl/dom/Nodes.h>

#include <y60/base/NodeNames.h>
#include <y60/base/PropertyNames.h>
#include <y60/base/DataTypes.h>
#include <y60/scene/Body.h>
#include <y60/scene/Viewport.h>
#include <y60/scene/Camera.h>

#include <y60/base/property_functions.h>
#include <y60/glutil/glExtensions.h>
#include <y60/glutil/GLUtils.h>

#include <asl/base/string_functions.h>
#include <asl/base/Logger.h>


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

    GLShader::GLShader(const dom::NodePtr theNode)
    :   _myType(FIXED_FUNCTION_MATERIAL),
        _myHasImagingEXT(hasCap("GL_ARB_imaging")),
        _myHasPointParmatersEXT(hasCap("GL_ARB_point_parameters")),
        _myHasBlendMinMaxEXT(hasCap("GL_EXT_blend_minmax"))
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
            for (VectorOfString::size_type myIndex = 0; myIndex < myFeaturesLeft->size(); myIndex++) {
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
        CHECK_OGL_ERROR;

        // line smooth
        if (myMaterialPropFacade->get<LineSmoothTag>()) {
            glEnable(GL_LINE_SMOOTH);
            CHECK_OGL_ERROR;
        } else {
            glDisable(GL_LINE_SMOOTH);
            CHECK_OGL_ERROR;
        }

        // line width
        float myLineWidth = myMaterialPropFacade->get<LineWidthTag>();
        glLineWidth(myLineWidth);
        CHECK_OGL_ERROR;

        // point size
        const asl::Vector3f & myPointSizeParams = myMaterialPropFacade->get<PointSizeTag>();
        glPointSize(myPointSizeParams[0]);
        CHECK_OGL_ERROR;
        if (_myHasPointParmatersEXT) {
            glPointParameterfARB(GL_POINT_SIZE_MIN_ARB, myPointSizeParams[1]);
            CHECK_OGL_ERROR;
            glPointParameterfARB(GL_POINT_SIZE_MAX_ARB, myPointSizeParams[2]);
            CHECK_OGL_ERROR;
        }

        // glow, blend function and blend color
        //
        // If (deprecated) glow is enabled, the alpha blend functions are overridden
        // and the blend color is ignored. Because of this, separate blend functions
        // specified on the material are not supported in this mode.
        //
        bool myGlobalGlowFlag = theViewport.get<ViewportDrawGlowTag>();
        float myGlow = myMaterialPropFacade->get<GlowTag>();
        const Vector4f & myBlendColor = myMaterialPropFacade->get<BlendColorTag>();
        const VectorOfBlendFunction & myBlendFunction = myMaterialPropFacade->get<BlendFunctionTag>();
        if(myGlobalGlowFlag) {
            // blend function for glow
            if(myBlendFunction.size() == 2) {
                glBlendFuncSeparate(asGLBlendFunction(myBlendFunction[0]),
                                    asGLBlendFunction(myBlendFunction[1]),
                                    GL_CONSTANT_ALPHA,
                                    GL_ONE_MINUS_SRC_ALPHA);
                CHECK_OGL_ERROR;
            } else if(myBlendFunction.size() == 4) {
                throw ShaderException(string("Blend function for material '")
                                      + theMaterial.get<NameTag>()
                                      + "' specifies separate blend functions"
                                      + ", which is unsupported in combination with glow",
                                      PLUS_FILE_LINE);
            } else {
                throw ShaderException(string("Blend function for material '")
                                      + theMaterial.get<NameTag>()
                                      + "' has invalid length "
                                      + asl::as_string(myBlendFunction.size())
                                      + ", expected 2 or 4",
                                      PLUS_FILE_LINE);
            }

            // blend color for glow
            if(myBlendColor == Vector4f(0,0,0,0)) {
                float myRealGlow = myGlow + 0.01f; // XXX nudge value to work around an alpha test bug
                glBlendColor(1.0,1.0,1.0, myRealGlow);
                CHECK_OGL_ERROR;
            } else {
                throw ShaderException(string("Blend color on material '")
                                      + theMaterial.get<NameTag>()
                                      + "' has been set"
                                      + ", which is unsupported in combination with glow",
                                      PLUS_FILE_LINE);
            }
        } else {
            // blend function
            if(myBlendFunction.size() == 2) {
                glBlendFunc(asGLBlendFunction(myBlendFunction[0]),
                            asGLBlendFunction(myBlendFunction[1]));
            } else if(myBlendFunction.size() == 4) {
                glBlendFuncSeparate(asGLBlendFunction(myBlendFunction[0]),
                                    asGLBlendFunction(myBlendFunction[1]),
                                    asGLBlendFunction(myBlendFunction[2]),
                                    asGLBlendFunction(myBlendFunction[3]));
            } else {
                throw ShaderException(string("Blend function for material '")
                                      + theMaterial.get<NameTag>()
                                      + "' has invalid length "
                                      + asl::as_string(myBlendFunction.size())
                                      + ", expected 2 or 4",
                                      PLUS_FILE_LINE);
            }
            CHECK_OGL_ERROR;

            // blend color
            glBlendColor(myBlendColor[0], myBlendColor[1], myBlendColor[2], myBlendColor[3]);
            CHECK_OGL_ERROR;
        }

        // blend equation
        if (_myHasImagingEXT && _myHasBlendMinMaxEXT) {
            const BlendEquation & myBlendEquation = myMaterialPropFacade->get<BlendEquationTag>();
            GLenum myEquation = asGLBlendEquation(myBlendEquation);
            glBlendEquation(myEquation);
            CHECK_OGL_ERROR;
        }
    }

    void
    GLShader::activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {
        MAKE_GL_SCOPE_TIMER(GLShader_activate);
        AC_TRACE << "GLShader::activate " << theMaterial.get<NameTag>();
        AC_TRACE << "GLShader::activate " << theMaterial.getNode();

        //theMaterial.updateParams(); // This call is still necessary, but should not be! (removal breaks e.g. glowcube.js test)

        MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial.getChild<MaterialPropertiesTag>();

        // depth test
        dom::NodePtr myDepthTestProp = myMaterialPropFacade->getProperty(DEPTHTEST_PROPERTY);
        if (myDepthTestProp) {
            bool myDepthTest = myDepthTestProp->nodeValueAs<bool>();
            if (myDepthTest) {
                glEnable(GL_DEPTH_TEST);
            } else {
                glDisable(GL_DEPTH_TEST);
            }
        }
        CHECK_OGL_ERROR;

        // line stipple
        dom::NodePtr myLineStippleProp = myMaterialPropFacade->getProperty(LINESTIPPLE_PROPERTY);
        if (myLineStippleProp) {
            glEnable(GL_LINE_STIPPLE);
            glLineStipple(1, myLineStippleProp->nodeValueAs<GLushort>());
        }
        CHECK_OGL_ERROR;

        // point attenuation
        dom::NodePtr myPointAttenuationProp = myMaterialPropFacade->getProperty(POINTATTENUATION_PROPERTY);
        if (_myHasPointParmatersEXT && myPointAttenuationProp) {
            glPointParameterfvARB(GL_POINT_DISTANCE_ATTENUATION_ARB,
                                  myPointAttenuationProp->nodeValueAs<asl::Vector3f>().begin());
        }
        AC_TRACE << "GLShader::activate " << theMaterial.get<NameTag>() << " done.";
    }

    void
    GLShader::enableTextures(const y60::MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(GLShader_enableTextures);

        unsigned myTextureCount = theMaterial.getTextureUnitCount();
        AC_TRACE << "GLShader::enableTextures '" << theMaterial.get<NameTag>() << "' id=" << theMaterial.get<IdTag>() << " texunits=" << myTextureCount;

        asl::Unsigned64 myFrameNumber = theMaterial.get<LastActiveFrameTag>();

        glMatrixMode(GL_TEXTURE);
        bool alreadyHasSpriteTexture = false;
        for (unsigned i = 0; i < myTextureCount; ++i) {

            // [VS;DS;UH;SH] someone please explain this
            // from disableTextures: Fixed Function Shaders only support paint & skybox usage
            TextureUsage myTextureUsage = theMaterial.getTextureUsage(i);
            /*
            if (myTextureUsage != PAINT && myTextureUsage != SKYBOX) {
                continue;
            }
            */

            const y60::TextureUnit & myTextureUnit = theMaterial.getTextureUnit(i);
            y60::TexturePtr myTexture = myTextureUnit.getTexture();
            myTexture->set<LastActiveFrameTag>(myFrameNumber);

            GLenum myTexUnit = asGLTextureRegister(i);
            glActiveTexture(myTexUnit);
            glClientActiveTexture(myTexUnit);

            // triggers texture setup/update
            unsigned myTextureId = myTexture->applyTexture(); //ensureTextureId();
            GLenum myTextureTarget = asGLTextureTarget(myTexture->getType());

            // bind/enable texture
            glBindTexture(myTextureTarget, myTextureId);
            glEnable(myTextureTarget);
            AC_TRACE << "GLShader::enableTextures unit=" << i << " texId=" << myTextureId << " target=0x" << hex << myTextureTarget << dec;

            // texture env apply mode
            GLenum myTexEnvMode = asGLTextureApplyMode(myTextureUnit.get<TextureUnitApplyModeTag>());
            glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, myTexEnvMode);

            // texture env const blend color (changed from black OpenGL default)
            glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, myTextureUnit.get<TextureUnitEnvColorTag>().begin());

            // load texture matrix
            asl::Matrix4f myMatrix;
            const y60::ImagePtr & myImage = myTexture->getImage();
            if (myImage) {
                const_cast<y60::Image&>(*myImage).set<LastActiveFrameTag>(myFrameNumber);
                myMatrix = myImage->get<ImageMatrixTag>();
                myMatrix.postMultiply(myTexture->get<TextureNPOTMatrixTag>());
            } else {
                myMatrix  = myTexture->get<TextureNPOTMatrixTag>();
            }
            myMatrix.postMultiply(myTexture->get<TextureMatrixTag>());
            myMatrix.postMultiply(myTextureUnit.get<TextureUnitMatrixTag>());
            glLoadMatrixf(static_cast<const GLfloat *>(myMatrix.getData()));

            // setup texture sprite
            if (myTextureUnit.get<TextureUnitSpriteTag>()) {
                if (!alreadyHasSpriteTexture) {
                    glEnable(GL_POINT_SPRITE_ARB);
                    CHECK_OGL_ERROR;
                    if (_myHasPointParmatersEXT) {
                        glPointParameterfARB(GL_POINT_SPRITE_R_MODE_NV, GL_S);
                        CHECK_OGL_ERROR;
                    }
                    alreadyHasSpriteTexture = true;
                }
                glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE );
                CHECK_OGL_ERROR;
            } else {
                if (alreadyHasSpriteTexture) {
                    glDisable(GL_POINT_SPRITE_ARB);
                    alreadyHasSpriteTexture = false;
                }
                glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE );
                CHECK_OGL_ERROR;
            }
        }
        glMatrixMode(GL_MODELVIEW);
    }

    void
    GLShader::disableTextures(const y60::MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(GLShader_disableTextures);

        unsigned myTextureCount = theMaterial.getTextureUnitCount();
        // AC_TRACE << "current texcount:" << myTextureCount << ", prev:" << myPreviousTextureCount << endl;
        for (unsigned i = 0; i < myTextureCount; ++i) {

            // Fixed Function Shaders only support paint & skybox usage
            TextureUsage myTextureUsage = theMaterial.getTextureUsage(i);
            /*
            if (myTextureUsage != PAINT && myTextureUsage != SKYBOX) {
                continue;
            }
            */

            const y60::TextureUnit & myTextureUnit = theMaterial.getTextureUnit(i);
            y60::TexturePtr myTexture = myTextureUnit.getTexture();

            GLenum myTexUnit = asGLTextureRegister(i);
            glActiveTexture(myTexUnit);
            glClientActiveTexture(myTexUnit);

            // unbind/disable texture
            GLenum myTextureTarget = asGLTextureTarget(myTexture->getType());
            glBindTexture(myTextureTarget, 0);
            glDisable(myTextureTarget);
        }
        glDisable(GL_POINT_SPRITE_ARB);
    }

    void
    GLShader::deactivate(const MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(GLShader_deactivate);
        AC_TRACE << "GLShader::deactivate " << theMaterial.get<NameTag>();
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
        MAKE_GL_SCOPE_TIMER(GLShader_loadParameters);
        theShader._myVertexParameters.clear();

        dom::NodePtr myParameterListNode = theParameterListNode->childNode(PARAMETER_LIST_NAME);
        NodeList::size_type myParameterCount = myParameterListNode->childNodesLength();
        for (unsigned i = 0; i < myParameterCount; ++i) {
            dom::NodePtr myParameterNode = myParameterListNode->childNode(i);
            if (myParameterNode->nodeType() != dom::Node::COMMENT_NODE) {
                const string & myPropertyName = myParameterNode->getAttributeString("name");
                const string & myPropertyTypeString = myParameterNode->nodeName();
                TypeId myPropertyType;
                myPropertyType.fromString(myPropertyTypeString);
                //= TypeId(asl::getEnumFromString(myPropertyTypeString, TypeIdStrings));

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
    GLShader::setupProjectorMatrix( Matrix4f & theMatrix,
                                    const MaterialBase & theMaterial,
                                    const Camera & theCamera,
                                    const TextureUnit & theTextureUnit)
    {
        MAKE_GL_SCOPE_TIMER(GLShader_setupProjectorMatrix);

        // projector model matrix
        const std::string & myProjectorId = theTextureUnit.get<TextureUnitProjectorIdTag>();
        NodePtr myProjectorNode = theTextureUnit.getNode().getElementById(myProjectorId);
        if (! myProjectorNode) {
            throw ShaderException(std::string("Projector with id '") + myProjectorId + "' not found.", PLUS_FILE_LINE);
        }
        ProjectiveNodePtr myProjector = myProjectorNode->getFacade<ProjectiveNode>();
        const Matrix4f & myProjectorModelMatrix = myProjector->get<y60::InverseGlobalMatrixTag>();

        // projector projection matrix
        TexturePtr myTexture = theTextureUnit.getTexture();
        Vector4f myPoTSize( static_cast<float>(myTexture->get<TextureWidthTag>())
                          , static_cast<float>(myTexture->get<TextureHeightTag>()), 0.0f, 1.0f );
        const Matrix4f & myImageMatrix = myTexture->getImage()->get<ImageMatrixTag>();
        Vector4f mySize = myPoTSize * myImageMatrix;
        float myAspect( mySize[0] / mySize[1] ); // XXX [DS] this aspect already contains the image matrix
                                                 // TODO: think about the texture matrix...

        myProjector->updateFrustum(theTextureUnit.get<ResizePolicyTag>(), myAspect);

        const Frustum & myFrustum = myProjector->get<FrustumTag>();
        // XXX myFrustum.updateCorners(myProjector->get<NearPlaneTag>(), myProjector->get<FarPlaneTag>(),
        //        myProjector->get<HfovTag>(), myProjector->get<OrthoWidthTag>(), myAspect);

        Matrix4f myProjectionMatrix;
        myFrustum.getProjectionMatrix( myProjectionMatrix );
        theMatrix.assign(theCamera.get<y60::GlobalMatrixTag>());

        theMatrix.postMultiply( myProjectorModelMatrix);
        theMatrix.postMultiply( myProjectionMatrix );

        // magic matrix: scale and bias to apply the projection to entire frustum,
        // not only the upper right quadrant; this also performs an y-flip so the projected image
        // has the correct orientation
        Matrix4f myMagicMatrix;
        myMagicMatrix.makeScaling(Vector3f(0.5, -0.5, 0.5));
        myMagicMatrix.translate( Vector3f(0.5, 0.5, 0.5));

        theMatrix.postMultiply( myMagicMatrix );
    }

    void
    GLShader::enableTextureProjection(const MaterialBase & theMaterial,
            const Viewport & theViewport,
            const Camera & theCamera)
    {
        MAKE_GL_SCOPE_TIMER(GLShader_enableTextureProjection);

        MaterialBase::TexGenModeList myTexGenModes = theMaterial.getTexGenModes();
        MaterialBase::TexGenParamsList myTexGenParams = theMaterial.getTexGenParams();

        // enable texture coordinate generation
        for (unsigned myTexUnit = 0; myTexUnit < myTexGenModes.size(); ++myTexUnit) {
            bool mustRestoreMatrix = false;

            glActiveTexture(asGLTextureRegister(myTexUnit));
            MaterialBase::TexGenMode myModes = myTexGenModes[myTexUnit];
            MaterialBase::TexGenParams myParams = myTexGenParams[myTexUnit];

            for (unsigned i = 0; i < myModes.size(); ++i) {

                //AC_DEBUG << "unit=" << myTexUnit << " coord=" << i << " mode=" << myModes[i];
                if (myModes[i] == NONE) {
                    continue;
                }

                // set texgen plane params
                if (myModes[i] == EYE_LINEAR) {

                    // push modelview matrix
                    if (mustRestoreMatrix == false) {

                        const TextureUnit & myTextureUnit = theMaterial.getTextureUnit(myTexUnit);
                        const std::string & myProjectorId = myTextureUnit.get<TextureUnitProjectorIdTag>();
                        if (!myProjectorId.empty()) {
                            Matrix4f myTextureMatrix;
                            setupProjectorMatrix( myTextureMatrix, theMaterial, theCamera, myTextureUnit );

                            glMatrixMode( GL_TEXTURE );
                            glMultMatrixf(static_cast<const GLfloat *>(myTextureMatrix.getData()));
                            glMatrixMode( GL_MODELVIEW );
                        }

                        // TODO: will this work if a material has more than one projective texture?
                        glPushMatrix();
                        glLoadIdentity();
                        mustRestoreMatrix = true;
                    }
                    glTexGenfv(ourTexGenCoord[i], GL_EYE_PLANE, &(*(myParams[i]).begin()));
                } else if (myModes[i] == OBJECT_LINEAR) {
                    // TODO: think about the object linear use-cases
                    glTexGenfv(ourTexGenCoord[i], GL_OBJECT_PLANE, &(*(myParams[i]).begin()));
                }

                // set texgen mode
                glTexGeni(ourTexGenCoord[i], GL_TEXTURE_GEN_MODE, asGLTexCoordMode(myModes[i]));
                glEnable(ourTexGenToken[i]);
            }
            if (mustRestoreMatrix) {
                glPopMatrix();
            }
            CHECK_OGL_ERROR;
        }
    }

    void
    GLShader::bindBodyParams(const MaterialBase & theMaterial,
            const Viewport & theViewport,
            const LightVector & theLights,
            const Body & theBody,
            const Camera & theCamera)
    {
    }

    const ShaderFeatureSet &
    GLShader::getFeatureSet() const {
        return _myFeatureSet;
    }

    const VectorOfString *
    GLShader::getFeatures(const string & theFeatureClass) const {
        return _myFeatureSet.getFeatures(theFeatureClass);
    }
    const VectorOfTextureUsage &
    GLShader::getTextureFeature() const {
        return _myFeatureSet.getTextureFeature();
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
        GLint myMaxTexUnits;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &myMaxTexUnits);
        if (myMaxTexUnits < 0) { // should not happen
            myMaxTexUnits = 0;
        }
        return unsigned(myMaxTexUnits);
    }
} // namespace y60

