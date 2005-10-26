#ifndef AC_Y60_SHADER_DESCRIPTION_INCLUDED
#define AC_Y60_SHADER_DESCRIPTION_INCLUDED

#include "ShaderTypes.h"

#include <y60/MaterialParameter.h>

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
    };

} // namespace

#endif// AC_Y60_SHADER_DESCRIPTION_INCLUDED

