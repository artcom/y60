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

namespace y60 {

#if (CG_VERSION_NUM < 2000)
#   error "CG below 2.0 won't work!"
#endif

    enum MaterialType { CG_MATERIAL, FIXED_FUNCTION_MATERIAL };
    enum ShaderType { NO_SHADER_TYPE, FRAGMENT_SHADER, VERTEX_SHADER };
    enum ShaderProfile {NO_PROFILE, GLSLV, GLSLF, ARBVP1, ARBFP1, VP20, FP20, VP30, FP30, VP40, FP40
                        ,GLSLC, GP4FP, GP4VP, GP4GP, GLSLG, GP5FP, GP5VP, GP5GP, GP5TCP, GP5TEP };


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
        0
    };

} // namespace
#endif // _ac_renderer_Shader_types_h_
