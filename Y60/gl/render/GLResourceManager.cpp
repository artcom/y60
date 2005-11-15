//=============================================================================
// Copyright (C) 2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $Author: $
//   $Revision: $
//   $Date: $
//
//  Description: This class performs texture loading and management.
//
//=============================================================================

#include "GLResourceManager.h"
#include <y60/Image.h>
#include <y60/PixelEncoding.h>
#include <GL/glext.h>

using namespace std;
using namespace asl;

#define DB(x) // x

namespace y60 {
    const unsigned int CUBEMAP_SIDES = 6;
    const GLenum ourCubeMapFaces[] = {
        GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB,  /// <behind
        GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB,  /// <right
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB,  /// <front
        GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB,  /// <left
        GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB,  /// <top
        GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB,  /// <bottom
    };

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
                throw TextureException(as_string(theDimensions)+"D Textures: size not queryable", PLUS_FILE_LINE);
        }
        return myMaxSize;
    }

    unsigned
    GLResourceManager::getMaxTextureUnits() const {
        int myMaxTexUnits;
        glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB, &myMaxTexUnits);
        if (myMaxTexUnits < 0) { // should not happen
            myMaxTexUnits = 0;
        }
        return unsigned(myMaxTexUnits);
    }

    /*
     * Single texture
     */
    unsigned
    GLResourceManager::setupSingleTexture(ImagePtr theImage) {
        if (!theImage) {
            throw TextureException("setupSingleTexture: ImagePtr is null!", PLUS_FILE_LINE);
        }
 
        unsigned int myId = theImage->getGraphicsId();
        unsigned int myWidth = theImage->get<ImageWidthTag>();
        unsigned int myHeight = theImage->get<ImageHeightTag>();
        unsigned int myDepth = theImage->get<ImageDepthTag>();

        AC_DEBUG << "setupSingleTexture: name='"<<theImage->get<NameTag>()<<"',id='" <<
            theImage->get<IdTag>()<<"',gfxid='" << theImage->getGraphicsId() << "',size=" <<
            myWidth << "x" << myHeight << "x" << myDepth << endl;

        if (myDepth == 0) {
            myDepth = 1;
            theImage->set<ImageDepthTag>(1);
        }
        if (myDepth == 1) {
            // 2D-Texture
            glBindTexture(GL_TEXTURE_2D, myId);
            CHECK_OGL_ERROR;
            // [DS] the min filter defaults to GL_NEAREST_MIPMAP_LINEAR. This
            // causes problems with offscreen rendering. 
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER , GL_NEAREST);
            CHECK_OGL_ERROR;
        } else {
            // 3D-Texture
            glBindTexture(GL_TEXTURE_3D, myId);
            CHECK_OGL_ERROR;
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S , GL_REPEAT);
            CHECK_OGL_ERROR;
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T , GL_REPEAT);
            CHECK_OGL_ERROR;
            glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R , GL_REPEAT);
            CHECK_OGL_ERROR;
            //glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER , GL_LINEAR);
            //CHECK_OGL_ERROR;
        }

        unsigned int myTopLevelTextureSize = theImage->getMemUsed();

        //[DS] allow empty textures ...
        const unsigned char * myReadonlyData = 0;
        if (theImage->getRasterPtr()) {
            myReadonlyData = theImage->getRasterPtr()->pixels().begin();
        }
        /* 
        if (!theImage->getRasterPtr()) {
            throw TextureException(string("setupSingleTexture: no raster value in image '")+
                theImage->get<NameTag>()+"'!", PLUS_FILE_LINE);
        }
        const unsigned char * myReadonlyData = theImage->getRasterPtr()->pixels().begin();
        */
        PixelEncodingInfo myPixelEncoding = getInternalTextureFormat(theImage);

        glPushAttrib(GL_PIXEL_MODE_BIT);
        setupPixelTransfer(theImage);

        // disable mipmap for compressed textures
        if (myPixelEncoding.compressedFlag && theImage->get<ImageMipmapTag>()) {
            AC_DEBUG << "disabling mipmap for compressed texture: " << theImage->get<NameTag>() << endl;
            theImage->set<ImageMipmapTag>(false);
        }

#ifdef GL_SGIS_generate_mipmap
        static bool SGIS_generate_mipmap = queryOGLExtension("GL_SGIS_generate_mipmap");
#endif

        if (theImage->get<ImageMipmapTag>() && myDepth == 1 /* no 3D-Mipmaps yet */) {
            // mipmap
            if (myDepth == 1) {
#ifdef GL_SGIS_generate_mipmap
                if (SGIS_generate_mipmap) {
                    //glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);
                }
#endif
                // 2D-Texture
                gluBuild2DMipmaps(GL_TEXTURE_2D, myPixelEncoding.internalformat,
                    myWidth, myHeight,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    myReadonlyData);
                CHECK_OGL_ERROR;
            } else {
#if HAS_3D_MIPMAPS
                // 3D-Texture
                gluBuild3DMipmaps(GL_TEXTURE_3D, myPixelEncoding.internalformat,
                    myWidth, myHeight, myDepth,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    myReadonlyData);
                CHECK_OGL_ERROR;
#endif
            }
            _myTextureMemUsage +=  myTopLevelTextureSize + myTopLevelTextureSize/3;
        } else {
            // no mipmapping
            if (myDepth == 1) {
#ifdef GL_SGIS_generate_mipmap
                if (SGIS_generate_mipmap) {
                    //glTexParameterf(GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_FALSE);
                }
#endif
            }
        }

        if (myPixelEncoding.compressedFlag) {
            if (myDepth == 1) {
                glCompressedTexImage2DARB(GL_TEXTURE_2D, 0,
                    myPixelEncoding.internalformat,
                    myWidth, myHeight, 0,
                    theImage->getMemUsed(), myReadonlyData);
            } else {
                glCompressedTexImage3DARB(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myWidth, myHeight, myDepth, 0,
                    theImage->getMemUsed(), myReadonlyData);
            }
        } else {
            // calculate texture width/height/depth
            unsigned int myTexWidth = nextPowerOfTwo(myWidth);
            unsigned int myTexHeight = nextPowerOfTwo(myHeight);
            unsigned int myTexDepth = nextPowerOfTwo(myDepth);
            AC_DEBUG << "image " << myWidth << "x" << myHeight << endl;
            AC_DEBUG << "tex " << myTexWidth << "x" << myTexHeight << endl;
            
            if (myDepth == 1) {
                AC_TRACE << "setupSingle internalFormat=" << hex << myPixelEncoding.internalformat << dec;
                // Two step initialization
                // First allocate the texture with power of two dimensions
                glTexImage2D(GL_TEXTURE_2D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, 0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    0);
                CHECK_OGL_ERROR;
                // Then upload it. This way we can upload textures that are not power of two.
                // XXX Empty textures just remain empty. Usefull for render to texture... [DS]
                if (myReadonlyData) {
                    glTexSubImage2D(GL_TEXTURE_2D, 0,
                            0, 0, myWidth, myHeight,
                            myPixelEncoding.externalformat,
                            myPixelEncoding.pixeltype, myReadonlyData);
                }
                CHECK_OGL_ERROR;
            } else {
                // upload texture
                glTexImage3D(GL_TEXTURE_3D, 0,
                    myPixelEncoding.internalformat,
                    myTexWidth, myTexHeight, myTexDepth, 0,
                    myPixelEncoding.externalformat, myPixelEncoding.pixeltype,
                    0);
                glTexSubImage3D(GL_TEXTURE_3D, 0,
                    0, 0, 0, myWidth, myHeight, myDepth,
                    myPixelEncoding.externalformat,
                    myPixelEncoding.pixeltype, myReadonlyData);
                CHECK_OGL_ERROR;
            }

            myTopLevelTextureSize = getBytesRequired(myTexWidth * myTexHeight * myTexDepth, theImage->getEncoding());

            CHECK_OGL_ERROR;
            _myTextureMemUsage +=  myTopLevelTextureSize;
        }
        theImage->storeTextureVersion();
        glPopAttrib();
        AC_DEBUG << "texmem usage="<< _myTextureMemUsage / (1024.0*1024.0) <<" MB" << endl;

        return myId;
    }

    /*
     * Cubemap texture
     */
    unsigned
    GLResourceManager::setupCubemapTexture(ImagePtr theImage)
    {
        unsigned int myId = theImage->getGraphicsId();
        glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, myId);
        CHECK_OGL_ERROR;
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                         GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                        GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        CHECK_OGL_ERROR;

        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                        GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_CUBE_MAP_ARB,
                        GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        CHECK_OGL_ERROR;

        GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();

        // number of tiles must match cubemap
        const asl::Vector2i myTileVec = theImage->get<ImageTileTag>();
        unsigned int myNumTiles = myTileVec[0] * myTileVec[1];
        if (myNumTiles < CUBEMAP_SIDES) {
            throw TextureException(std::string("Image '") + theImage->get<NameTag>() + "' number of tiles doesn't match cubemap: " + asl::as_string(myNumTiles), PLUS_FILE_LINE);
        }

        // tiles must be square
        GLsizei myTileWidth = myWidth / myTileVec[0];
        GLsizei myTileHeight = myHeight / myTileVec[1];
        if (myTileWidth != myTileHeight) {
            throw TextureException(std::string("Texture tiles are not square: ")+ theImage->get<NameTag>(), PLUS_FILE_LINE);
        }
        const unsigned int myTileSize = theImage->getMemUsed() / myNumTiles;

        glPushAttrib(GL_PIXEL_MODE_BIT);
		setupPixelTransfer(theImage);

		PixelEncodingInfo myPixelEncoding = getInternalTextureFormat(theImage);
        unsigned int myTopLevelTextureSize = theImage->getMemUsed();
        _myTextureMemUsage +=  myTopLevelTextureSize;

        if (myTileWidth != myWidth) {
            glPixelStorei(GL_UNPACK_ROW_LENGTH, myWidth);
        }

        unsigned int myBytesPerLine = getBytesRequired(myWidth, theImage->getEncoding());
        unsigned int myBytesPerTileLine = getBytesRequired(myTileWidth, theImage->getEncoding());

        // send GL the six tiles of the bitmap
        for (unsigned int i = 0; i < CUBEMAP_SIDES; ++i) {

            const unsigned char * myTile = theImage->getRasterPtr()->pixels().begin();

            // add tile offset
            unsigned int myColumn = i % myTileVec[0];
            unsigned int myRow = i / myTileVec[0];
            myTile += (myTileHeight * myBytesPerLine) * myRow +
                myBytesPerTileLine * myColumn;

            // load texture
            if (myPixelEncoding.compressedFlag) {
                glCompressedTexImage2DARB(ourCubeMapFaces[i], 0,
                                          myPixelEncoding.internalformat,
                                          myTileWidth, myTileHeight, 0,
                                          myTileSize, myTile);
            } else {
                glTexImage2D(ourCubeMapFaces[i], 0,
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
        }
        glPopAttrib();
        return myId;
    }

    unsigned
    GLResourceManager::setupTexture(ImagePtr theImage) {
        AC_DEBUG << "setupTexture(" << theImage->get<NameTag>() << ")";
        glPushAttrib(GL_ALL_ATTRIB_BITS);

        if (theImage->getGraphicsId()) {
            AC_TRACE << "setupTexture('" << theImage->get<NameTag>() << "') called on an already setup texture" << endl;
            unbindTexture(&*theImage);
        }

        unsigned myId;
        glGenTextures(1, &myId);
        CHECK_OGL_ERROR;
        theImage->setGraphicsId(myId);
        theImage->storeTextureVersion();

        switch (theImage->getType()) {
            case SINGLE:
                setupSingleTexture(theImage);
                break;
            case CUBEMAP:
                setupCubemapTexture(theImage);
                break;
            default:
                throw TextureException(std::string("Unknown texture type '")+
                        theImage->get<NameTag>() + "'", PLUS_FILE_LINE);
        }
        glPopAttrib();
        CHECK_OGL_ERROR;
        return myId;
    }

    void
    GLResourceManager::unbindTexture(Image * theImage) {
        AC_TRACE << "unbindTexture(" << theImage->get<NameTag>() << ")";
        unsigned myId = theImage->getGraphicsId();
        if (myId) {
            glDeleteTextures(1, &myId);
            theImage->setGraphicsId(0);

            // adjust texmem usage
            unsigned int myTopLevelTextureSize = theImage->getMemUsed();
            _myTextureMemUsage -=  myTopLevelTextureSize;
        }
    }

    void
    GLResourceManager::setupPixelTransfer(ImagePtr theImage) {
        const Vector4f & myColorBias   = theImage->get<ImageColorBiasTag>();
        const Vector4f & myColorScale  = theImage->get<ImageColorScaleTag>();

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
    }

    void
    GLResourceManager::updateTextureData(ImagePtr theImage) {
        PixelEncodingInfo myPixelEncoding = getInternalTextureFormat(theImage);

        theImage->storeTextureVersion();
        GLsizei myWidth  = theImage->get<ImageWidthTag>();
        GLsizei myHeight = theImage->get<ImageHeightTag>();
        glPushAttrib(GL_PIXEL_MODE_BIT | GL_TEXTURE_BIT);
        setupPixelTransfer(theImage);

        if (theImage->getType() == CUBEMAP) {

            glBindTexture(GL_TEXTURE_CUBE_MAP_ARB, theImage->getGraphicsId());
            CHECK_OGL_ERROR;

            const asl::Vector2i myTileVec = theImage->get<ImageTileTag>();
            unsigned int myNumTiles = myTileVec[0] * myTileVec[1];

            GLsizei myTileWidth = myWidth / myTileVec[0];
            GLsizei myTileHeight = myHeight / myTileVec[1];
            const unsigned int myTileSize = theImage->getMemUsed() / myNumTiles;

            if (myTileWidth != myWidth) {
                glPixelStorei(GL_UNPACK_ROW_LENGTH, myWidth);
            }
            unsigned int myBytesPerLine = getBytesRequired(myWidth, theImage->getEncoding());
            unsigned int myBytesPerTileLine = getBytesRequired(myTileWidth, theImage->getEncoding());

            for (unsigned int i = 0; i < CUBEMAP_SIDES; ++i) {

                const unsigned char * myTile = theImage->getRasterPtr()->pixels().begin();

                unsigned int myColumn = i % myTileVec[0];
                unsigned int myRow = i / myTileVec[0];
                myTile += (myTileHeight * myBytesPerLine) * myRow +
                    myBytesPerTileLine * myColumn;

                if (myPixelEncoding.compressedFlag) {
                    glCompressedTexSubImage2DARB(ourCubeMapFaces[i], 0,
                                                 0, 0,
                                                 myTileWidth, myTileHeight,
                                                 myPixelEncoding.externalformat,
                                                 myTileSize, myTile);
                } else {
                    glTexSubImage2D(ourCubeMapFaces[i], 0,
                                    0, 0,
                                    myTileWidth, myTileHeight,
                                    myPixelEncoding.externalformat,
                                    myPixelEncoding.pixeltype, myTile);
                }
                CHECK_OGL_ERROR;
            }
        } else {
            GLsizei myDepth  = theImage->get<ImageDepthTag>();

            if (myDepth == 0) {
                myDepth = 1;
            }

            if (myDepth == 1) {
                glBindTexture(GL_TEXTURE_2D, theImage->getGraphicsId());
            } else {
                glBindTexture(GL_TEXTURE_3D, theImage->getGraphicsId());
            }

            if (myPixelEncoding.compressedFlag) {
                if (myDepth == 1) {
                    glCompressedTexSubImage2DARB(GL_TEXTURE_2D, 0,
                                                0, 0, myWidth, myHeight,
                                                myPixelEncoding.externalformat,
                                                theImage->getMemUsed(),
                                                theImage->getRasterPtr()->pixels().begin());
                } else {
                    glCompressedTexSubImage3DARB(GL_TEXTURE_3D, 0,
                                                0, 0, 0, myWidth, myHeight, myDepth,
                                                myPixelEncoding.externalformat,
                                                theImage->getMemUsed(),
                                                theImage->getRasterPtr()->pixels().begin());
                }
            } else {
                if (myDepth == 1) {
                    AC_TRACE << "subloading 2D " << myWidth << "x" << myHeight << " internalFormat=" << hex << myPixelEncoding.internalformat << dec;
                    glTexSubImage2D(GL_TEXTURE_2D, 0,
                                    0, 0, myWidth, myHeight,
                                    myPixelEncoding.externalformat,
                                    myPixelEncoding.pixeltype, theImage->getRasterPtr()->pixels().begin());
                } else {
                    DB(AC_TRACE << "subloading 3D " << myWidth << "x" << myHeight << "x" << myDepth);
                    glTexSubImage3D(GL_TEXTURE_3D, 0,
                                    0, 0, 0, myWidth, myHeight, myDepth,
                                    myPixelEncoding.externalformat,
                                    myPixelEncoding.pixeltype, theImage->getRasterPtr()->pixels().begin());
                }
            }
        }
        glPopAttrib();
        CHECK_OGL_ERROR;
    }

    void
    GLResourceManager::setTexturePriority(Image * theImage, float thePriority) {
        unsigned myTextureId = theImage->getGraphicsId();
        glPrioritizeTextures(1, &myTextureId, &thePriority);
    }

    void
    GLResourceManager::rebindTexture(ImagePtr theImage) {
        AC_TRACE << "GLResourceManager::rebindTexture " << theImage->get<NameTag>();
        unbindTexture(&(*theImage));

        unsigned myId = setupTexture(theImage);
        theImage->setGraphicsId(myId);
        CHECK_OGL_ERROR;
    }

    PixelEncodingInfo
    GLResourceManager::getInternalTextureFormat(ImagePtr theImage) {
        PixelEncoding myEncoding = theImage->getEncoding();
        PixelEncodingInfo myEncodingInfo = getDefaultGLTextureParams(myEncoding);

        std::string myFormatString = theImage->get<ImageInternalFormatTag>();
        if (! myFormatString.empty()) {
            TextureInternalFormat myFormat = TextureInternalFormat(
                getEnumFromString(myFormatString, TextureInternalFormatStrings));
            myEncodingInfo.internalformat = asGLTextureInternalFormat(myFormat);
        }

        return myEncodingInfo;
    }

    IShaderLibraryPtr 
    GLResourceManager::getShaderLibrary() const {
        return _myShaderLibrary;
    }

    void GLResourceManager::loadShaderLibrary(const std::string & theShaderLibraryFile) {
        _myShaderLibrary = ShaderLibraryPtr(new ShaderLibrary);
        _myShaderLibrary->load(theShaderLibraryFile);
    }
}
