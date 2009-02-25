#ifndef _ac_renderer_Shader_types_h_
#define _ac_renderer_Shader_types_h_

#include "y60_glrender_settings.h"

#include <y60/base/NodeValueNames.h>
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>
#include <Cg/cg.h>

#include <string>
// Warning: There are several enum/string pairs in this file which need to
// stay consistent.

#if (CG_VERSION_NUM >= 2000)
#    define AC_NV_CG_20
#else
#    ifdef WIN32
#        pragma warning("CG Version < 2, compiling without gp4 support")
#    else
#        warning "CG Version < 2, compiling without gp4 support" 
#    endif
#endif

namespace y60 {

    enum MaterialType { CG_MATERIAL, FIXED_FUNCTION_MATERIAL };
    enum ShaderType { NO_SHADER_TYPE, FRAGMENT_SHADER, VERTEX_SHADER };
    enum ShaderProfile {NO_PROFILE, GLSLV, GLSLF, ARBVP1, ARBFP1, VP20, FP20, VP30, FP30, VP40, FP40
#   ifdef AC_NV_CG_20
    ,
    GLSLC, GP4FP, GP4VP, GP4GP
#   endif
    };
    
    const char * const ShaderProfileStrings[] = {
        "no-profile",
        "glslv",
        "glslf",
        "arbvp1",
        "arbfp1",
        "vp20",
        "fp20",
        "vp30",
        "fp30",
        "vp40",
        "fp40",
#       ifdef AC_NV_CG_20
        "glslc",
        "gp4fp",
        "gp4vp",
        "gp4gp",
#       endif
        0
    };

} // namespace
#endif // _ac_renderer_Shader_types_h_
