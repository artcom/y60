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
//   $RCSfile: GLResourceManager.h,v $
//   $Author: martin $
//   $Revision: 1.2 $
//   $Date: 2005/04/21 16:25:02 $
//
//  Description: 
//
//=============================================================================

#ifndef _ac_scene_GLResourceManager_h_
#define _ac_scene_GLResourceManager_h_

#include <y60/GLUtils.h>
#include <y60/PixelEncodingInfo.h>

#include <y60/VertexMemory.h>
#include <y60/VertexVector.h>
#include <y60/VertexArray.h>
#include <y60/VertexBufferObject.h>
#include <y60/GLAlloc.h>
#include "ShaderLibrary.h"
#include <asl/Singleton.h>
#include <y60/VertexData.h>
#include <y60/ResourceManager.h>

namespace y60 {

    enum VertexDataStorageType {
        /// always use std::vector in main memory to store vertex arraya
        VERTEX_STORAGE_IS_STD_VECTOR, 

        /// use std::vector with GLAlloc allocator that tries to use Vertex Array Range on AGP Memory
        VERTEX_STORAGE_IS_GFX_VECTOR,

        /// use OpenGL 1.5 Vertex Buffer Objects
        VERTEX_STORAGE_IS_VBO_VECTOR
    };

    class VertexDataStorageTypeSetting : public asl::Singleton<VertexDataStorageTypeSetting> {
    public:
        VertexDataStorageTypeSetting() : _myType(VERTEX_STORAGE_IS_STD_VECTOR),
                                         _myUsageType(VERTEX_USAGE_STATIC_DRAW)
        {   
            const char * myUsageString = getenv("Y60_VBO_USAGE");
            if (myUsageString) {
                VertexBufferUsageType myUsageType = getVertexBufferUsageTypeFromString(myUsageString);
                if (myUsageType !=VERTEX_USAGE_UNDEFINED) {
                    _myUsageType = myUsageType;
                    AC_INFO << "VertexBufferUsageType '"<<myUsageString<<"' is used from Y60_VBO_USAGE environment variable."<<std::endl; 
                } else {
                    AC_WARNING << "Unknown value '"<<myUsageString<<"' in environment variable Y60_VBO_USAGE ignored."<<std::endl; 
                    AC_INFO << "Recognized values are:";
                    for (int i = 0; VertexBufferUsageTypeList[i] != VERTEX_USAGE_UNDEFINED; ++i) {
                        AC_INFO << "  " << VertexBufferUsageTypeNames[i] << std::endl;
                    }
                }
            }

            if ((_myUsageType != VERTEX_USAGE_DISABLED) &&
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

        VertexDataStorageType getType() const {
            return _myType;
        }
        VertexBufferUsageType getUsageType() const {
            return _myUsageType;
        }
    private:
        VertexDataStorageType _myType;
        VertexBufferUsageType _myUsageType;
    };

    struct PixelEncodingInfo;   // Forward Declaration. Defined in PixelEncodingInfo.h
    /**
     * @ingroup y60render
     * OpenGL implementation of a ResourceManager. Handles texture uploads and 
     * vertex buffers and is some sort of representation of the graphics hardware. 
     * It is used by the TextureManager in Scene. Implemented as a singleton.
     */
    class GLResourceManager : public ResourceManager, public asl::Singleton<GLResourceManager> {
    public:
        GLResourceManager() : _myShaderLibrary(ShaderLibraryPtr(0)) {
        }

        void initVertexDataFactories() {
            _myVertexDataFactory1f.setFactoryMethod(& create<float> );
            _myVertexDataFactory2f.setFactoryMethod(& create<asl::Vector2f> );
            _myVertexDataFactory3f.setFactoryMethod(& create<asl::Vector3f> );
            _myVertexDataFactory4f.setFactoryMethod(& create<asl::Vector4f> );
        }

        template<class T>
        static asl::Ptr<VertexData<T> > create() {
            const VertexDataStorageTypeSetting & mySettings = VertexDataStorageTypeSetting::get();
            switch (mySettings.getType()) {
                case VERTEX_STORAGE_IS_STD_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexMemory<T>());
                case VERTEX_STORAGE_IS_GFX_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexArray<T>());
                case VERTEX_STORAGE_IS_VBO_VECTOR:
                    return asl::Ptr<VertexData<T> >(new VertexBufferObject<T>(mySettings.getUsageType()));
                default:
                    throw asl::Exception(JUST_FILE_LINE);
            }
        };

        virtual int getMaxTextureSize(int theDimensions) const;
        virtual void updateTextureData(asl::Ptr<Image, dom::ThreadingModel> theImage);
        virtual void rebindTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);
        virtual unsigned getMaxTextureUnits() const;
        virtual void setTexturePriority(Image * theImage, float thePriority);
        virtual void unbindTexture(Image * theImage);

        DEFINE_NESTED_EXCEPTION(GLResourceManager, TextureException, asl::Exception);
        /**
         *
         * @param theImage 
         * @return id of the texture generated from theImage
         * @throws TextureException Unknown type of texture or corrupt image provided
         */
        virtual unsigned setupTexture(asl::Ptr<Image, dom::ThreadingModel> theImage);
        virtual IShaderLibraryPtr getShaderLibrary() const;

        void loadShaderLibrary(const std::string & theShaderLibraryFile);
    private:
        static PixelEncodingInfo getInternalTextureFormat(ImagePtr theImage);

        /**
         *
         * @param theImage 
         * @throws TextureException theImage was invalid or contained no raster info
         * @return 
         */
        unsigned setupSingleTexture(ImagePtr theBmp);
        /**
         *
         * @param theBmp 
         * @return 
         * @throws TextureException theImage was not a cubemap or invalid
         */
        unsigned setupCubemapTexture(ImagePtr theBmp);
        /**
         * Sets up color scale and color bias
         * @note call glPushAttrib(GL_PIXEL_MODE_BIT) before usage
         * @param theImage Image containing the scale and bias attributes
         */
        void setupPixelTransfer(ImagePtr theImage);
        unsigned long _myTextureMemUsage;
        ShaderLibraryPtr _myShaderLibrary;
        
    };

}


#endif


