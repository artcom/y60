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
//   $Id: VertexDataRoles.h,v 1.1 2005/03/24 23:35:56 christian Exp $
//   $RCSfile: VertexDataRoles.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:35:56 $
//
//
//  Description: This file contains all the wellknown names for the
//               SOM
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef _ac_y60_VertexDataRoles_h_
#define _ac_y60_VertexDataRoles_h_

#include "y60_base_settings.h"

#include "NodeNames.h"

namespace y60 {
    enum VertexDataRole {
        POSITIONS,
        NORMALS,
        COLORS,
        TEXCOORD0,
        TEXCOORD1,
        TEXCOORD2,
        TEXCOORD3,
        TEXCOORD4,
        TEXCOORD5,
        TEXCOORD6,
        TEXCOORD7,
        JOINT_WEIGHTS_0,
        JOINT_INDICES_0,
        JOINT_WEIGHTS_1,
        JOINT_INDICES_1,
        MAX_VERTEX_DATA_ROLE
    };

    const char * const VertexDataRoleString[] = {
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
        JOINT_WEIGHTS_ROLE_0,
        JOINT_INDICES_ROLE_0,
        JOINT_WEIGHTS_ROLE_1,
        JOINT_INDICES_ROLE_1,
        0
    };

    inline
    VertexDataRole getTextureRole(unsigned theIndex) {
        return VertexDataRole(TEXCOORD0 + theIndex);
    }
}
#endif

