//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: glExtensions.h,v $
//   $Author: pavel $
//   $Revision: 1.1 $
//   $Date: 2004/10/22 14:36:22 $
//
//   Description:
//
//=============================================================================

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

