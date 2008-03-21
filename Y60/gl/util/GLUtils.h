//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_render_GLUtils_h_
#define _ac_render_GLUtils_h_

#include <asl/Exception.h>
#include <asl/string_functions.h>
#include <asl/Dashboard.h>
#include <y60/NodeValueNames.h>
#include <y60/MaterialParameter.h>

#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #undef max
#endif

#if defined(LINUX)
	#define AC_USE_X11
#endif

#if defined(OSX)
	#define AC_USE_OSX_CGL
	#define AC_USE_NSGL
#endif

#ifdef OSX
#ifdef AC_USE_OSX_CGL
#define Cursor X11_Cursor
#include <X11/X.h>
#undef Cursor
#endif
#endif

#ifdef AC_USE_OSX_CGL
	//#include <OpenGL/gl.h>
	//#include <OpenGL/glu.h>
	//#include <OpenGL/glext.h>
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#endif

// window system extensions
#ifdef WIN32
#   include <GL/glh_extensions.h>
#   include <GL/glh_genext.h>
#endif
#ifdef AC_USE_X11
    #include <GL/glx.h>
    #include <GL/glxext.h>
#endif

#ifdef AC_USE_OSX_CGL
	//#include <AGL/agl.h>
#endif


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

#ifdef OSX
#undef GL_GLEXT_VERSION
#undef GL_ARB_texture_float
#undef GL_ARB_vertex_buffer_object
#undef GL_ARB_texture_compression
#undef GL_ARB_multitexture
#undef GL_ARB_point_parameters
#undef GL_EXT_framebuffer_object

#undef GL_BLEND_EQUATION_RGB

#undef GL_VERSION_1_2
#undef GL_VERSION_1_3
#undef GL_VERSION_1_4
#undef GL_VERSION_1_5
#undef GL_VERSION_2_0

#define GLintptr ac_GLintptr
#define GLsizeiptr ac_GLsizeiptr
#define GLintptrARB ac_GLintptrARB
#define GLsizeiptrARB ac_GLsizeiptrARB
#include <GL/nvglext.h>
#endif


/* we have to define our own function pointers
 * we give them private names and
 * #define them to the usual gl function name
 * idea taken from glew.sourceforge.net
*/
#ifdef _ac_render_GLUtils_cpp_
    #define DEF_PROC_ADDRESS(T,X) \
    extern const char _name_ ## X [] = #X; \
    T Missing_ ## X = (T)&y60::GLExceptionHelper< _name_ ##X>::throwMissingExtension; \
    T  _ac_ ## X = Missing_ ## X;
#else
    #define DEF_PROC_ADDRESS(T,X) \
    extern T _ac_ ## X; \
    extern T Missing_ ## X;
#endif
#define IS_SUPPORTED(X) (X != 0 && X != Missing_ ## X)


typedef void (*PFNACTESTPROC)(int, int);

extern "C" {

// test cases for unit test
DEF_PROC_ADDRESS( PFNACTESTPROC, acTestMissingExtension );
#define acTestMissingExtension _ac_acTestMissingExtension
DEF_PROC_ADDRESS( PFNACTESTPROC, acTestMissingFunction );
#define acTestMissingFunction _ac_acTestMissingFunction

#if 1

// now the real extensions
DEF_PROC_ADDRESS( PFNGLBINDBUFFERARBPROC, glBindBufferARB );
#define glBindBufferARB _ac_glBindBufferARB
DEF_PROC_ADDRESS( PFNGLDELETEBUFFERSARBPROC, glDeleteBuffersARB );
#define glDeleteBuffersARB _ac_glDeleteBuffersARB
DEF_PROC_ADDRESS( PFNGLGENBUFFERSARBPROC, glGenBuffersARB );
#define glGenBuffersARB _ac_glGenBuffersARB
DEF_PROC_ADDRESS( PFNGLISBUFFERARBPROC, glIsBufferARB );
#define glIsBufferARB _ac_glIsBufferARB
DEF_PROC_ADDRESS( PFNGLBUFFERDATAARBPROC, glBufferDataARB );
#define glBufferDataARB _ac_glBufferDataARB
DEF_PROC_ADDRESS( PFNGLBUFFERSUBDATAARBPROC, glBufferSubDataARB );
#define glBufferSubDataARB _ac_glBufferSubDataARB
DEF_PROC_ADDRESS( PFNGLGETBUFFERSUBDATAARBPROC, glGetBufferSubDataARB );
#define glGetBufferSubDataARB _ac_glGetBufferSubDataARB
DEF_PROC_ADDRESS( PFNGLMAPBUFFERARBPROC, glMapBufferARB );
#define glMapBufferARB _ac_glMapBufferARB
DEF_PROC_ADDRESS( PFNGLUNMAPBUFFERARBPROC, glUnmapBufferARB );
#define glUnmapBufferARB _ac_glUnmapBufferARB
DEF_PROC_ADDRESS( PFNGLGETBUFFERPARAMETERIVARBPROC, glGetBufferParameterivARB );
#define glGetBufferParameterivARB _ac_glGetBufferParameterivARB
DEF_PROC_ADDRESS( PFNGLGETBUFFERPOINTERVARBPROC, glGetBufferPointervARB );
#define glGetBufferPointervARB _ac_glGetBufferPointervARB

/* OpenGL core 1.5 functions without ARB-identifier */

DEF_PROC_ADDRESS( PFNGLBINDBUFFERPROC, glBindBuffer );
#define glBindBuffer _ac_glBindBuffer
DEF_PROC_ADDRESS( PFNGLDELETEBUFFERSPROC, glDeleteBuffers );
#define glDeleteBuffers _ac_glDeleteBuffers
DEF_PROC_ADDRESS( PFNGLGENBUFFERSPROC, glGenBuffers );
#define glGenBuffers _ac_glGenBuffers
DEF_PROC_ADDRESS( PFNGLISBUFFERPROC, glIsBuffer );
#define glIsBuffer _ac_glIsBuffer
DEF_PROC_ADDRESS( PFNGLBUFFERDATAPROC, glBufferData );
#define glBufferData _ac_glBufferData
DEF_PROC_ADDRESS( PFNGLBUFFERSUBDATAPROC, glBufferSubData );
#define glBufferSubData _ac_glBufferSubData
DEF_PROC_ADDRESS( PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData );
#define glGetBufferSubData _ac_glGetBufferSubData
DEF_PROC_ADDRESS( PFNGLMAPBUFFERPROC, glMapBuffer );
#define glMapBuffer _ac_glMapBuffer
DEF_PROC_ADDRESS( PFNGLUNMAPBUFFERPROC, glUnmapBuffer );
#define glUnmapBuffer _ac_glUnmapBuffer
DEF_PROC_ADDRESS( PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv );
#define glGetBufferParameteriv _ac_glGetBufferParameteriv
DEF_PROC_ADDRESS( PFNGLGETBUFFERPOINTERVPROC, glGetBufferPointerv );
#define glGetBufferPointerv _ac_glGetBufferPointerv

DEF_PROC_ADDRESS( PFNGLFLUSHVERTEXARRAYRANGENVPROC, glFlushVertexArrayRangeNV );
#define glFlushVertexArrayRangeNV _ac_glFlushVertexArrayRangeNV
DEF_PROC_ADDRESS( PFNGLVERTEXARRAYRANGENVPROC, glVertexArrayRangeNV );
#define glVertexArrayRangeNV _ac_glVertexArrayRangeNV


DEF_PROC_ADDRESS( PFNGLBLENDEQUATIONPROC, glBlendEquation );
#define glBlendEquation _ac_glBlendEquation
DEF_PROC_ADDRESS( PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate );
#define glBlendFuncSeparate _ac_glBlendFuncSeparate
DEF_PROC_ADDRESS( PFNGLBLENDCOLORPROC, glBlendColor );
#define glBlendColor _ac_glBlendColor

//texture compression
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE1DARBPROC, glCompressedTexImage1DARB );
#define glCompressedTexImage1DARB _ac_glCompressedTexImage1DARB
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC, glCompressedTexImage2DARB );
#define glCompressedTexImage2DARB _ac_glCompressedTexImage2DARB
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE3DARBPROC, glCompressedTexImage3DARB );
#define glCompressedTexImage3DARB _ac_glCompressedTexImage3DARB
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC, glCompressedTexSubImage1DARB );
#define glCompressedTexSubImage1DARB _ac_glCompressedTexSubImage1DARB
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC, glCompressedTexSubImage2DARB );
#define glCompressedTexSubImage2DARB _ac_glCompressedTexSubImage2DARB
DEF_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC, glCompressedTexSubImage3DARB );
#define glCompressedTexSubImage3DARB _ac_glCompressedTexSubImage3DARB
DEF_PROC_ADDRESS( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC, glGetCompressedTexImageARB );
#define glGetCompressedTexImageARB _ac_glGetCompressedTexImageARB

/* Retrieve some ARB_multitexture routines. */
DEF_PROC_ADDRESS( PFNGLMULTITEXCOORD2IARBPROC, glMultiTexCoord2iARB );
#define glMultiTexCoord2iARB _ac_glMultiTexCoord2iARB
DEF_PROC_ADDRESS( PFNGLMULTITEXCOORD3FARBPROC, glMultiTexCoord3fARB );
#define glMultiTexCoord3fARB _ac_glMultiTexCoord3fARB
DEF_PROC_ADDRESS( PFNGLMULTITEXCOORD3FVARBPROC, glMultiTexCoord3fvARB );
#define glMultiTexCoord3fvARB _ac_glMultiTexCoord3fvARB
DEF_PROC_ADDRESS( PFNGLMULTITEXCOORD2FARBPROC, glMultiTexCoord2fARB );
#define glMultiTexCoord2fARB _ac_glMultiTexCoord2fARB
DEF_PROC_ADDRESS( PFNGLACTIVETEXTUREPROC,glActiveTexture );
#define glActiveTexture _ac_glActiveTexture
DEF_PROC_ADDRESS( PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture );
#define glClientActiveTexture _ac_glClientActiveTexture

// point sprites
DEF_PROC_ADDRESS( PFNGLPOINTPARAMETERFARBPROC, glPointParameterfARB );
#define glPointParameterfARB _ac_glPointParameterfARB
DEF_PROC_ADDRESS( PFNGLPOINTPARAMETERFVARBPROC, glPointParameterfvARB );
#define glPointParameterfvARB _ac_glPointParameterfvARB

//3D textures
DEF_PROC_ADDRESS( PFNGLTEXIMAGE3DPROC, glTexImage3D );
#define glTexImage3D _ac_glTexImage3D
DEF_PROC_ADDRESS( PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D );
#define glTexSubImage3D _ac_glTexSubImage3D
DEF_PROC_ADDRESS( PFNGLCOPYTEXSUBIMAGE3DPROC, glCopyTexSubImage3D );
#define glCopyTexSubImage3D _ac_glCopyTexSubImage3D

// (offscreen) framebuffer support
#ifdef GL_EXT_framebuffer_object
DEF_PROC_ADDRESS( PFNGLGENRENDERBUFFERSEXTPROC, glGenRenderbuffersEXT );
#define glGenRenderbuffersEXT _ac_glGenRenderbuffersEXT
DEF_PROC_ADDRESS( PFNGLDELETERENDERBUFFERSEXTPROC, glDeleteRenderbuffersEXT );
#define glDeleteRenderbuffersEXT _ac_glDeleteRenderbuffersEXT

DEF_PROC_ADDRESS( PFNGLGENFRAMEBUFFERSEXTPROC, glGenFramebuffersEXT );
#define glGenFramebuffersEXT _ac_glGenFramebuffersEXT
DEF_PROC_ADDRESS( PFNGLDELETEFRAMEBUFFERSEXTPROC, glDeleteFramebuffersEXT );
#define glDeleteFramebuffersEXT _ac_glDeleteFramebuffersEXT

DEF_PROC_ADDRESS( PFNGLBINDFRAMEBUFFEREXTPROC, glBindFramebufferEXT );
#define glBindFramebufferEXT _ac_glBindFramebufferEXT
DEF_PROC_ADDRESS( PFNGLBINDRENDERBUFFEREXTPROC, glBindRenderbufferEXT );
#define glBindRenderbufferEXT _ac_glBindRenderbufferEXT
DEF_PROC_ADDRESS( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC, glFramebufferTexture2DEXT );
#define glFramebufferTexture2DEXT _ac_glFramebufferTexture2DEXT
DEF_PROC_ADDRESS( PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC, glFramebufferRenderbufferEXT );
#define glFramebufferRenderbufferEXT _ac_glFramebufferRenderbufferEXT
DEF_PROC_ADDRESS( PFNGLRENDERBUFFERSTORAGEEXTPROC, glRenderbufferStorageEXT );
#define glRenderbufferStorageEXT _ac_glRenderbufferStorageEXT
DEF_PROC_ADDRESS( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, glCheckFramebufferStatusEXT );
#define glCheckFramebufferStatusEXT _ac_glCheckFramebufferStatusEXT
DEF_PROC_ADDRESS( PFNGLGENERATEMIPMAPEXTPROC, glGenerateMipmapEXT );
#define glGenerateMipmapEXT _ac_glGenerateMipmapEXT
#endif

#ifndef GL_EXT_framebuffer_blit
// XXX these should come from the gl.h header but currently don't (Jan.2007)
//     they do now, at least on current nvidia-glx. revisit. (Aug.2007)
#define GL_EXT_framebuffer_blit 1
#define GL_READ_FRAMEBUFFER_EXT         0x8CA8
#define GL_DRAW_FRAMEBUFFER_EXT         0x8CA9
#define GL_DRAW_FRAMEBUFFER_BINDING_EXT 0x8CA6
#define GL_READ_FRAMEBUFFER_BINDING_EXT 0x8CAA
typedef void (APIENTRYP PFNGLBLITFRAMEBUFFEREXTPROC) (GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1,
                            GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1,
                            GLbitfield mask, GLenum filter);
#endif

DEF_PROC_ADDRESS( PFNGLBLITFRAMEBUFFEREXTPROC, glBlitFramebufferEXT );
#define glBlitFramebufferEXT _ac_glBlitFramebufferEXT

#ifndef GL_EXT_framebuffer_multisample
// XXX these should come from the gl.h header but currently don't (Jan.2007)
//     they do now, at least on current nvidia-glx. revisit. (Aug.2007)
#define GL_EXT_framebuffer_multisample 1
#define GL_RENDERBUFFER_SAMPLES_EXT               0x8CAB
#define GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE_EXT 0x8D56
#define GL_MAX_SAMPLES_EXT                        0x8D57
typedef void (APIENTRYP PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC) (GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
#endif

DEF_PROC_ADDRESS( PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC, glRenderbufferStorageMultisampleEXT );
#define glRenderbufferStorageMultisampleEXT _ac_glRenderbufferStorageMultisampleEXT

#endif // 1

// Swap interval
#ifdef WIN32
    DEF_PROC_ADDRESS( PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT );
    #define wglSwapIntervalEXT _ac_wglSwapIntervalEXT
    DEF_PROC_ADDRESS( PFNWGLGETSWAPINTERVALEXTPROC, wglGetSwapIntervalEXT );
    #define wglGetSwapIntervalEXT _ac_wglGetSwapIntervalEXT
#endif
#ifdef AC_USE_X11
    DEF_PROC_ADDRESS( PFNGLXGETVIDEOSYNCSGIPROC, glXGetVideoSyncSGI );
    #define glXGetVideoSyncSGI _ac_glXGetVideoSyncSGI
    DEF_PROC_ADDRESS( PFNGLXWAITVIDEOSYNCSGIPROC, glXWaitVideoSyncSGI );
    #define glXWaitVideoSyncSGI _ac_glXWaitVideoSyncSGI
#endif

} // extern C



#ifndef GL_ARB_point_sprite
#define GL_ARB_point_sprite
#define GL_POINT_SPRITE_ARB               0x8861
#define GL_COORD_REPLACE_ARB              0x8862
#endif

namespace y60 {
    class GLScopeTimer {
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

    void checkOGLError(const std::string& theLocation);
    void queryGLVersion(unsigned & theMajor, unsigned & theMinor, unsigned & theRelease);

    // [CH+VS]: Do not turn this on in release mode, because each check takes
    // up to 50 ms on a 1 GHz Machine in the renderBodyPart method.
    #ifdef DEBUG_VARIANT
    #define CHECK_OGL_ERROR checkOGLError(PLUS_FILE_LINE)
    #else
    #define CHECK_OGL_ERROR
    #endif

    bool queryOGLExtension(const char *extName, bool theVerboseFlag = true);
#ifdef WIN32
    bool queryWGLExtension(const char *extension);
#endif
#ifdef AC_USE_X11
    bool queryGLXExtension(const char *extension);
#endif
    GLenum asGLBlendFunction(BlendFunction theFunction);
    GLenum asGLTextureRegister(unsigned theIndex);
    GLenum asGLTextureRegister(y60::GLRegister theRegister);
    GLenum asGLLightEnum(unsigned theLightNum);
    GLenum asGLClippingPlaneId(unsigned thePlaneNum);
    GLenum asGLTextureTarget(TextureType theTextureType);
    GLenum asGLTextureApplyMode(TextureApplyMode theApplyMode);
    GLenum asGLTextureWrapMode(TextureWrapMode theWrapMode);
    GLenum asGLTextureSampleFilter(TextureSampleFilter theSampleFilter, bool theMipmapsFlag=false);
    GLenum asGLTextureInternalFormat(TextureInternalFormat theFormat);
    GLenum asGLTexCoordMode(TexCoordMode theMode);
    GLenum asGLBlendEquation(const BlendEquation & theBlendEquation);
    GLenum asGLCubemapFace(unsigned theFace);

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
    void initGLExtensions(unsigned int theNeededExtensions, bool theVerboseFlag = true,
                          bool theInitGLH_extension = true);

    /// Query for supported capability.
    bool hasCap(unsigned int theCap);
    /// Query for supported OpenGL *extension*.
    bool hasCap(const std::string & theCapsStr);

    std::string getGLVersionString();
    std::string getGLVendorString();
    std::string getGLRendererString();
#ifndef _AC_NO_CG_
    std::string getLatestCgProfileString();
#endif
    unsigned int getGLExtensionStrings(std::vector<std::string> & theTokens);    
}

#endif // _ac_render_GLUtils_h_
