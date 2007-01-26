//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: ImageLoader.cpp,v $
//   $Author: martin $
//   $Revision: 1.3 $
//   $Date: 2005/04/29 13:37:56 $
//
//  Description: Adapter between Paintlib bitmap and Image
//
//=============================================================================

#include "ImageLoader.h"
#include "PLFilterResizePadded.h"
#include "HeightToNormalMap.h"
#include "PLFilterFactory.h"
#include "I60Header.h"

#include <asl/MappedBlock.h>
#include <asl/numeric_functions.h>
#include <asl/file_functions.h>
#include <asl/os_functions.h>
#include <asl/Block.h>
#include <asl/PackageManager.h>
#include <asl/Logger.h>

#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

using namespace std;
using namespace asl;

namespace y60 {

    /* Image filtering on save/load */
    void
    applyCustomFilter(PLBmp & theBitmap, const VectorOfString & theFilterName, const VectorOfVectorOfFloat & theFilterparams) {
        if (theFilterName.size() != theFilterparams.size()) {
            throw ImageLoaderException(std::string("Sorry, filter and params count do not match."), PLUS_FILE_LINE);
        }
        for (int myFilterIndex = 0; myFilterIndex < theFilterName.size(); myFilterIndex++) {
            applyCustomFilter(theBitmap, theFilterName[myFilterIndex], theFilterparams[myFilterIndex]);
        }
    }

    void
    applyCustomFilter(PLBmp & theBitmap, const std::string & theFilterName, const VectorOfFloat & theFilterparams) {
        asl::Ptr<PLFilter> myPaintLibFilter = y60::PaintLibFilterFactory::get().createFilter(theFilterName,
                                                                                             theFilterparams);
        if (myPaintLibFilter) {
            theBitmap.ApplyFilter(*myPaintLibFilter);
        } else {
            ImageFilter myFilter = ImageFilter(getEnumFromString(theFilterName, ImageFilterStrings));
            applyCustomFilter(theBitmap, myFilter, theFilterparams);
        }
    }

    void
    applyCustomFilter(PLBmp & theBitmap, ImageFilter theFilter, const std::vector<float> &  theFilterparams) {
        switch (theFilter) {
           /* case WINDOW_CW: {
                float myCenter = theFilterparams[0];
                float myWidth = theFilterparams[1];
                ApplyFilter(PLFilterWindowCW(myCenter, myWidth));
                break; }*/
            case HEIGHT_TO_NORMALMAP:
                theBitmap.ApplyFilter(HeightToNormalMap());
                break;
            default:
                break;
        }
    }

    ImageLoader::ImageLoader(const ITextureManagerPtr & theTextureManager) :
        PLBmp(),
        _myHeaderSize(0),
        _isI60(false),
        _myTextureManager(theTextureManager)
    {
        AC_DEBUG << "ImageLoader::ImageLoader (default)";
        // classes derived from PLBmpBase need to allocate
        // the palette.
        m_pClrTab = new PLPixel32[256];
        m_pLineArray = 0;
        _myImageMatrix.makeIdentity();
    }

    ImageLoader::ImageLoader(const string & theFilename,
            PackageManagerPtr thePackageManager,
            const ITextureManagerPtr & theTextureManager,
            unsigned /*theDepth*/) :
        PLBmp(),
        _myFilename(theFilename),
        _myHeaderSize(0),
        _isI60(false),
        _myTextureManager(theTextureManager)
    {
        AC_DEBUG << "ImageLoader::ImageLoader '" << theFilename << "'";
        // classes derived from PLBmpBase need to allocate
        // the palette.
        m_pClrTab = new PLPixel32[256];
        m_pLineArray = 0;
        _myImageMatrix.makeIdentity();
        // Cubemaps are encoded as
        // theFrontFileName + "|" + theRightFileName + "|" + theBackFileName + "|" + theLeftFileName + "|"
		// + theTopFileName + "|" + theBottomFileName;
        vector<string> myFilenames = asl::splitString(theFilename, "|");
        vector<asl::Ptr<ReadableBlock> > myBlocks;

        // Check if files exist
        for (unsigned i = 0; i < myFilenames.size(); ++i) {
            asl::Ptr<ReadableBlock> myBlock;
            if (thePackageManager) {
                myBlock = thePackageManager->openFile(myFilenames[i]);
            }
            // fall back to simply try to open the file
            AC_DEBUG << myBlock << " filename='" << myFilenames[i] << "'";
            if (!myBlock && fileExists(myFilenames[i])) {
                myBlock = asl::Ptr<ReadableBlock>(new ConstMappedBlock(myFilenames[i]));
            }
            if (!myBlock) {
                if (thePackageManager) {
                    throw ImageLoaderException(std::string("image file '") + myFilenames[i] + "' not found in " +
                        thePackageManager->getSearchPath(), PLUS_FILE_LINE);
                } else {
                    throw ImageLoaderException(std::string("image file '") + myFilenames[i] + "' not found in " +
                        "current directory.", PLUS_FILE_LINE);
                }
            }
            myBlocks.push_back(myBlock);
        }
        if (myBlocks.size() == 1) {
            loadSingleImage(myBlocks[0]);
        } else if (myBlocks.size() == 6) {
            loadCubemap(myBlocks, 1);
        } else {
            throw ImageLoaderException(string("Image ") + theFilename + " has " + as_string(myFilenames.size()) +
                " faces, should be one or six", PLUS_FILE_LINE);
        }
    }

    ImageLoader::ImageLoader(asl::Ptr<ReadableBlock> theInputData,
            const std::string & theFileDescription,
            const ITextureManagerPtr & theTextureManager,
            unsigned /*theDepth*/) :
        PLBmp(),
        _myFilename(theFileDescription.empty() ? "<asl::Block>" : theFileDescription),
        _myHeaderSize(0),
        _isI60(false),
        _myTextureManager(theTextureManager)
    {
        AC_DEBUG << "ImageLoader::ImageLoader '" << _myFilename << "'";
        // classes derived from PLBmpBase need to allocate
        // the palette.
        m_pClrTab = new PLPixel32[256];
        m_pLineArray = 0;
        _myImageMatrix.makeIdentity();
        loadSingleImage(theInputData);
    }

    ImageLoader::~ImageLoader() {
        AC_TRACE << "ImageLoader::~ImageLoader";
        delete[] m_pClrTab;
        freeMembers();
    }

    void
    ImageLoader::loadSingleImage(asl::Ptr<ReadableBlock> theImageBlock) {
        I60Header myHeader;
        theImageBlock->readData(myHeader, 0);

        if (myHeader.checkMagicNumber()) {
            loadI60File(theImageBlock);
        } else {
            try {
                PLAnyPicDecoder myDecoder;
                myDecoder.MakeBmpFromMemory(const_cast<unsigned char*>(theImageBlock->begin()),
                        theImageBlock->size(), this);
            } catch (const PLTextException & e) {
#if 0
                // create black image
                AC_ERROR << "Paintlib exception occured while loading " << _myFilename << ": " << static_cast<const char *>(e);
                internalCreate(512, 512, PLPixelFormat::B8G8R8);
#else
                // throw exception
                throw ImageLoaderException(_myFilename + " - " + static_cast<const char *>(e), PLUS_FILE_LINE);
#endif
			} catch(std::exception & e) {
                throw ImageLoaderException(_myFilename + " - " + e.what(), PLUS_FILE_LINE);
			} catch(const asl::Exception &) {
                throw;
			} catch(...) {
                throw ImageLoaderException(_myFilename + "Unknown (Paintlib) exception occured while loading.", PLUS_FILE_LINE);
            }
        }
    }

    void
    ImageLoader::loadCubemap(std::vector<asl::Ptr<ReadableBlock> > & theBlocks, unsigned theDepth) {

        for (unsigned i = 0; i < 6; ++i) {
            string myDescription = string("Face")+as_string(i)+"_"+_myFilename;
            ImageLoader myFaceBmp(theBlocks[i], myDescription, _myTextureManager.lock());
            myFaceBmp.ensurePowerOfTwo(IMAGE_RESIZE_SCALE, SINGLE, theDepth);

            if (i == 0) {
                // Setup current image to have 6 times the height of one face
                Create(myFaceBmp.GetWidth(), myFaceBmp.GetHeight() * 6 * theDepth, myFaceBmp.GetPixelFormat(),
                    0, 0, myFaceBmp.GetResolution());
            }

            // Check if new face is compatible with other faces
            if (myFaceBmp.GetBitsPerPixel() != GetBitsPerPixel()) {
                throw ImageLoaderException(string("The cubemap-face: ") + myDescription + " has " +
                    as_string(myFaceBmp.GetBitsPerPixel()) + " but should have " +
                    as_string(GetBitsPerPixel()), PLUS_FILE_LINE);
            }

            if (myFaceBmp.HasAlpha() != HasAlpha()) {
                throw ImageLoaderException(string("The cubemap-face: ") + myDescription + " alpha-flag: " +
                    as_string(myFaceBmp.HasAlpha()) + " but should be: " +
                    as_string(HasAlpha()), PLUS_FILE_LINE);
            }

            if (myFaceBmp.IsGreyscale() != IsGreyscale()) {
                throw ImageLoaderException(string("The cubemap-face: ") + myDescription + " greyscale-flag: " +
                    as_string(myFaceBmp.IsGreyscale()) + " but should be: " +
                    as_string(IsGreyscale()), PLUS_FILE_LINE);
            }

            unsigned myTargetWidth  = GetWidth();
            unsigned myTargetHeight = GetHeight() / 6 / theDepth;
            if (myFaceBmp.GetWidth() != myTargetWidth || myFaceBmp.GetHeight() != myTargetHeight) {
                AC_WARNING << "Resizing bitmap " << myDescription << " to " << myTargetWidth << "x" << myTargetHeight << ".";
                myFaceBmp.ApplyFilter(PLFilterResizeBilinear(myTargetWidth, myTargetHeight));
            }

            // Copy face into destination bitmap
            PLBYTE ** mySrcLines = myFaceBmp.GetLineArray();
            PLBYTE ** myDstLines = GetLineArray();

            unsigned myFaceHeight = myFaceBmp.GetHeight();
            long myLineSize = GetBytesPerLine();
            for (int y = 0; y < myFaceHeight; ++y) {
                memcpy(myDstLines[y + myFaceHeight * i], mySrcLines[y], myLineSize);
            }
        }
    }

    void
    ImageLoader::loadI60File(asl::Ptr<ReadableBlock> theImageBlock) {
        _myImageMatrix.makeIdentity();
        I60Header myHeader;
        theImageBlock->readData(myHeader, 0);

        if (!myHeader.checkMagicNumber()) {
            throw ImageLoaderException(string("Image ") + _myFilename +
                " has a wrong magic number. '" + I60_MAGIC_NUMBER + "' expeced.", PLUS_FILE_LINE);
        }
        if (!myHeader.checkVersion()) {
            throw ImageLoaderException(string("Image ") + _myFilename + " file format version: " +
                as_string(myHeader.version) + " does not match current reader version: " +
                as_string(CURRENT_IMAGE_FORMAT_VERSION), PLUS_FILE_LINE);
        }

        _myHeaderSize = myHeader.headersize;
        _myEncoding   = PixelEncoding(myHeader.encoding);
        _isI60        = true;

        unsigned myWidthPowerOfTwo  = asl::nextPowerOfTwo(myHeader.width);
        unsigned myHeightPowerOfTwo = asl::nextPowerOfTwo(myHeader.height);

        _myImageMatrix.scale(Vector3f(float(myHeader.width) / myWidthPowerOfTwo,
                                        float(myHeader.height) / myHeightPowerOfTwo,
                                        1.0f));
        // maybe we should cut off the i60 header here?
        //_myData = theImageBlock;
        _myRasterData = createRasterValue(_myEncoding, myWidthPowerOfTwo,
            myHeightPowerOfTwo * myHeader.layercount,
            asl::ReadableBlockAdapter(theImageBlock->begin()+sizeof(I60Header), theImageBlock->end()));

        // TODO: Add support for other compression formats
        SetBmpInfo(PLPoint(myWidthPowerOfTwo, myHeightPowerOfTwo * myHeader.layercount),
			GetResolution(), PLPixelFormat::L1 /* only correct for DXT5 */);
    }

    long
    ImageLoader::GetMemUsed () {
        return GetBytesPerLine() * GetHeight();
    }

    long
    ImageLoader::GetBytesPerLine () {
        return getBytesRequired(GetWidth(), _myEncoding);
    }

    /////////////////////////////////////////////////////////////////////
    // Protected callbacks
    /////////////////////////////////////////////////////////////////////

    // Create a new empty DIB. Bits are uninitialized.
    // Assumes that no memory is allocated before the call.
    void
    ImageLoader::internalCreate (PLLONG theWidth, PLLONG theHeight,
                                    const PLPixelFormat &  thePixelformat)
    {
        AC_DEBUG << "Pixelformat: " << thePixelformat.GetName();
/*
       int theBitsPerPixel = thePixelformat.GetBitsPerPixel();
        switch (theBitsPerPixel) {
            case 32:
                _myEncoding = BGRA;
                break;
            case 24:
                _myEncoding = BGR;
                break;
            case 16:
                _myEncoding = GRAY16;
                break;
            case 8:
                _myEncoding = GRAY;
                break;
            default:
                throw ImageLoaderException(string("Unsupported bits per pixel: ") + as_string(theBitsPerPixel), PLUS_FILE_LINE);
        }
*/
        if (!mapFormatToPixelEncoding(thePixelformat, _myEncoding)) {
            throw ImageLoaderException(string("Unsupported Pixel Encoding: ") + thePixelformat.GetName(), PLUS_FILE_LINE);
        }
        _myRasterData = createRasterValue(_myEncoding, theWidth, theHeight);

        initLocals (theWidth, theHeight, thePixelformat);
    }

    void ImageLoader::freeMembers() {
        if (m_pLineArray) {
            AC_TRACE << "ImageLoader::freeMembers delete m_pLineArray";
            delete[] m_pLineArray;
            m_pLineArray = 0;
        }
    }

    void
    ImageLoader::initLineArray () {
        m_pLineArray = new unsigned char * [GetHeight()];
        updateLineArray();
    }

    void
    ImageLoader::updateLineArray() {
        unsigned myHeight     = GetHeight();
        unsigned myLineStride = GetBytesPerLine();

#if 0
        asl::Ptr<Block> myWritableData = dynamic_cast_Ptr<Block>(_myData);
        if (!myWritableData) {
            throw ImageLoaderException(string("Non-writable block: ") +
                    _myFilename, PLUS_FILE_LINE);
        }
        unsigned char * pBits = myWritableData->begin();
#else
        const unsigned char * pBits = getRaster()->pixels().begin();
#endif
        for (unsigned i = 0; i < myHeight; ++i) {
            m_pLineArray[i] = (PLBYTE*)pBits; // caution: const leak
            pBits += myLineStride;
        }
    }

    /////////////////////////////////////////////////////////////////////
    // Custom ImageLoader Memberfunctions
    /////////////////////////////////////////////////////////////////////


    void
    ImageLoader::setFixedAlpha(float theAlpha) {
       if (!_isI60) {
			AC_INFO << "Set alpha channel : "
				    << theAlpha << " in image: " << _myFilename << endl;
            if (GetBitsPerPixel() != 32) {
				// Add Alpha channel (do we ever get here ? dk/vs)
                // A: yes - when 24-bit files and an alpha-scale != 1.0 are combined.
				AC_INFO << "Create alpha channel for setting alpha on a "
					    << getStringFromEnum(_myEncoding,PixelEncodingString)  << " encoded image: "
						<< _myFilename << endl;
				PLAnyBmp myCopy;
                switch (_myEncoding) {
                    case RGB:
                        _myEncoding = RGBA;
                        break;
                    case BGR:
                        _myEncoding = BGRA;
                        break;
                    default:
                        throw ImageLoaderException(string("setFixedAlpha: unsupported pixel encoding.")
                            + _myFilename, PLUS_FILE_LINE);
                }
				PLPixelFormat myFormat;
				mapPixelEncodingToFormat(_myEncoding, myFormat);

				myCopy.CreateCopy(*this, myFormat);
                CreateCopy(myCopy, myFormat);

			}
            unsigned myHeight = GetHeight();
            unsigned myWidth  = GetWidth();
            unsigned char ** myLineArray   = GetLineArray();

            unsigned char myAlpha = (unsigned char)(255 * theAlpha);
            for (unsigned i = 0; i < myHeight; ++i) {
                unsigned char * mySource = *myLineArray;
                for (unsigned j = 3; j < myWidth*4; j+=4) {
                    mySource[j] = myAlpha;
                }
                myLineArray++;
            }
        } else {
            throw ImageLoaderException(string("setFixedAlpha: does not work with i60 files: ")
                    + _myFilename, PLUS_FILE_LINE);
        }
    }

    void
    ImageLoader::removeUnusedAlpha() {
        if (!_isI60 && (GetBitsPerPixel() == 32 && !HasAlpha())) {
            // Compress unused fourth channel
            unsigned myHeight = GetHeight();
            unsigned myWidth  = GetWidth();
            unsigned myLineStride = GetBytesPerLine();

            asl::Ptr<Block> myDestinationBlock = asl::Ptr<Block>(new Block());
            myDestinationBlock->resize(myHeight * myWidth * 3);

            unsigned char ** myLineArray   = GetLineArray();
            unsigned char *  myDestination = myDestinationBlock->begin();

            for (unsigned i = 0; i < myHeight; ++i) {
                unsigned char * mySource = *myLineArray;
                for (unsigned j = 0; j < myWidth; ++j) {
                    myDestination[0]  = mySource[0];
                    myDestination[1]  = mySource[1];
                    myDestination[2]  = mySource[2];

                    mySource      += 4;
                    myDestination += 3;
                }
                myLineArray++;
            }

            // Update internal representation
            //_myData = myDestinationBlock;
            switch (_myEncoding) {
                case RGBA :
                    _myEncoding = RGB;
                    break;
                case BGRA :
                    _myEncoding = BGR;
                    break;
                default:
                    throw ImageLoaderException("Unsupported pixel encoding", PLUS_FILE_LINE);
            }
            _myRasterData = createRasterValue(_myEncoding, myWidth, myHeight,
                *myDestinationBlock);
            PLPixelFormat myPixelFormat;
            mapPixelEncodingToFormat(_myEncoding, myPixelFormat);
            SetBmpInfo(GetSize(), GetResolution(), myPixelFormat);
            updateLineArray();
        }
    }

    void
    ImageLoader::ensurePowerOfTwo(const std::string & theResizeMode, ImageType theType,
                                  unsigned theDepth, const asl::Vector2i * theTile)
    {
        if (theResizeMode == IMAGE_RESIZE_NONE) {
            return;
        }

        if (!powerOfTwo(theDepth)) {
            throw ImageLoaderException(
                string("ensurePowerOfTwo: third dimension (depth) must be power of two for now"),
                    PLUS_FILE_LINE);
        }

        try {
            unsigned myWidthFactor = 1, myHeightFactor = theDepth;
            if (theType == CUBEMAP && theTile) {
                myWidthFactor *= (*theTile)[0];
                myHeightFactor *= (*theTile)[1];
                AC_DEBUG << "tile=" << *theTile << " factor=" << myWidthFactor << "x" << myHeightFactor << " size=" << GetWidth() << "x" << GetHeight();
            }

            unsigned myPreScaleWidth  = GetWidth();
            unsigned myPreScaleHeight = GetHeight();

            if ((myPreScaleWidth % myWidthFactor) != 0) {
                throw ImageLoaderException(
                        string("ensurePowerOfTwo: '" + _myFilename + "' texture strip width must be a multiple of ")+
                        as_string(myWidthFactor), PLUS_FILE_LINE);
            }
            if ((myPreScaleHeight % myHeightFactor) != 0) {
                throw ImageLoaderException(
                        string("ensurePowerOfTwo: '" + _myFilename + "' texture strip height must be a multiple of ")+
                        as_string(myHeightFactor), PLUS_FILE_LINE);
            }

            if (!powerOfTwo(GetWidth() / myWidthFactor) || !powerOfTwo(GetHeight() / myHeightFactor)) {

                unsigned myWidth  = nextPowerOfTwo(GetWidth() / myWidthFactor) * myWidthFactor;
                unsigned myHeight = nextPowerOfTwo(GetHeight() / myHeightFactor) * myHeightFactor;

                if (theResizeMode == IMAGE_RESIZE_SCALE) {
                    AC_DEBUG << "Resizing bitmap " << _myFilename << " to next power of two: " << myWidth << "x" << myHeight << ".";
                    ApplyFilter(PLFilterResizeBilinear(myWidth, myHeight));
                } else if (theResizeMode == IMAGE_RESIZE_PAD) {
                    //TODO: make padding for for cube maps and 3D texture strips
                    AC_DEBUG << "Padding bitmap '" << _myFilename << "' to next power of two: " << myWidth << "x" << myHeight << ".";
                    ApplyFilter(PLFilterResizePadded(myWidth, myHeight));
                    _myImageMatrix.scale(Vector3f(float(myPreScaleWidth) / GetWidth(),
                            float(myPreScaleHeight) / GetHeight(), 1.0f));
                } else {
                    throw ImageLoaderException(string("ensurePowerOfTwo: Unknown resize mode: ")
                            + theResizeMode, PLUS_FILE_LINE);
                }
            }

            // limit texture size
            unsigned myTextureSizeLimit = 0;
            if (asl::Ptr<ITextureManager> myTextureManager = _myTextureManager.lock()) {
                myTextureSizeLimit = myTextureManager->getMaxTextureSize( theDepth = 1 ? 2 : 3 );
            }

            unsigned myTargetWidth = GetWidth();
            unsigned myTargetHeight = GetHeight();

            while (myTextureSizeLimit &&
                ((myTargetWidth / myWidthFactor) > myTextureSizeLimit ||
                 (myTargetHeight / myHeightFactor)  > myTextureSizeLimit) )
            {
                myTargetWidth  = (myTargetWidth / myWidthFactor)/2 * myWidthFactor;
                myTargetHeight = (myTargetHeight / myHeightFactor)/2 * myHeightFactor;
            }
            if (myTargetWidth != GetWidth() || myTargetHeight != GetHeight()) {
                AC_WARNING << "Texture size exceeds "<<Y60_TEXTURE_SIZE_LIMIT_ENV<<"="<<
                    myTextureSizeLimit<<", resizing to "<<
                    myTargetWidth<<"x"<<myTargetHeight<<endl;
                ApplyFilter(PLFilterResizeBilinear(myTargetWidth, myTargetHeight));
            }

            // convert paletted textures
        } catch (PLTextException & ex) {
            throw ImageLoaderException(std::string("Paintlib exception ")+std::string(ex), PLUS_FILE_LINE);
        }
    }

    void
    ImageLoader::applyCustomFilter(const std::string & theFilterName, const vector<float> & theFilterparams) {
        y60::applyCustomFilter(*this, theFilterName, theFilterparams);
    }

    void
    ImageLoader::applyCustomFilter(ImageFilter theFilter, const vector<float> & theFilterparams) {
        y60::applyCustomFilter(*this, theFilter, theFilterparams);
    }

}
