// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2005, ART+COM AG Berlin, Germany
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: FFShader.cpp,v $
//
//   $Revision: 1.27 $
//
// Description: Block Allocator
//
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __

#include <y60/GLUtils.h>
#include <asl/Logger.h>


#include "FFShader.h"
#include <y60/glExtensions.h>

#include <y60/Texture.h>
#include <y60/Image.h>
#include <y60/NodeNames.h>
#include <y60/PropertyNames.h>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) x
namespace y60 {

    FFShader::FFShader(const dom::NodePtr theNode) :
        GLShader(theNode) {
        _myType = FIXED_FUNCTION_MATERIAL;

        dom::NodePtr myFFShaderNode = theNode->childNode(FIXED_FUNCTION_SHADER_NODE_NAME);
        if (myFFShaderNode) {
            loadShaderProperties(myFFShaderNode, _myFixedFunctionShader);
            loadParameters(myFFShaderNode, _myFixedFunctionShader);
        } else {
            throw ShaderException(string("FFShader::FFShader() - missing node ")+
                    FIXED_FUNCTION_SHADER_NODE_NAME +
                    " in shader " + theNode->getAttributeString(ID_ATTRIB),PLUS_FILE_LINE);
        }
    }

    const MaterialParameterVector &
    FFShader::getVertexParameters() const {
        return _myFixedFunctionShader._myVertexParameters;
    }

    const y60::VertexRegisterFlags &
    FFShader::getVertexRegisterFlags() const {
        return _myFixedFunctionShader._myVertexRegisterFlags;
    }

    FFShader::~FFShader()  {
    }

    void
    FFShader::activate(MaterialBase & theMaterial) {
        GLShader::activate(theMaterial);
		MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial.getChild<MaterialPropertiesTag>();
        if (theMaterial.getLightingModel() != UNLIT) {
            glMaterialfv(GL_FRONT, GL_DIFFUSE,  myMaterialPropFacade->get<MaterialDiffuseTag>().begin());

            // Vertexcolor always goes into the ambient color for lit materials
            if (!_myFixedFunctionShader._myVertexRegisterFlags[COLORS_REGISTER]) {
                glMaterialfv(GL_FRONT, GL_AMBIENT,  myMaterialPropFacade->get<MaterialAmbientTag>().begin());
            }
            glMaterialfv(GL_FRONT, GL_SPECULAR, myMaterialPropFacade->get<MaterialSpecularTag>().begin());
            glMaterialf(GL_FRONT, GL_SHININESS, myMaterialPropFacade->get<ShininessTag>());
            glMaterialfv(GL_FRONT, GL_EMISSION, myMaterialPropFacade->get<MaterialEmissiveTag>().begin());
        } else {
            glColor4fv(myMaterialPropFacade->get<SurfaceColorTag>().begin());
        }
    }

    void
    FFShader::deactivate(const y60::MaterialBase & theMaterial) {
        GLShader::deactivate(theMaterial);
        if (theMaterial.getLightingModel() == UNLIT) {
            glColor4f(1.0f,1.0f,1.0f,1.0f);
        }
    }

    void
    FFShader::enableTextures(const y60::MaterialBase & theMaterial) {
        unsigned myTextureCount = theMaterial.getTextureCount();
        DB(AC_TRACE << "FFShader::enableTextures: Material " << theMaterial.get<NameTag>() << " has " << myTextureCount << " textures." << endl);
        glMatrixMode(GL_TEXTURE);

        bool alreadyHasSpriteTexture = false;
        for (unsigned myTextureCounter = 0; myTextureCounter < myTextureCount; ++myTextureCounter) {
            glActiveTextureARB(asGLTextureRegister(myTextureCounter));
            CHECK_OGL_ERROR;
            const y60::Texture & myTexture = theMaterial.getTexture(myTextureCounter);
            TextureUsage 
                myTextureUsage = theMaterial.getTextureUsage(myTextureCounter); 

            DB(AC_TRACE << "FFShader::enableTextures: texture index=" << myTextureCounter <<
                ", gl-id = " << myTexture.getId() <<
                ", image id =" << myTexture.getImage()->get<IdTag>() <<", size = " <<
                myTexture.getImage()->get<ImageWidthTag>() << "," <<
                myTexture.getImage()->get<ImageHeightTag>()<< "," <<
                myTexture.getImage()->get<ImageDepthTag>() << "," <<
                endl);
            // Fixed Function Shaders only support paint & skybox usage
            if (myTextureUsage == PAINT || myTextureUsage == SKYBOX) {
                switch (myTexture.getImage()->getType()) {
                    case SINGLE :
                        if (myTexture.getImage()->get<ImageDepthTag>()==1) {
                            glBindTexture(GL_TEXTURE_2D, myTexture.getId());
                            CHECK_OGL_ERROR;
                            glEnable(GL_TEXTURE_2D);
                            CHECK_OGL_ERROR;
                        } else {
                            glBindTexture(GL_TEXTURE_3D, myTexture.getId());
                            CHECK_OGL_ERROR;
                            glEnable(GL_TEXTURE_3D);
                            CHECK_OGL_ERROR;
                        }
                        break;
                    case CUBEMAP :
                        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, myTexture.getId());
                        glEnable(GL_TEXTURE_CUBE_MAP_ARB);
                        break;
                    default :
                        throw ShaderException(std::string("Unknown texture type '")+
                                myTexture.getImage()->get<NameTag>() + "'", PLUS_FILE_LINE);
                }
                asl::Matrix4f myMatrix = myTexture.getImage()->get<ImageMatrixTag>();
                myMatrix.postMultiply(myTexture.get<TextureMatrixTag>());
				AC_TRACE << "TM " << myTexture.get<TextureMatrixTag>();
				AC_TRACE << "MM " << myMatrix;
                glLoadMatrixf(static_cast<const GLfloat *>(myMatrix.getData()));

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

                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
                        GLfloat(asGLTextureFunc(myTexture.getApplyMode())));
            }
        }
        glMatrixMode(GL_MODELVIEW);
        GLShader::enableTextures(theMaterial);
    }

    void
    FFShader::disableTextures(const y60::MaterialBase & theMaterial) {
        GLShader::disableTextures(theMaterial);
        unsigned myTextureCount = theMaterial.getTextureCount();
        // AC_TRACE << "current texcount:" << myMaterial->getTextureCount() << ", prev:" << myPreviousTextureCount << endl;
        for (unsigned myTextureCounter = 0; myTextureCounter < myTextureCount; ++myTextureCounter) {
            const y60::Texture & myTexture = theMaterial.getTexture(myTextureCounter);
            TextureUsage myTextureUsage = theMaterial.getTextureUsage(myTextureCounter);

            // Fixed Function Shaders only support paint & skybox usage
            if (myTextureUsage == PAINT || myTextureUsage == SKYBOX) {
                glActiveTextureARB(asGLTextureRegister(myTextureCounter));
                glClientActiveTextureARB(asGLTextureRegister(myTextureCounter));

                switch (myTexture.getImage()->getType()) {
                    case SINGLE :
                        break;
                    case CUBEMAP :
                        glDisable(GL_TEXTURE_CUBE_MAP_ARB);
                        break;
                    default :
                        throw ShaderException(std::string("Unknown texture type '")+
                                myTexture.getImage()->get<NameTag>() + "'", PLUS_FILE_LINE);
                }
                if (myTexture.getImage()->get<ImageDepthTag>()==1) {
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glDisable(GL_TEXTURE_2D);
                } else {
                    glBindTexture(GL_TEXTURE_3D, 0);
                    glDisable(GL_TEXTURE_3D);
                }
            }
        }
        glDisable(GL_POINT_SPRITE_ARB);
        //glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_FALSE );
    }

} // namespace y60

