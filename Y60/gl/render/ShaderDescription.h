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

#ifndef AC_Y60_SHADER_DESCRIPTION_INCLUDED
#define AC_Y60_SHADER_DESCRIPTION_INCLUDED

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

