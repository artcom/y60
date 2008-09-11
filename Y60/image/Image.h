//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
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
#include <y60/base/CommonTags.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/AttributePlug.h>
#include <asl/dom/Facade.h>

#include <paintlib/planybmp.h>

namespace asl {
    class PackageManager;
}

namespace y60 {

    /**
     * Exception
     */
    DEFINE_EXCEPTION(ImageException, asl::Exception);
    DEFINE_EXCEPTION(RasterValueDoesNotExist, ImageException);

    /**
     * @ingroup y60image
     * Facade for an image node in the dom.
     */
    class Image :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public ImageSourceTag::Plug,
        public ImageResizeTag::Plug,
        public ImageFilterTag::Plug,
        public ImageFilterParamsTag::Plug,
        public ImageMatrixTag::Plug,
        public ImageTileTag::Plug,
        public ImageDepthTag::Plug,
        public TargetPixelFormatTag::Plug,
        public dom::FacadeAttributePlug<RasterPixelFormatTag>,   
        public dom::FacadeAttributePlug<ImageBytesPerPixelTag>,        
        public dom::FacadeAttributePlug<ImageWidthTag>,
        public dom::FacadeAttributePlug<ImageHeightTag>,
        public dom::FacadeAttributePlug<LastActiveFrameTag>
    {
        public:
            static bool allowInlineFlag;

            Image(dom::Node & theNode);
            IMPLEMENT_FACADE(Image);

            virtual ~Image();

            void unbind();

            virtual void load();

            /**
             * Creates a new empty raster with the given properties
             */
            dom::ResizeableRasterPtr createRaster(unsigned theWidth, unsigned theHeight, 
                                                  unsigned theDepth,
                                                  PixelEncoding theEncoding); 

            /**
             * Creates a new raster with the given properties and pixels
             */
            dom::ResizeableRasterPtr createRaster(unsigned theWidth, unsigned theHeight, 
                                                  unsigned theDepth,
                                                  PixelEncoding theEncoding,
                                                  const asl::ReadableBlock & thePixels); 

            void blitImage(const asl::Ptr<Image, dom::ThreadingModel> & theSourceImage,
                           const asl::Vector2i & theTargetPos,
                           const asl::Box2i * theSourceRect = 0);

            // Returns the pixel encoding of the raster image 
            PixelEncoding getRasterEncoding() const;

            unsigned getMemUsed() const {
                if (getRasterPtr()) {
                    return getRasterPtr()->pixels().size();
                }
                return 0;
            }

            /** Saves the image to disk (png, tiff, or jpeg).
             */
            void saveToFile(const std::string & theImagePath,
                    const VectorOfString & theFilter = VectorOfString(),
                    const VectorOfVectorOfFloat & theFilterParams = VectorOfVectorOfFloat());

            /** Apply filter to the image. 
             */
            void applyFilter(const std::string & theFilter, 
                             const VectorOfFloat & theFilterParam);

            /// Get the node version number.
            unsigned long long getValueVersion(unsigned theChildNodeNum = 0) const {
                dom::NodePtr myValueNode = getRasterValueNode(theChildNodeNum);
                if (myValueNode) {
                    return myValueNode->nodeVersion();
                }
                return 0;
            }

            dom::NodePtr getRasterValueNode(unsigned theChildNodeNum = 0) const {
				if (getNode().hasChildNodes() ) {
					dom::NodePtr myValueElement = getNode().childNode(theChildNodeNum);
					if (myValueElement) {
						return myValueElement->firstChild();
					}
				}
                return dom::NodePtr(0);
            }

            const dom::ValuePtr getRasterValue(unsigned theChildNodeNum = 0) const {
                dom::NodePtr myValueNode = getRasterValueNode(theChildNodeNum);
                if (myValueNode) {
                    dom::ValuePtr myValue = myValueNode->nodeValueWrapperPtr();
                    if (myValue->isDirty()) {
                        // Trigger raster reload
                        myValue->accessReadableBlock();

                        // Get node again, because reload might have changed the raster
                        // node in dom
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

            dom::ResizeableRasterPtr getRasterPtr(unsigned theChildNodeNum = 0) {
                return dynamic_cast_Ptr<dom::ResizeableRaster>(getRasterValue(theChildNodeNum));
            }

            const dom::ResizeableRasterPtr getRasterPtr(unsigned theChildNodeNum = 0) const {
                return dynamic_cast_Ptr<dom::ResizeableRaster>(getRasterValue(theChildNodeNum));
            }

            virtual void registerDependenciesRegistrators();

            void preload();

        private:
            Image();

            void registerDependenciesForRasterValueUpdate();
            void registerDependenciesForImageWidthUpdate();
            void registerDependenciesForImageHeightUpdate();

            void unregisterRasterValue();

            void calculateWidth();
            void calculateHeight();

            dom::ResizeableRasterPtr setRasterValue(dom::ValuePtr theRaster, 
                PixelEncoding theEncoding, unsigned theDepth);

            void convertToPLBmp(PLAnyBmp & theBitmap);
            void convertFromPLBmp(PLAnyBmp & theBitmap);

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
