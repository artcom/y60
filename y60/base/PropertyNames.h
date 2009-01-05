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
//
//  Description: This file contains all the wellknown names for the SOM
//
//=============================================================================

#ifndef _ac_PropertyNames_h_
#define _ac_PropertyNames_h_

namespace y60 {

    //=== Standard property names =================================================
    static const char * BONE_MATRIX_PROPERTY       = "BoneMatrix";
    static const char * AMBIENT_PROPERTY           = "ambient";
    static const char * SURFACE_COLOR_PROPERTY     = "surfacecolor";
    static const char * DIFFUSE_PROPERTY           = "diffuse";
    static const char * SPECULAR_PROPERTY          = "specular";
    static const char * SHININESS_PROPERTY         = "shininess";
    static const char * EMISSIVE_PROPERTY          = "emissive";
    // blinn parameters
    static const char * ECCENTRICITY_PROPERTY      = "eccentricity";
    static const char * SPECULAR_CUTOFF_PROPERTY   = "specularcutoff";

    static const char * ATTENUATION_PROPERTY       = "attenuation";
    static const char * LINEWIDTH_PROPERTY         = "linewidth";
    static const char * LINESTIPPLE_PROPERTY       = "linestipple";
    static const char * LINESMOOTH_PROPERTY        = "linesmooth";
    static const char * BLENDFUNCTION_PROPERTY     = "blendfunction";
    static const char * BLENDEQUATION_PROPERTY     = "blendequation";
    static const char * GLOW_PROPERTY              = "glow";
    static const char * POINTSIZE_PROPERTY         = "pointsize"; // PointSize, PointSizeMin, PointSizeMax 
                                                                  // (last two work with attenuation)
    static const char * POINTATTENUATION_PROPERTY  = "pointattenuation"; // v[0] + v[1] * d + v[2] * d ^ 2;

    static const char * AUTOTEXCOORDMODES_PROPERTY   = "autotexcoordmodes"; //[U_Mode, V_Mode, W_Mode, Q_Mode] where each
                                                                            // mode is one of OBJECT_LINEAR, EYE_LINEAR, SPHERE_MAP
    static const char * AUTOTEXCOORDPARAMS_PROPERTY = "autotexcoordparams"; // [U_Params, V_Params, W_Params, Q_Params]
                                                                            // Sphere mapped coords are not supported yet ... 
                                                                            // see also OGL Redbook p.413 ff
    static const char * TARGETBUFFERS_PROPERTY     = "targetbuffers";
    static const char * DEPTHTEST_PROPERTY         = "depthtest";
} // namespace
#endif
