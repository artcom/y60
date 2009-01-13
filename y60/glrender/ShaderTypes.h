#ifndef _ac_renderer_Shader_types_h_
#define _ac_renderer_Shader_types_h_

#include "y60_glrender_settings.h"

#include <y60/base/NodeValueNames.h>
#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>

#include <string>
// Warning: There are several enum/string pairs in this file which need to
// stay consistent.


#define AC_NV_CG_20 // XXX: this define does not make any sense like this.

namespace y60 {

    enum MaterialType { CG_MATERIAL, FIXED_FUNCTION_MATERIAL };
    enum ShaderType { NO_SHADER_TYPE, FRAGMENT_SHADER, VERTEX_SHADER };
    enum ShaderProfile {NO_PROFILE, GLSLV, GLSLF, ARBVP1, ARBFP1, VP40, FP40, VP30, FP30, VP20, FP20
#ifdef AC_NV_CG_20
    ,
    GLSLC, GP4FP, GP4VP, GP4GP
#endif
    };

    static const char * ShaderProfileStrings[] = {
        "no-profile",
        "glslv",
        "glslf",
        "arbvp1",
        "arbfp1",
        "vp40",
        "fp40",
        "vp30",
        "fp30",
        "vp20",
        "fp20",
#ifdef AC_NV_CG_20
        "glslc",
        "gp4fp",
        "gp4vp",
        "gp4gp",
#endif
        0
    };

} // namespace
#endif // _ac_renderer_Shader_types_h_
