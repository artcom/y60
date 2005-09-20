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
//   $RCSfile: Image.cpp,v $
//   $Author: ulrich $
//   $Revision: 1.4 $
//   $Date: 2005/04/19 10:02:40 $
//
//  Description: This class performs texture loading and management.
//
//=============================================================================

#include "Image.h"
#include "ImageLoader.h"
#include "ITextureManager.h"

#include <asl/string_functions.h>
#include <asl/RunLengthEncoder.h>
#include <asl/MappedBlock.h>
#include <asl/PackageManager.h>

#include <asl/subraster.h>
#include <asl/palgo.h>
#include <asl/pixels.h>
#include <asl/standard_pixel_types.h>

#include <paintlib/plpixelformat.h>
#include <paintlib/planybmp.h>
#include <paintlib/plpngenc.h>

using namespace asl;
using namespace std;

#undef DB
#define DB(x) // x

namespace y60 {

    const char * I60_MAGIC_NUMBER = "a+c ";

    DEFINE_EXCEPTION(UnknownEncodingException, asl::Exception);

    Image::Image(dom::Node & theNode) :
        Facade(theNode),
        IdTag::Plug(theNode),
        NameTag::Plug(theNode),
        ImageWidthTag::Plug(theNode),
        ImageHeightTag::Plug(theNode),
        ImageDepthTag::Plug(theNode),
        ImageSourceTag::Plug(theNode),
        ImagePixelFormatTag::Plug(theNode),
        ImageInternalFormatTag::Plug(theNode),
        ImageTypeTag::Plug(theNode),
        ImageMipmapTag::Plug(theNode),
        ImageResizeTag::Plug(theNode),
        ImageFilterTag::Plug(theNode),
        ImageFilterParamsTag::Plug(theNode),
        ImageMatrixTag::Plug(theNode),
        ImageTileTag::Plug(theNode),
        ImageColorScaleTag::Plug(theNode),
        ImageColorBiasTag::Plug(theNode),
        dom::FacadeAttributePlug<ImageBytesPerPixelTag>(this),
        _myRefCount(0),
        _myTexId(0),
        _myLoadedFilename(""),
        _myTextureImageVersion(0),
        _myTextureWidth(0), _myTextureHeight(0), _myTextureDepth(0)
    {
        if (theNode) {
            _myLoadedFilename      = get<ImageSourceTag>();
            _myAppliedFilter       = get<ImageFilterTag>();
            _myAppliedFilterParams = get<ImageFilterParamsTag>();
            _myAppliedColorBias    = get<ImageColorBiasTag>();
            _myAppliedColorScale   = get<ImageColorScaleTag>();
            _myAppliedInternalFormat = get<ImageInternalFormatTag>();
            _myAppliedPixelFormat  = get<ImagePixelFormatTag>();
            _myAppliedMipmap       = get<ImageMipmapTag>();
        }
    }

    Image::~Image() {
        if (asl::Ptr<ITextureManager> myTextureManager = _myTextureManager.lock()) {
            myTextureManager->unbindTexture(this);
        }
    }

    void
    Image::setTextureManager(const ITextureManager & theTextureManager) {
        _myTextureManager = theTextureManager.getSelf();
    }

    void
    Image::registerTexture() {
        AC_TRACE << "registerTexture " << _myLoadedFilename << " refcount: " << _myRefCount;
        if (_myRefCount == 0) {
            AC_TRACE << "first use of " << _myLoadedFilename;
            if (asl::Ptr<ITextureManager> myTextureManager = _myTextureManager.lock()) {
                myTextureManager->setPriority(this, TEXTURE_PRIORITY_IN_USE);
            }
        }
        ++_myRefCount;
    }

    void
    Image::deregisterTexture() {
        AC_TRACE << "deregisterTexture " << _myLoadedFilename << " refcount: " << _myRefCount;
        --_myRefCount;
        if (_myRefCount==0) {
            AC_TRACE << "last use of " << _myLoadedFilename;
            if (asl::Ptr<ITextureManager> myTextureManager = _myTextureManager.lock()) {
                myTextureManager->setPriority(this, TEXTURE_PRIORITY_IDLE);
            }
        }
    }

    DEFINE_EXCEPTION(BadRasterValue, asl::Exception);

    void
    Image::createRaster(PixelEncoding theEncoding) {
        dom::DOMString myRasterName = RasterElementNames[theEncoding];

        if (getNode().childNodes().size()) {
            getNode().childNodes().clear();
        }
        dom::NodePtr myRasterChild = getNode().appendChild(dom::NodePtr(new dom::Element(myRasterName)));
        dom::NodePtr myTextChild = myRasterChild->appendChild(dom::NodePtr(new dom::Text()));
    }

    void
    Image::set(unsigned int theNewWidth,
               unsigned int theNewHeight,
               unsigned int theNewDepth,
               PixelEncoding theEncoding)
    {
        set<ImageWidthTag>(theNewWidth);
        set<ImageHeightTag>(theNewHeight);
        set<ImageDepthTag>(theNewDepth);
        set<ImagePixelFormatTag>(getStringFromEnum(theEncoding, PixelEncodingString));
        
        set<ImageBytesPerPixelTag>(float(getBytesRequired(4, theEncoding))/4.0f);
        createRaster(theEncoding);

        //update applied members
        _myAppliedPixelFormat  = get<ImagePixelFormatTag>();
    }

    void
    Image::set(unsigned int theNewWidth,
               unsigned int theNewHeight,
               unsigned int theNewDepth,
               PixelEncoding theEncoding,
               const asl::ReadableBlock & thePixels)
    {
        set(theNewWidth, theNewHeight, theNewDepth, theEncoding);
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        if (!myRaster) {
            throw BadRasterValue(JUST_FILE_LINE);
        }
        myRaster->assign(theNewWidth, theNewHeight * theNewDepth, thePixels);
    }
    void
    Image::set(unsigned int theNewWidth,
               unsigned int theNewHeight,
               unsigned int theNewDepth,
               PixelEncoding theEncoding,
               dom::ValuePtr theRaster)
    {
        set(theNewWidth, theNewHeight, theNewDepth, theEncoding);
        setRasterValue(theRaster);
    }

    ImageType
    Image::getType() const {
        return ImageType(getEnumFromString(get<ImageTypeTag>(), ImageTypeStrings));
    }

    PixelEncoding
    Image::getEncoding() const {
        return PixelEncoding(getEnumFromString(get<ImagePixelFormatTag>(), PixelEncodingString));
    }

    void
    Image::load(PackageManager & thePackageManager) {
        loadFromFile(thePackageManager);
    }

    void
    Image::load(const std::string & theImagePath) {
        PackageManager myPackageManager;
        myPackageManager.add(theImagePath);
        loadFromFile(myPackageManager);
    }

    void
    Image::loadFromFile(asl::PackageManager & thePackageManager) {
        _myLoadedFilename      = get<ImageSourceTag>();
        _myAppliedFilter       = get<ImageFilterTag>();
        _myAppliedFilterParams = get<ImageFilterParamsTag>();
        _myAppliedColorBias    = get<ImageColorBiasTag>();
        _myAppliedColorScale   = get<ImageColorScaleTag>();
        _myAppliedPixelFormat  = get<ImagePixelFormatTag>();
        _myAppliedInternalFormat = get<ImageInternalFormatTag>();
        _myAppliedMipmap       = get<ImageMipmapTag>();

        if (get<ImageSourceTag>() == "") {
            //throw ImageException(string("Image ") + get<NameTag>() + " has empty source attribute", PLUS_FILE_LINE);
            //TODO: we need clarify the image loading concept here:
            AC_INFO << "Image '" << get<NameTag>() << " has not been loaded because of empty source attribute";
            return;
        }

        unsigned myDepth = get<ImageDepthTag>();
        ImageLoader myImageLoader(get<ImageSourceTag>(), &thePackageManager, _myTextureManager.lock(), myDepth);

        string myFilter = get<ImageFilterTag>();
        if (!myFilter.empty()) {
            AC_DEBUG << "Image::loadFromFile() filter=" << myFilter;
            VectorOfFloat myFilterParams = get<ImageFilterParamsTag>();
            myImageLoader.applyCustomFilter(myFilter, myFilterParams);
        }

        // ImageTileTag only relevant for getType() == CUBEMAP
        myImageLoader.ensurePowerOfTwo(get<ImageResizeTag>(), getType(), myDepth, &(get<ImageTileTag>()));

        // Drop alpha channel if unused
        const asl::Vector4f & myColorScale = get<ImageColorScaleTag>();
        if (!asl::almostEqual(myColorScale[3], 1)) {
            if (!myImageLoader.HasAlpha()) {
				// the real alpha scaling is done with glPixeltransfer routines with opengl,
				// so here we assure that the image has a alpha channel filled with 1.0 (vs/dk)
                myImageLoader.setFixedAlpha(1);
            }
        } else {
            myImageLoader.removeUnusedAlpha();
        }

        set(myImageLoader.GetWidth(),
            myImageLoader.GetHeight()/myDepth, myDepth,
            myImageLoader.getEncoding(),
            myImageLoader.getData());
        set<ImageMatrixTag>(myImageLoader.getImageMatrix());
    }

	void
	Image::saveToFile(const string & theImagePath) {
		int myWidth = get<ImageWidthTag>();
        int myHeight = get<ImageHeightTag>();
        // save 3d textures as one long stripe
        if (get<ImageDepthTag>() > 1) {
            AC_TRACE << "Saving 3D image " << theImagePath;
            myHeight *= get<ImageDepthTag>();
        }
		PLPixelFormat myPixelFormat;
        if (!mapPixelEncodingToFormat(getEncoding(), myPixelFormat)) {
              throw ImageException(std::string("Image::saveToFile(): Unsupported Encoding: ") +
                      asl::as_string(getEncoding()), PLUS_FILE_LINE);
        }
        AC_DEBUG << "Saving PNG as " << myPixelFormat.GetName();
		PLAnyBmp myBmp;
		myBmp.Create(myWidth, myHeight, myPixelFormat, NULL, 0, PLPoint(72, 72));
		PLBYTE **myLineArray = myBmp.GetLineArray();
		int myBytesPerLine = myPixelFormat.GetBitsPerPixel() * myWidth / 8;
		const unsigned char *myData = getRasterPtr()->pixels().begin();
		for(int y=0; y<myHeight; ++y) {
			memcpy(myLineArray[y], myData + myBytesPerLine * y, myBytesPerLine);
		}
		PLPNGEncoder myPNGEncoder;
		myPNGEncoder.MakeFileFromBmp(theImagePath.c_str(), &myBmp);
	}

    /// Reload *from file* required?
    bool
    Image::reloadRequired() const {

        bool myReloadRequired = !getRasterValue() ||
            !getRasterPtr() || !getRasterPtr()->pixels().size() ||
            _myLoadedFilename != get<ImageSourceTag>() ||
            _myAppliedFilter != get<ImageFilterTag>() ||
            _myAppliedFilterParams != get<ImageFilterParamsTag>();

		/*
         * want to check what condition the condition is in ? 	
         *	Ask theDude or try this:
         
if (myReloadRequired) {

		bool b1 = getRasterValue();
		bool b2 = getRasterPtr();
		bool b3 = getRasterPtr() && getRasterPtr()->pixels().size();
		const string s1 = get<ImageSourceTag>();
		bool b4 = _myLoadedFilename != get<ImageSourceTag>();
		bool b5 = _myAppliedFilterParams != get<ImageFilterParamsTag>();
		bool b6 = _myAppliedColorScale != get<ImageColorScaleTag>();
		bool b7 =_myAppliedColorBias != get<ImageColorBiasTag>();
		bool b8 =getGraphicsId() == 0; 
        bool b9 = _myAppliedPixelFormat != get<ImagePixelFormatTag>();
        bool b10= _myAppliedInternalFormat != get<ImageInternalFormatTag>();

        
        AC_PRINT 
<< "b1 = " << b1 << " " 
<< "b2 = " << b2 << " " 
<< "b3 = " << b3 << " " 
<< "b4 = " << b4 << " " 
<< "b5 = " << b5 << " " 
<< "b6 = " << b6 << " " 
<< "b7 = " << b7 << " " 
<< "b8 = " << b8 << " " 
<< "b9 = " << b9 << " " 
<< "b10 = " << b10 << " " 
<< "s1 = " << s1;

}
        */		

        return myReloadRequired;
    }

    /// Texture upload required?
    bool
    Image::textureUploadRequired() const {

        if (_myAppliedColorScale != get<ImageColorScaleTag>() ||
            _myAppliedColorBias != get<ImageColorBiasTag>()) {

            const asl::Vector4f & myColorScale = get<ImageColorScaleTag>();
            if (!asl::almostEqual(myColorScale[3], 1.0f)) {
                const std::string & myExternalFormat = get<ImagePixelFormatTag>();
                if (myExternalFormat == "RGB" || myExternalFormat == "BGR") {
                    const_cast<Image*>(this)->set<ImageInternalFormatTag>("RGBA");
                    AC_TRACE << "Using internalFormat '" << get<ImageInternalFormatTag>() << "'";
                }
            }
        }

        return getGraphicsId() == 0 ||
            _myAppliedColorScale != get<ImageColorScaleTag>() ||
            _myAppliedColorBias != get<ImageColorBiasTag>() ||
            _myAppliedPixelFormat != get<ImagePixelFormatTag>() ||
            !canReuseTexture();
    }

    /// Applied texture parameters are compatible with DOM values.
    bool
    Image::canReuseTexture() const {
        return getGraphicsId() != 0 &&
            _myTextureWidth == get<ImageWidthTag>() &&
            _myTextureHeight == get<ImageHeightTag>() &&
            _myTextureDepth == get<ImageDepthTag>() &&
            _myTexturePixelFormat == get<ImagePixelFormatTag>() &&
            _myAppliedInternalFormat == get<ImageInternalFormatTag>() &&
            _myAppliedMipmap == get<ImageMipmapTag>();
    }
}
