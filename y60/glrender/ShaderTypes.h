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

#if (CG_VERSION_NUM >= 3000)
#   define AC_NV_CG_30
#elif (CG_VERSION_NUM >= 2000)
#   define AC_NV_CG_20
#else
#   error "CG below 2.0 won't work!"
//#   ifdef WIN32
//#       define AC_STRINGIFY( L )                          #L
//#       define AC_MAKESTRING( M, L )                      M(L)
//#       define AC_STRINGIZE(X)                            AC_MAKESTRING( AC_STRINGIFY, X )
//#       define AC_SHOWWARNING                             __FILE__ "(" AC_STRINGIZE(__LINE__) "): warning: "
//#       pragma message(AC_SHOWWARNING"CG Version < 2, compiling without gp4 support")
//#   else
//#        warning "CG Version < 2, compiling without gp4 support"
//#   endif
#endif

namespace y60 {

    enum MaterialType { CG_MATERIAL, FIXED_FUNCTION_MATERIAL };
    enum ShaderType { NO_SHADER_TYPE, FRAGMENT_SHADER, VERTEX_SHADER };
    enum ShaderProfile {NO_PROFILE, GLSLV, GLSLF, ARBVP1, ARBFP1, VP20, FP20, VP30, FP30, VP40, FP40
#   ifdef AC_NV_CG_20
    ,
    GLSLC, GP4FP, GP4VP, GP4GP
#   endif
#   ifdef AC_NV_CG_30
    ,
    GLSLC, GP4FP, GP4VP, GP4GP, GLSLG, GP5FP, GP5VP, GP5GP, GP5TCP, GP5TEP
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
#       ifdef AC_NV_CG_30
        "glslc",
        "gp4fp",
        "gp4vp",
        "gp4gp",
        "glslg",
        "gp5fp",
        "gp5vp",
        "gp5gp",
        "gp5tcp",
        "gp5tep",
#       endif
        0
    };

} // namespace
#endif // _ac_renderer_Shader_types_h_
