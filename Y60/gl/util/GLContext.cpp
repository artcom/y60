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
//    $RCSfile: GLContext.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.7 $
//
// Description:
//
// (CVS log at the bottom of this file)
//
//=============================================================================

#include "GLContext.h"
#include "GLUtils.h"

#include <asl/Logger.h>
#include <asl/numeric_functions.h>
#include <asl/string_functions.h>

#include <math.h>
#include <iostream>

using namespace std;

#define DB(x) // x
namespace y60 {

    GLContext::GLContext()
#ifdef WIN32
        : _myHdc(0), _myHglrc(0)
#endif
#ifdef LINUX
        : _myDisplay(0), _myDrawable(0), _myGLXContext(0)  
#endif
    { }

    GLContext::~GLContext() {
    }

    bool
    GLContext::saveCurrent() {
#ifdef WIN32
        _myHdc   = wglGetCurrentDC();
        _myHglrc = wglGetCurrentContext();
#endif
#ifdef LINUX
        _myDisplay = glXGetCurrentDisplay();
        _myDrawable = glXGetCurrentDrawable();
        _myGLXContext = glXGetCurrentContext();
#endif
        return true;
    }

    bool
    GLContext::activate() const{
#ifdef WIN32

        if (!wglMakeCurrent( _myHdc, _myHglrc)) {
            checkLastError(PLUS_FILE_LINE);
            AC_ERROR << "could not make opengl context current!" << endl;
            return false;
        }
#endif
#ifdef LINUX
        if (!glXMakeCurrent(_myDisplay, _myDrawable, _myGLXContext )) {
            checkLastError(PLUS_FILE_LINE);
            AC_ERROR << "could not make opengl context current!" << endl;
            return false;
        }
#endif
        return true;
    }

    void
    GLContext::checkLastError(const std::string & theScope)
    {
#ifdef WIN32
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
