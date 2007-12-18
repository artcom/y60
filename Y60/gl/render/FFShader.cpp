//=============================================================================
// Copyright (C) 2003-2006 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include <y60/GLUtils.h>
#include <asl/Logger.h>

#include "FFShader.h"
#include <y60/glExtensions.h>

#include <y60/Texture.h>
#include <y60/Image.h>
#include <y60/NodeNames.h>
#include <y60/PropertyNames.h>


#include <asl/Dashboard.h>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) //x

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
    FFShader::activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {
        GLShader::activate(theMaterial, theViewport, 0); // activate stipple && attenuation

        if (!theLastMaterial || theLastMaterial->getGroup1Hash() != theMaterial.getGroup1Hash()) {
            GLShader::activateGroup1(theMaterial, theViewport); // activate exotic properties, like linewidth, glow, etc.
        }

		MaterialPropertiesFacadePtr myMaterialPropFacade = theMaterial.getChild<MaterialPropertiesTag>();
        if (theMaterial.getLightingModel() != UNLIT) {

            glMaterialfv(GL_FRONT, GL_DIFFUSE,  myMaterialPropFacade->get<MaterialDiffuseTag>().begin());
            CHECK_OGL_ERROR;

            // Vertexcolor always goes into the ambient color for lit materials
            if (!_myFixedFunctionShader._myVertexRegisterFlags[COLORS_REGISTER]) {
                glMaterialfv(GL_FRONT, GL_AMBIENT,  myMaterialPropFacade->get<MaterialAmbientTag>().begin());
                CHECK_OGL_ERROR;
            }
            glMaterialfv(GL_FRONT, GL_SPECULAR, myMaterialPropFacade->get<MaterialSpecularTag>().begin());
            CHECK_OGL_ERROR;
            float myShininess = myMaterialPropFacade->get<ShininessTag>();
            if (myShininess <0) {
                AC_WARNING << "shininess out of range 0..128, ("<<myShininess<<") setting to 0";
                myMaterialPropFacade->set<ShininessTag>(0);
            } 
            if (myShininess > 128) {
                AC_WARNING << "shininess out of range 0..128, ("<<myShininess<<") setting to 128";
                myMaterialPropFacade->set<ShininessTag>(128);
            } 
            glMaterialf(GL_FRONT, GL_SHININESS, myMaterialPropFacade->get<ShininessTag>());
            CHECK_OGL_ERROR;
            glMaterialfv(GL_FRONT, GL_EMISSION, myMaterialPropFacade->get<MaterialEmissiveTag>().begin());
            CHECK_OGL_ERROR;
        } else {
            glColor4fv(myMaterialPropFacade->get<SurfaceColorTag>().begin());
            CHECK_OGL_ERROR;
        }
    }

    void
    FFShader::deactivate(const y60::MaterialBase & theMaterial) {
        GLShader::deactivate(theMaterial);
        if (theMaterial.getLightingModel() == UNLIT) {
            glColor4f(1.0f,1.0f,1.0f,1.0f);
            CHECK_OGL_ERROR;
        }
    }

    void
    FFShader::enableTextures(const y60::MaterialBase & theMaterial) {
        GLShader::enableTextures(theMaterial);
    }

    void
    FFShader::disableTextures(const y60::MaterialBase & theMaterial) {
        GLShader::disableTextures(theMaterial);
    }

} // namespace y60

