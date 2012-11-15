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

// own header
#include "GLContext.h"

#include "GLUtils.h"

#include <asl/base/Logger.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/string_functions.h>

#include <math.h>
#include <iostream>

using namespace std;

#define DB(x) // x
namespace y60 {

    GLContext::GLContext()
#ifdef _WIN32
        : _myHdc(0), _myHglrc(0)
#endif
#ifdef AC_USE_X11
        : _myDisplay(0), _myDrawable(0), _myGLXContext(0)
#endif
#ifdef AC_USE_OSX_CGL
        : _myCGLContext(0)
#endif
    {
        _myStateCache = RenderStatePtr(new RenderState());
        _myStateCacheInitialized = false;
    }

    GLContext::~GLContext() {
    }

    bool
    GLContext::saveCurrent() {
#ifdef _WIN32
        _myHdc   = wglGetCurrentDC();
        _myHglrc = wglGetCurrentContext();
#endif
#ifdef AC_USE_X11
        _myDisplay = glXGetCurrentDisplay();
        _myDrawable = glXGetCurrentDrawable();
        _myGLXContext = glXGetCurrentContext();
#endif
#ifdef AC_USE_OSX_CGL
        _myCGLContext = CGLGetCurrentContext();
#endif
        return true;
    }

    bool
    GLContext::activate() const{
#ifdef _WIN32

        if (!wglMakeCurrent( _myHdc, _myHglrc)) {
            checkLastError(PLUS_FILE_LINE);
            AC_ERROR << "could not make opengl context current!" << endl;
            return false;
        }
#endif
#ifdef AC_USE_X11
        if (!glXMakeCurrent(_myDisplay, _myDrawable, _myGLXContext )) {
            checkLastError(PLUS_FILE_LINE);
            AC_ERROR << "could not make opengl context current!" << endl;
            return false;
        }
#endif
#ifdef AC_USE_OSX_CGL
        CGLError myError = CGLSetCurrentContext(_myCGLContext);
        if (myError != 0) {
            AC_ERROR << "could not make opengl context current, CGL error code = "<< CGLErrorString(myError) << endl;
            return false;
        }
#endif
        if(!_myStateCacheInitialized) {
            _myStateCache->init();
            _myStateCacheInitialized = true;
        }
        return true;
    }

    bool GLContext::isActive() {
#ifdef _WIN32
        return (_myHdc && _myHglrc);
#endif
#ifdef AC_USE_X11
        return (_myDisplay && _myDrawable && _myGLXContext);
#endif
#ifdef AC_USE_OSX_CGL
        return (_myCGLContext);
#endif
    }


    void
    GLContext::checkLastError(const std::string & theScope)
    {
#ifdef _WIN32
        DWORD myRetVal = GetLastError();
        switch(myRetVal) {
            case ERROR_INVALID_PIXEL_FORMAT:
                throw GLContextException("Win32 Error:  ERROR_INVALID_PIXEL_FORMAT", theScope);
                break;
            case ERROR_NO_SYSTEM_RESOURCES:
                throw GLContextException("Win32 Error:  ERROR_NO_SYSTEM_RESOURCES", theScope);
                break;
            case ERROR_INVALID_DATA:
                throw GLContextException("Win32 Error:  ERROR_INVALID_DATA", theScope);
                break;
            case ERROR_INVALID_WINDOW_HANDLE:
                throw GLContextException("Win32 Error:  ERROR_INVALID_WINDOW_HANDLE", theScope);
                break;
            case ERROR_RESOURCE_TYPE_NOT_FOUND:
                throw GLContextException("Win32 Error:  ERROR_RESOURCE_TYPE_NOT_FOUND", theScope);
                break;
            case ERROR_SUCCESS:
                // no error
                break;
            default:
                throw GLContextException(string("Win32 Unknown Error:  ") + asl::as_string(myRetVal),
                                         theScope);
                break;
        }
#endif
        //TODO: glx error handling
    }

}
