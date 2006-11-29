//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_scene_GLResourceManager_h_
#define _ac_scene_GLResourceManager_h_

#include "ShaderLibrary.h"

#include <asl/Singleton.h>

#include <y60/ResourceManager.h>
#include <y60/GLUtils.h>

#include <y60/VertexData.h>
#include <y60/VertexMemory.h>
#include <y60/VertexVector.h>
#include <y60/VertexArray.h>
#include <y60/VertexBufferObject.h>
#include <y60/GLAlloc.h>

namespace y60 {

    enum VertexDataStorageType {
        VERTEX_STORAGE_IS_STD_VECTOR, /// use std::vector in main memory
        VERTEX_STORAGE_IS_GFX_VECTOR, /// use std::vector with GLAlloc allocator that tries to use Vertex Array Range on AGP Memory
        VERTEX_STORAGE_IS_VBO_VECTOR /// use OpenGL 1.5 Vertex Buffer Objects
    };

    /**
     * @ingroup y60render
     * VertexData storage type singleton.
     * This singleton class configures and stores the VertexData storage type.
     * Storage types are system memory, AGP, or Vertex Buffer Objects.
     * If VBOs are used then the usage profile can be configured with the environment
     * variable Y60_VBO_USAGE.
     */
    class VertexDataStorageTypeSetting : public asl::Singleton<VertexDataStorageTypeSetting> {
    public:
        VertexDataStorageTypeSetting() : _myType(VERTEX_STORAGE_IS_STD_VECTOR)
        {
            const char * myUsageString = getenv("Y60_VBO_USAGE");
            bool myVBOUsage = true;
            if (myUsageString) {
                try {
                    myVBOUsage = asl::as<bool>(myUsageString);
                } catch (const std::exception & e) {
                    AC_WARNING << "Y60_VBO_USAGE environment must be 0 or 1; " << e.what();
                } catch (const asl::Exception & e) {
                    AC_WARNING << "Y60_VBO_USAGE environment must be 0 or 1; " << e;
                }
            }
            //AC_PRINT << "ENV: " << std::string(myUsageString ? myUsageString: "not defined") << " -> " << myVBOUsage;
            if (myVBOUsage &&
                (queryOGLExtension("GL_ARB_vertex_buffer_object", true) ||
                 queryOGLExtension("GL_EXT_vertex_buffer_object", true)))
            {
                _myType = VERTEX_STORAGE_IS_VBO_VECTOR;
                return;
            }
            if (asl::AGPVertexArray::getDefaultCapacity() != asl::AGPVertexArray::DISABLED) {
                _myType = VERTEX_STORAGE_IS_GFX_VECTOR;
                return;
            }
        }

        /**
         * Return VertexData storage type.
         * @return VertexData storage type, e.g. VERTEX_STORAGE_IS_VBO_VECTOR.
         */
        VertexDataStorageType getType() const {
            return _myType;
        }

    private:
        VertexDataStorageType _myType;
    };

    struct PixelEncodingInfo; // Forward Declaration. Defined in PixelEncodingInfo.h

    /**
     * @ingroup y60render
     * OpenGL implementation of a ResourceManager. Handles texture uploads and
     * vertex buffers and is some sort of representation of the graphics hardware.
     * It is used by the TextureManager in Scene. Implemented as a singleton.
     */
    class GLResourceManager : public ResourceManager, public asl::Singleton<GLResourceManager> {
    public:
        DEFINE_NESTED_EXCEPTION(GLResourceManager, TextureException, asl::Exception);

        GLResourceManager() : _myShaderLibrary(ShaderLibraryPtr(0)) {
            validateGLContext(true);
        }

        void initVertexDataFactories() {
            _myVertexDataFactory1f.setFactoryMethod(& create<float> );
            _myVertexDataFactory2f.setFactoryMethod(& create<asl::Vector2f> );
            _myVertexDataFactory3f.setFactoryMethod(& create<asl::Vector3f> );
            _myVertexDataFactory4f.setFactoryMethod(& create<asl::Vector4f> );
        }

        /**
         * Allocate VertexData from the configured memory (system memory, AGP, or VBO).
         * @return Pointer to VertexData<T> object.
         */
        template<class T>
        static asl::Ptr<VertexData<T> > create(const VertexBufferUsage & theUsage) {
            const VertexDataStorageTypeSetting & mySettings = VertexDataStorageTypeSetting::get();
            switch (mySettings.getType()) {
                case VERTEX_STORAGE_IS_STD_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexMemory<T>());
                case VERTEX_STORAGE_IS_GFX_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexArray<T>());
                case VERTEX_STORAGE_IS_VBO_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexBufferObject<T>(theUsage));
                default:
                    throw asl::Exception(JUST_FILE_LINE);
            }
        };

        /**
         * Return maximum texture size for a texture of the given dimension.
         * @param theDimension Texture dimension, may be 1,2, or 3.
         * @return Maximum texture size.
         * @throws TextureException Given texture dimension is not supported.
         */
        int getMaxTextureSize(int theDimensions) const;

        /**
         * Set the texture priority.
         * Zero (0.0) is the lowest, one (1.0) the highest priority.
         * Values outside this range are clamped.
         * @param theImage Image for which to set the priority.
         */
        void setTexturePriority(Image * theImage, float thePriority);

        /**
         * Updates the texture data.
         * This function re-uploads the Image pixels to the GL texture memory.
         * @param theImage Image to update.
         */
        void updateTextureData(ImagePtr theImage);

        /**
         * Rebinds the GL texture object.
         * This function re-binds the texture by unbinding it and calling setupTexture.
         * @param theImage Image to rebind.
         */
        //void rebindTexture(ImagePtr theImage);

        /**
         * Delete the texture object for the given Image.
         * @param theImage Image to unbind.
         */
        void unbindTexture(Image * theImage);

        /**
         * Setup the GL texture object for the given Image.
         * If the Image/texture is already bound it will be unbound and recreated.
         * @param theImage Image to setup/bind.
         * @return id of the texture generated from theImage
         * @throws TextureException Unknown type of texture or corrupt image provided
         */
        unsigned setupTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);

        /**
         * Load shader library from file.
         * Previously loaded library is dropped and replaced with the newly loaded one.
         * @param theShaderLibraryFile Shader library filename.
         */
        void loadShaderLibrary(const std::string & theShaderLibraryFile);

        /**
         * Return pointer to shader library.
         * @return Pointer to shader library.
         */
        IShaderLibraryPtr getShaderLibrary() const;

        /**
         * update texture wrap and min, map filter
         */
        void updateTextureParams(ImagePtr theImage);

    private:
        static PixelEncodingInfo getInternalTextureFormat(ImagePtr theImage);

        /**
         * Setup a 2D/3D texture.
         * @param theImage
         * @throws TextureException theImage was invalid or contained no raster info
         * @return
         */
        unsigned setupSingleTexture(ImagePtr theImage, unsigned theTextureId);

        /// Update 2D texture from image.
        void update2DTexture(ImagePtr theImage);

        /// Update 3D texture from image.
        void update3DTexture(ImagePtr theImage);

        /**
         * Setup a cubemap texture.
         * @param theImage
         * @return
         * @throws TextureException theImage was not a cubemap or invalid
         */
        unsigned setupCubemap(ImagePtr theImage,  unsigned theTextureId);

        /// Update cubemap from image.
        void updateCubemap(ImagePtr theImage);

        /**
         * Sets up color scale and color bias
         * @note call glPushAttrib(GL_PIXEL_MODE_BIT) before usage
         * @param theImage Image containing the scale and bias attributes
         */
        void setupPixelTransfer(ImagePtr theImage);

        GLenum getTextureType(ImagePtr theImage);
        /**
         * Sets up texture wrap and min, map filter
         */
        void setupTextureParams(ImagePtr theImage, GLenum theTextureType);

        unsigned long _myTextureMemUsage;
        ShaderLibraryPtr _myShaderLibrary;
    };
}

#endif
