//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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

    static const char * VertexDataRoleString[] = {
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

