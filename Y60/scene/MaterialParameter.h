//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
    static const char * GLRegisterString[] = {
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

