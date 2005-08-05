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
//   $RCSfile: glExtensions.cpp,v $
//   $Author: pavel $
//   $Revision: 1.1 $
//   $Date: 2004/10/22 14:36:22 $
//
//   Description:
//
//=============================================================================
#ifdef WIN32
// Note: GLH_EXT_SINGLE_FILE must be defined only in one object file
// it makes the header file to define the function pointer variables
// never set it in a .h file or any other file that shall be linked
// with this object file
// If you use GLH_EXT_SINGLE_FILE 1 make sure that glh_extensions.h
// and glh_genext.h has not been included from another include file
// already without GLH_EXT_SINGLE_FILE set
//

#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>
#else

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif
#endif

#include "glExtensions.h"

//PFNGLPOINTPARAMETERFARBPROC  glPointParameterfARB = 0;
//PFNGLPOINTPARAMETERFVARBPROC glPointParameterfvARB = 0;


