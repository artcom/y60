//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
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

    Image::Image(dom::Node & theNode) :
        Facade(theNode),
        IdTag::Plug(theNode),
        NameTag::Plug(theNode),
        ImageSourceTag::Plug(theNode),
        TexturePixelFormatTag::Plug(theNode),
        ImageTypeTag::Plug(theNode),
        ImageMipmapTag::Plug(theNode),
        ImageResizeTag::Plug(theNode),
        ImageFilterTag::Plug(theNode),
        ImageFilterParamsTag::Plug(theNode),
        ImageMatrixTag::Plug(theNode),
        ImageTileTag::Plug(theNode),
        ImageColorScaleTag::Plug(theNode),
        ImageColorBiasTag::Plug(theNode),
        ImageDepthTag::Plug(theNode),
        dom::FacadeAttributePlug<ImageBytesPerPixelTag>(this),
        dom::FacadeAttributePlug<TextureIdTag>(this),
        dom::FacadeAttributePlug<RasterPixelFormatTag>(this),
        dom::FacadeAttributePlug<ImageInternalFormatTag>(this),
        dom::FacadeAttributePlug<ImageWidthTag>(this),
        dom::FacadeAttributePlug<ImageHeightTag>(this),
        dom::FacadeAttributePlug<LoadCountTag>(this),
        _myRefCount(0),
        _myTextureId(0), 
        _myPixelBufferId(0),
        _myReuseRaster(false),
        _myRessourceManager(0)            
    {
        if (getNode()) {
            IScenePtr myScene = getNode().parentNode()->parentNode()->getFacade<IScene>();                
            _myRessourceManager = myScene->getResourceManager();
        }
    }

    Image::~Image() {
        if (_myRessourceManager) {
            _myRessourceManager->unbindTexture(this);
        }
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

        TextureIdTag::Plug::setReconnectFunction(&Image::registerDependenciesForTextureUpdate);
        ImageInternalFormatTag::Plug::setReconnectFunction(&Image::registerDependenciesForImageFormatUpdate);

        dom::ValuePtr myRasterValue = getRasterValue();
        if (myRasterValue) {
            PixelEncoding myEncoding = PixelEncoding(getEnumFromString(
                getNode().firstChild()->nodeName(), RasterElementNames));
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
            myRasterValue->registerPrecursor(ImageSourceTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageResizeTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageFilterTag::Plug::getValuePtr());
            myRasterValue->registerPrecursor(ImageFilterParamsTag::Plug::getValuePtr());
            myRasterValue->setCalculatorFunction(dynamic_cast_Ptr<Image>(getSelf()), 
                &Image::load);
            myRasterValue->setClean();
        }
    }

    void
    Image::registerDependenciesForTextureUpdate() {        
        if (getNode()) {
            // The rastervalue dependency has to be registered first, because getRasterValue()
            // might trigger an image reload, which clears the raster value node and 
            // destroys the dependency.
            TextureIdTag::Plug::dependsOn(getRasterValue());
            TextureIdTag::Plug::dependsOn<ImageColorBiasTag>(*this);  
            TextureIdTag::Plug::dependsOn<ImageColorScaleTag>(*this); 
            TextureIdTag::Plug::dependsOn<ImageTileTag>(*this);
            TextureIdTag::Plug::dependsOn<ImageMipmapTag>(*this);
            TextureIdTag::Plug::dependsOn<ImageInternalFormatTag>(*this);
            TextureIdTag::Plug::getValuePtr()->setCalculatorFunction(
                dynamic_cast_Ptr<Image>(getSelf()), &Image::uploadTexture);
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
    Image::registerDependenciesForImageFormatUpdate() {        
        if (getNode()) {
            ImageInternalFormatTag::Plug::dependsOn<ImageColorBiasTag>(*this);
            ImageInternalFormatTag::Plug::dependsOn<ImageColorScaleTag>(*this);
            ImageInternalFormatTag::Plug::dependsOn<TexturePixelFormatTag>(*this);
            ImageInternalFormatTag::Plug::dependsOn<RasterPixelFormatTag>(*this);
            ImageInternalFormatTag::Plug::getValuePtr()->setCalculatorFunction(
                dynamic_cast_Ptr<Image>(getSelf()), &Image::calculateInternalPixelFormat);
        }
    }


    void 
    Image::uploadTexture() {
        if (_myReuseRaster) { 
            _myRessourceManager->updateTextureData(dynamic_cast_Ptr<Image>(getSelf()));
        } else {
            if (_myTextureId) {
                // In order to prevent a texture leak, we need to unbind the texture before setupImage()
                _myRessourceManager->unbindTexture(this);
            }
            _myTextureId = _myRessourceManager->setupTexture(dynamic_cast_Ptr<Image>(getSelf()));
            set<TextureIdTag>(_myTextureId);
            _myReuseRaster = true;
        }
    }

    void
    Image::calculateWidth() {
        set<ImageWidthTag>(getRasterPtr()->width());
    }

    void
    Image::calculateHeight() {
        set<ImageHeightTag>(getRasterPtr()->height());
    }

    void 
    Image::calculateInternalPixelFormat() {
        PixelEncoding myRasterFormat = getRasterEncoding();
        const std::string & myTextureFormat = get<TexturePixelFormatTag>();     
        bool myAlphaChannelRequired = !(almostEqual(get<ImageColorScaleTag>()[3], 1) && 
            almostEqual(get<ImageColorBiasTag>()[3], 0));
        if (myTextureFormat.size()) {
            // If there is an texture pixel format set from outside, we use it
            set<ImageInternalFormatTag>(myTextureFormat);
        } else if ((myRasterFormat == y60::RGB || myRasterFormat == y60::BGR) && myAlphaChannelRequired) {
            // If a change in colorscale introduces an alpha channel ensure
            // that internal format has alpha
            set<ImageInternalFormatTag>(getStringFromEnum(TEXTURE_IFMT_RGBA8, TextureInternalFormatStrings));
        } else {
            TextureInternalFormat myInternalFormat = getInternalPixelFormat(myRasterFormat);
            set<ImageInternalFormatTag>(getStringFromEnum(myInternalFormat, TextureInternalFormatStrings));
        }        
    }

    void
    Image::load() {
        std::string mySrcAttrib = get<ImageSourceTag>();
        if (get<ImageSourceTag>() == "") {
            AC_INFO << "Image '" << get<NameTag>() << " has not been loaded because of empty source attribute";
            dom::ValuePtr myRasterValue = getRasterValue();
            if (!myRasterValue) {
                // Create default raster
                createRaster(1,1,1,GRAY);
            }
            return;
        }

        unsigned myDepth = get<ImageDepthTag>();
        ImageLoader myImageLoader(get<ImageSourceTag>(), AppPackageManager::get().getPtr(), 
            ITextureManagerPtr(0), myDepth);

        string myFilter = get<ImageFilterTag>();
        if (!myFilter.empty()) {
            AC_DEBUG << "Image::load() filter=" << myFilter;
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

        set<ImageMatrixTag>(myImageLoader.getImageMatrix());    
        setRasterValue(myImageLoader.getData(), myImageLoader.getEncoding(), myDepth);
        set<LoadCountTag>(get<LoadCountTag>() + 1);
    }

    dom::ResizeableRasterPtr
    Image::setRasterValue(dom::ValuePtr theRaster, PixelEncoding theEncoding, unsigned theDepth) {
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

        _myReuseRaster = false;
        return dynamic_cast_Ptr<dom::ResizeableRaster>(theRaster);
    }

    ImageType
    Image::getType() const {
        return ImageType(getEnumFromString(get<ImageTypeTag>(), ImageTypeStrings));
    }

    PixelEncoding
    Image::getRasterEncoding() const {
        return PixelEncoding(getEnumFromString(get<RasterPixelFormatTag>(), PixelEncodingString));
    }

    TextureInternalFormat
    Image::getInternalEncoding() const {
        return TextureInternalFormat(getEnumFromString(get<ImageInternalFormatTag>(), TextureInternalFormatStrings));
    }

    void 
    Image::triggerUpload() {
        _myReuseRaster = false;
        TextureIdTag::Plug::getValuePtr()->setDirty();
    }

    bool 
    Image::isUploaded() const {
        return _myReuseRaster;
    }

    void 
    Image::removeTextureFromResourceManager() {
        if (_myTextureId) {
            _myRessourceManager->unbindTexture(this);
            unbind();
        }
    }

    void 
    Image::unbind() {
        _myTextureId = 0;
        TextureIdTag::Plug::getValuePtr()->setDirty();
    }

    void
    Image::registerTexture() {
        if (_myRefCount == 0) {
            _myRessourceManager->setTexturePriority(this, TEXTURE_PRIORITY_IN_USE);
        }
        ++_myRefCount;
    }

    void
    Image::deregisterTexture() {
        --_myRefCount;
        if (_myRefCount==0) {
            _myRessourceManager->setTexturePriority(this, TEXTURE_PRIORITY_IDLE);
        }
    }

    bool
    Image::usePixelBuffer() const {
        // not much use for regular images
        return false;
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
                myRaster->pasteRaster(asl::AC_SIZE_TYPE(theTargetPos[0]), asl::AC_SIZE_TYPE(theTargetPos[1]),
                                      *mySourceRaster,
                                      theSourceRect->getMin()[0], theSourceRect->getMin()[1],
                                      theSourceRect->getSize()[0], theSourceRect->getSize()[1]);
            } else {
                myRaster->pasteRaster(asl::AC_SIZE_TYPE(theTargetPos[0]), asl::AC_SIZE_TYPE(theTargetPos[1]),
                                      *mySourceRaster);
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
    Image::saveToFileFiltered(const std::string & theImagePath, const VectorOfString & theFilter,
                              const VectorOfVectorOfFloat & theFilterParams)
    {
        PLAnyBmp myBmp;
        convertToPLBmp( myBmp );
        applyCustomFilter(myBmp, theFilter, theFilterParams);

        string myImagePath = toLowerCase(theImagePath);
        string::size_type pos = string::npos;

        pos = myImagePath.find_last_of(".");
        if ( pos != string::npos) {
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
            }
        }

        PLPNGEncoder myPNGEncoder;
        myPNGEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myBmp);
    }

    void
    Image::saveToFile(const string & theImagePath) {
        PLAnyBmp myBmp;
        convertToPLBmp( myBmp );

        string myImagePath = toLowerCase(theImagePath);
        string::size_type pos = string::npos;

        pos = myImagePath.find_last_of(".");
        if ( pos != string::npos) {
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

        PLPNGEncoder myPNGEncoder;
        myPNGEncoder.MakeFileFromBmp(Path(theImagePath, UTF8).toLocale().c_str(), &myBmp);
    }
}

