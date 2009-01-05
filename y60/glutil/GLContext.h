//============================================================================
//
// Copyright (C) 2002, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//    $RCSfile: GLContext.h,v $
//
//     $Author: janbo $
//
//   $Revision: 1.4 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#ifndef AC_Y60_GLCONTEXT_INCLUDED
#define AC_Y60_GLCONTEXT_INCLUDED

#include <asl/base/Exception.h>

#include "GLUtils.h"
#include "RenderState.h"

#ifdef AC_USE_OSX_CGL
//	#include <OpenGL/gliContext.h>
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/CGLTypes.h>
#endif

#include <vector>
#include <string>

namespace y60 {

    DEFINE_EXCEPTION(GLContextException, asl::Exception);

    class GLContext {
        public:
            GLContext();

            virtual ~GLContext();
    
            virtual bool activate() const;
            virtual bool saveCurrent();

    	    static void checkLastError(const std::string & theScope);
            
            RenderStatePtr getStateCache() const {
                return _myStateCache;
            }
            
            bool isActive();
            
         private:
    #ifdef WIN32
            HDC         _myHdc;
            HGLRC       _myHglrc;
    #endif
    #ifdef AC_USE_X11
            Display *   _myDisplay;
            GLXDrawable _myDrawable;
            GLXContext  _myGLXContext;
    #endif
	#ifdef AC_USE_OSX_CGL
			//GLIContext           _myGLIContext;
			CGLContextObj       _myCGLContext;
	#endif
            RenderStatePtr _myStateCache;
    };

    typedef asl::Ptr<GLContext> GLContextPtr;
} // namespace 


#endif // AC_Y60_PBUFFER
