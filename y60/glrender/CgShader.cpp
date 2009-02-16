//=============================================================================
// Copyright (C) 2003 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

//own header
#include "CgShader.h"


#include <y60/glutil/GLUtils.h>
#include <y60/base/NodeNames.h>
#include <asl/zip/PackageManager.h>
#include <asl/base/os_functions.h>

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

    CgShader::CgShader(const dom::NodePtr theNode, 
                       const std::string & theVertexProfileName,
                       const std::string & theFragmentProfileName) : GLShader(theNode)
    {
        _myType = CG_MATERIAL;
        dom::NodePtr myShaderNode = theNode->childNode(VERTEX_SHADER_NODE_NAME);
        if (myShaderNode) {
            loadShaderProperties(myShaderNode, _myVertexShader, theVertexProfileName);
            loadParameters(myShaderNode, _myVertexShader);
            _myVertexShader._myType = VERTEX_SHADER;
        } else {
            AC_WARNING << "no vertex prog found for " << theNode->nodeName();
         }

        myShaderNode = theNode->childNode(FRAGMENT_SHADER_NODE_NAME);
        if (myShaderNode) {
            loadShaderProperties(myShaderNode, _myFragmentShader, theFragmentProfileName);
            _myFragmentShader._myType = FRAGMENT_SHADER;
        } else {
            AC_WARNING << "no fragment prog found for " << theNode->nodeName();
        }
    }

    CgShader::~CgShader()  {
    }

    void CgShader::compile(IShaderLibrary & theShaderLibrary) {
        ShaderLibrary * myShaderLibrary = dynamic_cast<ShaderLibrary *>(&theShaderLibrary);
        if (!myShaderLibrary) {
            throw ShaderException("CgShader::compile() - Dynamic cast of IShaderLibrary failed",
                                  PLUS_FILE_LINE);
        }
        assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());

        // compile fragment shader
        if (!_myFragmentProgram && _myFragmentShader._myType != NO_SHADER_TYPE
            ) {
            DB(AC_TRACE << "CgShader::compile(): Loading fragment shader from file '"
               << _myFragmentShader._myFilename << "'");
            _myFragmentProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myFragmentShader,
                                                                           myShaderLibrary->getCgContext()));
            assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());
        }

        // compile vertex shader
        if (!_myVertexProgram && _myVertexShader._myType != NO_SHADER_TYPE
) {
            DB(AC_TRACE << "CgShader::compile(): Loading vertex shader from file '"
               << _myVertexShader._myFilename << "'");
            _myVertexProgram = asl::Ptr<CgProgramInfo>(new CgProgramInfo(_myVertexShader,
                                                                         myShaderLibrary->getCgContext()));
            assertCg(PLUS_FILE_LINE, myShaderLibrary->getCgContext());
        }

        AC_DEBUG << "Successfully compiled '" << getName() << "'";
    }

    void CgShader::load(IShaderLibrary & theShaderLibrary) {
        MAKE_GL_SCOPE_TIMER(CgShader_load);
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

    bool CgShader::isLoaded() const {
        return (_myFragmentProgram != 0) || (_myVertexProgram != 0);
    }

    void CgShader::unload() {
        _myFragmentProgram = CgProgramInfoPtr(0);
        _myVertexProgram = CgProgramInfoPtr(0);
    }

    const MaterialParameterVector &
    CgShader::getVertexParameters() const {
        return _myVertexShader._myVertexParameters;
    }

    const VertexRegisterFlags &
    CgShader::getVertexRegisterFlags() const {
        return _myVertexShader._myVertexRegisterFlags;
    }
#if 0
    // converts a comma-delimted list of compiler args ("-DFOO, -DBar") to
    // a null-terminated array of null-terminated args.
    void
    CgShader::processCompilerArgs(std::vector<std::string> & theArgs, const string & theArgList) {
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
#endif

    void
    CgShader::loadShaderProperties(const dom::NodePtr theShaderNode,
                                   ShaderDescription & theShader,
                                   const std::string & theProfileName)
    {
        MAKE_GL_SCOPE_TIMER(CgShader_loadShaderProperties);
        GLShader::loadShaderProperties(theShaderNode, theShader);
        
        theShader._myPossibleProfileNames = theShaderNode->getAttributeValue<VectorOfString>(CG_PROFILES_PROPERTY);
        VectorOfString::size_type myProfileIndex = static_cast<VectorOfString::size_type>(-1);
        for (VectorOfString::size_type i=0; i<theShader._myPossibleProfileNames.size();++i) {
            if (theShader._myPossibleProfileNames[i] == theProfileName) {
                myProfileIndex = i;
                break;
            }
        }
        if ((myProfileIndex >= 0) && (myProfileIndex < theShader._myPossibleProfileNames.size())) {
        } else {
            // throw ShaderException(std::string("Engine profile does not match any given profile in shader description, library node =")+
            //                       asl::as_string(*theShaderNode), PLUS_FILE_LINE);
            AC_DEBUG << "Engine profile does not match any given profile in shader description, library node =" << *theShaderNode;
            theShader._myProfile = NO_PROFILE;
            return;
        }
        AC_DEBUG << "profile '" << theProfileName << "' has index "<< myProfileIndex << " in shader description"; 
        theShader._myProfile = ShaderProfile(asl::getEnumFromString(theProfileName, ShaderProfileStrings));

        // now select matching file parameters; either one file for all profiles or exactly one per profile are allowed
        const VectorOfString myFilenames = theShaderNode->getAttributeValue<VectorOfString>(CG_FILES_PROPERTY);
        VectorOfString::size_type myFileIndex = 0;
        if (theShader._myPossibleProfileNames.size() == myFilenames.size()) {
            myFileIndex = myProfileIndex;
        } else  {
            if (myFilenames.size() != 1) {
                throw ShaderException("bad number of program file names, must be one or match number of profiles", PLUS_FILE_LINE);
            }
        }

        if (myFilenames.size() > myFileIndex) {
            theShader._myFilename = AppPackageManager::get().getPtr()->searchFile(myFilenames[myFileIndex]);
            if (theShader._myFilename.empty()) {
                throw ShaderException(string("Could not find cg-shader '") + myFilenames[myFileIndex] + "' in " +
                                      AppPackageManager::get().getPtr()->getSearchPath(), PLUS_FILE_LINE);
            }
        } else {
            throw ShaderException("bad number of program file names", PLUS_FILE_LINE);
        }    
        
        // now select matching entry function; either one same entry for all profiles or exactly one per profile are allowed
        const VectorOfString myEntryFunctions = theShaderNode->getAttributeValue<VectorOfString>(CG_ENTRY_FUNCTIONS_PROPERTY);
        VectorOfString::size_type myEntryIndex = 0;
        if (theShader._myPossibleProfileNames.size() == myEntryFunctions.size()) {
            myEntryIndex = myProfileIndex;
        } else  {
            if (myEntryFunctions.size() != 1) {
                throw ShaderException("bad number of entry function names, must be one or match number of profiles", PLUS_FILE_LINE);
            }
        }

        if (myEntryFunctions.size() > myEntryIndex) {
            theShader._myEntryFunction = myEntryFunctions[myEntryIndex];
            if (theShader._myEntryFunction.empty()) {
                throw ShaderException("Empty entry function name provided in shader library", PLUS_FILE_LINE);
            }
        } else {
            throw ShaderException("bad number of entry function names", PLUS_FILE_LINE);
        }    
        

        // now select matching compiler args; either one same set for all profiles or exactly one set per profile are allowed
        if (theShaderNode->getAttribute(CG_COMPILERARGS2_PROPERTY)) {
            VectorOfVectorOfString myCompilerArgs = theShaderNode->getAttributeValue<VectorOfVectorOfString>(CG_COMPILERARGS2_PROPERTY);
            VectorOfVectorOfString::size_type myCompilerArgsIndex = 0;
            if (theShader._myPossibleProfileNames.size() == myCompilerArgs.size()) {
                myCompilerArgsIndex = myProfileIndex;
            } else  {
                if (myCompilerArgs.size() != 1) {
                    throw ShaderException("bad number of compiler arg sets, must be one or match number of profiles", PLUS_FILE_LINE);
                }
            }

            for (int i = 0; i < myCompilerArgs.size(); i++) {
                PackageList myPackages = AppPackageManager::get().getPtr()->getPackageList();
                PackageList::const_iterator it = myPackages.begin();
                while( it != myPackages.end() ) {
                    std::string myArg = " -I" + (*it++)->getPath();
                    myCompilerArgs[i].push_back( myArg );
                }
            }

            if (myCompilerArgs.size() > myCompilerArgsIndex) {
                theShader._myCompilerArgs = myCompilerArgs[myCompilerArgsIndex];
            } else {
                throw ShaderException("bad number of compiler arg sets", PLUS_FILE_LINE);
            }    
        }    
#if 0
        //old:
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
#endif
    }

    bool
    CgShader::hasShader(const ShaderType &theShadertype) const {
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
    CgShader::getShader(const ShaderType & theShadertype) const {
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
    CgShader::activate(MaterialBase & theMaterial, const Viewport & theViewport, const MaterialBase * theLastMaterial) {
        MAKE_GL_SCOPE_TIMER(CgShader_activate);
        GLShader::activate(theMaterial, theViewport, 0); // activate stipple && attenuation

        if (!theLastMaterial || theLastMaterial->getGroup1Hash() != theMaterial.getGroup1Hash()) {
            GLShader::activateGroup1(theMaterial, theViewport); // activate exotic properties, like linewidth, glow, etc.
        }

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
    CgShader::deactivate(const MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(CgShader_deactivate);

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
    CgShader::enableTextures(const MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(CgShader_enableTextures);
        GLShader::enableTextures(theMaterial);
        if (_myVertexProgram) {
            _myVertexProgram->enableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->enableTextures();
        }
    }

    void
    CgShader::disableTextures(const MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(CgShader_disableTextures);
        GLShader::disableTextures(theMaterial);
        if (_myVertexProgram) {
            _myVertexProgram->disableTextures();
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->disableTextures();
        }
    }

    void
    CgShader::bindBodyParams(const MaterialBase & theMaterial,
                             const Viewport & theViewport,
                             const LightVector & theLights,
                             const Body & theBody,
                             const Camera & theCamera)
    {
        MAKE_GL_SCOPE_TIMER(CgShader_bindBodyParams);
        GLShader::bindBodyParams(theMaterial, theViewport, theLights, theBody, theCamera);

        if (_myVertexProgram) {
            /*bool b =*/ _myVertexProgram->reloadIfRequired(theLights, theMaterial);
            _myVertexProgram->bindBodyParams(theMaterial, theLights, theViewport, theBody, theCamera);
            _myVertexProgram->bind();
        }
        if (_myFragmentProgram) {
            /*bool b =*/ _myFragmentProgram->reloadIfRequired(theLights, theMaterial);
            _myFragmentProgram->bindBodyParams(theMaterial, theLights, theViewport, theBody, theCamera);
            _myFragmentProgram->bind();
        }
    }

    void
    CgShader::bindMaterialParams(const MaterialBase & theMaterial) {
        if (_myVertexProgram) {
            _myVertexProgram->bindMaterialParams(theMaterial);
        }
        if (_myFragmentProgram) {
            _myFragmentProgram->bindMaterialParams(theMaterial);
        }
    }

    void
    CgShader::bindOverlayParams(const MaterialBase & theMaterial) {
        MAKE_GL_SCOPE_TIMER(CgShader_bindOverlayParams);
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
    CgShader::getMaxTextureUnits() const {
        if (_myFragmentProgram) {
            // Fragment shader can use much more texture units than fixed function shaders
            GLint myMaxTexUnits;
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
