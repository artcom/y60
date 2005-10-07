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
//   $RCSfile: CgProgramInfo.cpp,v $
//   $Author: pavel $
//   $Revision: 1.17 $
//   $Date: 2005/04/24 00:41:18 $
//
//  Description: C++ representation of a Cg program
//=============================================================================

#include "CgProgramInfo.h"
#include "Renderer.h"

#include <y60/GLUtils.h>


#ifdef WIN32
#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>
#endif
#ifdef LINUX
    #include <GL/glext.h>
    #include <GL/glx.h>
#endif

#include <y60/NodeValueNames.h>
#include <asl/string_functions.h>
#include <dom/Nodes.h>

#define DB(x) // x

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    void
    assertCg(const std::string & theWhere) {
        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            throw RendererException(std::string("Cg error: ") + cgGetErrorString(myCgError),
                    theWhere);
        }
    }

    void CgProgramInfo::createAndCompileProgram() {
        AC_DEBUG << "reloadProgram " << _myPathName.c_str();

        if (_myCgProgramString.empty()) {
            AC_DEBUG << "loading from file";

            // create null terminated array of null terminated strings
            for (int i=0; i < _myShader._myCompilerArgs.size(); ++i) {
                AC_DEBUG << "Using arg " << _myShader._myCompilerArgs[i].c_str() << endl;
                _myCachedCompilerArgs.push_back(_myShader._myCompilerArgs[i].c_str());
            }
            _myCachedCompilerArgs.push_back(0);
            _myCgProgramID = cgCreateProgramFromFile(_myContext, CG_SOURCE,
                    _myPathName.c_str(),
                    asCgProfile(_myShader),
                    _myShader._myEntryFunction.c_str(),
                    &(*_myCachedCompilerArgs.begin()));

            assertCg("CgProgramInfo::reloadProgram - createProgram");

            //DK: we cannot keep a compiled version 
            //    because unsized arrays of size 0 are completely gone after compilation
            _myCgProgramString = cgGetProgramString(_myCgProgramID, CG_PROGRAM_SOURCE);

        } else {
            AC_DEBUG << "destroying and reloading from string "; // << _myCgProgramString;

            cgDestroyProgram(_myCgProgramID);
            _myCgProgramID = cgCreateProgram(_myContext, CG_SOURCE, _myCgProgramString.c_str(),
                    asCgProfile(_myShader), _myShader._myEntryFunction.c_str(), 
                    &(*_myCachedCompilerArgs.begin()));

        }

        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            std::string myErrorMessage = "Cg error loading '" + _myShader._myEntryFunction +
                "' from " + _myPathName + ": " + cgGetErrorString(myCgError);
            if (myCgError == CG_COMPILER_ERROR) {
                myErrorMessage += "\n" + std::string(cgGetLastListing(_myContext));
            }
            throw RendererException(myErrorMessage, "CgProgramInfo::CgProgramInfo()");
        }
        processParameters();
        cgCompileProgram(_myCgProgramID);
    }

    CgProgramInfo::CgProgramInfo(const ShaderDescription & myShader,
            const CGcontext theCgContext,
            const std::string & theShaderDir)
        : _myShader(myShader), _myContext(theCgContext)
    {
        _myPathName = theShaderDir + myShader._myFilename;

        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS] = 0;
        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_DIFFUSE_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_SPECULAR_COLOR] = 0;

        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS] = 0;
        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_DIFFUSE_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_SPECULAR_COLOR] = 0;

        //VS: 100 punkte
        cgSetAutoCompile(_myContext, CG_COMPILE_MANUAL);
        createAndCompileProgram();

        DB(AC_TRACE << "CgProgramInfo::CgProgramInfo(" << _myCgProgramID << ")" << endl;);
    }

    void
    CgProgramInfo::load() {
        cgGLLoadProgram(_myCgProgramID);
        assertCg("CgProgramInfo::load()");
    }

    CgProgramInfo::~CgProgramInfo() {
        if (cgIsProgram(_myCgProgramID)) {
            if (cgGetProgramContext(_myCgProgramID) == _myContext) {
                DB(AC_TRACE << "CgProgramInfo::~CgProgramInfo(" << _myCgProgramID << ")" << endl;);
                cgDestroyProgram(_myCgProgramID);
            }
        }
        assertCg("CgProgramInfo::~CgProgramInfo()");
    }

    void
    CgProgramInfo::processParameters() {
        AC_DEBUG << "processParameters";
        // CGparameter myParam = cgGetFirstParameter(_myCgProgramID, CG_PROGRAM);
        // while (myParam != 0) {
        //
        _myGlParams.clear();
        _myAutoParams.clear();
        _myTextureParams.clear();

        for (CGparameter myParam = cgGetFirstParameter(_myCgProgramID, CG_PROGRAM);
                myParam != 0;  myParam = cgGetNextParameter(myParam))
        {
            string myParamName(cgGetParameterName(myParam));
            //XXX cgGetParameterVariability crashes on unsized arrays
            //CGenum myParamVariability = cgGetParameterVariability(myParam);
            CGenum myParamVariability = CG_UNIFORM;
            CGtype myParameterType    = cgGetParameterType(myParam);
            AC_DEBUG << "processing " << myParamName;

            assertCg("CgProgramInfo::processParameters() - 1");

            // scan for reserved GL_ prefix in uniform parameters
            if (myParamName.compare(0, 3, "GL_") == 0) {
                if (myParamVariability != CG_UNIFORM) {
                    throw RendererException ("Error in " + _myPathName + ": Parameter " + myParamName
                            + " not uniform.", "CgProgramInfo::processParameters()");
                }
                CGGLenum myParamTransform = CG_GL_MATRIX_IDENTITY;
                string myParamBase = myParamName;
                if (myParamName.substr(myParamName.length()-2) == "_I") {
                    myParamTransform = CG_GL_MATRIX_INVERSE;
                    myParamBase = myParamName.substr(0, myParamName.length()-2);
                } else if (myParamName.substr(myParamName.length()-2) == "_T") {
                    myParamTransform = CG_GL_MATRIX_TRANSPOSE;
                    myParamBase = myParamName.substr(0, myParamName.length()-2);
                } else if (myParamName.substr(myParamName.length()-3) == "_IT") {
                    myParamTransform = CG_GL_MATRIX_INVERSE_TRANSPOSE;
                    myParamBase = myParamName.substr(0, myParamName.length()-3);
                }
                CGGLenum myParamType;
                if (myParamBase == CG_GL_MODELVIEW_PARAMETER) {
                    myParamType = CG_GL_MODELVIEW_MATRIX;
                } else if (myParamBase == CG_GL_PROJECTION_PARAMETER) {
                    myParamType = CG_GL_PROJECTION_MATRIX;
                } else if (myParamBase == CG_GL_TEXTURE_PARAMETER) {
                    myParamType = CG_GL_TEXTURE_MATRIX;
                } else if (myParamBase == CG_GL_MODELVIEW_PROJECTION_PARAMETER) {
                    myParamType = CG_GL_MODELVIEW_PROJECTION_MATRIX;
                } else {
                    throw RendererException ("Error in " + _myPathName +
                            ": CGGL Parameter base " + myParamBase + " unknown.",
                            "CgProgramInfo::processParameters()");
                }
                if (myParameterType == CG_ARRAY || cgIsParameterReferenced(myParam) ) {
                    _myGlParams.push_back(
                            CgProgramGlParam(myParamName, myParam, myParamType, myParamTransform));
                } else {
                    AC_WARNING << "skipping non-referenced CG-GL parameter " << myParamName << endl;
                }
            }
            // scan for reserved AC_ prefix
            if (myParamName.compare(0, 3, "AC_") == 0) {
                if (myParamVariability != CG_UNIFORM) {
                    throw RendererException ("Error in " + _myPathName + ": Parameter " + myParamName
                            + " not uniform.", "CgProgramInfo::processParameters()");
                }

                CgAutoParameterID myParamID = static_cast<CgAutoParameterID>(
                        asl::getEnumFromString(myParamName, CGAutoParamterString));

                if (myParamID >= MAX_AUTO_PARAMETER) {
                    throw RendererException ("Error in " + _myPathName +
                            ": CG Auto Parameter " + myParamName + " unknown.",
                            "CgProgramInfo::processParameters()");
                }


                if (myParameterType == CG_ARRAY && cgGetArraySize(myParam, 0) == 0) {


                    int myArraySize = _myUnsizedArrayAutoParamSizes[myParamID];
                    if (myArraySize > 0) {

                        AC_DEBUG << "setting array " << myParamName << " to size " << myArraySize;

                        CGtype myArrayType = cgGetArrayType(myParam);
                        CGparameter myArray 
                            = cgCreateParameterArray(_myContext, myArrayType, myArraySize);

                        for(int i = 0; i <  myArraySize; ++i) {

                            CGparameter myArrayElement 
                                = cgCreateParameter(_myContext, myArrayType);
                            cgConnectParameter(myArrayElement, cgGetArrayParameter(myArray, i));
                        }

                        cgConnectParameter(myArray, myParam);
                        assertCg("CgProgramInfo::processParameters() unsized array");
                        AC_DEBUG << "done. created unsized array of size " << myArraySize;
                    }
                }

                if (myParameterType == CG_ARRAY) {
                    AC_INFO << "array " << myParamName << " size " << cgGetArraySize(myParam, 0);
                }

                if (myParameterType == CG_ARRAY || cgIsParameterReferenced(myParam)) {

                    AC_DEBUG << "adding auto param " << myParamName << ":" << myParamID << endl;
                    _myAutoParams[myParamID]
                        = CgProgramAutoParam(myParamName, myParam, myParamID, myParameterType);

                } else {
                    AC_WARNING << "skipping non-referenced AC-CG parameter " 
                        << myParamName << endl;
                }
            }
            // scan for sampler params
            if (myParameterType == CG_SAMPLER1D ||
                    myParameterType == CG_SAMPLER2D ||
                    myParameterType == CG_SAMPLER3D ||
                    myParameterType == CG_SAMPLERCUBE ) {
                _myTextureParams.push_back(CgProgramTextureParam(myParamName, myParam));
            }
            assertCg("CgProgramInfo::processParameters() - 2");
        }
        assertCg("CgProgramInfo::processParameters()");
        AC_DEBUG << "DONE processParameters";
    }

    void
    CgProgramInfo::enableProfile() {
        cgGLEnableProfile(asCgProfile(_myShader));
        assertCg("CgProgramInfo::enableProfile()");
        AC_TRACE << "enabled CgProfile(" << asCgProfile(_myShader) << ")" << endl;
    }

    void
    CgProgramInfo::disableProfile() {
        cgGLDisableProfile(asCgProfile(_myShader));
        assertCg("CgProgramInfo::disbaleProfile()");
        AC_TRACE << "disabled CgProfile(" << asCgProfile(_myShader) << ")" << endl;
    }

    void CgProgramInfo::setCGGLParameters() {
        //AC_DEBUG << "CgProgramInfo::setCGGLParameters";
        for (unsigned i=0; i<_myGlParams.size(); ++i) {
            CgProgramGlParam curParam = _myGlParams[i];
            cgGLSetStateMatrixParameter(curParam._myParameter, curParam._myStateMatrixType,
                    curParam._myTransform);
            AC_TRACE << "setting CGGL parameter " << curParam._myParamName << " to " << curParam._myTransform << endl;
            assertCg("CgProgramInfo::setCGGLParameters()");
        }
    }
    
    bool
    CgProgramInfo::reloadIfRequired(
            const LightVector & theLightInstances,
            MaterialBase & theMaterial) 
    {
        //AC_DEBUG << "reloadIfRequired";

        //look at the number of lights to see if reload is req.
        unsigned myPositionalLightCount = 0;
        unsigned myDirectionalLightCount = 0;
        for (unsigned i = 0; i < theLightInstances.size(); ++i) {
            LightPtr myLight = theLightInstances[i];
            if ( ! myLight->get<VisibleTag>()) {
                continue;
            }
            LightSourcePtr myLightSource = myLight->getLightSource();
            switch (myLightSource->getType()) {
                case POSITIONAL :
                    ++myPositionalLightCount;
                    break;
                case DIRECTIONAL :
                    ++myDirectionalLightCount;
                    break;
                case SPOT:
                case AMBIENT :
                    break;
                default :
                    AC_WARNING << "Unknown light type for " << myLightSource->get<IdTag>();
            }
        }

        bool myReload = false;

        int myLightType = POSITIONAL_LIGHTS;
        if (_myAutoParams.find(myLightType) != _myAutoParams.end() &&
                cgGetArraySize(_myAutoParams[myLightType]._myParameter,0) != myPositionalLightCount) 
        {
            myReload = true;
        }

        myLightType = DIRECTIONAL_LIGHTS;
        if (_myAutoParams.find(myLightType) != _myAutoParams.end() &&
                cgGetArraySize(_myAutoParams[myLightType]._myParameter,0) != myDirectionalLightCount) 
        {
            myReload = true;
        }

        if (myReload) {
            //change unsized array sizes here
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS] = myPositionalLightCount;
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_DIFFUSE_COLOR] = myPositionalLightCount;
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_SPECULAR_COLOR] = myPositionalLightCount;

            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS] = myDirectionalLightCount;
            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_DIFFUSE_COLOR] = myDirectionalLightCount;
            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_SPECULAR_COLOR] = myDirectionalLightCount;

            createAndCompileProgram();
            cgGLLoadProgram(_myCgProgramID);

            AC_DEBUG << "reloaded Cg program";
            enableProfile();
            bindMaterialParams(theMaterial);
            enableTextures();
        }

        return myReload;

    }
            
    void
    CgProgramInfo::bindBodyParams(
            const LightVector & theLightInstances,
            const Viewport & theViewport,
            const Body & theBody,
            const Camera & theCamera)
    {
        //AC_DEBUG << "CgProgramInfo::bindBodyParams";
        setCGGLParameters();
        setAutoParameters(theLightInstances, theViewport, theBody, theCamera);
    }
    
    void
    CgProgramInfo::bindMaterialParams(MaterialBase & theMaterial) {
        AC_DEBUG << "CgProgramInfo::bindMaterialParams this=" << hex << (void*)this << dec 
			     << " material=" << theMaterial.get<NameTag>();

		MaterialPropertiesFacadePtr myPropFacade = theMaterial.getFacade<MaterialPropertiesTag>();
		const NameAttributeNodeMap & myPropMap = myPropFacade->getEnsuredPropertyList();

		for (unsigned myPropIndex = 0; myPropIndex != myPropMap.length(); ++myPropIndex) {
			const Node & myPropertyNode = myPropMap[myPropIndex];
            const string & myPropertyName 
                = myPropertyNode.getAttributeString("name");

            CGparameter myCgParameter 
                = cgGetNamedParameter(_myCgProgramID, myPropertyName.c_str());

            if (myCgParameter) {
                setCgMaterialParameter(myCgParameter, myPropertyNode, theMaterial);
            }
        }
    }

    void
    CgProgramInfo::setAutoParameters(
            const LightVector & theLightInstances,
            const Viewport & theViewport,
            const Body & theBody,
            const Camera & theCamera)
    {
        //AC_DEBUG << "CgProgramInfo::setAutoParameters";

        std::vector<Vector3f> myPositionalLights;
        std::vector<Vector4f> myPositionalLightDiffuseColors;
        std::vector<Vector4f> myPositionalLightSpecularColors;

        std::vector<Vector3f> myDirectionalLights;
        std::vector<Vector4f> myDirectionalLightDiffuseColors;
        std::vector<Vector4f> myDirectionalLightSpecularColors;

        Vector4f myAmbientLightColor(0,0,0,1);

        // Collect light information
        for (unsigned i = 0; i < theLightInstances.size(); ++i) {
            LightPtr myLight = theLightInstances[i];
            if ( ! myLight->get<VisibleTag>()) {
                continue;
            }
            LightSourcePtr myLightSource = myLight->getLightSource();
			LightPropertiesFacadePtr myLightPropFacade = myLightSource->getFacade<LightPropertiesTag>();
            switch (myLightSource->getType()) {
                case POSITIONAL :
                    myPositionalLights.push_back(myLight->get<GlobalMatrixTag>().getTranslation());
                    myPositionalLightDiffuseColors.push_back(myLightPropFacade->get<LightDiffuseTag>());
                    myPositionalLightSpecularColors.push_back(myLightPropFacade->get<LightSpecularTag>());
                    break;
                case DIRECTIONAL :
                    {
                        const Vector4f & myTranslation = myLight->get<GlobalMatrixTag>().getRow(2);
                        myDirectionalLights.push_back(Vector3f(myTranslation[0],
                                    myTranslation[1],myTranslation[2]));
                        myDirectionalLightDiffuseColors.push_back(myLightPropFacade->get<LightDiffuseTag>());
                        myDirectionalLightSpecularColors.push_back(myLightPropFacade->get<LightSpecularTag>());
                        break;
                    }
                case AMBIENT :
                    myAmbientLightColor = myLightPropFacade->get<LightAmbientTag>();
                    break;
                default :
                    AC_INFO << "unknown light type " << myLightSource->get<IdTag>();
            }
        }


        // set parameter values
        for (CgProgramAutoParams::iterator myIter = _myAutoParams.begin(); 
                myIter != _myAutoParams.end(); ++myIter) 
        {
            bool myParamValueFoundFlag = true;

            CgProgramAutoParam curParam = myIter->second;
            AC_DEBUG << "setting parameter " << curParam._myName;

            switch (curParam._myID) {
                case CAMERA_POSITION :
                    setCgVectorParameter(curParam, theCamera.get<GlobalMatrixTag>().getTranslation());
                    break;
                case POSITIONAL_LIGHTS :
                    AC_DEBUG << "setting POSITIONAL_LIGHTS to " << myPositionalLights;
                    setCgUnsizedArrayParameter(curParam, myPositionalLights);
                    break;
                case POSITIONAL_LIGHTS_DIFFUSE_COLOR :
                    setCgUnsizedArrayParameter(curParam, myPositionalLightDiffuseColors);
                    break;
                case POSITIONAL_LIGHTS_SPECULAR_COLOR :
                    setCgUnsizedArrayParameter(curParam, myPositionalLightSpecularColors);
                    break;
                case DIRECTIONAL_LIGHTS :
                    AC_DEBUG << "setting DIRECTIONAL_LIGHTS to " << myDirectionalLights;
                    setCgUnsizedArrayParameter(curParam, myDirectionalLights);
                    break;
                case DIRECTIONAL_LIGHTS_DIFFUSE_COLOR :
                    setCgUnsizedArrayParameter(curParam, myDirectionalLightDiffuseColors);
                    break;
                case DIRECTIONAL_LIGHTS_SPECULAR_COLOR :
                    setCgUnsizedArrayParameter(curParam, myDirectionalLightSpecularColors);
                    break;
                case AMBIENT_LIGHT_COLOR :
                    setCgVectorParameter(curParam, myAmbientLightColor);
                    break;
                case CAMERA_I:
                    setCgMatrixParameter(curParam, theCamera.get<InverseGlobalMatrixTag>());
                    break;
                case CAMERA_T:
                    {
                        Matrix4f myTransposedMatrix = theCamera.get<GlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        break;
                    }
                case VIEWPROJECTION:
                    {
                        Matrix4f myMatrix = theCamera.get<InverseGlobalMatrixTag>();
                        myMatrix.postMultiply(theViewport.get<ProjectionMatrixTag>());
                        setCgMatrixParameter(curParam, myMatrix);
                        break;
                    }
                case OBJECTWORLD:
                    setCgMatrixParameter(curParam, theBody.get<GlobalMatrixTag>());
                    break;
                case OBJECTWORLD_I:
                    setCgMatrixParameter(curParam, theBody.get<InverseGlobalMatrixTag>());
                    break;
                case OBJECTWORLD_T:
                    {
                        Matrix4f myTransposedMatrix = theBody.get<GlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        break;
                    }
                case OBJECTWORLD_IT:
                    {
                        Matrix4f myTransposedMatrix = theBody.get<InverseGlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        break;
                    }
                default:
                    myParamValueFoundFlag = false;
                    break;
            }
            if (!myParamValueFoundFlag) {
                AC_WARNING << "no value for cg auto parameter " << curParam._myName << endl;
            }
        }
    }

    void
    CgProgramInfo::setCgMaterialParameter(CGparameter & theCgParameter, 
                                          const dom::Node & theNode,
                                          const MaterialBase & theMaterial)
    {
        const string & thePropertyName = theNode.getAttributeString("name");
        AC_DEBUG << "CgProgramInfo::setCgMaterialParameter: cgparam=" << theCgParameter << " node=" << theNode.nodeName() <<" property=" << thePropertyName << " material=" << theMaterial.get<NameTag>();

        switch(TypeId(asl::getEnumFromString(theNode.nodeName(), TypeIdStrings))) {
            case FLOAT:
                {
                    float myValue = theNode("#text").dom::Node::nodeValueAs<float>();
                    cgGLSetParameter1f(theCgParameter, myValue);
                    break;
                }
            case VECTOR2F:
                {
                    Vector2f myValueV = theNode("#text").dom::Node::nodeValueAs<Vector2f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter2fv(theCgParameter, myValue);
                    break;
                }
            case VECTOR3F:
                {
                    Vector3f myValueV = theNode("#text").dom::Node::nodeValueAs<Vector3f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter3fv(theCgParameter, myValue);
                    break;
                }
            case VECTOR4F:
                {
                    Vector4f myValueV = theNode("#text").dom::Node::nodeValueAs<Vector4f>();
                    float * myValue = myValueV.begin();
                    cgGLSetParameter4fv(theCgParameter, myValue);
                    break;
                }
            case VECTOR_OF_VECTOR2F:
                {
                    VectorOfVector2f myValueV = theNode("#text").dom::Node::nodeValueAs<VectorOfVector2f>();
                    float * myValue = myValueV.begin()->begin();
                    cgGLSetParameterArray2f(theCgParameter, 0, myValueV.size(), myValue);
                    break;
                }
            case VECTOR_OF_VECTOR4F:
                {
                    VectorOfVector4f myValueV = theNode("#text").dom::Node::nodeValueAs<VectorOfVector4f>();
                    float * myValue = myValueV.begin()->begin();
                    cgGLSetParameterArray4f(theCgParameter, 0, myValueV.size(), myValue);
                    break;
                }
            case SAMPLER2D:
            case SAMPLERCUBE:
                {
                    unsigned myTextureIndex = theNode("#text").dom::Node::nodeValueAs<unsigned>();
                    if (myTextureIndex  < theMaterial.getTextureCount()) {
                        const Texture & myTexture = theMaterial.getTexture(myTextureIndex);
                        //AC_DEBUG << "cgGLSetTextureParameter param=" << theCgParameter << " texid=" << myTexture.getId();
                        cgGLSetTextureParameter( theCgParameter, myTexture.getId());
                        DB(AC_TRACE << "cgGLSetTextureParameter: Texture index: "<< as_string(myTextureIndex)
                                << " , glid : " << myTexture.getId()
                                << " , property : " << thePropertyName
                                << " to parameter : "<< cgGetParameterName(theCgParameter) << endl;)
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

        assertCg("setCgMaterialParameter");
    }

    void
    CgProgramInfo::setCgVectorParameter(const CgProgramAutoParam & theParam,
                                        const asl::Vector3f & theValue)
    {
        switch (theParam._myType) {
            case CG_FLOAT3:
                cgGLSetParameter3fv(theParam._myParameter, theValue.begin());
                break;
            case CG_FLOAT4:
                {
                    Vector4f myValue4f(theValue[0], theValue[1], theValue[2], 1);
                    cgGLSetParameter4fv(theParam._myParameter, myValue4f.begin());
                }
                break;
            default:
                throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                        + " should be FLOAT3 or FLOAT4.", "CgProgramInfo::setCgVectorParameter()");
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgVectorParameter(const CgProgramAutoParam & theParam,
                                        const asl::Vector4f & theValue)
    {
        switch (theParam._myType) {
            case CG_FLOAT3:
                cgGLSetParameter3fv(theParam._myParameter, theValue.begin());
                break;
            case CG_FLOAT4:
                cgGLSetParameter4fv(theParam._myParameter, theValue.begin());
                break;
            default:
                throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                        + " should be FLOAT3 or FLOAT4.", "CgProgramInfo::setCgVectorParameter()");
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgMatrixParameter(const CgProgramAutoParam & theParam,
                                        const asl::Matrix4f & theValue)
    {
        if (theParam._myType != CG_FLOAT4x4) {
            throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                    + " should be FLOAT4x4.", "CgProgramInfo::setCgVectorParameter()");
        }
        cgGLSetMatrixParameterfc(theParam._myParameter, theValue.getData());
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgArrayVector3fParameter(const CgProgramAutoParam & theParam,
                                               const vector<asl::Vector3f> & theValue)
    {
        if (theParam._myType == CG_ARRAY) {
            int mySize = cgGetArraySize(theParam._myParameter, 0);
            AC_DEBUG << "setting array 3f size " << mySize;
            if (mySize) {
                cgGLSetParameterArray3f(theParam._myParameter, 0,
                        mySize, theValue.begin()->begin());
            }
        } else {
            throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                    + " should be FLOAT3[].", "CgProgramInfo::setCgArrayVector3fParameter()");
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgArrayVector4fParameter(const CgProgramAutoParam & theParam,
                                               const vector<asl::Vector4f> & theValue)
    {
        if (theParam._myType == CG_ARRAY) {
            int mySize = cgGetArraySize(theParam._myParameter, 0);
            if (mySize) {
                cgGLSetParameterArray4f(theParam._myParameter, 0,
                        mySize, theValue.begin()->begin());
            }
        } else {
            throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                    + " should be FLOAT4[].", "CgProgramInfo::setCgArrayVector4fParameter()");
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam, 
                                              const vector<asl::Vector3f> & theValue)
    {
        int mySize = cgGetArraySize(theParam._myParameter, 0);
        for(int i = 0; i < mySize; ++i) {
            CGparameter myElement = cgGetArrayParameter(theParam._myParameter, i);
            AC_DEBUG << "setting component " << i << " to " << theValue[i];
            cgSetParameter3f(myElement, theValue[i][0], theValue[i][1], theValue[i][2]);
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam, 
                                              const vector<asl::Vector4f> & theValue)
    {
        int mySize = cgGetArraySize(theParam._myParameter, 0);
        for(unsigned i = 0; i < mySize; ++i) {
            CGparameter myElement = cgGetArrayParameter(theParam._myParameter, i);
            AC_DEBUG << "setting component " << i << " to " << theValue[i];
            cgSetParameter4f(myElement, theValue[i][0], theValue[i][1], theValue[i][2], theValue[i][3]);
        }
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::bind() {
        //AC_DEBUG << "CgProgramInfo::bind " << _myShader._myEntryFunction;
        CHECK_OGL_ERROR;
        DB(AC_TRACE << "binding " << _myShader._myEntryFunction << ":" <<_myCgProgramID << endl;)
        cgGLBindProgram(_myCgProgramID);
        assertCg(string("CgProgramInfo::bind()") + _myPathName);
    }

    void
    CgProgramInfo::enableTextures() {
        //AC_DEBUG << "CgProgramInfo::enableTextures";
        for (unsigned i=0; i < _myTextureParams.size(); ++i) {

            GLenum myTexUnit = cgGLGetTextureEnum(_myTextureParams[i]._myParameter);
            glActiveTextureARB(myTexUnit);

            //AC_DEBUG << "CgProgramInfo::enableTextures paramName=" << _myTextureParams[i]._myParamName << " param=" << _myTextureParams[i]._myParameter << " unit=" << hex << myTexUnit << dec;
            cgGLEnableTextureParameter(_myTextureParams[i]._myParameter);
            glEnable(GL_TEXTURE_2D);
            CHECK_OGL_ERROR;
        }
    }

    void
    CgProgramInfo::disableTextures() {
        //AC_DEBUG << "CgProgramInfo::disableTextures";
        for (unsigned i=0; i < _myTextureParams.size(); ++i) {

            GLenum myTexUnit = cgGLGetTextureEnum(_myTextureParams[i]._myParameter);
            glActiveTextureARB(myTexUnit);

            //AC_DEBUG << "CgProgramInfo::disableTextures paramName=" << _myTextureParams[i]._myParamName << " param=" << _myTextureParams[i]._myParameter << " unit=" << hex << myTexUnit << dec;
            cgGLDisableTextureParameter(_myTextureParams[i]._myParameter);
            glDisable(GL_TEXTURE_2D);
            CHECK_OGL_ERROR;
        }
    }

    CGprofile
    CgProgramInfo::asCgProfile(const ShaderDescription & theShader) {
        CGprofile myResult;
        switch (theShader._myProfile) {
            case ARBVP1 :
                myResult = CG_PROFILE_ARBVP1;
                break;
            case ARBFP1 :
                myResult = CG_PROFILE_ARBFP1;
                break;
            case  VP30:
                myResult = CG_PROFILE_VP30;
                break;
            case  FP30:
                myResult = CG_PROFILE_FP30;
                break;
            case  VP20:
                myResult = CG_PROFILE_VP20;
                break;
            case  FP20:
                myResult = CG_PROFILE_FP20;
                break;
            default:
                throw RendererException(string("Unknown shaderprofile : ") +
                        getStringFromEnum(theShader._myProfile, ShaderProfileStrings),
                        "CgProgramInfo::asCgProfile()");
        };
        return myResult;
    }
}
