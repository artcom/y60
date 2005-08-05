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

#include <y60/NodeNames.h>
using namespace std;
using namespace asl;
using namespace y60;

#define DB(x) // x

namespace y60 {

    CGShader::CGShader(const dom::NodePtr theNode) :
        GLShader(theNode)
    {
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

        // compile fragment shader
        if (!_myFragmentProgram) {
            DB(AC_TRACE << "CGShader::loadShader(): Loading fragment shader from file '"
                    << _myFragmentShader._myFilename << "'" << endl;);
            _myFragmentProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myFragmentShader,
                    myShaderLibrary->getCgContext(), myShaderLibrary->getShaderDir()));
        }

        // compile vertex shader
        if (!_myVertexProgram) {
            DB(AC_TRACE << "CGShader::loadShader(): Loading vertex shader from file '"
                    << _myVertexShader._myFilename << "'" << endl;);
            _myVertexProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myVertexShader,
                    myShaderLibrary->getCgContext(), myShaderLibrary->getShaderDir()));
        }
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

    const MaterialParameterVectorPtr
    CGShader::getVertexParameter() const {
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
            DB(AC_TRACE << "added arg " << trim(myRestArgString.substr(posStart, posEnd-posStart)) << endl;)
            theArgs.push_back(trim(myRestArgString.substr(posStart, posEnd-posStart)));
            posStart = posEnd +1;
        }

        myRestArgString = trim(myRestArgString.substr(posStart));
        if (myRestArgString.length() > 0 ) {
            DB(AC_TRACE << "added arg " << myRestArgString << endl;);
            theArgs.push_back (myRestArgString);
        }
    }

    void
    CGShader::loadShaderProperties(const dom::NodePtr theShaderNode,
            ShaderDescription & theShader)
    {
        GLShader::loadShaderProperties(theShaderNode, theShader);
        theShader._myFilename = theShaderNode->getAttributeString(CG_FILE_PROPERTY);
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
    CGShader::getShader(const ShaderType & theShadertype) const
    {
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
    CGShader::activate(MaterialBase & theMaterial) {
        GLShader::activate(theMaterial);
        if (_myVertexProgram) {
            _myVertexProgram->enableProfile();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->enableProfile();
        }
        bindMaterialParams(theMaterial);
    }

    void
    CGShader::deactivate(const MaterialBase &) {
        if (_myVertexProgram) {
            _myVertexProgram->disableProfile();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->disableProfile();
        }
    }

    void
    CGShader::enableTextures(const MaterialBase & theMaterial) {
        if (_myVertexProgram) {
            _myVertexProgram->enableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->enableTextures();
        }
    }

    void
    CGShader::disableTextures(const MaterialBase & theMaterial) {
        if (_myVertexProgram) {
            _myVertexProgram->disableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->disableTextures();
        }
    }

    void
    CGShader::setCgParameter(CGparameter & theCgParameter, const dom::NodePtr & theNode,
                             const MaterialBase & theMaterial)
    {
        const string & thePropertyName = theNode->getAttributeString("name");
        AC_TRACE << "setting " << theNode->nodeName() <<" property " << thePropertyName << endl;
        switch(TypeId(asl::getEnumFromString(theNode->nodeName(), TypeIdStrings)))
        {
            case FLOAT:
                {
                    float myValue = (*theNode)("#text").dom::Node::nodeValueAs<float>();
                    cgGLSetParameter1f(theCgParameter, myValue);
                    checkCgError();
                    break;
                }
            case VECTOR2F:
                {
                    Vector2f myValueV = (*theNode)("#text").dom::Node::nodeValueAs<Vector2f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter2fv(theCgParameter, myValue);
                    checkCgError();
                    break;
                }
            case VECTOR3F:
                {
                    Vector3f myValueV = (*theNode)("#text").dom::Node::nodeValueAs<Vector3f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter3fv(theCgParameter, myValue);
                    checkCgError();
                    break;
                }
            case VECTOR4F:
                {
                    Vector4f myValueV = (*theNode)("#text").dom::Node::nodeValueAs<Vector4f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter4fv(theCgParameter, myValue);
                    checkCgError();
                    break;
                }
             case VECTOR_OF_VECTOR2F:
                {
                    VectorOfVector2f myValueV = (*theNode)("#text").dom::Node::nodeValueAs<VectorOfVector2f>();
                    float * myValue = myValueV.begin()->begin();
                    cgGLSetParameterArray2f(theCgParameter, 0, myValueV.size(), myValue);
                    checkCgError();
                    break;
                }
             case VECTOR_OF_VECTOR4F:
                {
                    VectorOfVector4f myValueV = (*theNode)("#text").dom::Node::nodeValueAs<VectorOfVector4f>();
                    float * myValue = myValueV.begin()->begin();
                    cgGLSetParameterArray4f(theCgParameter, 0, myValueV.size(), myValue);
                    checkCgError();
                    break;
                }
            case SAMPLER2D:
            case SAMPLERCUBE:
                {
                    unsigned myTextureIndex = (*theNode)("#text").dom::Node::nodeValueAs<unsigned>();
                    if (myTextureIndex  < theMaterial.getTextureCount()) {
                        const Texture & myTexture = theMaterial.getTexture(myTextureIndex);
                        cgGLSetTextureParameter( theCgParameter, myTexture.getId());
                        DB(AC_TRACE << "cgGLSetTextureParameter: Texture index: "<< as_string(myTextureIndex)
                                << " , glid : " << myTexture.getId()
                                << " , property : " << thePropertyName
                                << " to parameter : "<< cgGetParameterName(theCgParameter) << endl;)
                        checkCgError();
                    } else {
                        throw ShaderException(string("Texture Index ") + as_string(myTextureIndex) +
                                " not found. Material has " + as_string(theMaterial.getTextureCount()) + " texture(s)",
                                "CGShader::setCgParameter()");
                    }
                    break;
                }
            default:
                throw ShaderException(string("Unknown CgParameter type in property '")+thePropertyName+"'",
                        "CGShader::setCgParameter()");
        }

    }

    void
    CGShader::checkCgError() const {
        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            throw ShaderException(std::string("Cg error: ") + cgGetErrorString(myCgError),
                                    "CGShader::checkCgError()");
        }
    }

    void
    CGShader::bindBodyParams(const MaterialBase & theMaterial,
            const Viewport & theViewport,
            const LightVector & theLights,
            const Body & theBody,
            const Camera & theCamera)
    {
        GLShader::bindBodyParams(theMaterial, theViewport, theLights, theBody, theCamera);

        if (_myFragmentProgram) {
            _myFragmentProgram->setCGGLParameters();
            _myFragmentProgram->setAutoParameters(theLights, theViewport, theBody, theCamera);
        }

        if (_myVertexProgram) {
            _myVertexProgram->setCGGLParameters();
            _myVertexProgram->setAutoParameters(theLights, theViewport, theBody, theCamera);
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->bind();
        }
        if (_myVertexProgram) {
            _myVertexProgram->bind();
        }
    }

    void
    CGShader::bindMaterialParams(const MaterialBase & theMaterial) {
        this->checkCgError();
        dom::NodePtr myPropertyList = theMaterial.getProperties();
        unsigned myPropertyCount = myPropertyList->childNodesLength();
        for (unsigned i = 0; i < myPropertyCount; ++i) {
            const string & myPropertyName = myPropertyList->childNode(i)->getAttributeString("name");
            if (_myFragmentProgram) {
                CGparameter myCgParameter = cgGetNamedParameter(_myFragmentProgram->getCgProgramID(), myPropertyName.c_str());
                if (myCgParameter) {
                    setCgParameter(myCgParameter, myPropertyList->childNode(i), theMaterial);
                }
            }
            if (_myVertexProgram) {
                CGparameter myCgParameter = cgGetNamedParameter(_myVertexProgram->getCgProgramID(), myPropertyName.c_str());
                if (myCgParameter) {
                    setCgParameter(myCgParameter, myPropertyList->childNode(i), theMaterial);
                }
            }
        }
    }

} // namespace y60

