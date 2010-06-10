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

extern "C" {
typedef void (*GdkGLProc)(void);
GdkGLProc gdk_gl_get_proc_address            (const char *proc_name);
}

#define _ac_render_GLUtils_cpp_
// own header
#include "GLUtils.h"

#include <iostream>
#include <iomanip>
#include <string.h>
#include <asl/base/Exception.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/Logger.h>

#ifndef _AC_NO_CG_
	#include <Cg/cgGL.h>
#endif
#ifdef OSX
    #include <OpenGL/CGLCurrent.h>
#endif

using namespace std;

namespace y60 {
    bool GLScopeTimer::_flushGL_before_stop = false;

    void checkOGLError(const std::string & theLocation) {
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
            std::string myErrorReport = std::string("OpenGL error: #(" + asl::as_string(err) + ") " + myErrorString);
            AC_ERROR << myErrorReport << " at " << theLocation;
            throw OpenGLException(myErrorReport, theLocation);
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

    DEFINE_EXCEPTION(GlTextureFunctionException, asl::Exception);

    GLenum
    asGLTextureWrapMode(TextureWrapMode theWrapMode) {
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
            case MIRROR:
                myTexWrapMode = GL_MIRRORED_REPEAT;
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
    asGLTextureTarget(TextureType theTextureType) {
        switch (theTextureType) {
        case TEXTURE_2D:
            return GL_TEXTURE_2D;
        case TEXTURE_RECTANGLE:
            return GL_TEXTURE_RECTANGLE_ARB;
        case TEXTURE_3D:
            return GL_TEXTURE_3D;
        case TEXTURE_CUBEMAP:
            return GL_TEXTURE_CUBE_MAP_ARB;
        default: break;
        }
        throw GlTextureFunctionException("Unknown texture type.", PLUS_FILE_LINE);
    }

    GLenum
    asGLTextureApplyMode(TextureApplyMode theApplyMode) {
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
            case SUBTRACT:
                return GL_SUBTRACT;
            case COMBINE:
                return GL_COMBINE_ARB;
            default: break;
        }
        throw GlTextureFunctionException("Unknown texture apply mode.", PLUS_FILE_LINE);
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
            default: break;
        }
        throw GLUnknownBlendEquation("Unknown blend equation.", PLUS_FILE_LINE);
    }

    DEFINE_EXCEPTION(GlCubemapIndexOutOfRange, asl::Exception);
    GLenum
    asGLCubemapFace(unsigned theFace) {
        switch (theFace) {
            case CUBEMAP_BEHIND: 
                return GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB;
            case CUBEMAP_RIGHT:
                return GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB;
            case CUBEMAP_FRONT:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB;
            case CUBEMAP_LEFT:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB;
            case CUBEMAP_TOP:
                return GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB;
            case CUBEMAP_BOTTOM:
                return GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB;
            default: break;
        }
        throw GlCubemapIndexOutOfRange("Unknown cubemap face.", PLUS_FILE_LINE);
    };


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
            case TEXTURE_IFMT_RGBA_FLOAT: return GL_RGBA_FLOAT32_ATI;
            case TEXTURE_IFMT_RGB_FLOAT: return GL_RGB_FLOAT32_ATI;
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
            default: break;
        }
        throw GLTextureUnknownInternalFormat("Unknown internal texture format.", PLUS_FILE_LINE);
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
            case GL_RGBA_FLOAT32_ATI: return TEXTURE_IFMT_RGBA_FLOAT;
            case GL_RGB_FLOAT32_ATI: return TEXTURE_IFMT_RGB_FLOAT;
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
            default: break;
        }
        throw GLTextureUnknownInternalFormat("Unknown internal texture format.", PLUS_FILE_LINE);
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

        if (static_cast<GLint>(theLightNum) <= myMaxLights) {
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
        if (static_cast<GLint>(theIndex) < myMaxTextureUnits) {
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
            default: break;
        }
        throw GlTextureOutOfRangeException(std::string("texture register out of range: ") + asl::as_string(theRegister), PLUS_FILE_LINE);
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
            default: break;
        }
        throw asl::Exception(std::string("Unkown TexCoordMode ") + asl::as_string(theMode), PLUS_FILE_LINE);
    }

    bool hasCap(const string & theCapStr) {
        bool myReturn = 0 != glewIsSupported(theCapStr.c_str());

        if (!myReturn) {
#ifdef OSX 
            CGLContextObj myContext = CGLGetCurrentContext();
            if (myContext == NULL) {
                return false;
            }
#endif
            // try again extension string,
            // maybe extension is not supported by glew-version
            const char * myExtensionsString = reinterpret_cast<const char *>(glGetString(GL_EXTENSIONS));
            if (myExtensionsString && std::string(myExtensionsString).find(theCapStr) != string::npos) {
                return true;
            }
            AC_DEBUG << "OpenGL Extension not supported: " << theCapStr;
        }
        return myReturn;
    }

    bool hasCap(unsigned int theCap) {
        bool myResult = false;
        switch (theCap) {
            case CUBEMAP_SUPPORT:
                myResult = hasCap("GL_ARB_texture_cube_map GL_EXT_texture_cube_map");
                break;
            case TEXTURECOMPRESSION_SUPPORT:
                myResult = hasCap("GL_ARB_texture_compression GL_EXT_texture_compression_s3tc");
                break;
            case MULTITEXTURE_SUPPORT :
                myResult = hasCap("GL_ARB_multitexture");
                break;
            case POINT_SPRITE_SUPPORT :
                myResult = hasCap("GL_NV_point_sprite GL_ARB_point_sprite");
                break;
            case POINT_PARAMETER_SUPPORT :
                myResult = hasCap("GL_ARB_point_parameters");
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
                myResult = hasCap("GL_EXT_framebuffer_object");
                break;
            default: break;
        }
        throw OpenGLException(string("Sorry, unknown capability : ") + asl::as_string(theCap), PLUS_FILE_LINE);
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
        string myVertexProfileName = "NONE";
        if(myVertexProfile != CG_PROFILE_UNKNOWN) {
            myVertexProfileName = string(cgGetProfileString(myVertexProfile));
        }

        CGprofile myFragmentProfile = cgGLGetLatestProfile(CG_GL_FRAGMENT);
        string myFragementProfileName = "NONE";
        if(myFragmentProfile != CG_PROFILE_UNKNOWN) {
            myFragementProfileName = string(cgGetProfileString(myFragmentProfile));
        }

        return myVertexProfileName + "/" + myFragementProfileName;
    }
#endif
}
