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

#ifndef _ac_glExtensions_h_
#define _ac_glExtensions_h_

#include "y60_glutil_settings.h"

#ifndef GL_ARB_point_sprite
#define GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif

#if 0
#ifndef GL_ARB_point_parameters
#define GL_ARB_point_parameters 1
#ifdef GL_GLEXT_PROTOTYPES
GLAPI void APIENTRY glPointParameterfARB (GLenum, GLfloat);
GLAPI void APIENTRY glPointParameterfvARB (GLenum, const GLfloat *);
#endif /* GL_GLEXT_PROTOTYPES */
typedef void (APIENTRYP PFNGLPOINTPARAMETERFARBPROC) (GLenum pname, GLfloat param);
typedef void (APIENTRYP PFNGLPOINTPARAMETERFVARBPROC) (GLenum pname, const GLfloat *params);
#endif


//extern PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB;
//extern PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB;
#endif

#endif

