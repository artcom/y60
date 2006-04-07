//============================================================================
// Copyright (C) 2000-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
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
    static const char * ATTENUATION_PROPERTY       = "attenuation";
    static const char * LINEWIDTH_PROPERTY         = "linewidth";
    static const char * LINESTIPPLE_PROPERTY       = "linestipple";
    static const char * BLENDFUNCTION_PROPERTY     = "blendfunction";
    static const char * BLENDEQUATION_PROPERTY     = "blendequation";
    static const char * POINTSIZE_PROPERTY         = "pointsize"; // PointSize, PointSizeMin, PointSizeMax 
                                                                  // (last two work with attenuation)
    static const char * POINTATTENUATION_PROPERTY  = "pointattenuation"; // v[0] + v[1] * d + v[2] * d ^ 2;

    static const char * AUTOTEXCOORDMODES_PROPERTY   = "autotexcoordmodes"; //[U_Mode, V_Mode, W_Mode, Q_Mode] where each
                                                                            // mode is one of OBJECT_LINEAR, EYE_LINEAR, SPHERE_MAP
    static const char * AUTOTEXCOORDPARAMS_PROPERTY = "autotexcoordparams"; // [U_Params, V_Params, W_Params, Q_Params]
                                                                            // Sphere mapped coords are not supported yet ... 
                                                                            // see also OGL Redbook p.413 ff
    static const char * TARGETBUFFERS_PROPERTY     = "targetbuffers";
} // namespace
#endif
