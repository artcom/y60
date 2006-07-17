//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_Image_h_
#define _ac_y60_Image_h_

#include "PixelEncoding.h"
#include "ImageTags.h"
#include <y60/CommonTags.h>
#include <y60/NodeValueNames.h>

#include <dom/AttributePlug.h>
#include <dom/Facade.h>

#include <paintlib/planybmp.h>

namespace asl {
    class PackageManager;
}

namespace y60 {

    /**
     * Exception
     */
    DEFINE_EXCEPTION(ImageException, asl::Exception);
    /**
     * Exception
     */
    DEFINE_EXCEPTION(RasterValueDoesNotExist, ImageException);

    /**
     * @ingroup y60image
     * Facade for an image node in the dom.
     *
     */
    class Image :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public ImageDepthTag::Plug,
        public ImageSourceTag::Plug,
        public TexturePixelFormatTag::Plug,
        public ImageTypeTag::Plug,
        public ImageMipmapTag::Plug,
        public ImageResizeTag::Plug,
        public ImageFilterTag::Plug,
        public ImageFilterParamsTag::Plug,
        public ImageMatrixTag::Plug,
        public ImageColorBiasTag::Plug,
        public ImageColorScaleTag::Plug,
        public ImageTileTag::Plug,
        public dom::FacadeAttributePlug<RasterPixelFormatTag>,   
        public dom::FacadeAttributePlug<ImageBytesPerPixelTag>,        
        public dom::FacadeAttributePlug<ImageWidthTag>,
        public dom::FacadeAttributePlug<ImageHeightTag>,
        public dom::FacadeAttributePlug<LoadCountTag>,
        protected dom::FacadeAttributePlug<ImageInternalFormatTag>,
        protected dom::FacadeAttributePlug<TextureIdTag>
    {
        public:
            Image(dom::Node & theNode);
            IMPLEMENT_FACADE(Image);

            virtual ~Image();

            void unbind();
            void removeTextureFromResourceManager();
            virtual bool usePixelBuffer() const;
            void setPixelBufferId(unsigned theId) {
                _myPixelBufferId = theId;
            }

            unsigned getPixelBufferId() const {
                return _myPixelBufferId;
            }

            // Gets graphics id, without uploading it. Id is 0 if image has not been uploaded
            unsigned getGraphicsId() const {
                return _myTextureId;
            }

            // Uploads the image if neccessary.
            unsigned ensureTextureId() {
                return get<TextureIdTag>();
            }

            // Dirty hack.
            void triggerUpload();

            void registerTexture();
            void deregisterTexture();

            virtual void load();


            /**
             * Creates a new empty raster with the given properties
             */
            dom::ResizeableRasterPtr createRaster(unsigned theWidth,
                                                  unsigned theHeight,
                                                  unsigned theDepth,
                                                  PixelEncoding theEncoding); 

            /**
             * Creates a new raster with the given properties and pixels
             */
            dom::ResizeableRasterPtr createRaster(unsigned theWidth,
                                                  unsigned theHeight,
                                                  unsigned theDepth,
                                                  PixelEncoding theEncoding,
                                                  const asl::ReadableBlock & thePixels); 

            void blitImage(const asl::Ptr<Image, dom::ThreadingModel> & theSourceImage,
                           const asl::Vector2i & theTargetPos,
                           const asl::Box2i * theSourceRect = 0);

            ImageType getType() const;

            // Returns the pixel encoding of the raster image 
            PixelEncoding getRasterEncoding() const;

            // Returns the pixel encoding of the texture 
            TextureInternalFormat getInternalEncoding() const;

            unsigned getMemUsed() const{
                if (getRasterPtr()) {
                    return getRasterPtr()->pixels().size();
                } else {
                    return 0;
                }
            }

            /** Saves the image to disk using the PNG format. 
             */
            void saveToFile(const std::string & theImagePath);

            void saveToFileFiltered(const std::string & theImagePath, const VectorOfString & theFilter,
                                    const VectorOfVectorOfFloat & theFilterParams);

            /** Apply filter to the image. 
             */
            void applyFilter(const std::string & theFilter, const VectorOfFloat & theFilterParam);

            /// Get the node version number.
            unsigned long long getValueVersion() const {
                dom::NodePtr myValueNode = getRasterValueNode();
                if (myValueNode) {
                    return myValueNode->nodeVersion();
                }
                return 0;
            }

            dom::NodePtr getRasterValueNode() const {
                dom::NodePtr myValueElement = getNode().firstChild();
                if (myValueElement) {
                    return myValueElement->firstChild();
                }
                return dom::NodePtr(0);
            }

            const dom::ValuePtr getRasterValue() const {
                dom::NodePtr myValueNode = getRasterValueNode();
                if (myValueNode) {
                    dom::ValuePtr myValue = myValueNode->nodeValueWrapperPtr();
                    if (myValue->isDirty()) {
                        // Trigger raster reload
                        myValue->accessReadableBlock();

                        // Get node again, because reload might have changed the raster node in dom
                        myValueNode = getRasterValueNode();
                        if (myValueNode) {
                            return myValueNode->nodeValueWrapperPtr();
                        } 
                    } else {
                        return myValue;
                    }
                }
                return dom::ValuePtr(0);
            }

            dom::ResizeableRasterPtr getRasterPtr() {
                return dynamic_cast_Ptr<dom::ResizeableRaster>(getRasterValue());
            }

            const dom::ResizeableRasterPtr getRasterPtr() const {
                return dynamic_cast_Ptr<dom::ResizeableRaster>(getRasterValue());
            }

            virtual void registerDependenciesRegistrators();
            bool isUploaded() const;

        private:
            Image();

            void registerDependenciesForRasterValueUpdate();
            void registerDependenciesForImageWidthUpdate();
            void registerDependenciesForImageHeightUpdate();
            void registerDependenciesForImageFormatUpdate();
            void registerDependenciesForTextureUpdate();

            void uploadTexture();
            void calculateInternalPixelFormat();
            void calculateWidth();
            void calculateHeight();

            dom::ResizeableRasterPtr setRasterValue(dom::ValuePtr theRaster, 
                PixelEncoding theEncoding, unsigned theDepth);

            void convertToPLBmp(PLAnyBmp & theBitmap);
            void convertFromPLBmp(PLAnyBmp & theBitmap);

            bool                          _myReuseRaster;
            unsigned                      _myTextureId;
            unsigned                      _myPixelBufferId;

            int                           _myRefCount;
            asl::Ptr<asl::PackageManager> _myPackageManager;
    };

    typedef asl::Ptr<Image, dom::ThreadingModel> ImagePtr;
    typedef asl::WeakPtr<Image, dom::ThreadingModel> ImageWeakPtr;

    inline
    ImageFilter lookupFilter(TextureUsage theUsage) {
        switch (theUsage) {
            case BUMP:
                return HEIGHT_TO_NORMALMAP;
            default:
                return NO_FILTER;
        }
        return NO_FILTER; // avoid gcc4.0 warning
    }
}

#endif
