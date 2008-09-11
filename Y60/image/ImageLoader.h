//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _ac_y60_ImageLoader_h_
#define _ac_y60_ImageLoader_h_

#include "PixelEncoding.h"
#include "ITextureManager.h"
#include <y60/base/NodeValueNames.h>

#include <asl/base/Ptr.h>
#include <asl/math/Matrix4.h>
#include <asl/base/Stream.h>
#include <asl/zip/PackageManager.h>

// Dumb hack since paintlib is installed as a non-debug variant
#ifdef _DEBUG
#undef _DEBUG
#include <paintlib/plbitmap.h>
#define _DEBUG
#else
#include <paintlib/plbitmap.h>
#endif

/*
namespace asl {
    //class ReadableStream;
    class PackageManager;
}
*/
namespace y60 {
    typedef std::vector<ImageFilter> VectorOfImageFilter;

    /* Image filtering on save/load */
    void applyCustomFilter(PLBmp & theBitmap, const VectorOfString & theFilterName,
                           const VectorOfVectorOfFloat & theFilterparams);

    void applyCustomFilter(PLBmp & theBitmap, ImageFilter theFilter,
                           const std::vector<float> & theFilterparams);

    void applyCustomFilter(PLBmp & theBitmap, const std::string & theFilterName, 
                           const VectorOfFloat & theFilterparams);

    DEFINE_EXCEPTION(ImageLoaderException, asl::Exception);

    class ImageLoader : public PLBmp {
    public:
        //! Creates an empty bitmap.
        ImageLoader(const ITextureManagerPtr & theTextureManager = ITextureManagerPtr(0));
        //! Creates and loads a bitmap from a block.
        ImageLoader(asl::Ptr<asl::ReadableBlockHandle> theInputBlock, 
                    const std::string & theFileDescription="",
                    const ITextureManagerPtr & theTextureManager = ITextureManagerPtr(0), 
                    unsigned theDepth = 1);
        //! Creates and loads a bitmap, using an optional package manager to search for the file.
        ImageLoader(const std::string & theFilename, 
               asl::Ptr<asl::PackageManager> thePackageManager = asl::Ptr<asl::PackageManager>(0), 
               const ITextureManagerPtr & theTextureManager = ITextureManagerPtr(0), 
               unsigned theDepth = 1);

        //! Destroys the bitmap.
        virtual ~ImageLoader();

        // ImageLoader information
        //! Returns the amount of memory used by the object.
        virtual long GetMemUsed();

        //! Returns number of bytes used per line.
        long GetBytesPerLine();

        /////////////////////////////////////////////////////////////
        // Members Specific to ImageLoader

        PixelEncoding getEncoding() const {
            return _myEncoding;
        }

        dom::ValuePtr getData() {
            return _myRasterData;
        }
        dom::ResizeableRasterPtr getRaster() {
            return dynamic_cast_Ptr<dom::ResizeableRaster>(_myRasterData);
        }

        const asl::Matrix4f & getImageMatrix() const {
            return _myImageMatrix;
        }

        /**
         * Ensure the loaded image is power-of-two.
         * @param theResizeMode controls how the image is to be resized if it's not power-of-two.
         * @param theDepth is for 3D textures. It describes how many slices are placed in the image.
         * @param theTile specifies the number and layout of tiles for CUBEMAP images.
         * For SINGLE images this can be 0 or (1,1).
         */
        void ensurePowerOfTwo(const std::string & theResizeMode,
                              unsigned theDepth = 1, const asl::Vector2i * theTile = 0);

        void removeUnusedAlpha();
        void setFixedAlpha(float theAlpha);

        void applyCustomFilter(ImageFilter theFilter, const std::vector<float> & theFilterparams);
        void applyCustomFilter(const std::string & theFilter, const std::vector<float> & theFilterparams);

        unsigned long getHeaderSize() const {
            return _myHeaderSize;
        }

    protected:
        //! Create a new empty DIB. Bits are uninitialized.
        //! Assumes that no memory is allocated before the call.
        virtual void internalCreate(PLLONG Width, PLLONG Height,
                                    const PLPixelFormat &  pf);

        //! Deletes memory allocated by member variables.
        virtual void freeMembers();

        //! Initializes internal table of line addresses.
        virtual void initLineArray();

        // Set color table pointer & pointer to bits
        //  virtual void initPointers ();

    private:
        void loadSingleImage(asl::Ptr<asl::ReadableBlockHandle> theImageBlock);
        void loadCubemap(std::vector<asl::Ptr<asl::ReadableBlockHandle> > & theBlocks, unsigned theDepth);
        void loadI60File(asl::Ptr<asl::ReadableBlockHandle> theImageBlock);
        void updateLineArray();

        dom::ValuePtr                 _myRasterData;
        PixelEncoding                 _myEncoding;
        std::string                   _myFilename;
        unsigned long                 _myHeaderSize;
        bool                          _isI60;
        asl::Matrix4f                 _myImageMatrix;
        asl::WeakPtr<ITextureManager> _myTextureManager;
    };

}
#endif
