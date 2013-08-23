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

/*
PM: We might consider to rewrite the texture handling or GLResourceManger and scene/Texture in
a way that the ResourceManager does not modify any state in texture, but that texture more
directly reflects the GL state.
E.g. modifying the texture size should then result in an actual resize of the gl texture.
We need to find a clean way to deal with NPOT textures then.
*/

//own header
#include "GLResourceManager.h"


#include <y60/image/Image.h>
#include <y60/image/PixelEncoding.h>
#include <y60/glutil/PixelEncodingInfo.h>
#include <asl/base/file_functions.h>
#include <asl/base/Dashboard.h>

using namespace asl;

#define DB(x) // x

namespace y60 {
    const unsigned int CUBEMAP_SIDES = 6;

    GLResourceManager::~GLResourceManager() {
        AC_DEBUG << "GLResourceManager::~GLResourceManager()" << std::endl;
    }

    void
    GLResourceManager::initCaps() {
        // workaround
        _myHasVBOExtension      = hasCap("GL_ARB_vertex_buffer_object");
        _myHasPixelUnpackBuffer = hasCap("GL_ARB_pixel_buffer_object") || hasCap("GL_EXT_pixel_buffer_object");
        _myHasAnisotropicTex    = hasCap("GL_EXT_texture_filter_anisotropic");
        _myHasSGIGenerateMipMap = hasCap("GL_SGIS_generate_mipmap");
    }

    int
    GLResourceManager::getMaxTextureSize(int theDimensions) const {
        GLint myMaxSize = 0;
        switch (theDimensions) {
            case 1:
            case 2:
                glGetIntegerv(GL_MAX_TEXTURE_SIZE, &myMaxSize);
                break;
            case 3:
                glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &myMaxSize);
                break;
            default:
                throw TextureException(as_string(theDimensions)
                                       + "D Textures: size not queryable", PLUS_FILE_LINE);
        }
        return myMaxSize;
    }

    unsigned
    GLResourceManager::setupTexture(TexturePtr & theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_setupTexture);
        glPushAttrib(GL_PIXEL_MODE_BIT | GL_TEXTURE_BIT); //GL_ALL_ATTRIB_BITS);

        if (theTexture->getTextureId() > 0) {
            unbindTexture(theTexture.get());
        }

        // generate texture object
        GLuint myTextureId;
        glGenTextures(1, &myTextureId);
        CHECK_OGL_ERROR;

        // bind texture object
        GLenum myTextureTarget = asGLTextureTarget(theTexture->getType());
        glBindTexture(myTextureTarget, myTextureId);
        CHECK_OGL_ERROR;

        AC_DEBUG << "GLResourceManager::setupTexture '" << theTexture->get<NameTag>()
                 << "' id=" << theTexture->get<IdTag>() << " texTarget=0x" << std::hex
                 << myTextureTarget << std::dec << " texId=" << myTextureId;
        updatePixelTransfer(theTexture);

        switch (theTexture->getType()) {
            case TEXTURE_2D:
                setupTexture2D(theTexture);
                break;
            case TEXTURE_3D:
                setupTexture3D(theTexture);
                break;
            case TEXTURE_CUBEMAP:
                setupCubemap(theTexture);
                break;
            default:
                throw TextureException(std::string("Unknown texture type '")+
                        theTexture->get<NameTag>() + "'", PLUS_FILE_LINE);
        }

        setTextureParams(theTexture, myTextureTarget);

        glPopAttrib();
        CHECK_OGL_ERROR;

        AC_DEBUG << "Texture memory usage=" << _myTextureMemUsage / (1024.0*1024.0) << " MB";
        AC_DEBUG << "setupTexture: returning texture id = "<<myTextureId;

        return myTextureId;
    }

    void
    GLResourceManager::updateTextureData(const TexturePtr & theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateTextureData);

        ImagePtr myImage = theTexture->getImage();
        if (!myImage) {
            AC_ERROR << "Texture id=" << theTexture->get<IdTag>()
                     << " has no image associated";
            return;
        }
        if (!myImage->getRasterPtr(theTexture->get<TextureImageIndexTag>())) {
            AC_ERROR << "No raster in image id=" << myImage->get<IdTag>()
                     << " src=" << myImage->get<ImageSourceTag>();
            return;
        }

        glPushAttrib(GL_PIXEL_MODE_BIT | GL_TEXTURE_BIT); //GL_ALL_ATTRIB_BITS);

        unsigned myTextureId = theTexture->getTextureId();
        GLenum myTextureTarget = asGLTextureTarget(theTexture->getType());
        glBindTexture(myTextureTarget, myTextureId);
        CHECK_OGL_ERROR;

        AC_DEBUG << "GLResourceManager::updateTextureData '" << theTexture->get<NameTag>()
                 << "' id=" << theTexture->get<IdTag>() << " texTarget=0x" << std::hex
                 << myTextureTarget << std::dec << " texId=" << myTextureId;
        updatePixelTransfer(theTexture);

        switch (theTexture->getType()) {
        case TEXTURE_2D:
            updateTexture2D(theTexture, myImage);
            break;
        case TEXTURE_3D:
            updateTexture3D(theTexture, myImage);
            break;
        case TEXTURE_CUBEMAP:
            updateCubemap(theTexture, myImage);
            break;
        case TEXTURE_RECTANGLE:
            {
                AC_ERROR << "GLResourceManager::updateTextureData: internal error: "
                         << "encountered texture type TEXTURE_RECTANGLE, which "
                         << "should be neither used nor is supported";
            }
            break;
        default:
            {
                AC_ERROR << "GLResourceManager::updateTextureData: internal error: "
                         << "illegal texture type:" << theTexture->getType();
            }
        }
        CHECK_OGL_ERROR;

        setTextureParams(theTexture, myTextureTarget);

        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    void
    GLResourceManager::unbindTexture(Texture * theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_unbindTexture);

        if (!hasGLContext()) {
            return;
        }

        GLuint myTextureId = theTexture->getTextureId();
        if (myTextureId) {
            AC_DEBUG << "GLResourceManager::unbindTexture '" << theTexture->get<NameTag>()
                     << "' id=" << theTexture->get<IdTag>() << " texId=" << myTextureId;

            glDeleteTextures(1, &myTextureId);
            CHECK_OGL_ERROR;
            theTexture->unbind();

            // adjust texmem usage
            _myTextureMemUsage -= theTexture->getTextureMemUsage();
            AC_DEBUG << "Texture memory usage (after unbind) =" << _myTextureMemUsage / (1024.0*1024.0) << " MB";
        }

        GLuint myBufferId = theTexture->getPixelBufferId();
        if (_myHasVBOExtension && myBufferId) {
            AC_DEBUG << "GLResourceManager::unbindTexture '" << theTexture->get<NameTag>()
                     << "' id=" << theTexture->get<IdTag>() << " pboId=" << myBufferId;

            glDeleteBuffersARB(1, &myBufferId);
            theTexture->setPixelBufferId(0);
            CHECK_OGL_ERROR;
        }
    }

    static bool only_power_of_two_textures() {
        static bool has_GL_ARB_texture_non_power_of_two = y60::hasCap("GL_ARB_texture_non_power_of_two");
        static bool Y60_GL_DISABLE_NON_POWER_OF_TWO = asl::getenv("Y60_GL_DISABLE_NON_POWER_OF_TWO", false);
        return !has_GL_ARB_texture_non_power_of_two || Y60_GL_DISABLE_NON_POWER_OF_TWO;
    }

    bool
    GLResourceManager::imageMatchesGLTexture2D(TexturePtr theTexture) const {
        AC_DEBUG << "imageMatchesGLTexture2D() binding Texture " << theTexture->getTextureId();
        glBindTexture(GL_TEXTURE_2D, theTexture->getTextureId());
        CHECK_OGL_ERROR;

        // get current uploaded image size
        GLint myUploadedWidth          = -1;
        GLint myUploadedHeight         = -1;
        GLint myUploadedInternalFormat = -1;
        GLint myUploadedMinFilter      = -1;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH, &myUploadedWidth);
        CHECK_OGL_ERROR;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT, &myUploadedHeight);
        CHECK_OGL_ERROR;
        glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_INTERNAL_FORMAT,
            &myUploadedInternalFormat);
        CHECK_OGL_ERROR;

        glGetTexParameteriv(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, &myUploadedMinFilter);

        CHECK_OGL_ERROR;
        ImagePtr myImage = theTexture->getImage();
        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = myImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = myImage->getRasterPtr(myIndex)->height();

        if (only_power_of_two_textures()) {
            myWidth = nextPowerOfTwo(myWidth);
            myHeight = nextPowerOfTwo(myHeight);
        }

        GLenum myInternalFormat = asGLTextureInternalFormat(theTexture->getInternalEncoding());

        bool myOpenGLMipMapSetting = myUploadedMinFilter == GL_NEAREST_MIPMAP_NEAREST ||
            myUploadedMinFilter == GL_NEAREST_MIPMAP_LINEAR ||
            myUploadedMinFilter == GL_LINEAR_MIPMAP_NEAREST ||
            myUploadedMinFilter == GL_LINEAR_MIPMAP_LINEAR;
        bool myMipMapMatch = theTexture->get<TextureMipmapTag>() == myOpenGLMipMapSetting;
        bool mySizeMatch = (static_cast<GLint>(myWidth) == myUploadedWidth) && (static_cast<GLint>(myHeight) == myUploadedHeight);
        bool myInternalFormatMatch = static_cast<GLint>(myInternalFormat) == myUploadedInternalFormat;
        AC_DEBUG << "MipMap settings match: " << myMipMapMatch;
        AC_DEBUG << "Width match: " << mySizeMatch;
        AC_DEBUG << "Image size: " << myWidth << "x" << myHeight;
        AC_DEBUG << "Uploaded Texture size: " << myUploadedWidth << "x" << myUploadedHeight;
        AC_DEBUG << "Internal format match: " << myInternalFormatMatch;
        AC_DEBUG << "Uploaded InternalFormat: " << getGLEnumString(myUploadedInternalFormat);
        AC_DEBUG << "Image InternalFormat: " << getGLEnumString(myInternalFormat);

        return myMipMapMatch && mySizeMatch && myInternalFormatMatch;
    }
    bool
    GLResourceManager::imageMatchesGLTexture3D(TexturePtr theTexture) const {
        AC_DEBUG << "imageMatchesGLTexture3D() binding Texture " << theTexture->getTextureId();
        glBindTexture(GL_TEXTURE_3D, theTexture->getTextureId());
        CHECK_OGL_ERROR;

        // get current uploaded image size
        GLint myUploadedWidth          = -1;
        GLint myUploadedHeight         = -1;
        GLint myUploadedDepth          = -1;
        GLint myUploadedInternalFormat = -1;
        GLint myUploadedMinFilter      = -1;
        glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_WIDTH, &myUploadedWidth);
        CHECK_OGL_ERROR;
        glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_HEIGHT, &myUploadedHeight);
        CHECK_OGL_ERROR;
        glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_DEPTH, &myUploadedDepth);
        CHECK_OGL_ERROR;
        glGetTexLevelParameteriv(GL_TEXTURE_3D,0,GL_TEXTURE_INTERNAL_FORMAT,
            &myUploadedInternalFormat);
        CHECK_OGL_ERROR;

        glGetTexParameteriv(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER, &myUploadedMinFilter);

        CHECK_OGL_ERROR;
        ImagePtr myImage = theTexture->getImage();

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = myImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = myImage->getRasterPtr(myIndex)->height();
        unsigned int myDepth = myImage->get<ImageDepthTag>();
        if (only_power_of_two_textures()) {
            myWidth = nextPowerOfTwo(myWidth);
            myHeight = nextPowerOfTwo(myHeight);
            myDepth = nextPowerOfTwo(myImage->get<ImageHeightTag>());
        }

        GLenum myInternalFormat = asGLTextureInternalFormat(theTexture->getInternalEncoding());

        bool myOpenGLMipMapSetting = myUploadedMinFilter == GL_NEAREST_MIPMAP_NEAREST ||
            myUploadedMinFilter == GL_NEAREST_MIPMAP_LINEAR ||
            myUploadedMinFilter == GL_LINEAR_MIPMAP_NEAREST ||
            myUploadedMinFilter == GL_LINEAR_MIPMAP_LINEAR;
        bool myMipMapMatch = theTexture->get<TextureMipmapTag>() == myOpenGLMipMapSetting;
        bool mySizeMatch = (static_cast<GLint>(myWidth) == myUploadedWidth)
                        && (static_cast<GLint>(myHeight) == myUploadedHeight)
                        && (static_cast<GLint>(myDepth) == myUploadedDepth);
        bool myInternalFormatMatch = static_cast<GLint>(myInternalFormat) == myUploadedInternalFormat;
        AC_DEBUG << "MipMap settings match: " << myMipMapMatch;
        AC_DEBUG << "Width match: " << mySizeMatch;
        AC_DEBUG << "Image size: " << myWidth << "x" << myHeight << "x" << myDepth;
        AC_DEBUG << "UploadedTexure size: " << myUploadedWidth << "x" << myUploadedHeight;
        AC_DEBUG << "Internal format match: " << myInternalFormatMatch;
        AC_DEBUG << "Uploaded InternalFormat: " << getGLEnumString(myUploadedInternalFormat);
        AC_DEBUG << "Image InternalFormat: " << getGLEnumString(myInternalFormat);

        return myMipMapMatch && mySizeMatch && myInternalFormatMatch;
    }

    bool
    GLResourceManager::imageMatchesGLTexture(TexturePtr theTexture) const {
        AC_DEBUG << "imageMatchesGLTexture()";
        if (theTexture->getTextureId() == 0) {
            AC_DEBUG << "imageMatchesGLTexture(): return false, texture has not been uploaded yet";
            return false;
        }
        CHECK_OGL_ERROR;
        if (theTexture->getType() == TEXTURE_2D) {
            return imageMatchesGLTexture2D(theTexture);
        }
        if (theTexture->getType() == TEXTURE_3D) {
            return imageMatchesGLTexture3D(theTexture);
        }
        AC_DEBUG << "imageMatchesGLTexture() returning false, unknown texture type (not TEXTURE_2D or TEXTURE_3D), id=  " << theTexture->getTextureId();
        return false;
    }

    void
    GLResourceManager::setTexturePriority(const TexturePtr & theTexture, float thePriority) {
        GLuint myTextureId = theTexture->getTextureId();
        glPrioritizeTextures(1, &myTextureId, &thePriority);
    }

    void GLResourceManager::loadShaderLibrary(const std::string & theShaderLibraryFile,
                                              const std::string & theVertexProfileName,
                                              const std::string & theFragmentProfileName)
    {
        if (!_myShaderLibrary) {
            _myShaderLibrary = ShaderLibraryPtr(new ShaderLibrary);
        }
        _myShaderLibrary->load(theShaderLibraryFile, theVertexProfileName,
                               theFragmentProfileName);

    }

    void GLResourceManager::loadShaderLibrary() {
        if (!_myShaderLibrary) {
            _myShaderLibrary = ShaderLibraryPtr(new ShaderLibrary);
        }
        _myShaderLibrary->load();
    }

    void GLResourceManager::prepareShaderLibrary(const std::string & theShaderLibraryFile,
                                              const std::string & theVertexProfileName,
                                              const std::string & theFragmentProfileName)
    {
        if (!_myShaderLibrary) {
            _myShaderLibrary = ShaderLibraryPtr(new ShaderLibrary);
        }
        _myShaderLibrary->prepare(theShaderLibraryFile, theVertexProfileName,
                                  theFragmentProfileName);
    }


    IShaderLibraryPtr
    GLResourceManager::getShaderLibrary() const {
        return _myShaderLibrary;
    }


    /**********************************************************************
     *
     * Texture setup
     *
     **********************************************************************/
    void
    GLResourceManager::setupTexture2D(TexturePtr & theTexture)
    {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_setupTexture2D);
        ImagePtr myImage = theTexture->getImage();
        if (!myImage) {
            AC_ERROR << "Texture id=" << theTexture->get<IdTag>()
                     << " has no image associated";
            return;
        }
        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = myImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = myImage->getRasterPtr(myIndex)->height();
        unsigned int myDepth = myImage->get<ImageDepthTag>();
        if (myDepth == 0) {
            myDepth = 1;
            myImage->set<ImageDepthTag>(myDepth);
        }

        AC_DEBUG << "setupTexture2D '" << theTexture->get<NameTag>()
                 << "' id=" << theTexture->get<IdTag>()
                 << " size=" << myWidth << "x" << myHeight << "x" << myDepth;

        // [DS] the min filter defaults to GL_NEAREST_MIPMAP_LINEAR. This
        // causes problems with offscreen rendering.
        // causes problems with non mipmaped textures.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        CHECK_OGL_ERROR;

        unsigned int myTopLevelTextureSize = myImage->getMemUsed();

        //[DS] allow empty textures ...
        const unsigned char * myPixels = 0;
        if (myImage->getRasterPtr(theTexture->get<TextureImageIndexTag>())) {
            unsigned myIndex = theTexture->get<TextureImageIndexTag>();
            myPixels = myImage->getRasterPtr(myIndex)->pixels().begin();
        }
        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, myImage);
        AC_DEBUG << "myPixelEncoding:"<< myPixelEncoding;

        // disable mipmap for compressed textures
        bool myMipmapFlag = theTexture->get<TextureMipmapTag>();
        if (myPixelEncoding.compressedFlag && myMipmapFlag) {
            AC_DEBUG << "disabling mipmap for compressed texture: "
                     << theTexture->get<NameTag>() << std::endl;
            theTexture->set<TextureMipmapTag>(false);
            myMipmapFlag = false;
        }
        AC_DEBUG << "myMipmapFlag="<< myMipmapFlag;

        // pixel buffer
        if (_myHasPixelUnpackBuffer && _myHasVBOExtension) {
            if (theTexture->usePixelBuffer()) {
                GLuint myBufferId = 0;
                glGenBuffersARB(1, &myBufferId);
                theTexture->setPixelBufferId(myBufferId);
                CHECK_OGL_ERROR;
                AC_DEBUG << "using pixel buffer id ="<< myBufferId;
            }

            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
            AC_DEBUG << "pixel buffer unpack set to 0";
        }

        unsigned myTextureMemUsage = 0;
        if (myMipmapFlag) {
            // mipmap

            if (_myHasSGIGenerateMipMap) {
                AC_DEBUG << "building 2D mipmap using SGIS_generate_mipmap, myPixelEncoding ="<<myPixelEncoding;
                glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            } else {
                AC_DEBUG << "building 2D mipmap, myPixelEncoding ="<<myPixelEncoding;
                // 2D-Texture
                gluBuild2DMipmaps(GL_TEXTURE_2D, myPixelEncoding.internalformat,
                        myWidth, myHeight,
                        myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                        myPixels);
            }
            CHECK_OGL_ERROR;

            // myTopLevelTextureSize is added later
            myTextureMemUsage += myTopLevelTextureSize/3;
        } else {
            AC_DEBUG << "no mipmapping, myPixelEncoding ="<<myPixelEncoding;
            // no mipmapping
            if (_myHasSGIGenerateMipMap) {
                glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
            }
        }

        // calculate texture width/height/depth
        // XXX left in to allow for power-of-two textures in case NPOT textures
        // are not supported
        unsigned int myTexWidth = myWidth;
        unsigned int myTexHeight = myHeight;
        unsigned int myTexDepth = myDepth;

        if (only_power_of_two_textures()) {
            AC_WARNING << "has_GL_ARB_texture_non_power_of_two not supported or disabled, using POT-Texture";
            myTexWidth = nextPowerOfTwo(myWidth);
            myTexHeight = nextPowerOfTwo(myHeight / myDepth);
            myTexDepth = nextPowerOfTwo(myDepth);
            asl::Matrix4<float> myCorrectionMatrix;
            myCorrectionMatrix.makeIdentity();
            // set texture correction matrix, does not work with texture tiling (u,v > 1)
            myCorrectionMatrix.scale(Vector3f(float(myWidth) / myTexWidth,
                                              float(myHeight) / myTexHeight,
                                              1.0f));
            theTexture->set<TextureNPOTMatrixTag>(myCorrectionMatrix);

       }
        AC_DEBUG << "image size=" << myWidth << "x" << myHeight << "x" << myDepth;
        AC_DEBUG << "tex size=" << myTexWidth << "x" << myTexHeight << "x" << myTexDepth;

        theTexture->set<TextureWidthTag>(myTexWidth);
        theTexture->set<TextureHeightTag>(myTexHeight);
        theTexture->set<TextureDepthTag>(myTexDepth);

        AC_TRACE << "setupTexture2D internalFormat="<< getGLEnumString(myPixelEncoding.internalformat);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (myPixelEncoding.compressedFlag) {
            // Upload
            AC_DEBUG << "uploading compressed texture";
            glCompressedTexImage2DARB(GL_TEXTURE_2D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, 0,
                    myImage->getMemUsed(), myPixels);
        } else {

            AC_DEBUG << "uploading texture, format = "<<myPixelEncoding;

            // First allocate the texture
            glTexImage2D(GL_TEXTURE_2D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, 0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    0);
            CHECK_OGL_ERROR;

            // Then upload it. This way we can upload textures that are not power of two.
            // Empty textures just remain empty. Useful for render to texture... [DS]
            if (myPixels) {
                glTexSubImage2D(GL_TEXTURE_2D, 0,
                        0, 0, myWidth, myHeight,
                        myPixelEncoding.externalformat,
                        myPixelEncoding.pixeltype, myPixels);
                CHECK_OGL_ERROR;
            }

            myTopLevelTextureSize = getBytesRequired(myTexWidth * myTexHeight * myTexDepth,
                                                     myImage->getRasterEncoding());
            myTextureMemUsage += myTopLevelTextureSize;
        }
        theTexture->setTextureMemUsage(myTextureMemUsage);
        _myTextureMemUsage+=myTextureMemUsage;
    }

    void
    GLResourceManager::setupTexture3D(TexturePtr & theTexture)
    {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_setupTexture3D);
        ImagePtr myImage = theTexture->getImage();
        if (!myImage) {
            AC_ERROR << "Texture id=" << theTexture->get<IdTag>()
                     << " has no image associated";
            return;
        }

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = myImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = myImage->getRasterPtr(myIndex)->height();

        unsigned int myDepth = myImage->get<ImageDepthTag>();
        if (myDepth == 0) {
            myDepth = 1;
            myImage->set<ImageDepthTag>(myDepth);
        }

        AC_DEBUG << "setupTexture3D '" << theTexture->get<NameTag>()
                 << "' id=" << theTexture->get<IdTag>()
                 << " size=" << myWidth << "x" << myHeight << "x" << myDepth;

        // [DS] the min filter defaults to GL_NEAREST_MIPMAP_LINEAR. This
        // causes problems with non mipmaped textures.
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        CHECK_OGL_ERROR;

        unsigned int myTopLevelTextureSize = myImage->getMemUsed();

        //[DS] allow empty textures ...
        const unsigned char * myPixels = 0;
        if (myImage->getRasterPtr(theTexture->get<TextureImageIndexTag>())) {
            unsigned myIndex = theTexture->get<TextureImageIndexTag>();
            myPixels = myImage->getRasterPtr(myIndex)->pixels().begin();
        }
        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, myImage);

        // disable mipmap
        bool myMipmapFlag = theTexture->get<TextureMipmapTag>();
        if (myMipmapFlag) {
            AC_DEBUG << "Disabling mipmap for 3D texture '" << theTexture->get<NameTag>() << "'";
            myMipmapFlag = false;
            theTexture->set<TextureMipmapTag>(myMipmapFlag);
        CHECK_OGL_ERROR;
        }

        // calculate texture width/height/depth
        unsigned int myTexWidth = myWidth;
        unsigned int myTexHeight = myHeight / myDepth;
        unsigned int myTexDepth = myDepth;
        AC_TRACE << "image size=" << myWidth << "x" << myHeight << "x" << myDepth;
        AC_TRACE << "tex size=" << myTexWidth << "x" << myTexHeight << "x" << myTexDepth;

        theTexture->set<TextureWidthTag>(myTexWidth);
        theTexture->set<TextureHeightTag>(myTexHeight);
        theTexture->set<TextureDepthTag>(myTexDepth);

        AC_TRACE << "setupTexture3D internalFormat=0x" << std::hex << myPixelEncoding.internalformat << std::dec;

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        if (myPixelEncoding.compressedFlag) {
            // Upload
            glCompressedTexImage3DARB(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, myTexDepth, 0,
                    myImage->getMemUsed(), myPixels);
        } else {
            // First allocate the texture with power of two dimensions

#if 0
            // some test code
            const unsigned char * myEmptyPixels = 0;
            myEmptyPixels = myPixels;
            dom::ValuePtr myEmptyMap = createRasterValue(myImage->getRasterEncoding(), myTexWidth, myTexHeight * myTexDepth);
            dom::ResizeableRasterPtr myRaster = dynamic_cast_Ptr<dom::ResizeableRaster>(myEmptyMap);
            //myRaster->clear();
            myRaster->fillRect(0,0,myRaster->width(), myRaster->height(), asl::Vector4<float>(1,0,0,1));
            myEmptyPixels = myRaster->pixels().begin();
            //myImage->saveToFile("/var/tmp/testimg.png");
#endif

#ifdef OSX
#define glTexSubImage3D_OSX_BUG_WORKAROUND
#endif
#ifdef glTexSubImage3D_OSX_BUG_WORKAROUND
            // Workaround for OSX driver bug; glTexSubImage3D does not work: use glTexImage3D only
            glTexImage3D(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, myTexDepth, 0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    myPixels);

            CHECK_OGL_ERROR;
#else
            glTexImage3D(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, myTexDepth, 0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    0);

            CHECK_OGL_ERROR;

            // Then upload it. This way we can upload textures that are not power of two.
            // Empty textures just remain empty. Useful for render to texture... [DS]
            if (myPixels) {
                glTexSubImage3D(GL_TEXTURE_3D, 0,
                        0, 0, 0, myTexWidth, myTexHeight, myTexDepth,
                        myPixelEncoding.externalformat,
                        myPixelEncoding.pixeltype, myPixels);
                CHECK_OGL_ERROR;
            }
#endif

            myTopLevelTextureSize = getBytesRequired(myTexWidth * myTexHeight * myTexDepth,
                                                     myImage->getRasterEncoding());
            _myTextureMemUsage += myTopLevelTextureSize;
        }
    }

    void
    GLResourceManager::setupCubemap(TexturePtr & theTexture)
    {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_setupCubemap);
        ImagePtr myImage = theTexture->getImage();
        if (!myImage) {
            AC_ERROR << "Texture node id=" << theTexture->get<IdTag>() << " has no image associated";
            return;
        }

        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECK_OGL_ERROR;

        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECK_OGL_ERROR;

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = myImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = myImage->getRasterPtr(myIndex)->height();

        /*GLsizei myWidth  = myImage->get<ImageWidthTag>();
        GLsizei myHeight = myImage->get<ImageHeightTag>();*/

        // number of tiles must match cubemap
        const asl::Vector2i myTileVec = myImage->get<ImageTileTag>();
        unsigned int myNumTiles = myTileVec[0] * myTileVec[1];
        if (myNumTiles < CUBEMAP_SIDES) {
            throw TextureException(std::string("Image '") + theTexture->get<NameTag>()
                                   + "' number of tiles doesn't match cubemap: "
                                   + asl::as_string(myNumTiles), PLUS_FILE_LINE);
        }

        // tiles must be square
        GLsizei myTileWidth = myWidth / myTileVec[0];
        GLsizei myTileHeight = myHeight / myTileVec[1];
        if (myTileWidth != myTileHeight) {
            throw TextureException(std::string("Texture tiles are not square: ")
                                   + theTexture->get<NameTag>(), PLUS_FILE_LINE);
        }

        theTexture->set<TextureWidthTag>(myTileWidth);
        theTexture->set<TextureHeightTag>(myTileHeight);

        AC_DEBUG << "setupCubemap texture name ='" << theTexture->get<NameTag>()
                 << "' node id=" << theTexture->get<IdTag>()
                 << " image=" << myWidth << "x" << myHeight << " #tiles=" << myNumTiles
                 << " tile=" << myTileWidth << "x" << myTileHeight;

        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, myImage);
        unsigned int myTopLevelTextureSize = myImage->getMemUsed();
        _myTextureMemUsage +=  myTopLevelTextureSize;

        if (static_cast<unsigned int>(myTileWidth) != myWidth) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, myWidth);
        }

        unsigned int myBytesPerLine = getBytesRequired(myWidth, myImage->getRasterEncoding());
        unsigned int myBytesPerTileLine = getBytesRequired(myTileWidth,
                                                           myImage->getRasterEncoding());

        // send GL the six tiles of the bitmap
        for (unsigned int i = 0; i < CUBEMAP_SIDES; ++i) {
            unsigned myIndex = theTexture->get<TextureImageIndexTag>();
            const unsigned char * myTile = myImage->getRasterPtr(myIndex)->pixels().begin();

            // add tile offset
            unsigned int myColumn = i % myTileVec[0];
            unsigned int myRow = i / myTileVec[0];
            myTile += (myTileHeight * myBytesPerLine) * myRow + myBytesPerTileLine * myColumn;

            // load texture
            GLenum myCubemapFace = asGLCubemapFace(i);
            if (myPixelEncoding.compressedFlag) {
                const unsigned int myTileSize = myImage->getMemUsed() / myNumTiles;
                glCompressedTexImage2DARB(myCubemapFace, 0,
                                          myPixelEncoding.internalformat,
                                          myTileWidth, myTileHeight, 0,
                                          myTileSize, myTile);
            } else {
                glTexImage2D(myCubemapFace, 0,
                             myPixelEncoding.internalformat,
                             myTileWidth, myTileHeight, 0,
                             myPixelEncoding.externalformat,
                             myPixelEncoding.pixeltype, myTile);
            }
            CHECK_OGL_ERROR;
        }

        // restore
        if (static_cast<unsigned int>(myTileWidth) != myWidth) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
            CHECK_OGL_ERROR;
        }
    }


    /**********************************************************************
     *
     * Texture update
     *
     **********************************************************************/
    void
    GLResourceManager::updateTexture2D(const TexturePtr & theTexture, ImagePtr & theImage) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateTexture2D);

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = theImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = theImage->getRasterPtr(myIndex)->height();

        /*GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();*/
        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);

        AC_DEBUG << "GLResourceManager::updateTexture2D '" << theTexture->get<NameTag>()
                 << "' node id=" << theTexture->get<IdTag>() << " size=" << myWidth
                 << "x" << myHeight;
        void * myImageData = theImage->getRasterPtr(myIndex)->pixels().begin();
        unsigned int myImageDataLen = theImage->getMemUsed();

        if (_myHasPixelUnpackBuffer) {
            unsigned int myBufferId = theTexture->getPixelBufferId();
            if (_myHasVBOExtension && myBufferId) {

                // bind buffer
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, myBufferId);
                CHECK_OGL_ERROR;

                // map buffer
                glBufferDataARB(GL_PIXEL_UNPACK_BUFFER_ARB, myImageDataLen, NULL, GL_STREAM_DRAW);
                void * myPBOData = glMapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, GL_WRITE_ONLY);
                memcpy(myPBOData, myImageData, myImageDataLen);
                glUnmapBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB);
                CHECK_OGL_ERROR;

                // treated as an offset into the pixel buffer
                myImageData = 0;
            }
        }
        if (myPixelEncoding.compressedFlag) {
            glCompressedTexSubImage2DARB(GL_TEXTURE_2D, 0,
                    0, 0, myWidth, myHeight,
                    myPixelEncoding.externalformat,
                    theImage->getMemUsed(), myImageData);
        } else {
            glTexSubImage2D(GL_TEXTURE_2D, 0,
                    0, 0, myWidth, myHeight,
                    myPixelEncoding.externalformat,
                    myPixelEncoding.pixeltype, myImageData);
        }

        if (_myHasPixelUnpackBuffer) {
            // unbind buffer
            if (_myHasVBOExtension) {
                glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
                CHECK_OGL_ERROR;
            }
        }
    }

    void
    GLResourceManager::updateTexture3D(const TexturePtr & theTexture, ImagePtr & theImage) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateTexture3D);

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = theImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = theImage->getRasterPtr(myIndex)->height();

        GLsizei myDepth  = theImage->get<ImageDepthTag>();

        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);
        void * myImageData = theImage->getRasterPtr(myIndex)->pixels().begin();

        AC_DEBUG << "GLResourceManager::updateTexture3D '" << theTexture->get<NameTag>()
                 << "'node id=" << theTexture->get<IdTag>() << " size=" << myWidth
                 << "x" << myHeight << "x" << myDepth;

        if (myPixelEncoding.compressedFlag) {
            glCompressedTexSubImage3DARB(GL_TEXTURE_3D, 0,
                    0, 0, 0, myWidth, myHeight / myDepth, myDepth,
                    myPixelEncoding.externalformat,
                    theImage->getMemUsed(), myImageData);
        } else {
#ifdef glTexSubImage3D_OSX_BUG_WORKAROUND
            glTexImage3D(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myWidth, myHeight / myDepth, myDepth,
                    0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    myImageData);
#else
            glTexSubImage3D(GL_TEXTURE_3D, 0,
                    0, 0, 0, myWidth, myHeight / myDepth, myDepth,
                    myPixelEncoding.externalformat,
                    myPixelEncoding.pixeltype, myImageData);
#endif
        }
    }

    void
    GLResourceManager::updateCubemap(const TexturePtr & theTexture, ImagePtr & theImage) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateCubemap);

        AC_DEBUG << "GLResourceManager::updateCubemap '" << theTexture->get<NameTag>()
                 << "' node id=" << theTexture->get<IdTag>();

        const asl::Vector2i myTileVec = theImage->get<ImageTileTag>();
        unsigned int myNumTiles = myTileVec[0] * myTileVec[1];

        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        unsigned int myWidth = theImage->getRasterPtr(myIndex)->width();
        unsigned int myHeight = theImage->getRasterPtr(myIndex)->height();

        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);

        GLsizei myTileWidth = myWidth / myTileVec[0];
        GLsizei myTileHeight = myHeight / myTileVec[1];

        if (static_cast<unsigned int>(myTileWidth) != myWidth) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, myWidth);
        }
        unsigned int myBytesPerLine = getBytesRequired(myWidth, theImage->getRasterEncoding());
        unsigned int myBytesPerTileLine = getBytesRequired(myTileWidth,
                                                           theImage->getRasterEncoding());

        for (unsigned int i = 0; i < CUBEMAP_SIDES; ++i) {
            unsigned myIndex = theTexture->get<TextureImageIndexTag>();
            const unsigned char * myTile = theImage->getRasterPtr(myIndex)->pixels().begin();

            // add tile offset
            unsigned int myColumn = i % myTileVec[0];
            unsigned int myRow = i / myTileVec[0];
            myTile += (myTileHeight * myBytesPerLine) * myRow + myBytesPerTileLine * myColumn;

            GLenum myCubemapFace = asGLCubemapFace(i);
            if (myPixelEncoding.compressedFlag) {
                const unsigned int myTileSize = theImage->getMemUsed() / myNumTiles;
                glCompressedTexSubImage2DARB(myCubemapFace, 0,
                        0, 0,
                        myTileWidth, myTileHeight,
                        myPixelEncoding.externalformat,
                        myTileSize, myTile);
            } else {
                glTexSubImage2D(myCubemapFace, 0,
                        0, 0,
                        myTileWidth, myTileHeight,
                        myPixelEncoding.externalformat,
                        myPixelEncoding.pixeltype, myTile);
            }
            CHECK_OGL_ERROR;
        }
    }

    /*
     * Texture params
     */
    void
    GLResourceManager::updateTextureParams(const TexturePtr & theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateTextureParams);


        glPushAttrib(GL_TEXTURE_BIT);

        unsigned myTextureId = theTexture->getTextureId();
        GLenum myTextureTarget = asGLTextureTarget(theTexture->getType());
        glBindTexture(myTextureTarget, myTextureId);
        CHECK_OGL_ERROR;

        setTextureParams(theTexture, myTextureTarget);

        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    void
    GLResourceManager::setTextureParams(const TexturePtr & theTexture, const GLenum theTextureTarget) {
        AC_DEBUG << "GLResourceManager::setTextureParams '" << theTexture->get<NameTag>()
                 << "' id='" << theTexture->get<IdTag>()
                 << "' texid='" << theTexture->get<TextureIdTag>()
                 << "' wrapmode='" << theTexture->getWrapMode()
                 << "' hasMipMaps='" << theTexture->get<TextureMipmapTag>()
                 << "' minfilter='" << theTexture->getMinFilter()
                 << "' magfilter='" << theTexture->getMagFilter()
                 << "'";

#define DONT_CHECK_TEX_ID
#ifdef CHECK_TEX_ID
        // Do some checks
        GLint myId = 0;
        switch (theTextureTarget) {
        case GL_TEXTURE_CUBE_MAP_ARB:
        case GL_TEXTURE_2D:
        case GL_TEXTURE_RECTANGLE_ARB:
            glGetIntegerv(GL_TEXTURE_2D_BINDING_EXT, &myId);
            AC_DEBUG << "current id = " << myId << "(GL_TEXTURE_2D or GL_TEXTURE_RECTANGLE_ARB or GL_TEXTURE_CUBE_MAP_ARB)";
            break;
        case GL_TEXTURE_3D:
            glGetIntegerv(GL_TEXTURE_3D_BINDING_EXT, &myId);
            AC_DEBUG << "current id = " << myId << "(GL_TEXTURE_3D_BINDING_EXT)";
            break;
        default:
            AC_ERROR << "Unknown texture target = " << theTextureTarget;
       }
       if (myId == 0) {
            AC_DEBUG << "No texture bound, trace = ";
            AC_DEBUG << asl::StackTrace();
        }
#endif

        glTexParameteri(theTextureTarget, GL_TEXTURE_WRAP_S,
                        asGLTextureWrapMode(theTexture->getWrapMode()));
        CHECK_OGL_ERROR;
        glTexParameteri(theTextureTarget, GL_TEXTURE_WRAP_T,
                        asGLTextureWrapMode(theTexture->getWrapMode()));
        CHECK_OGL_ERROR;
        if (theTextureTarget == GL_TEXTURE_3D) {
            glTexParameteri(theTextureTarget, GL_TEXTURE_WRAP_R,
                            asGLTextureWrapMode(theTexture->getWrapMode()));
            CHECK_OGL_ERROR;
        }

        bool hasMipMaps = theTexture->get<TextureMipmapTag>();
        GLenum myFilter = asGLTextureSampleFilter(theTexture->getMagFilter(), false);
        glTexParameteri(theTextureTarget, GL_TEXTURE_MAG_FILTER, myFilter);
        CHECK_OGL_ERROR;

        myFilter = asGLTextureSampleFilter(theTexture->getMinFilter(), hasMipMaps);
        glTexParameteri(theTextureTarget, GL_TEXTURE_MIN_FILTER, myFilter);
        CHECK_OGL_ERROR;

        if (_myHasAnisotropicTex && hasMipMaps) {
            float maxAnisotropy = theTexture->get<TextureAnisotropyTag>();
            if (maxAnisotropy > 1.0f) {
                AC_DEBUG << "setting max_anisotropy=" << maxAnisotropy;
                glTexParameterf(theTextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
                CHECK_OGL_ERROR;
            }
        }
    }

    void
    GLResourceManager::updatePixelTransfer(const TexturePtr & theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updatePixelTransfer);
        AC_DEBUG << "GLResourceManager::updatePixelTransfer '" << theTexture->get<NameTag>()
                 << ", colorbias = "<< theTexture->get<TextureColorBiasTag>()
                 << ", colorscale = "<< theTexture->get<TextureColorScaleTag>() ;

        const Vector4f & myColorBias = theTexture->get<TextureColorBiasTag>();
        if (myColorBias[0] != 0.0f) {
            glPixelTransferf(GL_RED_BIAS, myColorBias[0]);
        }
        if (myColorBias[1] != 0.0f) {
            glPixelTransferf(GL_GREEN_BIAS, myColorBias[1]);
        }
        if (myColorBias[2] != 0.0f) {
            glPixelTransferf(GL_BLUE_BIAS, myColorBias[2]);
        }
        if (myColorBias[3] != 0.0f) {
            glPixelTransferf(GL_ALPHA_BIAS, myColorBias[3]);
        }
        CHECK_OGL_ERROR;

        const Vector4f & myColorScale = theTexture->get<TextureColorScaleTag>();
        if (myColorScale[0] != 1.0f) {
            glPixelTransferf(GL_RED_SCALE, myColorScale[0]);
        }
        if (myColorScale[1] != 1.0f) {
            glPixelTransferf(GL_GREEN_SCALE, myColorScale[1]);
        }
        if (myColorScale[2] != 1.0f) {
            glPixelTransferf(GL_BLUE_SCALE, myColorScale[2]);
        }
        if (myColorScale[3] != 1.0f) {
            glPixelTransferf(GL_ALPHA_SCALE, myColorScale[3]);
        }
        CHECK_OGL_ERROR;
    }

    PixelEncodingInfo
    GLResourceManager::getPixelEncoding(const TexturePtr & theTexture,
                                        const ImagePtr & theImage)
    {
        PixelEncoding myEncoding = theImage->getRasterEncoding();
        PixelEncodingInfo myEncodingInfo = getDefaultGLTextureParams(myEncoding);
        myEncodingInfo.internalformat =
            asGLTextureInternalFormat(theTexture->getInternalEncoding());
        // after loading the bitmap from disk it is not clear wheater gray pixels
        // were meant as luminance or alpha
        // we treat them as luminance unless the user specifies the texture pixel format
        // as alpha (vs/ds, 2010)
        if (myEncoding == y60::GRAY) {
            if (myEncodingInfo.internalformat == GL_ALPHA ||
                myEncodingInfo.internalformat == GL_ALPHA8 ||
                myEncodingInfo.internalformat == GL_ALPHA16)
            {
                myEncodingInfo.externalformat = GL_ALPHA;
            }
        }

        AC_DEBUG << "GLResourceManager::getPixelEncoding, image raster encoding '" << asl::getStringFromEnum(myEncoding, PixelEncodingString)
                << ", texture internal format = " << asl::getStringFromEnum(theTexture->getInternalEncoding(), TextureInternalFormatStrings);
        return myEncodingInfo;
    }
}
