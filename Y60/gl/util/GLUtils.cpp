//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
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

#ifndef _AC_NO_CG_
	#include <Cg/cgGL.h>
#endif

using namespace std;

namespace y60 {

    void checkOGLError(const std::string& theLocation) {
        GLenum err = glGetError();
        if (err != GL_NO_ERROR) {
            std::string myErrorString;
            const char * myErrorCString = (const char*)(gluErrorString(err));
            if (myErrorCString) {
                myErrorString = myErrorCString;
            } else {
                // manually check for some errors not supported by gluErrorString
                switch (err) {
#ifdef GL_EXT_framebuffer_object
                    case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
                        myErrorString = "Invalid framebuffer operation (EXT_framebuffer_object)";
                        break;
#endif
                    default:
                        myErrorString = "no description found, but the number is valid";
                        break;
                }
            }
            throw OpenGLException("OpenGL error: #(" + asl::as_string(err) + ") " +
                    myErrorString, theLocation);
            if (err != GL_INVALID_OPERATION) {  // XXX: Do we really need this?
                checkOGLError(theLocation);     // [DS] no, we don't get here anyway ... see throw above
            }
        }
    }

    void queryGLVersion(unsigned & theMajor, unsigned & theMinor, unsigned & theRelease) {
        const char * myVersionPChar = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        if (!myVersionPChar) {
            theMajor = 0;
            theMinor = 0;
            theRelease = 0;
            return;
        }
        std::string myVersion(myVersionPChar);
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

    static bool queryStringForExtension(const char * supported,
                                        const char * extName) {

        if (supported == NULL) {
            return false;
        }
        bool supportedFlag = false;

        /*
        ** Search for extName in the extensions string. Use of strstr()
        ** is not sufficient because extension names can be prefixes of
        ** other extension names. Could use strtok() but the constant
        ** string returned by glGetString might be in read-only memory.
        */
        const char * p = supported;
        int extNameLen = strlen(extName);
        const char * end = p + strlen(p);

        while (p < end) {
            int n = strcspn(p, " ");
            if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
                AC_DEBUG << "Extension '" << extName << "' is supported";
                supportedFlag = true;
                break;
            }
            p += (n + 1);
        }

        if (supportedFlag) {
            const char* glQuirks = getenv("Y60_GL_QUIRKS");
            if (glQuirks && strstr(glQuirks, extName) != 0) {
                AC_WARNING << "Disabling quirky extension '" << extName << "'";
                supportedFlag = false;
            }
        } else {
            AC_INFO << "Extension '" << extName << "' is not supported";
        }

        return supportedFlag;
    }

#ifdef WIN32
    bool queryWGLExtension(const char *extension)
    {
    	const char *supported = NULL;

    	// Try To Use wglGetExtensionStringARB On Current DC, If Possible
    	PROC wglGetExtString = wglGetProcAddress("wglGetExtensionsStringARB");

    	if (wglGetExtString)
    		supported = ((char*(__stdcall*)(HDC))wglGetExtString)(wglGetCurrentDC());

    	// If That Failed, Try Standard Opengl Extensions String
    	if (supported == NULL)
    		supported = (char*)glGetString(GL_EXTENSIONS);

        return queryStringForExtension(supported, extension);
    }
#endif

#ifdef AC_USE_X11
    bool queryGLXExtension(const char *extName)
    {
        Display * dpy = glXGetCurrentDisplay();
        if (!dpy) {
            AC_ERROR << "Unable to contact display server for GLX extensions";
            return false;
        }

        const char * supports = glXQueryExtensionsString(dpy, 0);
        bool supported = queryStringForExtension(supports, extName);

        return supported;
    }
#endif

    bool queryOGLExtension(const char *extName, bool)
    {
        return queryStringForExtension((char*) glGetString(GL_EXTENSIONS), extName);
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
                throw GlTextureFunctionException("Unknown texture apply mode.", PLUS_FILE_LINE);
        }
        return GLenum(0);
    }

    GLenum
    asGLBlendEquation(const BlendEquation & theBlendEquation) {
        switch (theBlendEquation) {
            case EQUATION_MIN:
                return GL_MIN;
            case EQUATION_MAX:
                return GL_MAX;
            case EQUATION_ADD:
                return GL_FUNC_ADD;
            case EQUATION_SUBTRACT:
                return GL_FUNC_SUBTRACT;
            case EQUATION_REVERSE_SUBTRACT:
                return GL_FUNC_REVERSE_SUBTRACT;
            default:
                throw GLUnknownBlendEquation("Unknown blend equation", PLUS_FILE_LINE);
                break;
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
            case TEXTURE_IFMT_RGBA_HALF: return GL_RGBA16F_ARB;
            case TEXTURE_IFMT_RGB_HALF: return GL_RGB16F_ARB;
            case TEXTURE_IFMT_RGBA_FLOAT: return GL_RGBA32F_ARB;
            case TEXTURE_IFMT_RGB_FLOAT: return GL_RGB32F_ARB;
            case TEXTURE_IFMT_COMPRESSED_RGB_ARB: return GL_COMPRESSED_RGB_ARB;
            case TEXTURE_IFMT_COMPRESSED_RGBA_ARB: return GL_COMPRESSED_RGBA_ARB;
            case TEXTURE_IFMT_COMPRESSED_ALPHA_ARB: return GL_COMPRESSED_ALPHA_ARB;
            case TEXTURE_IFMT_COMPRESSED_LUMINANCE_ARB: return GL_COMPRESSED_LUMINANCE_ARB;
            case TEXTURE_IFMT_COMPRESSED_LUMINANCE_ALPHA_ARB: return GL_COMPRESSED_LUMINANCE_ALPHA_ARB;
            case TEXTURE_IFMT_COMPRESSED_INTENSITY_ARB: return GL_COMPRESSED_INTENSITY_ARB;
            case TEXTURE_IFMT_COMPRESSED_RGB_S3TC_DXT1_EXT: return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
            case TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT1_EXT: return GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT3_EXT: return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT5_EXT: return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
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
            case GL_RGBA16F_ARB: return TEXTURE_IFMT_RGBA_HALF;
            case GL_RGB16F_ARB: return TEXTURE_IFMT_RGB_HALF;
            case GL_RGBA32F_ARB: return TEXTURE_IFMT_RGBA_FLOAT;
            case GL_RGB32F_ARB: return TEXTURE_IFMT_RGB_FLOAT;
            case GL_COMPRESSED_RGB_ARB: return TEXTURE_IFMT_COMPRESSED_RGB_ARB;
            case GL_COMPRESSED_RGBA_ARB: return TEXTURE_IFMT_COMPRESSED_RGBA_ARB;
            case GL_COMPRESSED_ALPHA_ARB: return TEXTURE_IFMT_COMPRESSED_ALPHA_ARB;
            case GL_COMPRESSED_LUMINANCE_ARB: return TEXTURE_IFMT_COMPRESSED_LUMINANCE_ARB;
            case GL_COMPRESSED_LUMINANCE_ALPHA_ARB: return TEXTURE_IFMT_COMPRESSED_LUMINANCE_ALPHA_ARB;
            case GL_COMPRESSED_INTENSITY_ARB: return TEXTURE_IFMT_COMPRESSED_INTENSITY_ARB;
            case GL_COMPRESSED_RGB_S3TC_DXT1_EXT: return TEXTURE_IFMT_COMPRESSED_RGB_S3TC_DXT1_EXT;
            case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT: return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: return TEXTURE_IFMT_COMPRESSED_RGBA_S3TC_DXT5_EXT;
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
        static GLint myMaxLights = 0;
        if (myMaxLights == 0) {
            glGetIntegerv(GL_MAX_LIGHTS, &myMaxLights);
        }

        if (theLightNum <= myMaxLights) {
            return GL_LIGHT0 + theLightNum;
        }
        throw GlLightOutOfRangeException(string("light source out of range: ") + asl::as_string(theLightNum) + ", max: " + asl::as_string(myMaxLights),PLUS_FILE_LINE);
    }

    DEFINE_EXCEPTION(GlPlaneOutOfRangeException, asl::Exception);
    GLenum
    asGLClippingPlaneId(unsigned thePlaneNum) {
        if (thePlaneNum <= 5) {
            return GL_CLIP_PLANE0 + thePlaneNum;
        }
        throw GlPlaneOutOfRangeException(string("clipping plane out of range: ") + asl::as_string(thePlaneNum), PLUS_FILE_LINE);
    }

    DEFINE_EXCEPTION(GlTextureOutOfRangeException, asl::Exception);
    GLenum
    asGLTextureRegister(unsigned theIndex) {
        static GLint myMaxTextureUnits = 8;
        if (myMaxTextureUnits == 0) {
            glGetIntegerv(GL_MAX_TEXTURE_UNITS, &myMaxTextureUnits);
        }
        if (theIndex < myMaxTextureUnits) {
            return GL_TEXTURE0 + theIndex;
        }
        throw GlTextureOutOfRangeException(string("texture unit out of range: ") + asl::as_string(theIndex) + ", max: " + asl::as_string(myMaxTextureUnits),PLUS_FILE_LINE);
    }

    GLenum
    asGLTextureRegister(GLRegister theRegister) {
        switch (theRegister) {
            case TEXCOORD0_REGISTER:
                return GL_TEXTURE0;
            case TEXCOORD1_REGISTER:
                return GL_TEXTURE1;
            case TEXCOORD2_REGISTER:
                return GL_TEXTURE2;
            case TEXCOORD3_REGISTER:
                return GL_TEXTURE3;
            case TEXCOORD4_REGISTER:
                return GL_TEXTURE4;
            case TEXCOORD5_REGISTER:
                return GL_TEXTURE5;
            case TEXCOORD6_REGISTER:
                return GL_TEXTURE6;
            case TEXCOORD7_REGISTER:
                return GL_TEXTURE7;
            default:
                throw GlTextureOutOfRangeException(std::string("texture register out of range: ") + asl::as_string(theRegister), PLUS_FILE_LINE);
        }
        return GL_TEXTURE0;
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
            case REFLECTION:
                return GL_REFLECTION_MAP_EXT;
            default:
                throw asl::Exception(std::string("Unkown TexCoordMode ") + asl::as_string(theMode), PLUS_FILE_LINE);
        }
        return GL_OBJECT_LINEAR;
    }

#ifdef WIN32
#define SET_PROC_ADDRESS(p,x) \
    x = (p) wglGetProcAddress( #x ); \
    if (!x) { \
        x = Missing_ ## x; \
        AC_DEBUG << "GL Extension missing:" << #x << endl; \
    } else { \
        AC_DEBUG << "GL Extension available:" << #x << endl; \
    }
#endif

#ifdef AC_USE_X11
#define SET_PROC_ADDRESS(p,x) \
    _ac_ ## x = (p) glXGetProcAddressARB((const GLubyte*) #x ); \
    if (!x) { \
        x = Missing_ ## x; \
        AC_DEBUG << "GL Extension missing:" << #x << endl; \
    } else { \
        AC_DEBUG << "GL Extension available:" << #x << endl; \
    }
#endif

#ifdef AC_USE_OSX_CGL

// see http://developer.apple.com/qa/qa2001/qa1188.html for details

#ifdef AC_USE_NSGL

#include <mach-o/dyld.h>
#include <stdlib.h>
#include <string.h>

void * NSGLGetProcAddress (const char * name)
{
    NSSymbol symbol;
    char * symbolName;
    /* prepend a '_' for the Unix C symbol mangling convention */
    symbolName = (char*) malloc(strlen((const char*)name) + 2);
    strcpy(symbolName+1, (const char*)name);
    symbolName[0] = '_';
    symbol = NULL;
    if (NSIsSymbolNameDefined(symbolName))
        symbol = NSLookupAndBindSymbol(symbolName);
    free(symbolName);
    return symbol ? NSAddressOfSymbol(symbol) : NULL;
}

#define SET_PROC_ADDRESS(p,x) \
    _ac_ ## x = (p)  NSGLGetProcAddress( #x ); \
    if (!x) { \
        x = Missing_ ## x; \
        AC_DEBUG << "GL Extension missing:" << #x << endl; \
    } else { \
        AC_DEBUG << "GL Extension available:" << #x << endl; \
    }

#else

// Apple AGL Version
#include <Carbon/Carbon.h>

CFBundleRef gBundleRefOpenGL = NULL;

// -------------------------

OSStatus aglInitEntryPoints (void)
{
    OSStatus err = noErr;
    const Str255 frameworkName = "OpenGL.framework";
    FSRefParam fileRefParam;
    FSRef fileRef;
    CFURLRef bundleURLOpenGL;

    memset(&fileRefParam, 0, sizeof(fileRefParam));
    memset(&fileRef, 0, sizeof(fileRef));

    fileRefParam.ioNamePtr  = frameworkName;
    fileRefParam.newRef = &fileRef;

    // Frameworks directory/folder
    err = FindFolder (kSystemDomain, kFrameworksFolderType, false,
                      &fileRefParam.ioVRefNum, &fileRefParam.ioDirID);
    if (noErr != err) {
        AC_ERROR << "Could not find frameworks folder";
        return err;
    }
    err = PBMakeFSRefSync (&fileRefParam); // make FSRef for folder
    if (noErr != err) {
        AC_ERROR << "Could make FSref to frameworks folder";
        return err;
    }
    // create URL to folder
    bundleURLOpenGL = CFURLCreateFromFSRef (kCFAllocatorDefault,
                                            &fileRef);
    if (!bundleURLOpenGL) {
        AC_ERROR << "Could create OpenGL Framework bundle URL";
        return paramErr;
    }
    // create ref to GL's bundle
    gBundleRefOpenGL = CFBundleCreate (kCFAllocatorDefault,
                                       bundleURLOpenGL);
    if (!gBundleRefOpenGL) {
        AC_ERROR << "Could not create OpenGL Framework bundle";
        return paramErr;
    }
    CFRelease (bundleURLOpenGL); // release created bundle
    // if the code was successfully loaded, look for our function.
    if (!CFBundleLoadExecutable (gBundleRefOpenGL)) {
        AC_ERROR << "Could not load MachO executable";
        return paramErr;
    }
    return err;
}

// -------------------------

void aglDellocEntryPoints (void)
{
    if (gBundleRefOpenGL != NULL) {
        // unload the bundle's code.
        CFBundleUnloadExecutable (gBundleRefOpenGL);
        CFRelease (gBundleRefOpenGL);
        gBundleRefOpenGL = NULL;
    }
}

// -------------------------

void * aglGetProcAddress (char * pszProc)
{
    return CFBundleGetFunctionPointerForName (gBundleRefOpenGL,
                CFStringCreateWithCStringNoCopy (NULL,
                     pszProc, CFStringGetSystemEncoding (), NULL));
}

#define SET_PROC_ADDRESS(p,x) \
    _ac_ ## x = (p)  aglGetProcAddress( #x ); \
    if (!x) { \
        x = Missing_ ## x; \
    }

#endif

#endif


    void
    initGLExtensions(unsigned int /*theNeededExtensions*/,
                     bool /*theVerboseFlag*/,
                     bool theInitGLH_extension)
    {
        unsigned myVersionMajor = 0;
        unsigned myVersionMinor = 0;
        unsigned myVersionRelease = 0;
        queryGLVersion(myVersionMajor, myVersionMinor, myVersionRelease);
        AC_DEBUG << "Found OpenGL " << myVersionMajor << "." << myVersionMinor << " rel " << myVersionRelease;

        // don't call SET_PROC_ADDRESS to simulate a missing extenstion
        // SET_PROC_ADDRESS( PFNACTESTPROC, acTestMissingExtension );
        // call SET_PROC_ADDRESS to simulate a missing function pointer lookup
        SET_PROC_ADDRESS( PFNACTESTPROC, acTestMissingFunction );

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

            if (!(IS_SUPPORTED(glBindBuffer) &&
                IS_SUPPORTED(glDeleteBuffers) &&
                IS_SUPPORTED(glGenBuffers) &&
                IS_SUPPORTED(glIsBuffer) &&
                IS_SUPPORTED(glBufferData) &&
                IS_SUPPORTED(glBufferSubData) &&
                IS_SUPPORTED(glGetBufferSubData) &&
                IS_SUPPORTED(glMapBuffer) &&
                IS_SUPPORTED(glUnmapBuffer) &&
                IS_SUPPORTED(glGetBufferParameteriv) &&
                IS_SUPPORTED(glGetBufferPointerv))) {
                AC_WARNING << "Only ARB versions of vertex_buffer_object extension available";
            }
        }

        // cubemap extensions
        if (queryOGLExtension("GL_ARB_texture_cube_map") || queryOGLExtension("GL_EXT_texture_cube_map")) {
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
            if (theInitGLH_extension) {
                glh_init_extensions("GL_ARB_multitexture"); // who needs this (vs)? (gtk extension calling this crash)
            }
#endif
            /* Retrieve some ARB_multitexture routines. */
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD2IARBPROC, glMultiTexCoord2iARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD3FARBPROC, glMultiTexCoord3fARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD3FVARBPROC, glMultiTexCoord3fvARB );
            SET_PROC_ADDRESS( PFNGLMULTITEXCOORD2FARBPROC, glMultiTexCoord2fARB );
            SET_PROC_ADDRESS( PFNGLACTIVETEXTUREPROC, glActiveTexture );
            SET_PROC_ADDRESS( PFNGLCLIENTACTIVETEXTUREPROC, glClientActiveTexture );
        } else {
            //throw OpenGLException(string("GL_ARB_multitexture not supported but mandatory"), PLUS_FILE_LINE);
        }

        // DS: for some reason this extension is not in the nvidia extension list but I've
        //     seen a demo using it. According to the GL_NV_point_sprite extension spec it
        //     uses exact the same tokens as GL_ARB_point_sprite. So they are exchangable.
        //     the token names are different but we defined these tokens ourself in
        //     glExtensions.h anyway.
        // point sprites

        if (queryOGLExtension("GL_ARB_point_sprite") || queryOGLExtension("GL_NV_point_sprite")) {
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
            SET_PROC_ADDRESS( PFNGLDELETERENDERBUFFERSEXTPROC, glDeleteRenderbuffersEXT );

            SET_PROC_ADDRESS( PFNGLGENFRAMEBUFFERSEXTPROC, glGenFramebuffersEXT );
            SET_PROC_ADDRESS( PFNGLDELETEFRAMEBUFFERSEXTPROC, glDeleteFramebuffersEXT );

            SET_PROC_ADDRESS( PFNGLBINDFRAMEBUFFEREXTPROC, glBindFramebufferEXT );
            SET_PROC_ADDRESS( PFNGLBINDRENDERBUFFEREXTPROC, glBindRenderbufferEXT );
            SET_PROC_ADDRESS( PFNGLFRAMEBUFFERTEXTURE2DEXTPROC, glFramebufferTexture2DEXT );
            SET_PROC_ADDRESS( PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC, glFramebufferRenderbufferEXT );
            SET_PROC_ADDRESS( PFNGLRENDERBUFFERSTORAGEEXTPROC, glRenderbufferStorageEXT );
            SET_PROC_ADDRESS( PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC, glCheckFramebufferStatusEXT );
            SET_PROC_ADDRESS( PFNGLGENERATEMIPMAPEXTPROC, glGenerateMipmapEXT );
        }
#endif
#ifdef GL_EXT_framebuffer_blit
        if (queryOGLExtension("GL_EXT_framebuffer_blit")) {
            SET_PROC_ADDRESS( PFNGLBLITFRAMEBUFFEREXT, glBlitFramebufferEXT);
        }
#endif
#ifdef GL_EXT_framebuffer_multisample
        if (queryOGLExtension("GL_EXT_framebuffer_multisample")) {
            SET_PROC_ADDRESS( PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXT, glRenderbufferStorageMultisampleEXT);
        }
#endif

        SET_PROC_ADDRESS( PFNGLBLENDEQUATIONPROC, glBlendEquation );
        if (queryOGLExtension("GL_EXT_blend_func_separate")) {
            SET_PROC_ADDRESS( PFNGLBLENDFUNCSEPARATEPROC, glBlendFuncSeparate );
        }
        if (queryOGLExtension("GL_EXT_blend_color")) {
            SET_PROC_ADDRESS( PFNGLBLENDCOLORPROC, glBlendColor );
        }
        if (queryOGLExtension("GL_EXT_texture_filter_anisotropic")) {
            AC_INFO << "Anisotropic filter support";
        }

#ifdef WIN32
        if (queryWGLExtension("WGL_EXT_swap_control")) {
            SET_PROC_ADDRESS( PFNWGLSWAPINTERVALEXTPROC, wglSwapIntervalEXT );
            SET_PROC_ADDRESS( PFNWGLGETSWAPINTERVALEXTPROC, wglGetSwapIntervalEXT );
        }
#endif
#ifdef AC_USE_X11
        if (queryGLXExtension("GLX_SGI_video_sync")) {
            SET_PROC_ADDRESS( PFNGLXGETVIDEOSYNCSGIPROC, glXGetVideoSyncSGI );
            SET_PROC_ADDRESS( PFNGLXWAITVIDEOSYNCSGIPROC, glXWaitVideoSyncSGI );
        }
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
            case FRAMEBUFFER_SUPPORT:
#ifdef GL_FRAMEBUFFER_EXT
                myResult = queryOGLExtension("GL_EXT_framebuffer_object");
#else
                myResult = false;
#endif
                break;
            default:
             throw OpenGLException(string("Sorry, unknown capability : ") + asl::as_string(theCap), PLUS_FILE_LINE);
        }
        return myResult;
    }

    std::string
    getGLVersionString() {
        return string(reinterpret_cast<const char *>(glGetString(GL_VERSION)));
    }

    std::string
    getGLVendorString() {
        return string(reinterpret_cast<const char *>(glGetString(GL_VENDOR)));
    }
    std::string
    getGLRendererString() {
        return string(reinterpret_cast<const char *>(glGetString(GL_RENDERER)));
    }

    unsigned int
    getGLExtensionStrings(std::vector<std::string> & theTokens) {
        std::string myExtensions(reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS)));
        string myBuffer; // Have a buffer string
        stringstream myStream(myExtensions); // Insert the string into a stream
        theTokens.clear();

        while (myStream >> myBuffer) {
            theTokens.push_back(myBuffer);
        }
        return theTokens.size();
    }
#ifndef _AC_NO_CG_
    std::string
    getLatestCgProfileString() {
        CGprofile myVertexProfile = cgGLGetLatestProfile(CG_GL_VERTEX);
        CGprofile myFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        
        return string("Vertex profile: ") + cgGetProfileString(myVertexProfile) + 
           ", Fragment profile: " + cgGetProfileString(myFragmentProfile);
    }
#endif
}
