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

#include <y60/glutil/GLUtils.h> // must be included first for <GL/glew.h>

//own header
#include "CgProgramInfo.h"
#include "GLShader.h"

#ifdef _WIN32
#    include <direct.h>
#endif

#include <asl/base/begin_end.h>
#include <y60/base/iostream_functions.h>
#include <asl/base/string_functions.h>
#include <asl/base/file_functions.h>
#include <asl/dom/Nodes.h>

#include <y60/base/NodeValueNames.h>

#include "Renderer.h"

// #ifdef _WIN32
// #   include <GL/glh_extensions.h>
// #   include <GL/glh_genext.h>
// #endif
// #ifdef LINUX
//     #include <GL/glext.h>
//     #include <GL/glx.h>
// #endif

#define DB(x) // x

// profiling
#ifdef PROFILING_LEVEL_NORMAL
#warning PROFILING_LEVEL_NORMAL is enabled
#define DBP(x)  x
#else
#define DBP(x) // x
#endif

// more profiling
//#define PROFILING_LEVEL_FULL
#ifdef PROFILING_LEVEL_FULL
#warning PROFILING_LEVEL_FULL is enabled
#define DBP2(x)  x
#else
#define DBP2(x) // x
#endif

using namespace std;
using namespace asl;
using namespace dom;

namespace y60 {

    void
    assertCg(const std::string & theWhere, const CGcontext theCgContext) {
        CGerror myCgError = cgGetError();
        if (myCgError == CG_COMPILER_ERROR && theCgContext) {
            throw RendererException(std::string("Cg compile error: ") + cgGetLastListing(theCgContext),
                                    theWhere);
        } else if (myCgError != CG_NO_ERROR) {
            throw RendererException(cgGetErrorString(myCgError), theWhere);
        }
    }

#ifdef DEBUG_VARIANT
#define ASSERTCG(where, what) assertCg(where, what)
#else
#define ASSERTCG(where, what)
#endif

    CgProgramInfo::CgProgramInfo(const ShaderDescription & myShader,
                                 const CGcontext theCgContext)
        : _myShader(myShader), _myContext(theCgContext), _myUsesLights(false)
    {
        _myPathName = myShader._myFilename;

        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS] = 0;
        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_DIFFUSE_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_SPECULAR_COLOR] = 0;

        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS] = 0;
        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_DIFFUSE_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_SPECULAR_COLOR] = 0;

        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_DIFFUSE_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_SPECULAR_COLOR] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_CUTOFF] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_EXPONENT] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_DIRECTION] = 0;
        _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_ATTENUATION] = 0;

        _myUnsizedArrayAutoParamSizes[TEXTURE_MATRICES] = 0; // bug in Cg prevents unsized arrays of type float4x4 of size 0 to compile

        //VS: 100 punkte
        cgSetAutoCompile(_myContext, CG_COMPILE_MANUAL);
        createAndCompileProgram();

        DB(AC_TRACE << "CgProgramInfo::CgProgramInfo(" << _myCgProgramID << ")" << endl;);
    }

    CgProgramInfo::~CgProgramInfo() {
        if (cgIsProgram(_myCgProgramID)) {
            if (cgGetProgramContext(_myCgProgramID) == _myContext) {
                DB(AC_TRACE << "CgProgramInfo::~CgProgramInfo(" << _myCgProgramID << ")" << endl;);
                cgDestroyProgram(_myCgProgramID);
            }
            assertCg("CgProgramInfo::~CgProgramInfo()", _myContext);
        }
    }

    // This should go away when the cg bug will be fixed that prevents vertex shaders to accept
    // unsized arrays of size 0; theArgs pointer will also become invalid after destruction of theArgsStrings, so take care
    void CgProgramInfo::appendUnsizedArrayBugWorkaroundCompilerArgs(std::vector<std::string> & theArgStrings, std::vector<const char*> & theArgs) const {
        for (std::map<int,int>::const_iterator it = _myUnsizedArrayAutoParamSizes.begin();
             it !=_myUnsizedArrayAutoParamSizes.end(); ++it)
        {
            const std::string myParamName = asl::getStringFromEnum(it->first, CgAutoParameterString);
            int myArrayLength = it->second;
            string myDefine = string("-D")+myParamName+"_LENGTH="+asl::as_string(myArrayLength);
            //string myDefine = string("-D")+myParamName+"_LENGTH=1";
            theArgStrings.push_back(myDefine);
        }
        for (unsigned i = 0; i < theArgStrings.size(); ++i) {
            theArgs.push_back(theArgStrings[i].c_str());
        }
    }

    void CgProgramInfo::createAndCompileProgram() {
        AC_DEBUG << "createAndCompileProgram '" << _myPathName.c_str() <<"', profile = " << ShaderProfileStrings[_myShader._myProfile];
        DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_createAndCompileProgram));

        if (_myCgProgramString.empty()) {
            AC_DEBUG << "loading from file";
            DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_loadFromFile));

            // create null terminated array of null terminated strings
            _myCachedCompilerArgs.resize(0);
            for (std::vector<std::string>::size_type i=0; i < _myShader._myCompilerArgs.size(); ++i) {
                AC_DEBUG << "Using arg '" << _myShader._myCompilerArgs[i].c_str() << "'" << endl;
                _myCachedCompilerArgs.push_back(_myShader._myCompilerArgs[i].c_str());
            }

            //_myCachedCompilerArgs.push_back(0);

            AC_DEBUG << "cgCreateProgramFromFile profile = " << ShaderProfileStrings[_myShader._myProfile] << ", entry= "<<_myShader._myEntryFunction;
            DBP2(START_TIMER(CgProgramInfo_cgCreateProgramFromFile));
            std::vector<string> myArgStrings;
            std::vector<const char *> myArgs = _myCachedCompilerArgs;
            appendUnsizedArrayBugWorkaroundCompilerArgs(myArgStrings, myArgs);
#if (CG_VERSION_NUM >= 2200)
            char const ** ppOptions = cgGLGetOptimalOptions(getCgProfile());
            if (ppOptions && *ppOptions) {
                while (*ppOptions) {
                    myArgs.push_back(*ppOptions);
                    ppOptions++;
                }
            }
#endif
            myArgs.push_back(0);
            AC_TRACE << "cgCreateProgramFromFile  args = " << myArgs;
            //AC_TRACE << "cgCreateProgram  unsized array fix args = " << myArgStrings;
            _myCgProgramID = cgCreateProgramFromFile(_myContext, CG_SOURCE,
                                                     _myPathName.c_str(),
                                                     getCgProfile(),
                                                     _myShader._myEntryFunction.c_str(),
                                                     asl::begin_ptr(myArgs));
            DBP2(STOP_TIMER(CgProgramInfo_cgCreateProgramFromFile));
            AC_TRACE << "cgCreateProgramFromFile created program id = "<<_myCgProgramID;

            assertCg(PLUS_FILE_LINE, _myContext);

            //DK: we cannot keep a compiled version
            //    because unsized arrays of size 0 are completely gone after compilation
            _myCgProgramString = cgGetProgramString(_myCgProgramID, CG_PROGRAM_SOURCE);
            _myCWD = getDirectoryPart(_myPathName);
        } else {
            AC_DEBUG << "destroying and reloading from string";
            DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_destroyReload));

            cgDestroyProgram(_myCgProgramID);
            std::string myCWD = getCWD();
            asl::changeDirectory(_myCWD.c_str());

            std::vector<string> myArgStrings;
            std::vector<const char *> myArgs = _myCachedCompilerArgs;
            appendUnsizedArrayBugWorkaroundCompilerArgs(myArgStrings, myArgs);
#if (CG_VERSION_NUM >= 2200)
            char const ** ppOptions = cgGLGetOptimalOptions(getCgProfile());
            if (ppOptions && *ppOptions) {
                while (*ppOptions) {
                    myArgs.push_back(*ppOptions);
                    ppOptions++;
                }
            }
#endif
            myArgs.push_back(0);
            //AC_TRACE << "cgCreateProgram  unsized array fix args = " << myArgStrings;
            AC_TRACE << "cgCreateProgram  args = " << myArgs;
            AC_DEBUG << "cgCreateProgram  profile = " << ShaderProfileStrings[_myShader._myProfile] << ", entry= "<<_myShader._myEntryFunction;
            _myCgProgramID = cgCreateProgram(_myContext, CG_SOURCE, _myCgProgramString.c_str(),
                                             getCgProfile(), _myShader._myEntryFunction.c_str(),
                                             asl::begin_ptr(myArgs));
            AC_TRACE << "cgCreateProgram created program id = "<<_myCgProgramID;

            asl::changeDirectory(myCWD.c_str());
            assertCg(PLUS_FILE_LINE, _myContext);
        }

        DBP2(START_TIMER(CgProgramInfo_processParameters));
        processParameters();
        DBP2(STOP_TIMER(CgProgramInfo_processParameters));

        DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_compileProgram));
        AC_TRACE << "cgCompileProgram("<<_myCgProgramID<<")";
        cgCompileProgram(_myCgProgramID);
        assertCg(PLUS_FILE_LINE, _myContext);

        updateTextureUnits();
    }

    void
    CgProgramInfo::load() {
        cgGLLoadProgram(_myCgProgramID);
        assertCg(PLUS_FILE_LINE, _myContext);
    }

    void
    CgProgramInfo::processParameters() {
        AC_TRACE << "processParameters -" << ShaderProfileStrings[_myShader._myProfile];

        _myGlParams.clear();
        _myAutoParams.clear();
        _myTextureParams.clear();
        _myMiscParams.clear();

        for (CGparameter myParam = cgGetFirstParameter(_myCgProgramID, CG_PROGRAM);
             myParam != 0;  myParam = cgGetNextParameter(myParam))
        {
            string myParamName(cgGetParameterName(myParam));
            //XXX cgGetParameterVariability crashes on unsized arrays
            //CGenum myParamVariability = cgGetParameterVariability(myParam);
            CGenum myParamVariability = CG_UNIFORM;
            CGtype myParameterType    = cgGetParameterType(myParam);
            AC_TRACE << "processing " << myParamName;

            if (myParamName.compare(0, 3, "GL_") == 0) { // scan for reserved GL_ prefix in uniform parameters
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
                _myGlParams.push_back(
                    CgProgramGlParam(myParamName, myParam, myParamType, myParamTransform));
            } else if (myParamName.compare(0, 3, "AC_") == 0) { // scan for reserved AC_ prefix
                if (myParamVariability != CG_UNIFORM) {
                    throw RendererException ("Error in " + _myPathName + ": Parameter " + myParamName
                                             + " not uniform.", "CgProgramInfo::processParameters()");
                }

                CgAutoParameterID myParamID = static_cast<CgAutoParameterID>(
                    asl::getEnumFromString(myParamName, CgAutoParameterString));

                if (myParamID >= MAX_AUTO_PARAMETER) {
                    throw RendererException ("Error in " + _myPathName +
                                             ": CG Auto Parameter " + myParamName + " unknown.",
                                             "CgProgramInfo::processParameters()");
                }

                if (myParameterType == CG_ARRAY) /* && cgGetArraySize(myParam, 0) == 0)*/ {
                    int myArraySize = _myUnsizedArrayAutoParamSizes[myParamID];
                    AC_TRACE << "setting array " << myParamName << " to size " << myArraySize;
#ifdef ALLOW_SIZED_ARRAYS_AND_UNSIZED_ARRAYS_AS_AUTOPARAMS
                    CGtype myArrayType = cgGetArrayType(myParam);
                    CGparameter myArray = cgCreateParameterArray(_myContext, myArrayType, myArraySize);
                    assertCg(PLUS_FILE_LINE, _myContext);

                    for(int i = 0; i <  myArraySize; ++i) {
                        CGparameter myArrayElement = cgCreateParameter(_myContext, myArrayType);
                        cgConnectParameter(myArrayElement, cgGetArrayParameter(myArray, i));
                    }

                    cgConnectParameter(myArray, myParam);
#else
                    cgSetArraySize(myParam, myArraySize);
#endif
                    assertCg(PLUS_FILE_LINE, _myContext);
                    AC_TRACE << "done. created unsized array of size " << myArraySize;
                }
                AC_TRACE << "adding auto param " << myParamName << ":" << myParamID << endl;
                _myAutoParams[myParamID] =
                    CgProgramAutoParam(myParamName, myParam, myParamID, myParameterType);
                    if (myParamID >= POSITIONAL_LIGHTS && myParamID <= SPOT_LIGHTS_ATTENUATION) {
                        _myUsesLights = true;
                    }
            } else if (myParameterType == CG_SAMPLER1D ||
                       // scan for sampler params
                       myParameterType == CG_SAMPLER2D ||
                       myParameterType == CG_SAMPLER3D ||
                       myParameterType == CG_SAMPLERCUBE )
            {
                _myTextureParams.push_back(CgTextureParam(myParamName, myParam));
            } else {
                _myMiscParams.push_back(CgProgramNamedParam(myParamName, myParam));
            }
            assertCg(PLUS_FILE_LINE, _myContext);
        }
    }

    void
    CgProgramInfo::enableProfile() {
        AC_TRACE << "enabled CgProfile(" << getCgProfile() << ") - " << ShaderProfileStrings[_myShader._myProfile];
        cgGLEnableProfile(getCgProfile());
        assertCg(PLUS_FILE_LINE, _myContext);
    }

    void
    CgProgramInfo::disableProfile() {
        AC_TRACE << "disabled CgProfile(" << getCgProfile() << ") - " << ShaderProfileStrings[_myShader._myProfile];
        cgGLDisableProfile(getCgProfile());
        assertCg(PLUS_FILE_LINE, _myContext);
    }

    void CgProgramInfo::setCGGLParameters() {
        AC_TRACE << "CgProgramInfo::setCGGLParameters";
        for (unsigned i=0; i<_myGlParams.size(); ++i) {
            CgProgramGlParam myParam = _myGlParams[i];
            AC_TRACE << "setting CgGL parameter " << myParam._myParamName << " param=" << myParam._myParameter << " type=" << myParam._myStateMatrixType << " transform=" << myParam._myTransform;
            cgGLSetStateMatrixParameter(myParam._myParameter,
                                        myParam._myStateMatrixType,
                                        myParam._myTransform);
            assertCg(PLUS_FILE_LINE, _myContext);
        }
    }

    bool
    CgProgramInfo::reloadIfRequired(
        const LightVector & theLightInstances,
        const MaterialBase & theMaterial)
    {
        DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_reloadIfRequired));
        AC_DEBUG << "reloadIfRequired - " << ShaderProfileStrings[_myShader._myProfile];

        bool myReload = false;
        unsigned myPositionalLightCount = 0;
        unsigned myDirectionalLightCount = 0;
        unsigned mySpotLightCount = 0;

        if (_myUsesLights) {
            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_count_lights));
            //look at the number of lights to see if reload is req.
            DBP2(COUNT_N(CgProgramInfo_reloadIfRequired_lights, theLightInstances.size()));
            for (unsigned i = 0; i < theLightInstances.size(); ++i) {
                LightPtr myLight = theLightInstances[i];
                if ( ! myLight->get<VisibleTag>()) {
                    continue;
                }
                LightSourcePtr myLightSource = myLight->getLightSource();
                switch (myLightSource->getType()) {
                case POSITIONAL:
                    ++myPositionalLightCount;
                    break;
                case DIRECTIONAL:
                    ++myDirectionalLightCount;
                    break;
                case SPOT:
                    ++mySpotLightCount;
                    break;
                case AMBIENT :
                    break;
                default :
                    AC_WARNING << "Unknown light type for " << myLightSource->get<IdTag>();
                }
            }
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_count_lights));


            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_pos_lights));
            if (myReload == false /* && _myAutoParams.find(POSITIONAL_LIGHTS) != _myAutoParams.end()*/) {
                unsigned myLastCount = _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS];
                if (myLastCount != myPositionalLightCount) {
                    DBP2(COUNT(CgProgramInfo_posLightCount_differ));
                    DBP2(COUNT_N(CgProgramInfo_posLightCountLast, myLastCount));
                    DBP2(COUNT_N(CgProgramInfo_posLightCountNew, myPositionalLightCount));
                    myReload = true;
                }
            }
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_pos_lights));

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_dir_lights));
            if (myReload == false  /* && _myAutoParams.find(DIRECTIONAL_LIGHTS) != _myAutoParams.end()*/) {
                unsigned myLastCount = _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS];
                if (myLastCount != myDirectionalLightCount)  {
                    DBP2(COUNT(CgProgramInfo_dirLightCount_differ));
                    DBP2(COUNT_N(CgProgramInfo_dirLightCountLast, myLastCount));
                    DBP2(COUNT_N(CgProgramInfo_dirLightCountNew, myDirectionalLightCount));
                    myReload = true;
                }
            }
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_dir_lights));

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_spot_lights));
            if (myReload == false /* && _myAutoParams.find(SPOT_LIGHTS) != _myAutoParams.end()*/) {
                unsigned myLastCount = _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS];
                if (myLastCount != mySpotLightCount)  {
                    DBP2(COUNT(CgProgramInfo_spotLightCount_differ));
                    DBP2(COUNT_N(CgProgramInfo_spotLightCountLast, myLastCount));
                    DBP2(COUNT_N(CgProgramInfo_spotLightCountNew, mySpotLightCount));
                    myReload = true;
                }
            }
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_spot_lights));
        }

        if (myReload == false /* && _myAutoParams.find(TEXTURE_MATRICES) != _myAutoParams.end() */) {
            unsigned myLastCount = _myUnsizedArrayAutoParamSizes[TEXTURE_MATRICES];
            if (myLastCount != theMaterial.getTextureUnitCount()) {
                AC_TRACE << "myLastCount="<< myLastCount;
                myReload = true;
            }
        }

        if (myReload) {
            //change unsized array sizes here
            AC_DEBUG << "Cg program reload required";

            DBP2(MAKE_GL_SCOPE_TIMER(CgProgramInfo_reloadIfRequired_reload));
            AC_DEBUG << "#of positional lights:" << myPositionalLightCount;
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS] = myPositionalLightCount;
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_DIFFUSE_COLOR] = myPositionalLightCount;
            _myUnsizedArrayAutoParamSizes[POSITIONAL_LIGHTS_SPECULAR_COLOR] = myPositionalLightCount;

            AC_DEBUG << "#of directional lights:" << myDirectionalLightCount;
            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS] = myDirectionalLightCount;
            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_DIFFUSE_COLOR] = myDirectionalLightCount;
            _myUnsizedArrayAutoParamSizes[DIRECTIONAL_LIGHTS_SPECULAR_COLOR] = myDirectionalLightCount;

            AC_DEBUG << "#of spot lights:" << mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_DIFFUSE_COLOR] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_SPECULAR_COLOR] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_CUTOFF] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_EXPONENT] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_DIRECTION] = mySpotLightCount;
            _myUnsizedArrayAutoParamSizes[SPOT_LIGHTS_ATTENUATION] = mySpotLightCount;

            AC_DEBUG << "#of texture matrices:" << theMaterial.getTextureUnitCount();
            _myUnsizedArrayAutoParamSizes[TEXTURE_MATRICES] = theMaterial.getTextureUnitCount();

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_reload_compile));
            createAndCompileProgram();
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_reload_compile));

            START_TIMER(CgProgramInfo_reloadIfRequired_reload_load);
            cgGLLoadProgram(_myCgProgramID);
            STOP_TIMER(CgProgramInfo_reloadIfRequired_reload_load);
            assertCg(PLUS_FILE_LINE, _myContext);

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_enableProfile));
            enableProfile();
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_enableProfile));

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_bindMaterialParams));
            bindMaterialParams(theMaterial);
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_bindMaterialParams));

            DBP2(START_TIMER(CgProgramInfo_reloadIfRequired_enableTextures));
            enableTextures(theMaterial);
            DBP2(STOP_TIMER(CgProgramInfo_reloadIfRequired_enableTextures));

            assertCg(PLUS_FILE_LINE, _myContext);
        }

        return myReload;
    }

    void
    CgProgramInfo::bindBodyParams(
        const MaterialBase & theMaterial,
        const LightVector & theLightInstances,
        const Viewport & theViewport,
        const Body & theBody,
        const Camera & theCamera)
    {
        AC_TRACE << "CgProgramInfo::bindBodyParams for shader filename=" << _myShader._myFilename << " entry=" << _myShader._myEntryFunction
                 << " body=" << theBody.get<NameTag>()<< " material=" << theMaterial.get<NameTag>()
                 << " camera=" << theCamera.get<NameTag>() << " viewport=" << theViewport.get<NameTag>();

        MAKE_GL_SCOPE_TIMER(CgProgramInfo_bindBodyParams);
        setCGGLParameters();

        std::vector<Vector3f> myPositionalLights;
        std::vector<Vector4f> myPositionalLightDiffuseColors;
        std::vector<Vector4f> myPositionalLightSpecularColors;

        std::vector<Vector3f> myDirectionalLights;
        std::vector<Vector4f> myDirectionalLightDiffuseColors;
        std::vector<Vector4f> myDirectionalLightSpecularColors;

        std::vector<Vector3f> mySpotLights;
        std::vector<Vector4f> mySpotLightDiffuseColors;
        std::vector<Vector4f> mySpotLightSpecularColors;
        std::vector<Vector3f> mySpotLightDirection;
        std::vector<float> mySpotLightCutoff;
        std::vector<float> mySpotLightExponent;
        std::vector<float> mySpotLightAttenuation;

        Vector4f myAmbientLightColor(0,0,0,1);

        // Collect light information
        for (unsigned i = 0; i < theLightInstances.size(); ++i) {
            LightPtr myLight = theLightInstances[i];
            if ( ! myLight->get<VisibleTag>()) {
                continue;
            }
            LightSourcePtr myLightSource = myLight->getLightSource();
            LightPropertiesFacadePtr myLightPropFacade = myLightSource->getChild<LightPropertiesTag>();
            AC_TRACE << "CgProgramInfo::bindBodyParams - Light#" << i << " source = "<< myLightSource->get<NameTag>();
            switch (myLightSource->getType()) {
            case POSITIONAL :
                myPositionalLights.push_back(myLight->get<GlobalMatrixTag>().getTranslation());
                myPositionalLightDiffuseColors.push_back(myLightPropFacade->get<LightDiffuseTag>());
                myPositionalLightSpecularColors.push_back(myLightPropFacade->get<LightSpecularTag>());
                break;
            case DIRECTIONAL :
            {
                const Vector4f & myDirection = myLight->get<GlobalMatrixTag>().getRow(2);
                myDirectionalLights.push_back(Vector3f(myDirection[0],
                                                       myDirection[1],myDirection[2]));
                myDirectionalLightDiffuseColors.push_back(myLightPropFacade->get<LightDiffuseTag>());
                myDirectionalLightSpecularColors.push_back(myLightPropFacade->get<LightSpecularTag>());
                break;
            }
            case AMBIENT :
                myAmbientLightColor = myLightPropFacade->get<LightAmbientTag>();
                break;
            case SPOT:
            {
                const Matrix4f & myGlobalMatrix = myLight->get<GlobalMatrixTag>();
                Vector4f myDirection = myGlobalMatrix.getRow(2);
                myDirection.mult(-1.0);
                mySpotLightDirection.push_back(normalized(Vector3f(myDirection[0],myDirection[1],myDirection[2])));
                mySpotLights.push_back(myGlobalMatrix.getTranslation());
                mySpotLightDiffuseColors.push_back(myLightPropFacade->get<LightDiffuseTag>());
                mySpotLightSpecularColors.push_back(myLightPropFacade->get<LightSpecularTag>());
                mySpotLightCutoff.push_back(myLightPropFacade->get<CutOffTag>());
                mySpotLightExponent.push_back(myLightPropFacade->get<ExponentTag>());
                mySpotLightAttenuation.push_back(myLightPropFacade->get<AttenuationTag>());
                break;
            }
            default :
                AC_WARNING << "unknown light type " << myLightSource->get<IdTag>();
            }
        }

        // set parameter values
        for (CgProgramAutoParams::iterator myIter = _myAutoParams.begin();
             myIter != _myAutoParams.end(); ++myIter)
        {
            bool myParamValueFoundFlag = true;

            const CgProgramAutoParam & curParam = myIter->second;
            AC_TRACE << "setting parameter " << curParam._myName;

            switch (curParam._myID) {
            case CAMERA_POSITION :
				{AC_TRACE << "setting CAMERA_POSITION to " << theCamera.get<GlobalMatrixTag>().getTranslation();}
                setCgVectorParameter(curParam, theCamera.get<GlobalMatrixTag>().getTranslation());
                break;
            case POSITIONAL_LIGHTS :
				{AC_TRACE << "setting POSITIONAL_LIGHTS to " << myPositionalLights;}
                setCgUnsizedArrayParameter(curParam, myPositionalLights);
                break;
            case POSITIONAL_LIGHTS_DIFFUSE_COLOR :
				{AC_TRACE << "setting POSITIONAL_LIGHTS_DIFFUSE_COLOR to " << myPositionalLightDiffuseColors;}
                setCgUnsizedArrayParameter(curParam, myPositionalLightDiffuseColors);
                break;
            case POSITIONAL_LIGHTS_SPECULAR_COLOR :
				{AC_TRACE << "setting POSITIONAL_LIGHTS_SPECULAR_COLOR to " << myPositionalLightSpecularColors;}
                setCgUnsizedArrayParameter(curParam, myPositionalLightSpecularColors);
                break;
            case DIRECTIONAL_LIGHTS :
				{AC_TRACE << "setting DIRECTIONAL_LIGHTS to " << myDirectionalLights;}
                setCgUnsizedArrayParameter(curParam, myDirectionalLights);
                break;
            case DIRECTIONAL_LIGHTS_DIFFUSE_COLOR :
				{AC_TRACE << "setting DIRECTIONAL_LIGHTS_DIFFUSE_COLOR to " << myDirectionalLightDiffuseColors;}
                setCgUnsizedArrayParameter(curParam, myDirectionalLightDiffuseColors);
                break;
            case DIRECTIONAL_LIGHTS_SPECULAR_COLOR :
				{AC_TRACE << "setting DIRECTIONAL_LIGHTS_SPECULAR_COLOR to " << myDirectionalLightSpecularColors;}
                setCgUnsizedArrayParameter(curParam, myDirectionalLightSpecularColors);
                break;
            case SPOT_LIGHTS :
				{AC_TRACE << "setting SPOT_LIGHTS to " << mySpotLights;}
                setCgUnsizedArrayParameter(curParam, mySpotLights);
                break;
            case SPOT_LIGHTS_DIFFUSE_COLOR :
				{AC_TRACE << "setting SPOT_LIGHTS_DIFFUSE_COLOR to " << mySpotLightDiffuseColors;}
                setCgUnsizedArrayParameter(curParam, mySpotLightDiffuseColors);
                break;
            case SPOT_LIGHTS_SPECULAR_COLOR :
				{AC_TRACE << "setting SPOT_LIGHTS_SPECULAR_COLOR to " << mySpotLightSpecularColors;}
                setCgUnsizedArrayParameter(curParam, mySpotLightSpecularColors);
                break;
            case SPOT_LIGHTS_EXPONENT :
				{AC_TRACE << "setting SPOT_LIGHTS_EXPONENT to " << mySpotLightExponent;}
                setCgUnsizedArrayParameter(curParam, mySpotLightExponent);
                break;
            case SPOT_LIGHTS_CUTOFF :
				{AC_TRACE << "setting SPOT_LIGHTS_CUTOFF to " << mySpotLightCutoff;}
                setCgUnsizedArrayParameter(curParam, mySpotLightCutoff);
                break;
            case SPOT_LIGHTS_DIRECTION :
				{AC_TRACE << "setting SPOT_LIGHTS_DIRECTION to " << mySpotLightDirection;}
                setCgUnsizedArrayParameter(curParam, mySpotLightDirection);
                break;
            case SPOT_LIGHTS_ATTENUATION :
				{AC_TRACE << "setting SPOT_LIGHTS_ATTENUATION to " << mySpotLightAttenuation;}
                setCgUnsizedArrayParameter(curParam, mySpotLightAttenuation);
                break;
            case AMBIENT_LIGHT_COLOR :
				{AC_TRACE << "setting AMBIENT_LIGHT_COLOR to " << myAmbientLightColor;}
                setCgVectorParameter(curParam, myAmbientLightColor);
                break;
            case CAMERA_I:
				{AC_TRACE << "setting CAMERA_I to " << theCamera.get<InverseGlobalMatrixTag>();}
                setCgMatrixParameter(curParam, theCamera.get<InverseGlobalMatrixTag>());
                break;
            case CAMERA_T:
            {
                Matrix4f myTransposedMatrix = theCamera.get<GlobalMatrixTag>();
                myTransposedMatrix.transpose();
                {AC_TRACE << "setting CAMERA_T to " << myTransposedMatrix;}
                setCgMatrixParameter(curParam, myTransposedMatrix);
                break;
            }
            case VIEWPROJECTION:
            {
                Matrix4f myMatrix = theCamera.get<InverseGlobalMatrixTag>();
                //myMatrix.postMultiply(theViewport.get<ProjectionMatrixTag>()); XXX
                Matrix4f myProjectionMatrix;
                theCamera.get<FrustumTag>().getProjectionMatrix( myProjectionMatrix );
                myMatrix.postMultiply(myProjectionMatrix);
                {AC_TRACE << "setting VIEWPROJECTION to " << myMatrix;}
                setCgMatrixParameter(curParam, myMatrix);
                break;
            }
            case OBJECTWORLD:
				{AC_TRACE << "setting OBJECTWORLD to " << theBody.get<GlobalMatrixTag>();}
                setCgMatrixParameter(curParam, theBody.get<GlobalMatrixTag>());
                break;
            case OBJECTWORLD_I:
				{AC_TRACE << "setting OBJECTWORLD_I to " << theBody.get<InverseGlobalMatrixTag>();}
                setCgMatrixParameter(curParam, theBody.get<InverseGlobalMatrixTag>());
                break;
            case OBJECTWORLD_T:
            {
                Matrix4f myTransposedMatrix = theBody.get<GlobalMatrixTag>();
                myTransposedMatrix.transpose();
                {AC_TRACE << "setting OBJECTWORLD_T to " << myTransposedMatrix;}
                setCgMatrixParameter(curParam, myTransposedMatrix);
                break;
            }
            case OBJECTWORLD_IT:
            {
                Matrix4f myTransposedMatrix = theBody.get<InverseGlobalMatrixTag>();
                myTransposedMatrix.transpose();
                {AC_TRACE << "setting OBJECTWORLD_T to " << myTransposedMatrix;}
                setCgMatrixParameter(curParam, myTransposedMatrix);
                break;
            }
            case TEXTURE_MATRICES:
            {
                {AC_TRACE << "setting TEXTURE_MATRICES";}
                unsigned mySize = cgGetArraySize(curParam._myParameter, 0);
                for (unsigned i = 0; i < mySize; ++i) {
                    CGparameter myParam = cgGetArrayParameter(curParam._myParameter, i);
                    const TextureUnit & myTextureUnit = theMaterial.getTextureUnit(i);
                    const y60::TexturePtr & myTexture = myTextureUnit.getTexture();
                    const y60::ImagePtr & myImage = myTexture->getImage();
                    asl::Matrix4f myMatrix;
                    if (myImage) {
                        myMatrix = myImage->get<ImageMatrixTag>();
                        myMatrix.postMultiply(myTexture->get<TextureNPOTMatrixTag>());
                    } else {
                        myMatrix  = myTexture->get<TextureNPOTMatrixTag>();
                    }
                    myMatrix.postMultiply(myTexture->get<TextureMatrixTag>());
                    myMatrix.postMultiply(myTextureUnit.get<TextureUnitMatrixTag>());
                    {AC_TRACE << "setting texture matrix << " << i << " param=" << myParam << " to " << myMatrix;}
                    cgGLSetMatrixParameterfc(myParam, myMatrix.getData());
                }
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
    CgProgramInfo::bindOverlayParams() {
        setCGGLParameters();

        for (CgProgramAutoParams::iterator myIter = _myAutoParams.begin();
             myIter != _myAutoParams.end(); ++myIter)
        {
            CgProgramAutoParam curParam = myIter->second;
            AC_WARNING << "Cg parameter " << curParam._myName << " is not supported for overlays, yet.";
        }
    }

    void
    CgProgramInfo::bindMaterialParams(const MaterialBase & theMaterial) {
        AC_TRACE << "CgProgramInfo::bindMaterialParams shader filename=" << _myShader._myFilename << " entry=" << _myShader._myEntryFunction << " material=" << theMaterial.get<NameTag>();
        const MaterialPropertiesFacadePtr myPropFacade = theMaterial.getChild<MaterialPropertiesTag>();
        const Facade::PropertyMap & myProperties = myPropFacade->getProperties();
        for (unsigned i=0; i < _myMiscParams.size(); ++i) {
            Facade::PropertyMap::const_iterator myIter = myProperties.find(_myMiscParams[i]._myParamName);

            if (myIter != myProperties.end()) {

                if (myIter->second == 0){
                    AC_ERROR << "material property "
                             << _myMiscParams[i]._myParamName
                             << " not defined!";
                    return;
                }

                setCgMaterialParameter(_myMiscParams[i]._myParameter, *(myIter->second),
                                       _myMiscParams[i]._myParamName, theMaterial);
            }
        }
        for (unsigned i=0; i < _myTextureParams.size(); ++i) {
            Facade::PropertyMap::const_iterator myIter = myProperties.find(_myTextureParams[i]._myParamName);
            if (myIter != myProperties.end()) {

                setCgMaterialParameter(_myTextureParams[i]._myParameter, *(myIter->second),
                                       _myTextureParams[i]._myParamName, theMaterial);
            }
        }
    }

    void
    CgProgramInfo::setCgMaterialParameter(CGparameter & theCgParameter,
                                          const dom::Node & theNode,
                                          const std::string & thePropertyName,
                                          const MaterialBase & theMaterial)
    {
        AC_TRACE << "CgProgramInfo::setCgMaterialParameter: cgparam=" << theCgParameter << " node=" << theNode.nodeName() << " value=" << theNode.nodeValue() << " property=" << thePropertyName << " material=" << theMaterial.get<NameTag>() << " type=" << theNode.parentNode()->nodeName();
        TypeId myType;
        myType.fromString(theNode.parentNode()->nodeName());
        switch(myType) {
        case INT:
        case FLOAT:
        {
            float myValue = theNode.nodeValueAs<float>();
            cgGLSetParameter1f(theCgParameter, myValue);
            break;
        }
        case DOUBLE:
        {
            double myValue = theNode.nodeValueAs<double>();
            cgGLSetParameter1d(theCgParameter, myValue);
            break;
        }
        case VECTOR2F:
        {
            Vector2f myValueV = theNode.nodeValueAs<Vector2f>();
            float * myValue = myValueV.begin();
            cgGLSetParameter2fv(theCgParameter, myValue);
            break;
        }
        case VECTOR3F:
        {
            Vector3f myValueV = theNode.nodeValueAs<Vector3f>();
            float * myValue = myValueV.begin();
            cgGLSetParameter3fv(theCgParameter, myValue);
            break;
        }
        case VECTOR4F:
        {
            Vector4f myValueV = theNode.nodeValueAs<Vector4f>();
            float * myValue = myValueV.begin();
            cgGLSetParameter4fv(theCgParameter, myValue);
            break;
        }
        case VECTOR_OF_FLOAT:
        {
            VectorOfFloat myValueV = theNode.nodeValueAs<VectorOfFloat>();
            float * myValue = &myValueV[0];
            cgGLSetParameterArray1f(theCgParameter, 0, myValueV.size(), myValue);
            break;
        }
        case VECTOR_OF_VECTOR2F:
        {
            VectorOfVector2f myValueV = theNode.nodeValueAs<VectorOfVector2f>();
            float * myValue = myValueV.begin()->begin();
            cgGLSetParameterArray2f(theCgParameter, 0, myValueV.size(), myValue);
            break;
        }
        case VECTOR_OF_VECTOR4F:
        {
            VectorOfVector4f myValueV = theNode.nodeValueAs<VectorOfVector4f>();
            float * myValue = myValueV.begin()->begin();
            cgGLSetParameterArray4f(theCgParameter, 0, myValueV.size(), myValue);
            break;
        }
        case MATRIX4F:
        {
            Matrix4f myMatrix = theNode.nodeValueAs<Matrix4f>();
            cgGLSetMatrixParameterfc(theCgParameter, myMatrix.getData());
            break;
        }
        case SAMPLER2D:
        case SAMPLER3D:
        case SAMPLERCUBE:
        {
            unsigned myTextureIndex = theNode.nodeValueAs<unsigned>();
            if (myTextureIndex < theMaterial.getTextureUnitCount()) {
                const TextureUnit & myTextureUnit = theMaterial.getTextureUnit(myTextureIndex);
                TexturePtr myTexture = myTextureUnit.getTexture();
                unsigned myTextureId = myTexture->applyTexture();
                AC_TRACE << "cgGLSetTextureParameter param=" << theCgParameter << " texid=" << myTextureId;
                cgGLSetTextureParameter( theCgParameter, myTextureId);
                    AC_TRACE << "cgGLSetTextureParameter: Texture index " << as_string(myTextureIndex)
                   << ", texid=" << myTextureId << ", property=" << thePropertyName
                       << " to parameter : "<< cgGetParameterName(theCgParameter) << endl;
            } else {
                throw ShaderException(string("Texture index ") + as_string(myTextureIndex) +
                                      " not found. Material id=" + theMaterial.get<IdTag>() + " name=" + theMaterial.get<NameTag>() + " has " + as_string(theMaterial.getTextureUnitCount()) + " texture(s)",
                                      "CgProgramInfo::setCgMaterialParameter()");
            }
            break;
        }
        default:
            throw ShaderException(string("Unknown CgParameter type in property '")+thePropertyName+"'",
                                  "CgProgramInfo::setCgMaterialParameter()");
        }

        assertCg(PLUS_FILE_LINE, 0);
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
        assertCg(string("setCgVectorParameter ") + theParam._myName, _myContext);
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
        assertCg(string("setCgVectorParameter ") + theParam._myName, _myContext);
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
        assertCg(string("setCgMatrixParameter ") + theParam._myName, _myContext);
    }

    void
    CgProgramInfo::setCgArrayVector3fParameter(const CgProgramAutoParam & theParam,
                                               const vector<asl::Vector3f> & theValue)
    {
        if (theParam._myType == CG_ARRAY) {
            int mySize = cgGetArraySize(theParam._myParameter, 0);
            AC_TRACE << "setting array 3f size " << mySize;
            if (mySize) {
                cgGLSetParameterArray3f(theParam._myParameter, 0,
                                        mySize, theValue.begin()->begin());
            }
        } else {
            throw RendererException ("Error in " + _myPathName + ": Parameter " + theParam._myName
                                     + " should be FLOAT3[].", "CgProgramInfo::setCgArrayVector3fParameter()");
        }
        assertCg(string("setCgArrayVector3fParameter ") + theParam._myName, _myContext);
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
        assertCg(string("setCgArrayVector4fParameter ") + theParam._myName, _myContext);
    }

    void
    CgProgramInfo::setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam,
                                              const vector<float> & theValue)
    {
        int mySize = _myUnsizedArrayAutoParamSizes[theParam._myID];
        // int mySize = cgGetArraySize(theParam._myParameter, 0);
        if (mySize != static_cast<int>(theValue.size())) {
            AC_ERROR << "BUG 391: Cg Array " << theParam._myName << " expects " << mySize <<
                " elements, have " << theValue.size() << "(" << _myPathName << ")";
        }
        for(int i = 0; i < mySize; ++i) {
            CGparameter myElement = cgGetArrayParameter(theParam._myParameter, i);
            //AC_TRACE << "setting component " << i << " to " << theValue[i];
            if (i < static_cast<int>(theValue.size())) {
                cgSetParameter1f(myElement, theValue[i]);
            }
        }
        assertCg(string("setCgUnsizedArrayParameter ") + theParam._myName, _myContext);
    }

    void
    CgProgramInfo::setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam,
                                              const vector<asl::Vector3f> & theValue)
    {
        int mySize = _myUnsizedArrayAutoParamSizes[theParam._myID];
        // int mySize = cgGetArraySize(theParam._myParameter, 0);
        if (mySize != static_cast<int>(theValue.size())) {
            AC_ERROR << "BUG 391: Cg Array " << theParam._myName << " expects " << mySize <<
                " elements, have " << theValue.size() << "(" << _myPathName << ")";
        }
        for(int i = 0; i < mySize; ++i) {
            CGparameter myElement = cgGetArrayParameter(theParam._myParameter, i);
            AC_TRACE << "setting component " << i << " to " << theValue[i];
            if (i < static_cast<int>(theValue.size())) {
                cgSetParameter3f(myElement, theValue[i][0], theValue[i][1], theValue[i][2]);
            }
        }
        assertCg(string("setCgUnsizedArrayParameter ") + theParam._myName, _myContext);
    }

    void
    CgProgramInfo::setCgUnsizedArrayParameter(const CgProgramAutoParam & theParam,
                                              const vector<asl::Vector4f> & theValue)
    {
        int mySize = _myUnsizedArrayAutoParamSizes[theParam._myID];
        // int mySize = cgGetArraySize(theParam._myParameter, 0);
        if (mySize != static_cast<int>(theValue.size())) {
            AC_ERROR << "BUG 391: Cg Array " << theParam._myName << " expects " << mySize <<
                " elements, have " << theValue.size();
        }
        for(int i = 0; i < mySize; ++i) {
            CGparameter myElement = cgGetArrayParameter(theParam._myParameter, i);
            //AC_TRACE << "setting component " << i << " to " << theValue[i];
            cgSetParameter4f(myElement, theValue[i][0], theValue[i][1], theValue[i][2], theValue[i][3]);
        }
        assertCg(string("setCgUnsizedArrayParameter ") + theParam._myName, _myContext);
    }

    void
    CgProgramInfo::bind() {
        AC_TRACE << "CgProgramInfo::bind " << _myShader._myEntryFunction;
        CHECK_OGL_ERROR;
        AC_TRACE << "binding " << _myShader._myEntryFunction << ":" <<_myCgProgramID << endl;
            cgGLBindProgram(_myCgProgramID);
        assertCg(string("CgProgramInfo::bind() ") + _myPathName, _myContext);
    }

    void
    CgProgramInfo::enableTextures(const y60::MaterialBase & theMaterial) {
        AC_TRACE << "CgProgramInfo::enableTextures - " << ShaderProfileStrings[_myShader._myProfile];

        if (_myTextureParams.size() == 0) {
            return;
        }
        asl::Unsigned64 myFrameNumber = theMaterial.get<LastActiveFrameTag>();
        glMatrixMode(GL_TEXTURE);
        bool alreadyHasSpriteTexture = false;
        for (unsigned int i = 0; i < _myTextureParams.size(); ++i) {
            if (_myTextureParams[i].isUsedByShader()) {
                setGLTextureState(true, _myTextureParams[i]);
                cgGLEnableTextureParameter(_myTextureParams[i]._myParameter);
                if (i >= theMaterial.getTextureUnitCount()) {
                    throw ShaderException(std::string("Texture index ") + as_string(i) +
                                          " not found. Material id=" + theMaterial.get<IdTag>() + 
                                          " name=" + theMaterial.get<NameTag>() + " has " + as_string(theMaterial.getTextureUnitCount()) + " texture(s)",
                                          PLUS_FILE_LINE);
                }
                const TextureUnit & myTextureUnit = theMaterial.getTextureUnit(i);
                setTextureParameters(myTextureUnit, alreadyHasSpriteTexture, myFrameNumber);
            }
        }
        glMatrixMode(GL_MODELVIEW);
        assertCg(string("CgProgramInfo::enableTextures() ") + _myPathName, _myContext);
    }

    void
    CgProgramInfo::setGLTextureState(bool theEnableFlag, const CgTextureParam & theParameter) {

        GLenum myTextureTarget = GL_TEXTURE_1D;
        switch (cgGetParameterType(theParameter._myParameter)) {
            case CG_SAMPLER1D:
                myTextureTarget = GL_TEXTURE_1D;
                break;
            case CG_SAMPLER2D:
                myTextureTarget = GL_TEXTURE_2D;
                break;
            case CG_SAMPLER3D:
                myTextureTarget = GL_TEXTURE_3D;
                break;
            case CG_SAMPLERCUBE:
                myTextureTarget = GL_TEXTURE_CUBE_MAP_ARB;
                break;
            case CG_SAMPLERRECT:
                myTextureTarget = GL_TEXTURE_RECTANGLE_ARB;
                break;
            default:
                throw ShaderException(std::string("unknown texture type in Material id=") + theParameter._myParamName, PLUS_FILE_LINE);
                break;
        };
        glActiveTexture(theParameter.getTextureUnit());
        if (theEnableFlag) {
            glEnable(myTextureTarget);
        } else {
            glDisable(myTextureTarget);
        }
    }

    void
    CgProgramInfo::disableTextures() {
        AC_TRACE << "CgProgramInfo::disableTextures - " << ShaderProfileStrings[_myShader._myProfile];
        for (unsigned i=0; i < _myTextureParams.size(); ++i) {
            if (_myTextureParams[i].isUsedByShader()) {
                cgGLDisableTextureParameter(_myTextureParams[i]._myParameter);
                setGLTextureState(false, _myTextureParams[i]);
            }
        }
        glDisable(GL_POINT_SPRITE_ARB);
    }


    void
    CgProgramInfo::updateTextureUnits() {
        for (unsigned i=0; i < _myTextureParams.size(); ++i) {
            _myTextureParams[i].updateTextureUnit(_myShader._myFilename);
        }
    }

    CGprofile
    CgProgramInfo::getCgProfile() const {
        CGprofile myResult;
        switch (_myShader._myProfile) {
        case ARBVP1 :
            myResult = CG_PROFILE_ARBVP1;
            break;
        case ARBFP1 :
            myResult = CG_PROFILE_ARBFP1;
            break;
        case  VP40:
            myResult = CG_PROFILE_VP40;
            break;
        case  FP40:
            myResult = CG_PROFILE_FP40;
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
#if (CG_VERSION_NUM >= 2000)
        case GLSLV:
            myResult = CG_PROFILE_GLSLV;
            break;
        case GLSLF:
            myResult = CG_PROFILE_GLSLF;
            break;
#   if (CG_VERSION_NUM >= 3000)
        case GP5VP:
            myResult = CG_PROFILE_GP5VP;
            break;
        case GP5FP:
            myResult = CG_PROFILE_GP5FP;
            break;
        case GP4VP:
            myResult = CG_PROFILE_GP4VP;
            break;
        case GP4FP:
            myResult = CG_PROFILE_GP4FP;
            break;
#   else
        case GP4VP:
            myResult = CG_PROFILE_GPU_VP;
            break;
        case GP4FP:
            myResult = CG_PROFILE_GPU_FP;
            break;
#   endif
#else
#   error "CG below 2.0 won't work!"
#endif
        default:
            throw RendererException(string("Unsupported shaderprofile : ") +
                                    getStringFromEnum(_myShader._myProfile, ShaderProfileStrings),
                                    PLUS_FILE_LINE);
        };
        return myResult;
    }
}
