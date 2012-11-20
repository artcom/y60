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
*/
//
//   $RCSfile: MaterialParameter.h,v $
//   $Author: christian $
//   $Revision: 1.7 $
//   $Date: 2004/11/15 10:05:49 $
//
//   Description:
//
//=============================================================================

#ifndef AC_Y60_MATERIAL_PARAMETER_INCLUDED
#define AC_Y60_MATERIAL_PARAMETER_INCLUDED

#include "y60_scene_settings.h"

#include "MaterialTypes.h"

#include <y60/base/VertexDataRoles.h>
#include <asl/base/Ptr.h>

#include <string>
#include <vector>
#include <bitset>

namespace y60 {

    enum GLRegister {
        POSITION_REGISTER,
        NORMAL_REGISTER,
        COLORS_REGISTER,
        TEXCOORD0_REGISTER,
        TEXCOORD1_REGISTER,
        TEXCOORD2_REGISTER,
        TEXCOORD3_REGISTER,
        TEXCOORD4_REGISTER,
        TEXCOORD5_REGISTER,
        TEXCOORD6_REGISTER,
        TEXCOORD7_REGISTER,
        MAX_REGISTER
    };

    // Gl register use the same names as roles to enable default mapping from role to register
    const char * const GLRegisterString[] = {
        POSITION_ROLE,
        NORMAL_ROLE,
        COLOR_ROLE,
        TEXCOORD0_ROLE,
        TEXCOORD1_ROLE,
        TEXCOORD2_ROLE,
        TEXCOORD3_ROLE,
        TEXCOORD4_ROLE,
        TEXCOORD5_ROLE,
        TEXCOORD6_ROLE,
        TEXCOORD7_ROLE,
        0
    };

    typedef std::bitset<MAX_REGISTER> VertexRegisterFlags;

    class MaterialParameter {
        public:
            MaterialParameter(const std::string & theName,
                              VertexDataRole theParameterRole,
                              GLRegister theRegister,
                              TypeId theParameterType,
                              ParameterFunction theFunction,
                              const std::string & theValue) :
                _myName(theName),
                _myParameterRole(theParameterRole),
                _myParameterType(theParameterType),
                _myFunction(theFunction),
                _myValue(theValue),
                _myRegister(theRegister)
            {}

            virtual ~MaterialParameter() {}

            const VertexDataRole getRole() const {
                return _myParameterRole;
            }
            const GLRegister getRegister() const {
                return _myRegister;
            }
            const std::string & getName() const {
                return _myName;
            }
            const TypeId getType() const {
                return _myParameterType;
            }
            const ParameterFunction getFunction() const {
                return _myFunction;
            }
            const std::string & getValue() const {
                return _myValue;
            }
        private:
            std::string          _myName;
            VertexDataRole  _myParameterRole;
            TypeId          _myParameterType;
            ParameterFunction    _myFunction;
            std::string          _myValue;
            GLRegister           _myRegister;
    };

    typedef std::vector<MaterialParameter> MaterialParameterVector;
    typedef asl::Ptr<MaterialParameterVector> MaterialParameterVectorPtr;
}
#endif //AC_Y60_MATERIAL_PARAMETER_INCLUDED

