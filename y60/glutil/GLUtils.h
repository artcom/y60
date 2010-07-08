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

#ifndef _ac_render_GLUtils_h_
#define _ac_render_GLUtils_h_

#include "y60_glutil_settings.h"

#include <GL/glew.h>
#ifdef _WIN32
#   include <GL/wglew.h>
#elif LINUX
#   include <GL/glxew.h>
#endif

// XXX: X11 defines this
#ifdef GenericEvent
#warning undefing polluting GenericEvent from X11
#undef GenericEvent
#endif

#include <limits.h>

#include <asl/base/Exception.h>
#include <asl/base/string_functions.h>
#include <asl/base/Dashboard.h>
#include <y60/base/NodeValueNames.h>
#include <y60/scene/MaterialParameter.h>

namespace y60 {
    DEFINE_EXCEPTION(OpenGLException, asl::Exception);
    DEFINE_EXCEPTION(CantParseOpenGLVersion, asl::Exception);
    DEFINE_EXCEPTION(CantQueryOpenGLVersion, asl::Exception);
    DEFINE_EXCEPTION(MissingExtensionsException, asl::Exception);
    DEFINE_EXCEPTION(GLUnknownBlendEquation, asl::Exception);

template <const char* NAME>
struct GLExceptionHelper {
    static void throwMissingExtension() {
        throw y60::MissingExtensionsException(NAME, PLUS_FILE_LINE);
    }
};

}

namespace y60 {
    class Y60_GLUTIL_DECL GLScopeTimer {
	public:
		GLScopeTimer(asl::TimerPtr theTimer) : _myTimer(theTimer) {
           if (_flushGL_before_stop) {
                glFlush();
            }
			_myTimer->start();
		}
		~GLScopeTimer() {
            if (_flushGL_before_stop) {
                glFlush();
            }
			_myTimer->stop();
		}
        static bool _flushGL_before_stop;
	private:
		asl::TimerPtr _myTimer;
	};
    #define MAKE_GL_SCOPE_TIMER(NAME) \
    static asl::TimerPtr myScopeTimer ## NAME = asl::getDashboard().getTimer(#NAME);\
        y60::GLScopeTimer myScopeTimerWrapper ## NAME ( myScopeTimer ## NAME);

    Y60_GLUTIL_DECL void checkOGLError(const std::string& theLocation);
    Y60_GLUTIL_DECL void queryGLVersion(unsigned & theMajor, unsigned & theMinor, unsigned & theRelease);

    // [CH+VS]: Do not turn this on in release mode, because each check takes
    // up to 50 ms on a 1 GHz Machine in the renderBodyPart method.
    #ifdef DEBUG_VARIANT
    #define CHECK_OGL_ERROR checkOGLError(PLUS_FILE_LINE)
    #else
    #define CHECK_OGL_ERROR
    #endif

    Y60_GLUTIL_DECL GLenum asGLBlendFunction(BlendFunction theFunction);
    Y60_GLUTIL_DECL GLenum asGLTextureRegister(unsigned theIndex);
    Y60_GLUTIL_DECL GLenum asGLTextureRegister(y60::GLRegister theRegister);
    Y60_GLUTIL_DECL GLenum asGLLightEnum(unsigned theLightNum);
    Y60_GLUTIL_DECL GLenum asGLClippingPlaneId(unsigned thePlaneNum);
    Y60_GLUTIL_DECL GLenum asGLTextureTarget(TextureType theTextureType);
    Y60_GLUTIL_DECL GLenum asGLTextureApplyMode(TextureApplyMode theApplyMode);
    Y60_GLUTIL_DECL GLenum asGLTextureWrapMode(TextureWrapMode theWrapMode);
    Y60_GLUTIL_DECL GLenum asGLTextureSampleFilter(TextureSampleFilter theSampleFilter, bool theMipmapsFlag=false);
    Y60_GLUTIL_DECL GLenum asGLTextureInternalFormat(TextureInternalFormat theFormat);
    Y60_GLUTIL_DECL GLenum asGLTexCoordMode(TexCoordMode theMode);
    Y60_GLUTIL_DECL GLenum asGLBlendEquation(const BlendEquation & theBlendEquation);
    Y60_GLUTIL_DECL GLenum asGLCubemapFace(unsigned theFace);

    TextureInternalFormat fromGLTextureInternalFormat(GLenum theFormat);

    enum GL_EXTENSION_ENUM {
        CUBEMAP_SUPPORT            = (1<<0),
        TEXTURECOMPRESSION_SUPPORT = (1<<1),
        MULTITEXTURE_SUPPORT       = (1<<2),
        POINT_SPRITE_SUPPORT       = (1<<3),
        POINT_PARAMETER_SUPPORT    = (1<<4),
        TEXTURE_3D_SUPPORT         = (1<<5),
        FRAMEBUFFER_SUPPORT        = (1<<6),
        HAVE_EM_ALL                = UINT_MAX
    };

    /**
     * Initialize all supported OpenGL extensions.
     * @param theNeededExtensions DEPRECATED.
     * @param theVerboseFlag DEPRECATED.
     */

    /// Query for supported capability.
    Y60_GLUTIL_DECL bool hasCap(unsigned int theCap);
    /// Query for supported OpenGL *extension*.
    Y60_GLUTIL_DECL bool hasCap(const std::string & theCapsStr);

    Y60_GLUTIL_DECL std::string getGLVersionString();
    Y60_GLUTIL_DECL std::string getGLVendorString();
    Y60_GLUTIL_DECL std::string getGLRendererString();
#ifndef _AC_NO_CG_
    Y60_GLUTIL_DECL std::string getLatestCgProfileString();
#endif
    Y60_GLUTIL_DECL unsigned int getGLExtensionStrings(std::vector<std::string> & theTokens);
}

#endif // _ac_render_GLUtils_h_
