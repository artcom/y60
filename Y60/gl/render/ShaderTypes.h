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

#ifndef _ac_renderer_Shader_types_h_
#define _ac_renderer_Shader_types_h_


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
