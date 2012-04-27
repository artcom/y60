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

#ifndef _ac_y60_Image_h_
#define _ac_y60_Image_h_

#include "y60_image_settings.h"

#include "PixelEncoding.h"
#include "ImageTags.h"
#include "ImageLoader.h"
#include <y60/base/CommonTags.h>
#include <y60/base/NodeValueNames.h>

#include <asl/dom/AttributePlug.h>
#include <asl/dom/Facade.h>
#include <asl/base/Singleton.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planybmp.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <asl/thread/threadpool.hpp>
#include <asl/thread/concurrent_queue.h>

namespace asl {
    class PackageManager;
}

namespace y60 {

    class ImageLoaderThreadPool : public asl::Singleton<ImageLoaderThreadPool>
    {
        friend class asl::SingletonManager;
        public:
            virtual ~ImageLoaderThreadPool();
            boost::threadpool::pool & getThreadPool(); 
        private:
            ImageLoaderThreadPool();
            boost::threadpool::pool _myThreadPool;
    };
    /**
    * Exception
    */
    DEFINE_EXCEPTION(ImageException, asl::Exception);
    DEFINE_EXCEPTION(RasterValueDoesNotExist, ImageException);

    /**
    * @ingroup y60image
    * Facade for an image node in the dom.
    */
    class Y60_IMAGE_DECL Image :
        public dom::Facade,
        public IdTag::Plug,
        public NameTag::Plug,
        public ImageSourceTag::Plug,
        public ImageResizeTag::Plug,
        public ImageFilterTag::Plug,
        public ImageFilterParamsTag::Plug,
        public ImageMatrixTag::Plug,
        public ImageAsyncFlagTag::Plug,
        public ImageTileTag::Plug,
        public ImageDepthTag::Plug,
        public TargetPixelFormatTag::Plug,        
        public LoadStateTag::Plug,
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
        // check if a async load process is done
        void checkAsyncLoad();

        /** Saves the image to a block in memory (currently only as png) */
        void saveToBlock(asl::Ptr<asl::Block> & theBlock,
            const VectorOfString & theFilter = VectorOfString(),
            const VectorOfVectorOfFloat & theFilterParams = VectorOfVectorOfFloat());

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
            return dom::NodePtr();
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
            return dom::ValuePtr();
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
        void processNewRaster(boost::shared_ptr<ImageLoader> theImageLoader);

        dom::ResizeableRasterPtr setRasterValue(dom::ValuePtr theRaster,
            PixelEncoding theEncoding, unsigned theDepth);

        void convertToPLBmp(PLAnyBmp & theBitmap);
        void convertFromPLBmp(PLAnyBmp & theBitmap);

        asl::Ptr<asl::PackageManager> _myPackageManager;

        void aSyncLoad(unsigned theDepth);
        asl::thread::concurrent_queue<boost::shared_ptr<ImageLoader> > _myImageLoaderQueue;

    };

    typedef asl::Ptr<Image, dom::ThreadingModel> ImagePtr;
    typedef asl::WeakPtr<Image, dom::ThreadingModel> ImageWeakPtr;

    inline
        ImageFilter lookupFilter(TextureUsage theUsage) {
            switch (theUsage) {
            case BUMP:
                return HEIGHT_TO_NORMALMAP;
            default:
                break;
            }
            return NO_FILTER; // avoid gcc4.0 warning
    }
}

#endif
