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
#include <y60/NodeValueNames.h>
#include <y60/MaterialParameter.h>

#ifdef WIN32
    #ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN 1
    #endif
    #include <windows.h>
    #undef max
#endif

#if defined(LINUX) || defined(OSX)
	#define AC_USE_X11
#endif

#ifdef OSX
#define Cursor X11_Cursor
#include <X11/X.h>
#undef Cursor
#endif

#ifdef AC_USE_OSX_CGL
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <OpenGL/glext.h>
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
    extern T Missing_ ## X = (T)&y60::GLExceptionHelper< _name_ ##X>::throwMissingExtension; \
    T  _ac_ ## X = Missing_ ## X;
#else
    #define DEF_PROC_ADDRESS(T,X) \
    extern  T _ac_ ## X; \
    extern T Missing_ ## X;
#endif


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
DEF_PROC_ADDRESS( PFNGLGENFRAMEBUFFERSEXTPROC, glGenFramebuffersEXT );
#define glGenFramebuffersEXT _ac_glGenFramebuffersEXT
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

#endif // ndef OSX

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
    GLenum asGLTextureFunc(TextureApplyMode theApplyMode);
    GLenum asGLTextureWrapmode(TextureWrapMode theWrapMode);
    GLenum asGLTextureSampleFilter(TextureSampleFilter theSampleFilter, bool theMipmapsFlag=false);
    GLenum asGLTextureInternalFormat(TextureInternalFormat theFormat);
    GLenum asGLTexCoordMode(TexCoordMode theMode);
    GLenum asGLBlendEquation(const BlendEquation & theBlendEquation);

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
    void initGLExtensions(unsigned int theNeededExtensions, bool theVerboseFlag = true);

    /// Query for supported capability.
    bool hasCap(unsigned int theCap);
    /// Query for supported OpenGL *extension*.
    bool hasCap(const std::string & theCapsStr);

    std::string getGLVersionString();
    std::string getGLVendorString();
    std::string getGLRendererString();
    std::string getLatestCgProfileString();
    unsigned int getGLExtensionStrings(std::vector<std::string> & theTokens);    
}

#endif // _ac_render_GLUtils_h_
