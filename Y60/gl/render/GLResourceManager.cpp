//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

/* 
PM: We might consider to rewrite the texture handling od GLResourceManger and scene/Texture in a way that the
Resourcemanager does not modify any state in texture, but that texture more directly reflects the GL state.
E.g. modifying the texture size should then result in an actual resize of the gl texture.
We need to find a clean way to deal with NPOT textures then.
*/

#include "GLResourceManager.h"

#include <y60/Image.h>
#include <y60/PixelEncoding.h>
#include <y60/PixelEncodingInfo.h>
#include <asl/file_functions.h>
#include <asl/Dashboard.h>

#include <GL/glext.h>

using namespace std;
using namespace asl;

#define DB(x) // x

namespace y60 {
    const unsigned int CUBEMAP_SIDES = 6;

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
            unbindTexture(&*theTexture);
        }

        // generate texture object
        unsigned myTextureId;
        glGenTextures(1, &myTextureId);
        CHECK_OGL_ERROR;

        // bind texture object
        GLenum myTextureTarget = asGLTextureTarget(theTexture->getType());
        glBindTexture(myTextureTarget, myTextureId);
        CHECK_OGL_ERROR;

        AC_DEBUG << "GLResourceManager::setupTexture '" << theTexture->get<NameTag>() 
                 << "' id=" << theTexture->get<IdTag>() << " texTarget=0x" << hex 
                 << myTextureTarget << dec << " texId=" << myTextureId;
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

        updateTextureParams(theTexture);

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
                 << "' id=" << theTexture->get<IdTag>() << " texTarget=0x" << hex 
                 << myTextureTarget << dec << " texId=" << myTextureId;
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
        }
        CHECK_OGL_ERROR;

        updateTextureParams(theTexture);

        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    void
    GLResourceManager::unbindTexture(Texture * theTexture) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_unbindTexture);

        if (!hasGLContext()) {
            return;
        }

        unsigned myTextureId = theTexture->getTextureId();
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

        unsigned int myBufferId = theTexture->getPixelBufferId();
        if (IS_SUPPORTED(glDeleteBuffersARB) && myBufferId) {
            AC_DEBUG << "GLResourceManager::unbindTexture '" << theTexture->get<NameTag>() 
                     << "' id=" << theTexture->get<IdTag>() << " pboId=" << myBufferId;

            glDeleteBuffersARB(1, &myBufferId);
            theTexture->setPixelBufferId(0);
            CHECK_OGL_ERROR;
        }
    }

    static bool only_power_of_two_textures() {
        static bool has_GL_ARB_texture_non_power_of_two = queryOGLExtension("GL_ARB_texture_non_power_of_two");
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
        unsigned int myWidth = myImage->get<ImageWidthTag>();
        unsigned int myHeight = myImage->get<ImageHeightTag>();
        if (only_power_of_two_textures()) {
            myWidth = nextPowerOfTwo(myImage->get<ImageWidthTag>());
            myHeight = nextPowerOfTwo(myImage->get<ImageHeightTag>());
        }

        GLenum myInternalFormat = asGLTextureInternalFormat(theTexture->getInternalEncoding());

		bool myOpenGLMipMapSetting = myUploadedMinFilter == GL_NEAREST_MIPMAP_NEAREST || 
			                         myUploadedMinFilter == GL_NEAREST_MIPMAP_LINEAR || 
			                         myUploadedMinFilter == GL_LINEAR_MIPMAP_NEAREST || 
			                         myUploadedMinFilter == GL_LINEAR_MIPMAP_LINEAR;
        bool myMipMapMatch = theTexture->get<TextureMipmapTag>() == myOpenGLMipMapSetting;
        bool mySizeMatch = (myWidth == myUploadedWidth) && (myHeight == myUploadedHeight); 
        bool myInternalFormatMatch = myInternalFormat == myUploadedInternalFormat;
        AC_DEBUG << "MipMap settings match: " << myMipMapMatch;
        AC_DEBUG << "Width match: " << mySizeMatch;
        AC_DEBUG << "Image size: " << myWidth << "x" << myHeight;
        AC_DEBUG << "UploadedTexure size: " << myUploadedWidth << "x" << myUploadedHeight;
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
        unsigned int myWidth = myImage->get<ImageWidthTag>();
        unsigned int myHeight = myImage->get<ImageHeightTag>();
        unsigned int myDepth = myImage->get<ImageDepthTag>();
        if (only_power_of_two_textures()) {
            myWidth = nextPowerOfTwo(myImage->get<ImageWidthTag>());
            myHeight = nextPowerOfTwo(myImage->get<ImageHeightTag>());
            myDepth = nextPowerOfTwo(myImage->get<ImageHeightTag>());
        }

        GLenum myInternalFormat = asGLTextureInternalFormat(theTexture->getInternalEncoding());

		bool myOpenGLMipMapSetting = myUploadedMinFilter == GL_NEAREST_MIPMAP_NEAREST || 
			                         myUploadedMinFilter == GL_NEAREST_MIPMAP_LINEAR || 
			                         myUploadedMinFilter == GL_LINEAR_MIPMAP_NEAREST || 
			                         myUploadedMinFilter == GL_LINEAR_MIPMAP_LINEAR;
        bool myMipMapMatch = theTexture->get<TextureMipmapTag>() == myOpenGLMipMapSetting;
        bool mySizeMatch = (myWidth == myUploadedWidth) && (myHeight == myUploadedHeight) && (myDepth == myUploadedDepth); 
        bool myInternalFormatMatch = myInternalFormat == myUploadedInternalFormat;
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
        unsigned myTextureId = theTexture->getTextureId();
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

        unsigned int myWidth = myImage->get<ImageWidthTag>();
        unsigned int myHeight = myImage->get<ImageHeightTag>();
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
                     << theTexture->get<NameTag>() << endl;
            theTexture->set<TextureMipmapTag>(false);
            myMipmapFlag = false;
        }
        AC_DEBUG << "myMipmapFlag="<< myMipmapFlag;

#ifdef GL_PIXEL_UNPACK_BUFFER_ARB
        // pixel buffer
        if (IS_SUPPORTED(glGenBuffersARB) && theTexture->usePixelBuffer()) {
            unsigned int myBufferId = 0;
            glGenBuffersARB(1, &myBufferId);
            theTexture->setPixelBufferId(myBufferId);
            CHECK_OGL_ERROR;
            AC_DEBUG << "using pixel buffer id ="<< myBufferId;
        }
        if (IS_SUPPORTED(glBindBufferARB)) {
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
            AC_DEBUG << "pixel buffer unpack set to 0";
        }
#endif

#ifdef GL_SGIS_generate_mipmap
        // UH: disabled since it produces problems with alpha (according to DS)
        static bool SGIS_generate_mipmap = queryOGLExtension("GL_SGIS_generate_mipmap");
#endif
        unsigned myTextureMemUsage = 0;
        if (myMipmapFlag) {
            // mipmap
#ifdef GL_SGIS_generate_mipmap
            if (SGIS_generate_mipmap) {
                AC_DEBUG << "building 2D mipmap using SGIS_generate_mipmap, myPixelEncoding ="<<myPixelEncoding;
                glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
            }
            else
#endif
            {
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
#ifdef GL_SGIS_generate_mipmap
            if (SGIS_generate_mipmap) {
                glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
            }
#endif
        }

        // calculate texture width/height/depth
        // XXX left in to allow for power-of-two textures in case NPOT textures 
        // are not supported
        unsigned int myTexWidth = myWidth;
        unsigned int myTexHeight = myHeight;
        unsigned int myTexDepth = myDepth;
        
        if (only_power_of_two_textures()) {
            AC_DEBUG << "has_GL_ARB_texture_non_power_of_two not supported or disabled, using POT-Texture";
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
            AC_DEBUG << "uploadinf compressed texture";
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

        unsigned int myWidth = myImage->get<ImageWidthTag>();
        unsigned int myHeight = myImage->get<ImageHeightTag>();
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

        AC_TRACE << "setupTexture3D internalFormat=0x" << hex << myPixelEncoding.internalformat << dec;

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

        GLsizei myWidth  = myImage->get<ImageWidthTag>();
        GLsizei myHeight = myImage->get<ImageHeightTag>();

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

        if (myTileWidth != myWidth) {
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
        if (myTileWidth != myWidth) {
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

        GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();
        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);

        AC_DEBUG << "GLResourceManager::updateTexture2D '" << theTexture->get<NameTag>() 
                 << "' node id=" << theTexture->get<IdTag>() << " size=" << myWidth 
                 << "x" << myHeight;
        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
        void * myImageData = theImage->getRasterPtr(myIndex)->pixels().begin();
        unsigned int myImageDataLen = theImage->getMemUsed();

#ifdef GL_PIXEL_UNPACK_BUFFER_ARB
        unsigned int myBufferId = theTexture->getPixelBufferId();
        if (IS_SUPPORTED(glBindBufferARB) && myBufferId) {

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
#endif

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

#ifdef GL_PIXEL_UNPACK_BUFFER_ARB
        // unbind buffer
        if (IS_SUPPORTED(glBindBufferARB) && myBufferId) {
            glBindBufferARB(GL_PIXEL_UNPACK_BUFFER_ARB, 0);
            CHECK_OGL_ERROR;
        }
#endif
    }

    void
    GLResourceManager::updateTexture3D(const TexturePtr & theTexture, ImagePtr & theImage) {
        MAKE_GL_SCOPE_TIMER(GLResourceManager_updateTexture3D);

        GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();
        GLsizei myDepth  = theImage->get<ImageDepthTag>();

        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);
        unsigned myIndex = theTexture->get<TextureImageIndexTag>();
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

        GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();
        PixelEncodingInfo myPixelEncoding = getPixelEncoding(theTexture, theImage);

        GLsizei myTileWidth = myWidth / myTileVec[0];
        GLsizei myTileHeight = myHeight / myTileVec[1];

        if (myTileWidth != myWidth) {
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
        
        AC_DEBUG << "GLResourceManager::updateTextureParams '" << theTexture->get<NameTag>() 
                 << "' id='" << theTexture->get<IdTag>()
                 << "' texid='" << theTexture->get<TextureIdTag>()
                 << "' wrapmode='" << theTexture->getWrapMode()
                 << "' hasMipMaps='" << theTexture->get<TextureMipmapTag>()
                 << "' minfilter='" << theTexture->getMinFilter()
                 << "' magfilter='" << theTexture->getMagFilter()
                 << "'";

        GLenum myTextureTarget = asGLTextureTarget(theTexture->getType());

#define DONT_CHECK_TEX_ID
#ifdef CHECK_TEX_ID
        // Do some checks
        GLint myId = 0;
        switch (myTextureTarget) {
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
            AC_ERROR << "Unknow texture target = " << myTextureTarget;
       }
       if (myId == 0) {
            AC_DEBUG << "No texture bound, trace = ";
            AC_DEBUG << asl::StackTrace();
        }
#endif

        glTexParameteri(myTextureTarget, GL_TEXTURE_WRAP_S, 
                        asGLTextureWrapMode(theTexture->getWrapMode()));
        CHECK_OGL_ERROR;
        glTexParameteri(myTextureTarget, GL_TEXTURE_WRAP_T, 
                        asGLTextureWrapMode(theTexture->getWrapMode()));
        CHECK_OGL_ERROR;
        if (myTextureTarget == GL_TEXTURE_3D) {
            glTexParameteri(myTextureTarget, GL_TEXTURE_WRAP_R, 
                            asGLTextureWrapMode(theTexture->getWrapMode()));
            CHECK_OGL_ERROR;
        }

        bool hasMipMaps = theTexture->get<TextureMipmapTag>();
        GLenum myFilter = asGLTextureSampleFilter(theTexture->getMagFilter(), false);
        glTexParameteri(myTextureTarget, GL_TEXTURE_MAG_FILTER, myFilter);
        CHECK_OGL_ERROR;

        myFilter = asGLTextureSampleFilter(theTexture->getMinFilter(), hasMipMaps);
        glTexParameteri(myTextureTarget, GL_TEXTURE_MIN_FILTER, myFilter);
        CHECK_OGL_ERROR;

#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
        if (hasMipMaps) {
            float maxAnisotropy = theTexture->get<TextureAnisotropyTag>();
            if (maxAnisotropy > 1.0f) {
                AC_DEBUG << "setting max_anisotropy=" << maxAnisotropy;
                glTexParameterf(myTextureTarget, GL_TEXTURE_MAX_ANISOTROPY_EXT, maxAnisotropy);
                CHECK_OGL_ERROR;
            }
        }
#endif
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
        AC_DEBUG << "GLResourceManager::getPixelEncoding, image raster encoding '" << asl::getStringFromEnum(myEncoding, PixelEncodingString) 
                << ", texture internal format = " << asl::getStringFromEnum(theTexture->getInternalEncoding(), TextureInternalFormatStrings);
        return myEncodingInfo;
    }
}
