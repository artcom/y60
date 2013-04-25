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
//  Description: This file contains all the wellknown names for the SOM
//
//=============================================================================

#ifndef _ac_PropertyNames_h_
#define _ac_PropertyNames_h_

#include "y60_base_settings.h"

namespace y60 {

    //=== Standard property names =================================================
    const char * const BONE_MATRIX_PROPERTY       = "BoneMatrix";
    const char * const AMBIENT_PROPERTY           = "ambient";
    const char * const SURFACE_COLOR_PROPERTY     = "surfacecolor";
    const char * const DIFFUSE_PROPERTY           = "diffuse";
    const char * const SPECULAR_PROPERTY          = "specular";
    const char * const SHININESS_PROPERTY         = "shininess";
    const char * const EMISSIVE_PROPERTY          = "emissive";
    // blinn parameters
    const char * const ECCENTRICITY_PROPERTY      = "eccentricity";
    const char * const SPECULAR_CUTOFF_PROPERTY   = "specularcutoff";

    const char * const ATTENUATION_PROPERTY       = "attenuation";
    const char * const LINEWIDTH_PROPERTY         = "linewidth";
    const char * const LINESTIPPLE_PROPERTY       = "linestipple";
    const char * const LINESMOOTH_PROPERTY        = "linesmooth";
    const char * const BLENDFUNCTION_PROPERTY     = "blendfunction";
    const char * const BLENDEQUATION_PROPERTY     = "blendequation";
    const char * const BLENDCOLOR_PROPERTY        = "blendcolor";
    const char * const GLOW_PROPERTY              = "glow";
    const char * const POINTSMOOTH_PROPERTY        = "pointsmooth";
    const char * const POINTSIZE_PROPERTY         = "pointsize"; // PointSize, PointSizeMin, PointSizeMax
                                                                  // (last two work with attenuation)
    const char * const POINTATTENUATION_PROPERTY  = "pointattenuation"; // v[0] + v[1] * d + v[2] * d ^ 2;

    const char * const AUTOTEXCOORDMODES_PROPERTY   = "autotexcoordmodes"; //[U_Mode, V_Mode, W_Mode, Q_Mode] where each
                                                                            // mode is one of OBJECT_LINEAR, EYE_LINEAR, SPHERE_MAP
    const char * const AUTOTEXCOORDPARAMS_PROPERTY = "autotexcoordparams"; // [U_Params, V_Params, W_Params, Q_Params]
                                                                            // Sphere mapped coords are not supported yet ...
                                                                            // see also OGL Redbook p.413 ff
    const char * const TARGETBUFFERS_PROPERTY     = "targetbuffers";
    const char * const DEPTHTEST_PROPERTY         = "depthtest";
} // namespace
#endif
