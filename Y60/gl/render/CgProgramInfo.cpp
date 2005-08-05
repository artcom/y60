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
////  Description: C++ representation of a Cg program

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

#define DB(x) // x

using namespace std;
using namespace asl;

namespace y60 {

    void
    assertCg(const std::string & theWhere) {
        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            throw RendererException(std::string("Cg error: ") + cgGetErrorString(myCgError),
                    theWhere);
        }
    }

    CgProgramInfo::CgProgramInfo(const ShaderDescription & myShader,
                                 const CGcontext theCgContext,
                                 const std::string & theShaderDir)
        : _myShader(myShader), _myContext(theCgContext)
    {
        assertCg("before constr");
        _myPathName = theShaderDir + myShader._myFilename;
        CGprofile myCgProfile = asCgProfile(myShader);
        // create null terminated array of null terminated strings
        vector<const char *> myC_StrArgs;
        for (int i=0; i < myShader._myCompilerArgs.size(); ++i) {
            AC_DEBUG << "Using arg " << myShader._myCompilerArgs[i].c_str() << endl;
            myC_StrArgs.push_back(myShader._myCompilerArgs[i].c_str());
        }
        myC_StrArgs.push_back(0);

        _myCgProgramID = cgCreateProgramFromFile(theCgContext, CG_SOURCE,
                                                            _myPathName.c_str(),
                                                            myCgProfile,
                                                            myShader._myEntryFunction.c_str(),
                                                            &(*myC_StrArgs.begin()));
        CGerror myCgError = cgGetError();
        if (myCgError != CG_NO_ERROR) {
            std::string myErrorMessage = "Cg error loading '" + myShader._myEntryFunction +
                   "' from " + _myPathName + ": " + cgGetErrorString(myCgError);
            if (myCgError == CG_COMPILER_ERROR) {
                myErrorMessage += "\n" + std::string(cgGetLastListing(theCgContext));
            }
            throw RendererException(myErrorMessage, "CgProgramInfo::CgProgramInfo()");
        }
        assertCg("CgProgramInfo::CgProgramInfo()");
        processParameters();
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
        // CGparameter myParam = cgGetFirstParameter(_myCgProgramID, CG_PROGRAM);
        // while (myParam != 0) {
        for (CGparameter myParam = cgGetFirstParameter(_myCgProgramID, CG_PROGRAM);
                myParam != 0;  myParam = cgGetNextParameter(myParam))
        {
            string myParamName(cgGetParameterName(myParam));
            CGenum myParamVariability = cgGetParameterVariability(myParam);
            CGtype myParameterType    = cgGetParameterType(myParam);

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
                if (myParameterType == CG_ARRAY || cgIsParameterReferenced(myParam)) {
                    AC_DEBUG << "adding auto param " << myParamName << ":" << myParamID << endl;
                    _myAutoParams.push_back(CgProgramAutoParam(myParamName, myParam, myParamID, myParameterType));
                } else {
                    AC_WARNING << "skipping non-referenced AC-CG parameter " << myParamName << endl;
                }
            }
            // scan for sampler params
            if (myParameterType == CG_SAMPLER1D
                    || myParameterType == CG_SAMPLER2D
                    || myParameterType == CG_SAMPLER3D
					|| myParameterType == CG_SAMPLERCUBE ) {
                _myTextureParams.push_back(CgProgramTextureParam(myParamName, myParam));
            }
            assertCg("CgProgramInfo::processParameters() - 2");
        }
        assertCg("CgProgramInfo::processParameters()");
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

    void
    CgProgramInfo::setCGGLParameters() {
        for (int i=0; i<_myGlParams.size(); ++i) {
            CgProgramGlParam curParam = _myGlParams[i];
            cgGLSetStateMatrixParameter(curParam._myParameter, curParam._myStateMatrixType,
                    curParam._myTransform);
            AC_TRACE << "setting CGGL parameter " << curParam._myParamName << " to " << curParam._myTransform << endl;
            assertCg("CgProgramInfo::setCGGLParameters()");
        }
    }

    void
    CgProgramInfo::setAutoParameters(
            const LightVector & theLightInstances,
            const Viewport & theViewport,
            const Body & theBody,
            const Camera & theCamera)
    {
        int myPositionalLightCount = 0;
        int myDirectionalLightCount = 0;
        std::vector<Vector3f> myPositionalLights(8,Vector3f(0,0,0));
        std::vector<Vector3f> myDirectionalLights(8,Vector3f(0,0,0));
        std::vector<Vector4f> myPositionalLightColors(8,Vector4f(0,0,0,1));
        std::vector<Vector4f> myDirectionalLightColors(8,Vector4f(0,0,0,1));
        Vector4f myAmbientLightColor(0,0,0,1);

        // Collect light information
        for (unsigned i = 0; i < theLightInstances.size(); ++i) {
            LightPtr myLight = theLightInstances[i];
            if ( ! myLight->get<VisibleTag>()) {
                continue;
            }
            LightSourcePtr myLightSource = myLight->getLightSource();
            switch (myLightSource->getType()) {
                case POSITIONAL :
                    myPositionalLights[myPositionalLightCount] =
                        myLight->get<GlobalMatrixTag>().getTranslation();
                    myPositionalLightColors[myPositionalLightCount] =
                        myLightSource->get<DiffuseTag>();
                    ++myPositionalLightCount;
                    break;
                case DIRECTIONAL :
                    {
                        const Vector4f & myTranslation = myLight->get<GlobalMatrixTag>().getRow(2);
                        myDirectionalLights[myDirectionalLightCount] = Vector3f(myTranslation[0],
                                            myTranslation[1],myTranslation[2]);
                        myDirectionalLightColors[myDirectionalLightCount] =
                                            myLightSource->get<DiffuseTag>();
                        ++myDirectionalLightCount;
                        break;
                    }
               case AMBIENT :
                    myAmbientLightColor = myLightSource->get<AmbientTag>();
                    break;
               default :
                   throw RendererException(string("Unknown Light Source type: '")+myLightSource->get<IdTag>()+"'",
                                PLUS_FILE_LINE);
            }
        }


        // set parameter values
        for (int i=0; i < _myAutoParams.size(); ++i) {
            bool myParamValueFoundFlag = false;

            CgProgramAutoParam curParam = _myAutoParams[i];

            switch (curParam._myID) {
                case CAMERA_POSITION :
                    setCgVectorParameter(curParam, theCamera.get<GlobalMatrixTag>().getTranslation());
                    myParamValueFoundFlag = true;
                    break;
                case POSITIONAL_LIGHTS :
                    setCgArrayVector3fParameter(curParam, myPositionalLights);
                    myParamValueFoundFlag = true;
                    break;
                case POSITIONAL_LIGHT_COLORS :
                    setCgArrayVector4fParameter(curParam, myPositionalLightColors);
                    myParamValueFoundFlag = true;
                    break;
                case DIRECTIONAL_LIGHTS :
                    setCgArrayVector3fParameter(curParam, myDirectionalLights);
                    myParamValueFoundFlag = true;
                    break;
                case DIRECTIONAL_LIGHT_COLORS :
                    setCgArrayVector4fParameter(curParam, myDirectionalLightColors);
                    myParamValueFoundFlag = true;
                    break;
                case AMBIENT_LIGHT_COLOR :
                    setCgVectorParameter(curParam, myAmbientLightColor);
                    myParamValueFoundFlag = true;
                    break;
                case CAMERA_I:
                    setCgMatrixParameter(curParam, theCamera.get<InverseGlobalMatrixTag>());
                    myParamValueFoundFlag = true;
                    break;
                case CAMERA_T:
                    {
                        Matrix4f myTransposedMatrix = theCamera.get<GlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        myParamValueFoundFlag = true;
                        break;
                    }
                case VIEWPROJECTION:
                    {
                        Matrix4f myMatrix = theCamera.get<InverseGlobalMatrixTag>();
                        myMatrix.postMultiply(theViewport.get<ProjectionMatrixTag>());
                        setCgMatrixParameter(curParam, myMatrix);
                        myParamValueFoundFlag = true;
                        break;
                    }
                case OBJECTWORLD:
                    setCgMatrixParameter(curParam, theBody.get<GlobalMatrixTag>());
                    myParamValueFoundFlag = true;
                    break;
                case OBJECTWORLD_I:
                    setCgMatrixParameter(curParam, theBody.get<InverseGlobalMatrixTag>());
                    myParamValueFoundFlag = true;
                    break;
                case OBJECTWORLD_T:
                    {
                        Matrix4f myTransposedMatrix = theBody.get<GlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        myParamValueFoundFlag = true;
                        break;
                    }
                case OBJECTWORLD_IT:
                    {
                        Matrix4f myTransposedMatrix = theBody.get<InverseGlobalMatrixTag>();
                        myTransposedMatrix.transpose();
                        setCgMatrixParameter(curParam, myTransposedMatrix);
                        myParamValueFoundFlag = true;
                        break;
                    }
            }
            if (!myParamValueFoundFlag) {
                AC_WARNING << "no value for cg auto parameter " << curParam._myName << endl;
            }
        }

    }

    void
    CgProgramInfo::setCgVectorParameter(const CgProgramAutoParam & theParam, const asl::Vector3f & theValue) {
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
    CgProgramInfo::setCgVectorParameter(const CgProgramAutoParam & theParam, const asl::Vector4f & theValue) {
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
    CgProgramInfo::setCgMatrixParameter(const CgProgramAutoParam & theParam, const asl::Matrix4f & theValue) {
        if (theParam._myType != CG_FLOAT4x4) {
            throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                        + " should be FLOAT4x4.", "CgProgramInfo::setCgVectorParameter()");
        }
        cgGLSetMatrixParameterfc(theParam._myParameter, theValue.getData());
        assertCg(string("setting auto parameter ") + theParam._myName);
    }

    void
    CgProgramInfo::setCgArrayVector3fParameter(const CgProgramAutoParam & theParam, const vector<asl::Vector3f> & theValue) {
        if (theParam._myType == CG_ARRAY) {
            int mySize = cgGetArraySize(theParam._myParameter, 0);
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
    CgProgramInfo::setCgArrayVector4fParameter(const CgProgramAutoParam & theParam, const vector<asl::Vector4f> & theValue) {
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
    CgProgramInfo::bind() {
        CHECK_OGL_ERROR;
        DB(AC_TRACE << "binding " << _myShader._myEntryFunction << ":" <<_myCgProgramID << endl;)
        cgGLBindProgram(_myCgProgramID);

        assertCg(string("CgProgramInfo::bind()") + _myPathName);
    }

    void
    CgProgramInfo::enableTextures() {
        for (int i=0; i < _myTextureParams.size(); ++i) {
            GLenum myTexUnit = cgGLGetTextureEnum(_myTextureParams[i]._myParameter);
            glActiveTextureARB(myTexUnit);
            DB(AC_TRACE << "enabling texture " << _myTextureParams[i]._myParamName << endl;);
            cgGLEnableTextureParameter(_myTextureParams[i]._myParameter);
            assertCg("CgProgramInfo::bind: enabling texture");
            glEnable(GL_TEXTURE_2D);
            CHECK_OGL_ERROR;
        }
    }

    void
    CgProgramInfo::disableTextures() {
        for (int i=0; i < _myTextureParams.size(); ++i) {
            GLenum myTexUnit = cgGLGetTextureEnum(_myTextureParams[i]._myParameter);
            glActiveTextureARB(myTexUnit);
            DB(AC_TRACE << "disabling texture " << _myTextureParams[i]._myParamName << endl;);
            cgGLDisableTextureParameter(_myTextureParams[i]._myParameter);
            assertCg("CgProgramInfo::disableProfile: disabling texture");
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

