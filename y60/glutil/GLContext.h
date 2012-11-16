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

#ifndef AC_Y60_GLCONTEXT_INCLUDED
#define AC_Y60_GLCONTEXT_INCLUDED

#include "y60_glutil_settings.h"

#include <asl/base/Exception.h>

#include "GLUtils.h"
#include "RenderState.h"

#ifdef AC_USE_OSX_CGL
//  #include <OpenGL/gliContext.h>
    #include <OpenGL/OpenGL.h>
    #include <OpenGL/CGLTypes.h>
#endif

#include <vector>
#include <string>

namespace y60 {

    DEFINE_EXCEPTION(GLContextException, asl::Exception);

    class Y60_GLUTIL_DECL GLContext {
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
    #ifdef _WIN32
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
            mutable RenderStatePtr _myStateCache;
            mutable bool           _myStateCacheInitialized;
    };

    typedef asl::Ptr<GLContext> GLContextPtr;
} // namespace


#endif // AC_Y60_PBUFFER
