//=============================================================================
// Copyright (C) 2003-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
extern "C" {
typedef void (*GdkGLProc)(void);
GdkGLProc gdk_gl_get_proc_address            (const char *proc_name);
}

#define _ac_render_GLUtils_cpp_
#include "GLUtils.h"

#include <iostream>
#include <iomanip>
#include <string.h>
#include <asl/Exception.h>
#include <asl/numeric_functions.h>
#include <asl/Logger.h>

using namespace std;

namespace y60 {

    void checkOGLError(const std::string& theLocation) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            throw OpenGLException("OpenGL error: #(" + asl::as_string(err) + ") " +
                (const char *)(gluErrorString(err)), theLocation);
            if (err != GL_INVALID_OPERATION) {  // XXX: Do we really need this?
                checkOGLError(theLocation);
            }
        }
    }

    void queryGLVersion(unsigned & theMajor, unsigned & theMinor, unsigned & theRelease) {
        std::string myVersion = (const char*)glGetString(GL_VERSION);
        std::vector<std::string> myVendorSplit = asl::splitString(myVersion, " ");
        if (myVendorSplit.size()) {
            std::vector<std::string> myVersionSplit = asl::splitString(myVendorSplit[0], ".");
            if (myVersionSplit.size() > 1) {
                theMajor = asl::as<unsigned>(myVersionSplit[0]);
                theMinor = asl::as<unsigned>(myVersionSplit[1]);
                if (myVersionSplit.size() > 2) {
                    theRelease = asl::as<unsigned>(myVersionSplit[2]);
                } else {
                    theRelease = 0;
                }
            } else {
                throw CantParseOpenGLVersion(JUST_FILE_LINE);
            }
        } else {
            throw CantQueryOpenGLVersion(JUST_FILE_LINE);
        }
    }

#ifdef WIN32
    bool queryWGLExtension(const char *extension)
    {
    	const size_t extlen = strlen(extension);
    	const char *supported = NULL;

    	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
    	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

    	if (wglGetExtString)
    		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

    	// If That Failed, Try Standard Opengl Extensions String
    	if (supported == NULL)
    		supported = (char*)glGetString(GL_EXTENSIONS);

    	// If That Failed Too, Must Be No Extensions Supported
    	if (supported == NULL)
    		return false;

    	// Begin Examination At Start Of String, Increment By 1 On False Match
    	for (const char* p = supported; ; p++)
    	{
    		// Advance p Up To The Next Possible Match
    		p = strstr(p, extension);

    		if (p == NULL)
    			return false; // No Match

    		// Make Sure That Match Is At The Start Of The String Or That
    		// The Previous Char Is A Space, Or Else We Could Accidentally
    		// Match "wglFunkywglExtension" With "wglExtension"

    		// Also, Make Sure That The Following Character Is Space Or NULL
    		// Or Else "wglExtensionTwo" Might Match "wglExtension"
    		if ((p==supported || p[-1]==' ') && (p[extlen]=='\0' || p[extlen]==' '))
    			return true; // Match
    	}
    }
#endif

    bool queryOGLExtension(const char *extName, bool theVerboseFlag)
    {
        /*
        ** Search for extName in the extensions string. Use of strstr()
        ** is not sufficient because extension names can be prefixes of
        ** other extension names. Could use strtok() but the constant
        ** string returned by glGetString might be in read-only memory.
        */

        char * p = (char *)glGetString(GL_EXTENSIONS);
        if (NULL == p) {
            AC_ERROR << "GL_EXTENSIONS is NULL; is there a valid GL context?";
            return false;
        }

        int extNameLen = strlen(extName);
        char * end = p + strlen(p);
        while (p < end) {
            int n = strcspn(p, " ");
            if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
                AC_INFO << "Extension " << extName << " is supported";
                return true;
            }
            p += (n + 1);
        }

        AC_INFO << "Extension " << extName << " is not supported";
        return false;
    }

    DEFINE_EXCEPTION(GlTextureFunctionException, asl::Exception);

    GLenum
    asGLTextureWrapmode(TextureWrapMode theWrapMode) {
        GLenum myTexWrapMode;
        switch(theWrapMode) {
            case CLAMP:
               myTexWrapMode = GL_CLAMP;
               break;
            case CLAMP_TO_EDGE:
               myTexWrapMode = GL_CLAMP_TO_EDGE;
               break;
            case REPEAT:
               myTexWrapMode = GL_REPEAT;
               break;
            default:
               throw GlTextureFunctionException("Unknown texture wrap mode.", PLUS_FILE_LINE);
        }
        return myTexWrapMode;
    }
    
    GLenum
    asGLTextureSampleFilter(TextureSampleFilter theSampleFilter, bool theMipmapsFlag) {
        GLenum myTexSampleFilter;
        switch(theSampleFilter) {
            case LINEAR:
               myTexSampleFilter = theMipmapsFlag ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR;
               break;
            case NEAREST:
               myTexSampleFilter = theMipmapsFlag ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST;
               break;
            default:
               throw GlTextureFunctionException("Unknown texture mag filter mode.", PLUS_FILE_LINE);
        }
        return myTexSampleFilter;
    }

    GLenum
    asGLTextureFunc(TextureApplyMode theApplyMode) {
        switch(theApplyMode) {
            case MODULATE:
               return GL_MODULATE;
            case DECAL:
               return GL_DECAL;
            case REPLACE:
               return GL_REPLACE;
            case BLEND:
               return GL_BLEND;
            case ADD:
               return GL_ADD;
            default:
               throw GlTextureFunctionException("Unknown texture function.", PLUS_FILE_LINE);
        }
        return GLenum(0);
    }

    DEFINE_EXCEPTION(GLTextureUnknownInternalFormat, asl::Exception);

    GLenum asGLTextureInternalFormat(TextureInternalFormat theFormat) {
        switch(theFormat) {
            case TEXTURE_IFMT_DEPTH: return GL_DEPTH;
            case TEXTURE_IFMT_ALPHA: return GL_ALPHA;
            case TEXTURE_IFMT_ALPHA4: return GL_ALPHA4;
            case TEXTURE_IFMT_ALPHA8: return GL_ALPHA8;
            case TEXTURE_IFMT_ALPHA12: return GL_ALPHA12;
            case TEXTURE_IFMT_ALPHA16: return GL_ALPHA16;
            case TEXTURE_IFMT_LUMINANCE: return GL_LUMINANCE;
            case TEXTURE_IFMT_LUMINANCE4: return GL_LUMINANCE4;
            case TEXTURE_IFMT_LUMINANCE8: return GL_LUMINANCE8;
            case TEXTURE_IFMT_LUMINANCE12: return GL_LUMINANCE12;
            case TEXTURE_IFMT_LUMINANCE16: return GL_LUMINANCE16;
            case TEXTURE_IFMT_LUMINANCE_ALPHA: return GL_LUMINANCE_ALPHA;
            case TEXTURE_IFMT_LUMINANCE4_ALPHA4: return GL_LUMINANCE4_ALPHA4;
            case TEXTURE_IFMT_LUMINANCE6_ALPHA2: return GL_LUMINANCE6_ALPHA2;
            case TEXTURE_IFMT_LUMINANCE8_ALPHA8: return GL_LUMINANCE8_ALPHA8;
            case TEXTURE_IFMT_LUMINANCE12_ALPHA4: return GL_LUMINANCE12_ALPHA4;
            case TEXTURE_IFMT_LUMINANCE12_ALPHA12: return GL_LUMINANCE12_ALPHA12;
            case TEXTURE_IFMT_LUMINANCE16_ALPHA16: return GL_LUMINANCE16_ALPHA16;
            case TEXTURE_IFMT_INTENSITY: return GL_INTENSITY;
            case TEXTURE_IFMT_INTENSITY4: return GL_INTENSITY4;
            case TEXTURE_IFMT_INTENSITY8: return GL_INTENSITY8;
            case TEXTURE_IFMT_INTENSITY12: return GL_INTENSITY12;
            case TEXTURE_IFMT_INTENSITY16: return GL_INTENSITY16;
            case TEXTURE_IFMT_RGB: return GL_RGB;
            case TEXTURE_IFMT_R3_G3_B2: return GL_R3_G3_B2;
            case TEXTURE_IFMT_RGB4: return GL_RGB4;
            case TEXTURE_IFMT_RGB5: return GL_RGB5;
            case TEXTURE_IFMT_RGB8: return GL_RGB8;
            case TEXTURE_IFMT_RGB10: return GL_RGB10;
            case TEXTURE_IFMT_RGB12: return GL_RGB12;
            case TEXTURE_IFMT_RGB16: return GL_RGB16;
            case TEXTURE_IFMT_RGBA: return GL_RGBA;
            case TEXTURE_IFMT_RGBA2: return GL_RGBA2;
            case TEXTURE_IFMT_RGBA4: return GL_RGBA4;
            case TEXTURE_IFMT_RGB5_A1: return GL_RGB5_A1;
            case TEXTURE_IFMT_RGBA8: return GL_RGBA8;
            case TEXTURE_IFMT_RGB10_A2: return GL_RGB10_A2;
            case TEXTURE_IFMT_RGBA12: return GL_RGBA12;
            case TEXTURE_IFMT_RGBA16: return GL_RGBA16;
            default:
               throw GLTextureUnknownInternalFormat("Unknown internal texture format.", PLUS_FILE_LINE);
        }
        return GLenum(0);
    };

    TextureInternalFormat fromGLTextureInternalFormat(GLenum theFormat) {
        switch(theFormat) {
            case GL_DEPTH: return TEXTURE_IFMT_DEPTH;
            case GL_ALPHA: return TEXTURE_IFMT_ALPHA;
            case GL_ALPHA4: return TEXTURE_IFMT_ALPHA4;
            case GL_ALPHA8: return TEXTURE_IFMT_ALPHA8;
            case GL_ALPHA12: return TEXTURE_IFMT_ALPHA12;
            case GL_ALPHA16: return TEXTURE_IFMT_ALPHA16;
            case GL_LUMINANCE: return TEXTURE_IFMT_LUMINANCE;
            case GL_LUMINANCE4: return TEXTURE_IFMT_LUMINANCE4;
            case GL_LUMINANCE8: return TEXTURE_IFMT_LUMINANCE8;
            case GL_LUMINANCE12: return TEXTURE_IFMT_LUMINANCE12;
            case GL_LUMINANCE16: return TEXTURE_IFMT_LUMINANCE16;
            case GL_LUMINANCE_ALPHA: return TEXTURE_IFMT_LUMINANCE_ALPHA;
            case GL_LUMINANCE4_ALPHA4: return TEXTURE_IFMT_LUMINANCE4_ALPHA4;
            case GL_LUMINANCE6_ALPHA2: return TEXTURE_IFMT_LUMINANCE6_ALPHA2;
            case GL_LUMINANCE8_ALPHA8: return TEXTURE_IFMT_LUMINANCE8_ALPHA8;
            case GL_LUMINANCE12_ALPHA4: return TEXTURE_IFMT_LUMINANCE12_ALPHA4;
            case GL_LUMINANCE12_ALPHA12: return TEXTURE_IFMT_LUMINANCE12_ALPHA12;
            case GL_LUMINANCE16_ALPHA16: return TEXTURE_IFMT_LUMINANCE16_ALPHA16;
            case GL_INTENSITY: return TEXTURE_IFMT_INTENSITY;
            case GL_INTENSITY4: return TEXTURE_IFMT_INTENSITY4;
            case GL_INTENSITY8: return TEXTURE_IFMT_INTENSITY8;
            case GL_INTENSITY12: return TEXTURE_IFMT_INTENSITY12;
            case GL_INTENSITY16: return TEXTURE_IFMT_INTENSITY16;
            case GL_RGB: return TEXTURE_IFMT_RGB;
            case GL_R3_G3_B2: return TEXTURE_IFMT_R3_G3_B2;
            case GL_RGB4: return TEXTURE_IFMT_RGB4;
            case GL_RGB5: return TEXTURE_IFMT_RGB5;
            case GL_RGB8: return TEXTURE_IFMT_RGB8;
            case GL_RGB10: return TEXTURE_IFMT_RGB10;
            case GL_RGB12: return TEXTURE_IFMT_RGB12;
            case GL_RGB16: return TEXTURE_IFMT_RGB16;
            case GL_RGBA: return TEXTURE_IFMT_RGBA;
            case GL_RGBA2: return TEXTURE_IFMT_RGBA2;
            case GL_RGBA4: return TEXTURE_IFMT_RGBA4;
            case GL_RGB5_A1: return TEXTURE_IFMT_RGB5_A1;
            case GL_RGBA8: return TEXTURE_IFMT_RGBA8;
            case GL_RGB10_A2: return TEXTURE_IFMT_RGB10_A2;
            case GL_RGBA12: return TEXTURE_IFMT_RGBA12;
            case GL_RGBA16: return TEXTURE_IFMT_RGBA16;
            default:
               throw GLTextureUnknownInternalFormat("Unknown internal texture format.", PLUS_FILE_LINE);
        }
        return TextureInternalFormat(0);
    };

    DEFINE_EXCEPTION(GLBlendFunctionOutOfRangeException, asl::Exception);
    
    GLenum
    asGLBlendFunction(BlendFunction theBlendFunction) {
        GLenum myBlendFunc;
        switch (theBlendFunction) {
            case ONE:
                myBlendFunc = GL_ONE;
                break;
            case ZERO:
                myBlendFunc = GL_ZERO;
                break;
            case DST_COLOR:
                myBlendFunc = GL_DST_COLOR;
                break;
            case SRC_COLOR:
                myBlendFunc = GL_SRC_COLOR;
                break;
            case ONE_MINUS_DST_COLOR:
                myBlendFunc = GL_ONE_MINUS_DST_COLOR;
                break;
            case ONE_MINUS_SRC_COLOR:
                myBlendFunc = GL_ONE_MINUS_SRC_COLOR;
                break;
            case SRC_ALPHA:
                myBlendFunc = GL_SRC_ALPHA;
                break;
            case ONE_MINUS_SRC_ALPHA:
                myBlendFunc = GL_ONE_MINUS_SRC_ALPHA;
                break;
            case DST_ALPHA:
                myBlendFunc = GL_DST_ALPHA;
                break;
            case ONE_MINUS_DST_ALPHA:
                myBlendFunc = GL_ONE_MINUS_DST_ALPHA;
                break;
            case SRC_ALPHA_SATURATE:
                myBlendFunc = GL_SRC_ALPHA_SATURATE;
                break;
            case CONSTANT_COLOR:
                myBlendFunc = GL_CONSTANT_COLOR;
                break;
            case ONE_MINUS_CONSTANT_COLOR:
                myBlendFunc = GL_ONE_MINUS_CONSTANT_COLOR;
                break;
            case CONSTANT_ALPHA:
                myBlendFunc = GL_CONSTANT_ALPHA;
                break;
            case ONE_MINUS_CONSTANT_ALPHA:
                myBlendFunc = GL_ONE_MINUS_CONSTANT_ALPHA;
                break;
            default:
                throw GLBlendFunctionOutOfRangeException("Blend function out of range", PLUS_FILE_LINE);
                break;
        }
        return myBlendFunc;
    }

    DEFINE_EXCEPTION(GlLightOutOfRangeException, asl::Exception);

    GLenum
    asGLLightEnum(unsigned theLightNum) {
#if 1
        if (theLightNum >= 0 && theLightNum <= 7) {
            return GL_LIGHT0 + theLightNum;
        }
        throw GlLightOutOfRangeException("too many light sources",PLUS_FILE_LINE);
#else
        switch (theLightNum) {
            case 0 :
                return GL_LIGHT0;
            case 1 :
                return GL_LIGHT1;
            case 2 :
                return GL_LIGHT2;
            case 3 :
                return GL_LIGHT3;
            case 4 :
                return GL_LIGHT4;
            case 5 :
                return GL_LIGHT5;
            case 6 :
                return GL_LIGHT6;
            case 7 :
                return GL_LIGHT7;
            default :
                throw GlLightOutOfRangeException("too many light sources",PLUS_FILE_LINE);
        }
#endif
    }

    DEFINE_EXCEPTION(GlPlaneOutOfRangeException, asl::Exception);
    GLenum 
    asGLClippingPlaneId(unsigned thePlaneNum) {
        switch (thePlaneNum) {
            case 0 :
                return GL_CLIP_PLANE0;
            case 1 :
                return GL_CLIP_PLANE1;
            case 2 :
                return GL_CLIP_PLANE2;
            case 3 :
                return GL_CLIP_PLANE3;
            case 4 :
                return GL_CLIP_PLANE4;
            case 5 :
                return GL_CLIP_PLANE5;
            default :
                throw GlPlaneOutOfRangeException(
                        std::string("too many Clipping Planes, max index = 5, got ") +
                        asl::as_string(thePlaneNum), PLUS_FILE_LINE);
        }
    }

    DEFINE_EXCEPTION(GlTextureOutOfRangeException, asl::Exception);

    GLenum
    asGLTextureRegister(unsigned theIndex) {
#if 1
        if (theIndex >= 0 && theIndex <= 7) {
            return GL_TEXTURE0_ARB + theIndex;
        }
        throw GlTextureOutOfRangeException("Primitive uses more texture units than currently implemented", PLUS_FILE_LINE);
#else
        GLenum myUnit;
        switch (theIndex) {
            case 0:
                myUnit = GL_TEXTURE0_ARB;
                break;
            case 1:
                myUnit = GL_TEXTURE1_ARB;
                break;
            case 2:
                myUnit = GL_TEXTURE2_ARB;
                break;
            case 3:
                myUnit = GL_TEXTURE3_ARB;
                break;
            case 4:
                myUnit = GL_TEXTURE4_ARB;
                break;
            case 5:
                myUnit = GL_TEXTURE5_ARB;
                break;
            case 6:
                myUnit = GL_TEXTURE6_ARB;
                break;
            case 7:
                myUnit = GL_TEXTURE7_ARB;
                break;
            default:
                throw GlTextureOutOfRangeException("Primitive uses more texture units than currently implemented", PLUS_FILE_LINE);
        }
        return myUnit;
#endif
    }

    GLenum
    asGLTextureRegister(GLRegister theRegister) {
        switch (theRegister) {
            case TEXCOORD0_REGISTER:
                return GL_TEXTURE0_ARB;
            case TEXCOORD1_REGISTER:
                return GL_TEXTURE1_ARB;
            case TEXCOORD2_REGISTER:
                return GL_TEXTURE2_ARB;
            case TEXCOORD3_REGISTER:
                return GL_TEXTURE3_ARB;
            case TEXCOORD4_REGISTER:
                return GL_TEXTURE4_ARB;
            case TEXCOORD5_REGISTER:
                return GL_TEXTURE5_ARB;
            case TEXCOORD6_REGISTER:
                return GL_TEXTURE6_ARB;
            case TEXCOORD7_REGISTER:
                return GL_TEXTURE7_ARB;
            default:
                throw GlTextureOutOfRangeException(std::string("Cannot convert texture register ") + 
                    asl::as_string(theRegister) + " to gl register.", PLUS_FILE_LINE);
        }
        return GL_TEXTURE0_ARB;
    }
    
    GLenum 
    asGLTexCoordMode(TexCoordMode theMode) {
        switch (theMode) {
            case OBJECT_LINEAR:
                return GL_OBJECT_LINEAR;
            case EYE_LINEAR:
                return GL_EYE_LINEAR;
            case SPHERE_MAP:
                return GL_SPHERE_MAP;
            default:
                throw asl::Exception("Unkown TexCoordMode", PLUS_FILE_LINE);
        }
        return GL_OBJECT_LINEAR;
    }
 
#ifdef WIN32
#define SET_PROC_ADDRESS(p,x) \
    x = (p) wglGetProcAddress( #x ); \
    if (!x) AC_ERROR << "Unable to get proc address for " << #x
#endif    
    
#ifdef LINUX
#define SET_PROC_ADDRESS(p,x) \
    _ac_ ## x = (p) glXGetProcAddressARB((const GLubyte*) #x ); \
    if (!x) AC_ERROR << "Unable to get proc address for " << #x
#endif
    
    void
    initGLExtensions(unsigned int /*theNeededExtensions*/,
                     bool /*theVerboseFlag*/)
    {
        unsigned myVersionMajor = 0;
        unsigned myVersionMinor = 0;
        unsigned myVersionRelease = 0;
        queryGLVersion(myVersionMajor, myVersionMinor, myVersionRelease);
        AC_DEBUG << "Found OpenGL " << myVersionMajor << "." << myVersionMinor << " rel " << myVersionRelease;

        if (queryOGLExtension("GL_ARB_vertex_buffer_object") ||
            queryOGLExtension("GL_EXT_vertex_buffer_object")) {

            /* ARB_vertex_buffer_object */
            SET_PROC_ADDRESS( PFNGLBINDBUFFERARBPROC, glBindBufferARB );
            SET_PROC_ADDRESS( PFNGLDELETEBUFFERSARBPROC, glDeleteBuffersARB );
            SET_PROC_ADDRESS( PFNGLGENBUFFERSARBPROC, glGenBuffersARB );
            SET_PROC_ADDRESS( PFNGLISBUFFERARBPROC, glIsBufferARB );
            SET_PROC_ADDRESS( PFNGLBUFFERDATAARBPROC, glBufferDataARB );
            SET_PROC_ADDRESS( PFNGLBUFFERSUBDATAARBPROC, glBufferSubDataARB );
            SET_PROC_ADDRESS( PFNGLGETBUFFERSUBDATAARBPROC, glGetBufferSubDataARB );
            SET_PROC_ADDRESS( PFNGLMAPBUFFERARBPROC, glMapBufferARB );
            SET_PROC_ADDRESS( PFNGLUNMAPBUFFERARBPROC, glUnmapBufferARB );
            SET_PROC_ADDRESS( PFNGLGETBUFFERPARAMETERIVARBPROC, glGetBufferParameterivARB );
            SET_PROC_ADDRESS( PFNGLGETBUFFERPOINTERVARBPROC, glGetBufferPointervARB );

            /* OpenGL core 1.5 functions without ARB-identifier */
            
            SET_PROC_ADDRESS( PFNGLBINDBUFFERPROC, glBindBuffer );
            SET_PROC_ADDRESS( PFNGLDELETEBUFFERSPROC, glDeleteBuffers );
            SET_PROC_ADDRESS( PFNGLGENBUFFERSPROC, glGenBuffers );
            SET_PROC_ADDRESS( PFNGLISBUFFERPROC, glIsBuffer );
            SET_PROC_ADDRESS( PFNGLBUFFERDATAPROC, glBufferData );
            SET_PROC_ADDRESS( PFNGLBUFFERSUBDATAPROC, glBufferSubData );
            SET_PROC_ADDRESS( PFNGLGETBUFFERSUBDATAPROC, glGetBufferSubData );
            SET_PROC_ADDRESS( PFNGLMAPBUFFERPROC, glMapBuffer );
            SET_PROC_ADDRESS( PFNGLUNMAPBUFFERPROC, glUnmapBuffer );
            SET_PROC_ADDRESS( PFNGLGETBUFFERPARAMETERIVPROC, glGetBufferParameteriv );
            SET_PROC_ADDRESS( PFNGLGETBUFFERPOINTERVPROC, glGetBufferPointerv );

            SET_PROC_ADDRESS( PFNGLFLUSHVERTEXARRAYRANGENVPROC, glFlushVertexArrayRangeNV );
            SET_PROC_ADDRESS( PFNGLVERTEXARRAYRANGENVPROC, glVertexArrayRangeNV );
 
            if (!(glBindBuffer && 
                glDeleteBuffers &&
                glGenBuffers &&
                glIsBuffer &&
                glBufferData &&
                glBufferSubData &&
                glGetBufferSubData &&
                glMapBuffer &&
                glUnmapBuffer &&
                glGetBufferParameteriv &&
                glGetBufferPointerv)) {
                AC_WARNING << "Only ARB versions of vertex_buffer_object extension available";
            }
        }

        // cubemap extensions
        if (queryOGLExtension("GL_ARB_texture_cube_map") ||
            queryOGLExtension("GL_EXT_texture_cube_map")) {
        }
        
        // texture compression
        if (queryOGLExtension("GL_ARB_texture_compression") ||
            queryOGLExtension("GL_EXT_texture_compression_s3tc")) {
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE1DARBPROC, glCompressedTexImage1DARB );
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE2DARBPROC, glCompressedTexImage2DARB );
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXIMAGE3DARBPROC, glCompressedTexImage3DARB );
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE1DARBPROC, glCompressedTexSubImage1DARB );
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE2DARBPROC, glCompressedTexSubImage2DARB );
            SET_PROC_ADDRESS( PFNGLCOMPRESSEDTEXSUBIMAGE3DARBPROC, glCompressedTexSubImage3DARB );
            SET_PROC_ADDRESS( PFNGLGETCOMPRESSEDTEXIMAGEARBPROC, glGetCompressedTexImageARB );
        }

        // multitexture
        if (queryOGLExtension("GL_ARB_multitexture")) {
#ifdef WIN32
            glh_init_extensions("GL_ARB_multitexture");
#endif
            /* Retrieve some ARB_multitexture routines. */
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD2IARBPROC, glMultiTexCoord2iARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD3FARBPROC, glMultiTexCoord3fARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD3FVARBPROC, glMultiTexCoord3fvARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD2FARBPROC, glMultiTexCoord2fARB );
            SET_PROC_ADDRESS( PFNGLACTIVETEXTUREARBPROC,glActiveTextureARB );
            SET_PROC_ADDRESS( PFNGLCLIENTACTIVETEXTUREARBPROC, glClientActiveTextureARB );
        }        
        
      // DS: for some reason this extension is not in the nvidia extension list but I've
      //     seen a demo using it. According to the GL_NV_point_sprite extension spec it
      //     uses exact the same tokens as GL_ARB_point_sprite. So they are exchangable.
      //     the token names are different but we defined these tokens ourself in 
      //     glExtensions.h anyway.
        // point sprites
        if (queryOGLExtension("GL_ARB_point_sprite") ||
            queryOGLExtension("GL_NV_point_sprite")) {
        }

        // point parameters
        if (queryOGLExtension("GL_ARB_point_parameters")) {
            SET_PROC_ADDRESS( PFNGLPOINTPARAMETERFARBPROC, glPointParameterfARB );
            SET_PROC_ADDRESS( PFNGLPOINTPARAMETERFVARBPROC, glPointParameterfvARB );
        }
        
        // 3d texture
        if (myVersionMajor >= 2 || myVersionMinor >= 2) {
            GLint myMaxSize = 0;
            glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &myMaxSize);
            AC_INFO << "Max.texture size 3D " << myMaxSize;
            SET_PROC_ADDRESS( PFNGLTEXIMAGE3DPROC, glTexImage3D );
            SET_PROC_ADDRESS( PFNGLTEXSUBIMAGE3DPROC, glTexSubImage3D );
            SET_PROC_ADDRESS( PFNGLCOPYTEXSUBIMAGE3DPROC, glCopyTexSubImage3D );
        } else {
            AC_WARNING << "Probably no 3D texture support";
        }

        // frame buffer objects
#ifdef GL_EXT_framebuffer_object
        if (queryOGLExtension("GL_EXT_framebuffer_object")) {
            //these are not all but just those needed in OffScreenRenderArea
            SET_PROC_ADDRESS( PFNGLGENRENDERBUFFERSEXTPROC, glGenRenderbuffersEXT );
            SET_PROC_ADDRESS( PFNGLGENFRAMEBUFFERSEXTPROC, glGenFramebuffersEXT );
            SET_PROC_ADDRESS( PFNGLBINDFRAMEBUFFEREXTPROC, glBindFramebufferEXT );
            SET_PROC_ADDRESS( PFNGLBINDRENDERBUFFEREXTPROC, glBindRenderbufferEXT );
            SET_PROC_ADDRESS( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC, glFramebufferTexture2DEXT );
            SET_PROC_ADDRESS( PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC, glFramebufferRenderbufferEXT );
            SET_PROC_ADDRESS( PFNGLRENDERBUFFERSTORAGEEXTPROC, glRenderbufferStorageEXT );
            SET_PROC_ADDRESS( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, glCheckFramebufferStatusEXT );
        }
#endif
#if 0        
        GLuint myTest;
        cerr << " test... " << endl;
        glGenFramebuffersEXT = (PFNGLGENRENDERBUFFERSEXTPROC) 
            glXGetProcAddressARB( (const GLubyte *)"glGenFramebuffersEXT" );
        glGenFramebuffersEXT( 1, &myTest);
        cerr << " test done. " << endl;
#endif        
    }

    bool hasCap(const string & theCapStr) {        
        return queryOGLExtension(theCapStr.c_str());
    }
    
    bool hasCap(unsigned int theCap) {        
        bool myResult = false;
        switch (theCap) {
            case CUBEMAP_SUPPORT:
                myResult = queryOGLExtension("GL_ARB_texture_cube_map") || queryOGLExtension("GL_EXT_texture_cube_map");
                break;
            case TEXTURECOMPRESSION_SUPPORT:
                myResult = queryOGLExtension("GL_ARB_texture_compression") || queryOGLExtension("GL_EXT_texture_compression_s3tc");
                break;
            case MULTITEXTURE_SUPPORT :
                myResult = queryOGLExtension("GL_ARB_multitexture");
                break;
            case POINT_SPRITE_SUPPORT :
                myResult = queryOGLExtension("GL_NV_point_sprite") || queryOGLExtension("GL_ARB_point_sprite");
                break;
            case POINT_PARAMETER_SUPPORT :
                myResult = queryOGLExtension("GL_ARB_point_parameters");
                break;
            case TEXTURE_3D_SUPPORT :
                {
                    unsigned myVersionMajor = 0;
                    unsigned myVersionMinor = 0;
                    unsigned myVersionRelease = 0;
                    queryGLVersion(myVersionMajor, myVersionMinor, myVersionRelease);
                    myResult = ((myVersionMajor == 1 &&  myVersionMinor >= 2) || (myVersionMajor > 1));
                }
                break;
            default:
             throw OpenGLException(string("Sorry, unknown capability : ") + asl::as_string(theCap), PLUS_FILE_LINE);
            
        }
        return myResult;
    }
}
