//=============================================================================
// Copyright (C) 2003 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: CGShader.cpp,v $
//   $Author: pavel $
//   $Revision: 1.15 $
//   $Date: 2005/04/24 00:41:18 $
//
//   Description:
//
//=============================================================================

#include "CGShader.h"

#include <y60/GLUtils.h>
#include <y60/NodeNames.h>
#include <asl/PackageManager.h>

using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) //x

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

    CGShader::CGShader(const dom::NodePtr theNode) : GLShader(theNode) {
        _myType = CG_MATERIAL;
        dom::NodePtr myShaderNode = theNode->childNode(VERTEX_SHADER_NODE_NAME);
        if (myShaderNode) {
            loadShaderProperties(myShaderNode, _myVertexShader);
            loadParameters(myShaderNode, _myVertexShader);
        } else {
            throw ShaderException(string("CGShader::CGShader() - missing node ")+ VERTEX_SHADER_NODE_NAME +
                    " in shader " + theNode->getAttributeString(ID_ATTRIB),PLUS_FILE_LINE);
        }

        myShaderNode = theNode->childNode(FRAGMENT_SHADER_NODE_NAME);
        if (myShaderNode) {
            loadShaderProperties(myShaderNode, _myFragmentShader);
        } else {
            throw ShaderException(string("CGShader::CGShader() - missing node ")+ FRAGMENT_SHADER_NODE_NAME +
                    " in shader " + theNode->getAttributeString(ID_ATTRIB),PLUS_FILE_LINE);
        }
    }

    CGShader::~CGShader()  {
    }

    void CGShader::compile(IShaderLibrary & theShaderLibrary) {
        ShaderLibrary * myShaderLibrary = dynamic_cast<ShaderLibrary *>(&theShaderLibrary);
        if (!myShaderLibrary) {
            throw ShaderException("CGShader::compile() - Dynamic cast of IShaderLibrary failed",
                                  PLUS_FILE_LINE);
        }
        assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());

        // compile fragment shader
        if (!_myFragmentProgram) {
            DB(AC_TRACE << "CGShader::loadShader(): Loading fragment shader from file '"
                    << _myFragmentShader._myFilename << "'");
            _myFragmentProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myFragmentShader,
                    myShaderLibrary->getCgContext()));
            assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());
        }

        // compile vertex shader
        if (!_myVertexProgram) {
            DB(AC_TRACE << "CGShader::loadShader(): Loading vertex shader from file '"
                    << _myVertexShader._myFilename << "'");
            _myVertexProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myVertexShader,
                    myShaderLibrary->getCgContext()));
            assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());
        }

        AC_DEBUG << "Successfully compiled '" << getName() << "'";
    }

    void CGShader::load(IShaderLibrary & theShaderLibrary) {
        compile(theShaderLibrary);

        // load fragment shader
        if (_myFragmentProgram) {
            _myFragmentProgram->load();
        }

        // load fragment shader
        if (_myVertexProgram) {
            _myVertexProgram->load();
        }
    }

    bool CGShader::isLoaded() const {
        return (_myFragmentProgram != 0) || (_myVertexProgram != 0);
    }

    void CGShader::unload() {
        _myFragmentProgram = CgProgramInfoPtr(0);
        _myVertexProgram = CgProgramInfoPtr(0);
    }

    const MaterialParameterVector &
    CGShader::getVertexParameters() const {
        return _myVertexShader._myVertexParameters;
    }

    const VertexRegisterFlags &
    CGShader::getVertexRegisterFlags() const {
        return _myVertexShader._myVertexRegisterFlags;
    }

    // converts a comma-delimted list of compiler args ("-DFOO, -DBar") to
    // a null-terminated array of null-terminated args.
    void
    CGShader::processCompilerArgs(std::vector<std::string> & theArgs, const string & theArgList) {
        theArgs.clear();
        string myRestArgString(theArgList);
        string::size_type posStart = 0;
        string::size_type posEnd;

        while ((posEnd = myRestArgString.find(',', posStart)) != string::npos) {
            DB(AC_TRACE << "added arg " << trim(myRestArgString.substr(posStart, posEnd-posStart)));
            theArgs.push_back(trim(myRestArgString.substr(posStart, posEnd-posStart)));
            posStart = posEnd +1;
        }

        myRestArgString = trim(myRestArgString.substr(posStart));
        if (myRestArgString.length() > 0 ) {
            DB(AC_TRACE << "added arg " << myRestArgString);
            theArgs.push_back (myRestArgString);
        }
    }

    void
    CGShader::loadShaderProperties(const dom::NodePtr theShaderNode,
            ShaderDescription & theShader)
    {
        GLShader::loadShaderProperties(theShaderNode, theShader);

        string myFilename = theShaderNode->getAttributeString(CG_FILE_PROPERTY);
        theShader._myFilename = AppPackageManager::get().getPtr()->searchFile(myFilename);
        if (theShader._myFilename.empty()) {
            throw ShaderException(string("Could not find cg-shader '") + myFilename + "' in " +
                AppPackageManager::get().getPtr()->getSearchPath(), PLUS_FILE_LINE);
        }
        theShader._myEntryFunction = theShaderNode->getAttributeString(CG_ENTRY_FUNCTION_PROPERTY);
        string myProfile = theShaderNode->getAttributeString(CG_PROFILE_PROPERTY);
        theShader._myProfile = ShaderProfile(asl::getEnumFromString(myProfile,
                ShaderProfileStrings));
        if (theShaderNode->getAttribute(CG_COMPILERARGS_PROPERTY)) {
            processCompilerArgs(theShader._myCompilerArgs,
                    theShaderNode->getAttributeString(CG_COMPILERARGS_PROPERTY));
        }
    }

    bool
    CGShader::hasShader(const ShaderType &theShadertype) const {
        if (theShadertype == VERTEX_SHADER ) {
            return ( ! _myVertexShader._myFilename.empty());
        } else if (theShadertype == FRAGMENT_SHADER) {
            return ( ! _myFragmentShader._myFilename.empty());
        } else {
            throw ShaderException(string("Unknown Shadertype : ") + as_string(theShadertype) ,
                                    "CgMaterial::hasShader()" );
        }
    }

    const ShaderDescription &
    CGShader::getShader(const ShaderType & theShadertype) const {
        if (theShadertype == VERTEX_SHADER ) {
            return _myVertexShader;
        } else if (theShadertype == FRAGMENT_SHADER) {
            return _myFragmentShader;
        } else {
            throw ShaderException(string("Unknown Shadertype : ") + as_string(theShadertype) ,
                                    "CgMaterial::getShader()" );
        }
    }

    void
    CGShader::activate(MaterialBase & theMaterial, const Viewport & theViewport) {
        GLShader::activate(theMaterial, theViewport);
        if (_myVertexProgram) {
            _myVertexProgram->enableProfile();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->enableProfile();
        }
        bindMaterialParams(theMaterial);

        glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
    }

    void
    CGShader::deactivate(const MaterialBase & theMaterial) {

        GLShader::deactivate(theMaterial);

        if (_myVertexProgram) {
            _myVertexProgram->disableProfile();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->disableProfile();
        }

        glDisable(GL_VERTEX_PROGRAM_POINT_SIZE_ARB);
    }

    void
    CGShader::enableTextures(const MaterialBase & theMaterial) {
        GLShader::enableTextures(theMaterial);
        if (_myVertexProgram) {
            _myVertexProgram->enableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->enableTextures();
        }
    }

    void
    CGShader::disableTextures(const MaterialBase & theMaterial) {
        GLShader::disableTextures(theMaterial);
        if (_myVertexProgram) {
            _myVertexProgram->disableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->disableTextures();
        }
    }

    void
    CGShader::bindBodyParams(const MaterialBase & theMaterial,
            const Viewport & theViewport,
            const LightVector & theLights,
            const Body & theBody,
            const Camera & theCamera)
    {
        DBP2(MAKE_SCOPE_TIMER(CGShader_bindBodyParams));
        GLShader::bindBodyParams(theMaterial, theViewport, theLights, theBody, theCamera);

        if (_myVertexProgram) {
            bool b = _myVertexProgram->reloadIfRequired(theLights, theMaterial);
            _myVertexProgram->bindBodyParams(theMaterial, theLights, theViewport, theBody, theCamera);
            _myVertexProgram->bind();
        }
        if (_myFragmentProgram) {
            bool b = _myFragmentProgram->reloadIfRequired(theLights, theMaterial);
            _myFragmentProgram->bindBodyParams(theMaterial, theLights, theViewport, theBody, theCamera);
            _myFragmentProgram->bind();
        }
    }

    void
    CGShader::bindMaterialParams(const MaterialBase & theMaterial) {
        if (_myVertexProgram) {
            _myVertexProgram->bindMaterialParams(theMaterial);
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->bindMaterialParams(theMaterial);
        }
    }

    void
    CGShader::bindOverlayParams(const MaterialBase & theMaterial) {
        LightVector myEmptyLights;
        if (_myVertexProgram) {
            _myVertexProgram->reloadIfRequired(myEmptyLights, theMaterial);
            _myVertexProgram->bindOverlayParams();
            _myVertexProgram->bind();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->reloadIfRequired(myEmptyLights, theMaterial);
            _myFragmentProgram->bindOverlayParams();
            _myFragmentProgram->bind();
        }
    }

    unsigned
    CGShader::getMaxTextureUnits() const {
        if (_myFragmentProgram) {
            // Fragment shader can use much more texture units than fixed function shaders
            int myMaxTexUnits;
            glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &myMaxTexUnits);
            if (myMaxTexUnits < 0) { // should not happen
                myMaxTexUnits = 0;
            }
            return unsigned(myMaxTexUnits);
        } else {
            return GLShader::getMaxTextureUnits();
        }
    }
}
