//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#include "Image.h"
#include "ImageLoader.h"

#include <y60/IScene.h>

#include <dom/Field.h>

#include <asl/string_functions.h>
#include <asl/PackageManager.h>

#include <asl/subraster.h>
#include <asl/palgo.h>
#include <asl/pixels.h>
#include <asl/standard_pixel_types.h>

#include <paintlib/plpixelformat.h>
#include <paintlib/plpngenc.h>
#include <paintlib/pljpegenc.h>
#include <paintlib/pltiffenc.h>
#include <paintlib/Filter/plfilterflip.h>

using namespace asl;
using namespace std;

#undef DB
#define DB(x) // x

namespace y60 {

    DEFINE_EXCEPTION(UnknownEncodingException, asl::Exception);

    const char * I60_MAGIC_NUMBER = "a+c ";
    bool Image::allowInlineFlag = true;

    Image::Image(dom::Node & theNode) :
        Facade(theNode),
        IdTag::Plug(theNode),
        NameTag::Plug(theNode),
        ImageSourceTag::Plug(theNode),
        ImageTypeTag::Plug(theNode),
        ImageResizeTag::Plug(theNode),
        ImageFilterTag::Plug(theNode),
        ImageFilterParamsTag::Plug(theNode),
        ImageMatrixTag::Plug(theNode),
        ImageTileTag::Plug(theNode),
        ImageDepthTag::Plug(theNode),
        dom::FacadeAttributePlug<ImageBytesPerPixelTag>(this),
        dom::FacadeAttributePlug<RasterPixelFormatTag>(this),
        dom::FacadeAttributePlug<ImageWidthTag>(this),
        dom::FacadeAttributePlug<ImageHeightTag>(this)
    {
    }

    Image::~Image() {
    }

    void
    Image::unregisterRasterValue() {
        dom::ValuePtr myRasterValue = getRasterValue();
        if (myRasterValue) {
            ImageWidthTag::Plug::noLongerDependsOn(getRasterValue());
            ImageHeightTag::Plug::noLongerDependsOn(getRasterValue());
        }
    }

    dom::ResizeableRasterPtr
    Image::createRaster(unsigned theWidth,
                        unsigned theHeight,
                        unsigned theDepth,
                        PixelEncoding theEncoding) 
    {
        return setRasterValue(createRasterValue(theEncoding, theWidth, theHeight), theEncoding, theDepth);
    }


    dom::ResizeableRasterPtr
    Image::createRaster(unsigned theWidth,
                        unsigned theHeight,
                        unsigned theDepth,
                        PixelEncoding theEncoding,
                        const asl::ReadableBlock & thePixels) 
    {
        unregisterRasterValue();
        return setRasterValue(createRasterValue(theEncoding, theWidth, theHeight, thePixels), theEncoding, theDepth);
    }

    void
    Image::registerDependenciesRegistrators() {
        Facade::registerDependenciesRegistrators();
        AC_DEBUG << "Image::registerDependenciesRegistrators '" << get<NameTag>();
        
        ImageSourceTag::Plug::getValuePtr()->setImmediateCallBack(dynamic_cast_Ptr<Image>(getSelf()), &Image::load);

        dom::ValuePtr myRasterValue = getRasterValue();
        if (myRasterValue) {
            PixelEncoding myEncoding = PixelEncoding(getEnumFromString(getNode().firstChild()->nodeName(), RasterElementNames));
            set<RasterPixelFormatTag>(getStringFromEnum(myEncoding, PixelEncodingString));
            set<ImageBytesPerPixelTag>(float(getBytesRequired(4, myEncoding))/4.0f);   

            registerDependenciesForRasterValueUpdate();
        } else {
            // Load the raster value, and register dependency update functions
            load();            
        }

        ImageWidthTag::Plug::setReconnectFunction(&Image::registerDependenciesForImageWidthUpdate);
        ImageHeightTag::Plug::setReconnectFunction(&Image::registerDependenciesForImageHeightUpdate);
    }

    void
    Image::registerDependenciesForRasterValueUpdate() {        
        if (getNode()) {
            dom::ValuePtr myRasterValue = getRasterValue();
            //myRasterValue->registerPrecursor(ImageSourceTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageResizeTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageFilterTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageFilterParamsTag::Plug::getValuePtr());
            myRasterValue->setCalculatorFunction(dynamic_cast_Ptr<Image>(getSelf()), 
                &Image::load);
            myRasterValue->setClean();

        }
    }

    void
    Image::registerDependenciesForImageWidthUpdate() {        
        if (getNode()) {
            ImageWidthTag::Plug::dependsOn(getRasterValue());
            ImageWidthTag::Plug::getValuePtr()->setCalculatorFunction(
                dynamic_cast_Ptr<Image>(getSelf()), &Image::calculateWidth);
        }
    }

    void
    Image::registerDependenciesForImageHeightUpdate() {        
        if (getNode()) {
            ImageHeightTag::Plug::dependsOn(getRasterValue());
            ImageHeightTag::Plug::getValuePtr()->setCalculatorFunction(
                dynamic_cast_Ptr<Image>(getSelf()), &Image::calculateHeight);
        }
    }

    void
    Image::calculateWidth() {
        if (getRasterPtr()) {
            set<ImageWidthTag>(getRasterPtr()->width());
        }
    }

    void
    Image::calculateHeight() {
        if (getRasterPtr()) {
            set<ImageHeightTag>(getRasterPtr()->height());
        }
    }

    void
    Image::preload() {
        AC_DEBUG << "Image::preload '" << get<NameTag>() << "' id=" << get<IdTag>();
        ImageSourceTag::Plug::getValuePtr()->setDirty(); // force call to load()
    }

    void
    Image::load() {
        // facade contruction will always lead to a raster ctor, 
        // we do not want this behaviour for inlining textures
        if (!allowInlineFlag) {
            return;
        }
        std::string mySrcAttrib = get<ImageSourceTag>();
        if (get<ImageSourceTag>() == "") {
            AC_DEBUG << "Image '" << get<NameTag>() << "' has not been loaded because of empty source attribute";
            dom::ValuePtr myRasterValue = getRasterValue();
            if (!myRasterValue) {
                // Create default raster
                createRaster(1,1,1,GRAY);
            }
            return;
        }

        AC_INFO << "Image::load loading '" << get<ImageSourceTag>() << "'";
        unsigned myDepth = get<ImageDepthTag>();
        ImageLoader myImageLoader(get<ImageSourceTag>(), AppPackageManager::get().getPtr(), 
            ITextureManagerPtr(0), myDepth);

        string myFilter = get<ImageFilterTag>();
        if (!myFilter.empty()) {
            AC_INFO << "Image::load filter=" << myFilter;
            VectorOfFloat myFilterParams = get<ImageFilterParamsTag>();
            myImageLoader.applyCustomFilter(myFilter, myFilterParams);
        }

        // ImageTileTag only relevant for getType() == CUBEMAP
        myImageLoader.ensurePowerOfTwo(get<ImageResizeTag>(), myDepth, &(get<ImageTileTag>()));

        // Drop alpha channel if unused
        myImageLoader.removeUnusedAlpha();

        set<ImageMatrixTag>(myImageLoader.getImageMatrix());    
        setRasterValue(myImageLoader.getData(), myImageLoader.getEncoding(), myDepth);
    }

    dom::ResizeableRasterPtr
    Image::setRasterValue(dom::ValuePtr theRaster, PixelEncoding theEncoding, unsigned theDepth) {
        AC_DEBUG << "setRasterValue '" << get<NameTag>() << "' id=" << get<IdTag>();

        // Remove existing raster
        if (getNode().childNodes().size()) {
            getNode().childNodes().clear();
        }

        // Setup raster attributes
        set<ImageDepthTag>(theDepth);
        set<RasterPixelFormatTag>(getStringFromEnum(theEncoding, PixelEncodingString));
        set<ImageBytesPerPixelTag>(float(getBytesRequired(4, theEncoding))/4.0f);   

        // Setup raster nodes
        dom::DOMString myRasterName = RasterElementNames[theEncoding];
        dom::NodePtr myRasterChild = getNode().appendChild(dom::NodePtr(new dom::Element(myRasterName)));
        dom::NodePtr myTextChild = myRasterChild->appendChild(dom::NodePtr(new dom::Text()));
        myTextChild->nodeValueWrapperPtr(theRaster);

        // We have to reconnect the dependency graph, because the value has changed
        registerDependenciesForRasterValueUpdate();        

        return dynamic_cast_Ptr<dom::ResizeableRaster>(theRaster);
    }

    PixelEncoding
    Image::getRasterEncoding() const {
        return PixelEncoding(getEnumFromString(get<RasterPixelFormatTag>(), PixelEncodingString));
    }



    void
    Image::blitImage(const asl::Ptr<Image, dom::ThreadingModel> & theSourceImage, const asl::Vector2i & theTargetPos,
            const asl::Box2i * theSourceRect)
    {
        if (get<ImageDepthTag>() != theSourceImage->get<ImageDepthTag>() ||
            get<RasterPixelFormatTag>() != theSourceImage->get<RasterPixelFormatTag>()) {
            // depth and encoding must match
              throw ImageException(std::string("Image::blitImage(): Sourceimage and subimage must have same depth and encoding."), PLUS_FILE_LINE);
        }
        int sourceWidth = theSourceRect ? theSourceRect->getSize()[0] : theSourceImage->get<ImageWidthTag>();
        int sourceHeight = theSourceRect ? theSourceRect->getSize()[1] : theSourceImage->get<ImageHeightTag>();
        dom::ResizeableRasterPtr myRaster = getRasterPtr();
        if (theTargetPos[0] + sourceWidth <= get<ImageWidthTag>() &&
            theTargetPos[1] + sourceHeight <= get<ImageHeightTag>())
        {
            // everything super, subimage fits without resizing the image
            if (!myRaster) {
                throw asl::Exception("Bad raster value", PLUS_FILE_LINE);
            }
            dom::ValuePtr mySourceRaster = theSourceImage->getNode().childNode(0)->childNode(0)->nodeValueWrapperPtr();
            if ( theSourceRect ) {
                myRaster->pasteRaster(*mySourceRaster,
                                      theSourceRect->getMin()[0], theSourceRect->getMin()[1],
                                      theSourceRect->getSize()[0], theSourceRect->getSize()[1],
                                      asl::AC_SIZE_TYPE(theTargetPos[0]), asl::AC_SIZE_TYPE(theTargetPos[1]),
                                      theSourceRect->getSize()[0], theSourceRect->getSize()[1]);
            } else {
                myRaster->pasteRaster(*mySourceRaster,
                                      0,0,0,0,
                                      asl::AC_SIZE_TYPE(theTargetPos[0]), asl::AC_SIZE_TYPE(theTargetPos[1]),
                                      sourceWidth, sourceHeight);
            }
        } else {
            // image must be resized to fit new size
            throw ImageException(std::string("Image::blitImage(): Sourceimage does not fit into targetimage."), PLUS_FILE_LINE);
        }

    }

    void
    Image::convertFromPLBmp(PLAnyBmp & theBitmap) {
        PixelEncoding mySourceEncoding;
        mapFormatToPixelEncoding(theBitmap.GetPixelFormat(), mySourceEncoding);

        if (getRasterEncoding() != mySourceEncoding) {
            throw ImageException(std::string("Image::convertFromPLBmp(): Encoding do not match: destination : ") +
                getStringFromEnum(getRasterEncoding(), PixelEncodingString) + " source : " + 
                getStringFromEnum(mySourceEncoding, PixelEncodingString), PLUS_FILE_LINE);

        }
        getRasterPtr()->resize(theBitmap.GetWidth(), theBitmap.GetHeight());
        PLBYTE ** mySrcLines = theBitmap.GetLineArray();

        unsigned myFaceHeight = theBitmap.GetHeight();
        long myLineSize = theBitmap.GetBytesPerLine();
        int myBytesPerLine = theBitmap.GetPixelFormat().GetBitsPerPixel() * theBitmap.GetWidth() / 8;
        unsigned char *myData = getRasterPtr()->pixels().begin();

        for (int y = 0; y < myFaceHeight; ++y) {
            memcpy(myData + myBytesPerLine *y, mySrcLines[y], myLineSize);
        }
    }

    void
    Image::convertToPLBmp(PLAnyBmp & theBitmap) {
        int myWidth = get<ImageWidthTag>();
        int myHeight = get<ImageHeightTag>();

        PLPixelFormat myPixelFormat;
        if (!mapPixelEncodingToFormat(getRasterEncoding(), myPixelFormat)) {
              throw ImageException(std::string("Image::convertToPLBmp(): Unsupported Encoding: ") +
                      asl::as_string(getRasterEncoding()), PLUS_FILE_LINE);
        }
        theBitmap.Create(myWidth, myHeight, myPixelFormat, NULL, 0, PLPoint(72, 72));
        PLBYTE **myLineArray = theBitmap.GetLineArray();
        int myBytesPerLine = myPixelFormat.GetBitsPerPixel() * myWidth / 8;
        const unsigned char *myData = getRasterPtr()->pixels().begin();
        for(int y=0; y<myHeight; ++y) {
            memcpy(myLineArray[y], myData + myBytesPerLine * y, myBytesPerLine);
        }
    }

    void
    Image::applyFilter(const std::string & theFilter, const VectorOfFloat & theFilterParam) {
        PLAnyBmp myBmp;
        convertToPLBmp( myBmp );
        applyCustomFilter(myBmp, theFilter, theFilterParam);

#if 0 // debug
        PLPNGEncoder myEncoder;
        myEncoder.MakeFileFromBmp("test_filtered.png", &myBmp);
#endif

        convertFromPLBmp(myBmp);
    }

    void
    Image::saveToFile(const string & theImagePath,
            const VectorOfString & theFilter,
            const VectorOfVectorOfFloat & theFilterParams)
    {
        PLAnyBmp myBmp;
        convertToPLBmp( myBmp );
        if (theFilter.size()) {
            applyCustomFilter(myBmp, theFilter, theFilterParams);
        }

        string myImagePath = toLowerCase(theImagePath);
        string::size_type pos = myImagePath.find_last_of(".");
        if (pos != string::npos) {
            string myExtension = myImagePath.substr(pos);

            if (myExtension == ".jpg" || myExtension == ".jpeg") {
                // [jb] PLJPEGEncoder expects images to have alpha (32bit),
                //      this hack stuffs the pixelformat with the missing 8bit:
                PLAnyBmp myTmpBmp;
                convertToPLBmp( myTmpBmp );
                myTmpBmp.CreateCopy(myBmp, PLPixelFormat::X8B8G8R8);
                PLJPEGEncoder myJPEGEncoder;
                myJPEGEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myTmpBmp);
                return;
            } else if (myExtension == ".tif" || myExtension == ".tiff") {
                PLTIFFEncoder myTiffEncoder;
                myTiffEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myBmp);
                return;
            }
        }

        // default to png
        PLPNGEncoder myPNGEncoder;
        myPNGEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myBmp);
    }
}
