#ifndef AC_Y60_SHADER_DESCRIPTION_INCLUDED
#define AC_Y60_SHADER_DESCRIPTION_INCLUDED

#include "y60_glrender_settings.h"

#include "ShaderTypes.h"

#include <y60/scene/MaterialParameter.h>

#include <string>
#include <vector>

namespace y60 {    
    
    struct ShaderDescription {
        ShaderDescription() : 
            _myType(NO_SHADER_TYPE),
            _myProgramIndex(-1)
            
        {}
        ShaderType                       _myType;
        std::string                      _myFilename;
        ShaderProfile                    _myProfile;
        y60::MaterialParameterVector     _myVertexParameters;
        int                              _myProgramIndex;
        std::string                      _myEntryFunction;
        std::vector<std::string>         _myCompilerArgs; 
        y60::VertexRegisterFlags         _myVertexRegisterFlags;
        y60::VectorOfString              _myPossibleProfileNames;
    };

#if 0
    struct ShaderDescription {
        ShaderDescription() : 
            _myType(NO_SHADER_TYPE),
            _myProgramIndex(-1),
            _myPreferedFile(-1),
            _myPreferedProfile(-1),
            _myPreferedCompilerArgs(-1)
        {}
        ShaderType                       _myType;

        VectorOfString                   _myFilenames;
        int                              _myPreferedFile;

        std::vector<ShaderProfile>       _myProfiles;
        int                              _myPreferedProfile;

        y60::MaterialParameterVector     _myVertexParameters;
        int                              _myProgramIndex;

        VectorOfString                   _myEntryFunctions;
        int                              _myPreferedEntryFunction;

        VectorOfVectorOfString           _myCompilerArgs; 
        int                              _myPreferedCompilerArgs;

        y60::VertexRegisterFlags         _myVertexRegisterFlags;
    };
#endif
} // namespace

#endif// AC_Y60_SHADER_DESCRIPTION_INCLUDED

