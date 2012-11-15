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
#include "FFShader.h"

#include <y60/glutil/GLUtils.h>
#include <asl/base/Logger.h>

#include <y60/glutil/glExtensions.h>

#include <y60/scene/Texture.h>
#include <y60/image/Image.h>
#include <y60/base/NodeNames.h>
#include <y60/base/PropertyNames.h>


#include <asl/base/Dashboard.h>

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
        MAKE_GL_SCOPE_TIMER(FFShader_activate);
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
        MAKE_GL_SCOPE_TIMER(FFShader_deactivate);
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

