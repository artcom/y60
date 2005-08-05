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
//    $RCSfile: OffscreenBuffer.cpp,v $
//
//     $Author: pavel $
//
//   $Revision: 1.7 $
//
//   see example code at 
//   http://cvs1.nvidia.com/DEMOS/OpenGL/src/shared/pbuffer.cpp
//
//=============================================================================

#include "OffscreenBuffer.h"
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

#define MAX_ATTRIBS     256
#define MAX_PFORMATS    256


    bool checkPBufferExtensions() {
    static bool initialized = false;
        if (!initialized) {

#ifdef WIN32
            wglChoosePixelFormatARB  = (PFNWGLCHOOSEPIXELFORMATARBPROC) wglGetProcAddress("wglChoosePixelFormatARB");
            wglCreatePbufferARB      = (PFNWGLCREATEPBUFFERARBPROC) wglGetProcAddress("wglCreatePbufferARB");
            wglGetPbufferDCARB       = (PFNWGLGETPBUFFERDCARBPROC) wglGetProcAddress("wglGetPbufferDCARB");
            wglQueryPbufferARB       = (PFNWGLQUERYPBUFFERARBPROC) wglGetProcAddress("wglQueryPbufferARB");
            wglReleaseTexImageARB    = (PFNWGLRELEASETEXIMAGEARBPROC) wglGetProcAddress("wglReleaseTexImageARB");
            wglBindTexImageARB       = (PFNWGLBINDTEXIMAGEARBPROC) wglGetProcAddress("wglBindTexImageARB");
            wglReleasePbufferDCARB   = (PFNWGLRELEASEPBUFFERDCARBPROC) wglGetProcAddress("wglReleasePbufferDCARB");
            wglDestroyPbufferARB     = (PFNWGLDESTROYPBUFFERARBPROC) wglGetProcAddress("wglDestroyPbufferARB");
            initialized = (wglChoosePixelFormatARB && wglCreatePbufferARB && wglGetPbufferDCARB && wglDestroyPbufferARB &&
                wglQueryPbufferARB && wglReleaseTexImageARB && wglBindTexImageARB && wglReleasePbufferDCARB );
#else
            initialized = false; //not yet implemented
#endif
        }
        if (!initialized) {
            throw FrameBuffer::Exception("### ERROR: PBuffer::initialize failed",PLUS_FILE_LINE);
        }
        return initialized;
    }

    bool
    OffscreenBuffer::create() {
        checkPBufferExtensions();
#ifdef WIN32
        HDC hdc = wglGetCurrentDC();
        HGLRC hglrc = wglGetCurrentContext();
        GLContext::checkLastError(PLUS_FILE_LINE);

        // Query for a suitable pixel format based on the specified mode.
        int     format;
        int     pformat[MAX_PFORMATS];
        unsigned int nformats;
        int     iattributes[2*MAX_ATTRIBS];
        float   fattributes[2*MAX_ATTRIBS];
        int     nfattribs = 0;
        int     niattribs = 0;

        // Attribute arrays must be "0" terminated - for simplicity, first
        // just zero-out the array entire, then fill from left to right.
        memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
        memset(fattributes,0,sizeof(float)*2*MAX_ATTRIBS);
        // Since we are trying to create a pbuffer, the pixel format we
        // request (and subsequently use) must be "p-buffer capable".
        iattributes[niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
        iattributes[++niattribs] = GL_TRUE;
        // we are asking for a pbuffer that is meant to be bound
        // as an RGBA texture - therefore we need a color plane
        switch (_myPixelType) {
            case RGB: iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_RGB_ARB;break;
                //case RGBA_FLOAT: iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_RGBA_FLOAT_NV;break;
            case RGBA: iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_RGBA_ARB;break;
            case DEPTH: iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_DEPTH_NV;break;
            default:
                throw FrameBuffer::Exception(string("OffscreenFrameBuffer::create: unknown Pixel type:") + asl::as_string(int(_myPixelType)),PLUS_FILE_LINE);

        }
        iattributes[++niattribs] = GL_TRUE;
        iattributes[++niattribs] = WGL_ACCELERATION_ARB;
        iattributes[++niattribs] = WGL_FULL_ACCELERATION_ARB;

        iattributes[++niattribs] = WGL_PIXEL_TYPE_ARB;
        switch (_myPixelType) {
            case RGB: 
            case RGBA: iattributes[++niattribs] = WGL_TYPE_RGBA_ARB;break;
                //case RGBA_FLOAT: iattributes[++niattribs] = WGL_TYPE_RGBA_FLOAT_ATI;break;
            case DEPTH: iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_DEPTH_NV;break;
            default:
                throw FrameBuffer::Exception(string("OffscreenFrameBuffer::create: unknown Pixel type:") + asl::as_string(_myPixelType),PLUS_FILE_LINE);
        }

        iattributes[++niattribs] = WGL_COLOR_BITS_ARB;
        iattributes[++niattribs] = 24;
        if (_myPixelType == RGBA) {
            iattributes[++niattribs] = WGL_ALPHA_BITS_ARB;
            iattributes[++niattribs] = 8;
        }
        iattributes[++niattribs] = WGL_DEPTH_BITS_ARB;
        iattributes[++niattribs] = 24;

#ifdef PBUFFER_HAS_MULTISAMPLE
        iattributes[++niattribs] = WGL_SAMPLE_BUFFERS_ARB;
        iattributes[++niattribs] = GL_TRUE;
        iattributes[++niattribs] = WGL_SAMPLES_ARB;
        iattributes[++niattribs] = 4;
#endif
        iattributes[++niattribs] = 0;
        iattributes[++niattribs] = 0;

        GLContext::checkLastError(PLUS_FILE_LINE);
        if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
        {
            AC_ERROR << "pbuffer creation error:  wglChoosePixelFormatARB() failed.\n";
            exit( -1 );
        }
        GLContext::checkLastError(PLUS_FILE_LINE);
        if ( nformats <= 0 )
        {
            AC_ERROR << "pbuffer creation error:  Couldn't find a suitable pixel format.\n";
            exit( -1 );
        }

        for(int i = 0 ; i< nformats; i++) { 
            AC_DEBUG <<" found possible pf : "<<pformat[i] << endl; 
        } 
        format = pformat[0];

        // Set up the pbuffer attributes
        memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
        niattribs = 0;
        // the render texture format 
        switch (_myPixelType) {
            case RGB: 
                iattributes[niattribs] = WGL_TEXTURE_FORMAT_ARB;
                iattributes[++niattribs] = WGL_TEXTURE_RGB_ARB;break;
            case RGBA: 
                iattributes[niattribs] = WGL_TEXTURE_FORMAT_ARB;
                iattributes[++niattribs] = WGL_TEXTURE_RGBA_ARB;break;
            case DEPTH:
                iattributes[niattribs] = WGL_DEPTH_TEXTURE_FORMAT_NV;
                iattributes[++niattribs] = WGL_TEXTURE_DEPTH_COMPONENT_NV;break;
            default:
                throw FrameBuffer::Exception(string("OffscreenFrameBuffer::create: unknown Pixel type:") + asl::as_string(_myPixelType),PLUS_FILE_LINE);
        }

        // the render texture target is GL_TEXTURE_2D
        iattributes[++niattribs] = WGL_TEXTURE_TARGET_ARB;
        switch (_myTextureType) {
            case TEXTURE2D: 
                iattributes[++niattribs] = WGL_TEXTURE_2D_ARB;break;
            case CUBE: 
                iattributes[++niattribs] = WGL_TEXTURE_CUBE_MAP_ARB;break;
            default:
                throw FrameBuffer::Exception(string("OffscreenFrameBuffer::create: unknown texture type:") + asl::as_string(_myTextureType),PLUS_FILE_LINE);
        }

        // ask to allocate the largest pbuffer it can, if it is
        // unable to allocate for the width and height
        iattributes[++niattribs] = WGL_PBUFFER_LARGEST_ARB;
        iattributes[++niattribs] = FALSE;


        GLContext::checkLastError(PLUS_FILE_LINE);

        // Create the p-buffer.
        _myHpbuffer = wglCreatePbufferARB( hdc, format, _myWidth, _myHeight, iattributes );
        if ( _myHpbuffer == 0)
        {
            throw FrameBuffer::Exception(string("OffscreenFrameBuffer::pbuffer creation error:  wglCreatePbufferARB() failed"),PLUS_FILE_LINE);
        }
        GLContext::checkLastError(PLUS_FILE_LINE);

        _myContext = asl::Ptr<GLContext>(new GLContext(*this));

        // Determine the actual width and height we were able to create.
        wglQueryPbufferARB( _myHpbuffer, WGL_PBUFFER_WIDTH_ARB, &_myWidth );
        wglQueryPbufferARB( _myHpbuffer, WGL_PBUFFER_HEIGHT_ARB, &_myHeight );

        AC_INFO << "PBuffer created width : " << _myWidth << " and height : "<< _myHeight << endl;
#endif
        return true;
    }
    void OffscreenBuffer::startTexture() {
#ifdef WIN32
        // bind the pbuffer to the render texture object
        if (wglBindTexImageARB(_myHpbuffer, WGL_FRONT_LEFT_ARB) == FALSE) {
            GLContext::checkLastError(PLUS_FILE_LINE);
        }
#endif
#ifdef LINUX
        //TODO: Port startTexture to glx: 
#endif
    }

    void OffscreenBuffer::finishTexture(int theTargetTextureID) {
        activate();
        glBindTexture(GL_TEXTURE_2D, theTargetTextureID);
        // release the pbuffer from the render texture object

#ifdef WIN32        
        if (wglReleaseTexImageARB(_myHpbuffer, WGL_FRONT_LEFT_ARB) == FALSE) {
            GLContext::checkLastError(PLUS_FILE_LINE);
        }
#endif
#ifdef LINUX        
        // TODO: Port finishTexture to glx
#endif        
    }

    void
    OffscreenBuffer::shutdown() {
        if (_myContext) {
            _myContext->destroy(*this);   
        }
        if ( _myHpbuffer) {
#ifdef WIN32
            wglDestroyPbufferARB( _myHpbuffer );
#endif
#ifdef LINUX
            glXDestroyGLXPbufferSGIX( _myDisplay, _myHpbuffer );
#endif            
        }
        _myHpbuffer = 0; 
    }

/*

    PBuffer::PBuffer(GLContext * theVisibleContext, int theWidth, int theHeight):
#ifdef WIN32
		_myHpbuffer(0),
#endif
		_myScreenWidth(asl::nextPowerOfTwo(theWidth)),
		_myScreenHeight(asl::nextPowerOfTwo(theHeight)),
		_allExtensionsReady(false),
		_myVisibleContext(theVisibleContext),
		_myTextureId(0)
	{
	    _allExtensionsReady = initializePBufferExtensions();
	}

	PBuffer::~PBuffer(){
	    shutdown();
	}

    void
    PBuffer::activate() const {
#ifdef WIN32
        // be sure the texture is in the visible context
        if (_myVisibleContext) {
            _myVisibleContext->activate();
            glBindTexture(GL_TEXTURE_2D, _myTextureId);

        	// release the pbuffer from the render texture object
            if (wglReleaseTexImageARB(_myHpbuffer, WGL_FRONT_LEFT_ARB) == FALSE) {
                checkLastError(PLUS_FILE_LINE);
            }
        }

        GLContext::activate();
#endif

    }

    void
    PBuffer::bindAsTexture() {
#ifdef WIN32
        if (!_myVisibleContext) {
            return;
        }
        // bind the pbuffer to the render texture object
        if (wglBindTexImageARB(_myHpbuffer, WGL_FRONT_LEFT_ARB) == FALSE) {
                checkLastError(PLUS_FILE_LINE);
        }
#endif
    }

	bool
	PBuffer::create() {
#ifdef WIN32
        if (! _allExtensionsReady) {
            return false;
        }
        HDC hdc = wglGetCurrentDC();
    	HGLRC hglrc = wglGetCurrentContext();
        checkLastError(PLUS_FILE_LINE);

        // Query for a suitable pixel format based on the specified mode.
        int     format;
        int     pformat[MAX_PFORMATS];
        unsigned int nformats;
        int     iattributes[2*MAX_ATTRIBS];
        float   fattributes[2*MAX_ATTRIBS];
        int     nfattribs = 0;
        int     niattribs = 0;

        // Attribute arrays must be "0" terminated - for simplicity, first
        // just zero-out the array entire, then fill from left to right.
        memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
        memset(fattributes,0,sizeof(float)*2*MAX_ATTRIBS);
        // Since we are trying to create a pbuffer, the pixel format we
        // request (and subsequently use) must be "p-buffer capable".
        iattributes[niattribs  ] = WGL_DRAW_TO_PBUFFER_ARB;
        iattributes[++niattribs] = GL_TRUE;
        // we are asking for a pbuffer that is meant to be bound
        // as an RGBA texture - therefore we need a color plane
        iattributes[++niattribs] = WGL_BIND_TO_TEXTURE_RGBA_ARB;
        iattributes[++niattribs] = GL_TRUE;
        iattributes[++niattribs] = WGL_ACCELERATION_ARB;
        iattributes[++niattribs] = WGL_FULL_ACCELERATION_ARB;


        iattributes[++niattribs] = WGL_PIXEL_TYPE_ARB;
        iattributes[++niattribs] = WGL_TYPE_RGBA_ARB;

        iattributes[++niattribs] = WGL_COLOR_BITS_ARB;
        iattributes[++niattribs] = 24;
        iattributes[++niattribs] = WGL_ALPHA_BITS_ARB;
        iattributes[++niattribs] = 8;
        iattributes[++niattribs] = WGL_DEPTH_BITS_ARB;
        iattributes[++niattribs] = 24;

#ifdef PBUFFER_HAS_MULTISAMPLE
        iattributes[++niattribs] = WGL_SAMPLE_BUFFERS_ARB;
        iattributes[++niattribs] = GL_TRUE;
        iattributes[++niattribs] = WGL_SAMPLES_ARB;
        iattributes[++niattribs] = 4;
#endif
        iattributes[++niattribs] = 0;
        iattributes[++niattribs] = 0;

        checkLastError(PLUS_FILE_LINE);
        if ( !wglChoosePixelFormatARB( hdc, iattributes, fattributes, MAX_PFORMATS, pformat, &nformats ) )
        {
            AC_ERROR << "pbuffer creation error:  wglChoosePixelFormatARB() failed.\n";
            exit( -1 );
        }
        checkLastError(PLUS_FILE_LINE);
    	if ( nformats <= 0 )
        {
            AC_ERROR << "pbuffer creation error:  Couldn't find a suitable pixel format.\n";
            exit( -1 );
        }

        DB(for(int i = 0 ; i< nformats; i++) { AC_DEBUG <<" found possible pf : "<<pformat[i] << endl; } )
        format = pformat[0];


        // Set up the pbuffer attributes
        memset(iattributes,0,sizeof(int)*2*MAX_ATTRIBS);
        niattribs = 0;
        // the render texture format is RGBA
        iattributes[niattribs] = WGL_TEXTURE_FORMAT_ARB;
        iattributes[++niattribs] = WGL_TEXTURE_RGBA_ARB;
        // the render texture target is GL_TEXTURE_2D
        iattributes[++niattribs] = WGL_TEXTURE_TARGET_ARB;
        iattributes[++niattribs] = WGL_TEXTURE_2D_ARB;
        // ask to allocate the largest pbuffer it can, if it is
        // unable to allocate for the width and height
        iattributes[++niattribs] = WGL_PBUFFER_LARGEST_ARB;
        iattributes[++niattribs] = FALSE;


        checkLastError(PLUS_FILE_LINE);
        // Create the p-buffer.
        _myHpbuffer = wglCreatePbufferARB( hdc, format, _myScreenWidth, _myScreenHeight, iattributes );
        if ( _myHpbuffer == 0)
        {
            cerr << "pbuffer creation error:  wglCreatePbufferARB() failed\n";
            checkLastError(PLUS_FILE_LINE);
            exit( -1 );
        }
        checkLastError(PLUS_FILE_LINE);

        // Get the device context.
        _myHdc = wglGetPbufferDCARB( _myHpbuffer );
        if ( _myHdc == 0)
        {
            AC_ERROR << "pbuffer creation error:  wglGetPbufferDCARB() failed\n";
            checkLastError(PLUS_FILE_LINE);
            exit( -1 );
        }
        checkLastError(PLUS_FILE_LINE);

        // Create a gl context for the p-buffer.
        _myHglrc = wglCreateContext( _myHdc );
        if ( _myHglrc == 0)
        {
             AC_ERROR << "pbuffer creation error:  wglCreateContext() failed\n";
             checkLastError(PLUS_FILE_LINE);
             exit( -1 );
        }
        checkLastError(PLUS_FILE_LINE);

        // Determine the actual width and height we were able to create.
        wglQueryPbufferARB( _myHpbuffer, WGL_PBUFFER_WIDTH_ARB, &_myScreenWidth );
        wglQueryPbufferARB( _myHpbuffer, WGL_PBUFFER_HEIGHT_ARB, &_myScreenHeight );

	    AC_DEBUG << "PBuffer created width : " << _myScreenWidth << " and height : "<< _myScreenHeight << endl;

        // create texture to render in the visible context
        if (_myVisibleContext) {
            // Clear color
            glClearColor( 0.1f, 0.1f, 0.7f, 0.0f );

            _myVisibleContext->activate(); // create a texture in the visible context

            glShadeModel(GL_SMOOTH);
            // create our render texture object
            glGenTextures(1, &_myTextureId);
            glBindTexture(GL_TEXTURE_2D, _myTextureId);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);
         }

#endif
         return true;
    }

	void
	PBuffer::shutdown() {
#ifdef WIN32
        if ( _myHpbuffer)
        {
            // Check if we are currently rendering in the pbuffer
            if (wglGetCurrentContext() == _myHglrc ) {
                wglMakeCurrent(0,0);
            }
            // delete the pbuffer context
            wglDeleteContext( _myHglrc );
            wglReleasePbufferDCARB( _myHpbuffer, _myHdc );
            wglDestroyPbufferARB( _myHpbuffer );
            _myHpbuffer = 0;
        }
#endif
    }
*/
}
