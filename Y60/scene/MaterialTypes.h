#ifndef _ac_scene_Material_types_h_
#define _ac_scene_Material_types_h_

#include <y60/base/TypeTraits.h>
#include <y60/base/NodeValueNames.h>

namespace y60 {

    enum ParameterFunction { PASS_THROUGH, TANGENT };
    static const char * ParameterFunctionStrings[] =
    {
        VERTEXPARAM_PASS_THROUGH,
        VERTEXPARAM_TANGENT,
        0
    };
}

#endif
