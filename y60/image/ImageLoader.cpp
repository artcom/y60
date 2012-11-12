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
//
//  Description: Adapter between Paintlib bitmap and Image
//
//=============================================================================

// own header
#include "ImageLoader.h"

#include <asl/base/MappedBlock.h>
#include <asl/math/numeric_functions.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <asl/base/Block.h>
#include <asl/zip/PackageManager.h>
#include <asl/base/Logger.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/planydec.h>
#include <paintlib/plpngenc.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include "PLFilterResizePadded.h"
#include "HeightToNormalMap.h"
#include "PLFilterFactory.h"
#include "I60Header.h"

using namespace std;
using namespace asl;

namespace y60 {
    /* Image filtering on save/load */
    void
    applyCustomFilter(PLBmp & theBitmap, const VectorOfString & theFilterName, const VectorOfVectorOfFloat & theFilterparams) {
        if (theFilterName.size() != theFilterparams.size()) {
            throw ImageLoaderException(std::string("Sorry, filter and params count do not match."), PLUS_FILE_LINE);
        }
        for (VectorOfString::size_type myFilterIndex = 0; myFilterIndex < theFilterName.size(); myFilterIndex++) {
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
        _myImageMatrix.makeIdentity();
        // Cubemaps are encoded as
        // theFrontFileName + "|" + theRightFileName + "|" + theBackFileName + "|" + theLeftFileName + "|"
        // + theTopFileName + "|" + theBottomFileName;
        vector<string> myFilenames = asl::splitString(theFilename, "|");
        vector<asl::Ptr<ReadableBlockHandle> > myBlocks;

        // Check if files exist
        for (unsigned i = 0; i < myFilenames.size(); ++i) {
            asl::Ptr<ReadableBlockHandle> myBlock;
            if (thePackageManager) {
                myBlock = thePackageManager->readFile(myFilenames[i], "" , true);
            }
#if 1
            // PM: This seems unreasonable, but need to watch out for compat problems
            // fall back to simply try to open the file
            //AC_DEBUG << myBlock << " filename='" << myFilenames[i] << "'";
            if (!myBlock && fileExists(myFilenames[i])) {
                myBlock = asl::Ptr<asl::ReadableBlockHandle>(new asl::AnyReadableBlockHandle(asl::Ptr<asl::ReadableBlock>(new ConstMappedBlock(myFilenames[i])),myFilenames[i]));
            }
#endif
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

    ImageLoader::ImageLoader(asl::Ptr<ReadableBlockHandle> theInputData,
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
        _myImageMatrix.makeIdentity();
        loadSingleImage(theInputData);
    }

    ImageLoader::~ImageLoader() {
        AC_TRACE << "ImageLoader::~ImageLoader";
        delete[] m_pClrTab;
        freeMembers();
    }

    void
    ImageLoader::loadSingleImage(asl::Ptr<ReadableBlockHandle> theImageBlock) {

        I60Header myHeader;
        theImageBlock->getBlock().readData(myHeader, 0);

        if (myHeader.checkMagicNumber()) {
            loadI60File(theImageBlock);
        } else {
            try {
                PLAnyPicDecoder myDecoder;
                myDecoder.MakeBmpFromMemory(const_cast<unsigned char*>(theImageBlock->getBlock().begin()),
                        theImageBlock->getBlock().size(), this);
            } catch (const PLTextException & e) {
                throw ImageLoaderException(_myFilename + " - " + static_cast<const char *>(e), PLUS_FILE_LINE);
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
    ImageLoader::loadCubemap(std::vector<asl::Ptr<ReadableBlockHandle> > & theBlocks, unsigned theDepth) {

        for (unsigned i = 0; i < 6; ++i) {
            string myDescription = string("Face")+as_string(i)+"_"+_myFilename;
            ImageLoader myFaceBmp(theBlocks[i], myDescription, _myTextureManager.lock());
            myFaceBmp.ensurePowerOfTwo(IMAGE_RESIZE_SCALE, theDepth);

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

            int myTargetWidth  = GetWidth();
            int myTargetHeight = GetHeight() / 6 / theDepth;
            if (myFaceBmp.GetWidth() != myTargetWidth || myFaceBmp.GetHeight() != myTargetHeight) {
                AC_WARNING << "Resizing bitmap " << myDescription << " to " << myTargetWidth << "x" << myTargetHeight << ".";
                myFaceBmp.ApplyFilter(PLFilterResizeBilinear(myTargetWidth, myTargetHeight));
            }

            // Copy face into destination bitmap
            PLBYTE ** mySrcLines = myFaceBmp.GetLineArray();
            PLBYTE ** myDstLines = GetLineArray();

            unsigned int myFaceHeight = myFaceBmp.GetHeight();
            long myLineSize = GetBytesPerLine();
            for (unsigned int y = 0; y < myFaceHeight; ++y) {
                memcpy(myDstLines[y + myFaceHeight * i], mySrcLines[y], myLineSize);
            }
        }
    }

    void
    ImageLoader::loadI60File(asl::Ptr<ReadableBlockHandle> theImageBlock) {
        _myImageMatrix.makeIdentity();
        I60Header myHeader;
        theImageBlock->getBlock().readData(myHeader, 0);

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
        unsigned myBlockSize = theImageBlock->getBlock().size();
        _myData = asl::Ptr<Block>(new Block());
        _myData->resize(myBlockSize - sizeof(I60Header));
        std::copy(theImageBlock->getBlock().begin()+sizeof(I60Header),
                  theImageBlock->getBlock().end(), _myData->begin());

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
        AC_TRACE << "Pixelformat: " << thePixelformat.GetName();
        if (!mapFormatToPixelEncoding(thePixelformat, _myEncoding)) {
            throw ImageLoaderException(string("Unsupported Pixel Encoding: ") + thePixelformat.GetName(), PLUS_FILE_LINE);
        }
        _myData = asl::Ptr<Block>(new Block());
        _myData->resize(getBytesRequired(theWidth * theHeight, _myEncoding));

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
        if (GetBitsPerPixel() <= 8) {
            // note: this is not a nice place to plug this in,
            // but there seems to be no other place
            m_pClrTab = new PLPixel32[256];
        }
        m_pLineArray = new unsigned char * [GetHeight()];
        updateLineArray();
    }

    void
    ImageLoader::updateLineArray() {
        unsigned myHeight     = GetHeight();
        unsigned myLineStride = GetBytesPerLine();

        const unsigned char * pBits = _myData->begin();
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
        if (!_isI60 && GetBitsPerPixel() == 32 && !HasAlpha()) {
            // Compress unused fourth channel
            AC_DEBUG << "ImageLoader removing unused fourth channel";

            unsigned myHeight = GetHeight();
            unsigned myWidth  = GetWidth();
            //unsigned myLineStride = GetBytesPerLine();

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
            _myData = myDestinationBlock;
            PLPixelFormat myPixelFormat;
            mapPixelEncodingToFormat(_myEncoding, myPixelFormat);
            SetBmpInfo(GetSize(), GetResolution(), myPixelFormat);
            updateLineArray();
        }
    }

    void
    ImageLoader::ensurePowerOfTwo(const std::string & theResizeMode,
                                  unsigned theDepth, const asl::Vector2i * theTile)
    {
        // limit texture size
        int myTextureSizeLimit = 0;
        if (asl::Ptr<ITextureManager> myTextureManager = _myTextureManager.lock()) {
            myTextureSizeLimit = myTextureManager->getMaxTextureSize( theDepth = 1 ? 2 : 3 );
            AC_INFO << "Texture size limit set to " << myTextureSizeLimit;
        } else {
            myTextureSizeLimit = ITextureManager::getTextureSizeLimit();
        }
        AC_DEBUG << "Texture size limit = " << myTextureSizeLimit;

        if (theResizeMode == IMAGE_RESIZE_NONE &&
            (!myTextureSizeLimit || (GetWidth() <= myTextureSizeLimit && GetHeight() <= myTextureSizeLimit))) {
            return;
        }

        if (!powerOfTwo(theDepth)) {
            throw ImageLoaderException(
                string("ensurePowerOfTwo: third dimension (depth) must be power of two for now"),
                    PLUS_FILE_LINE);
        }

        bool myIsCubemapFlag = false;
        if (theTile) {
            unsigned myNumTiles = (*theTile)[0] * (*theTile)[1];
            myIsCubemapFlag = (myNumTiles == 6 ? true : false);
        }
        try {
            int myWidthFactor = 1, myHeightFactor = theDepth;
            if (myIsCubemapFlag) {
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

            AC_DEBUG << "GetWidth() = " << GetWidth();
            AC_DEBUG << "GetHeight() = " << GetHeight();
            AC_DEBUG << "myWidthFactor = " << myWidthFactor;
            AC_DEBUG << "myHeightFactor = " << myHeightFactor;

            int myTargetWidth = GetWidth();
            int myTargetHeight = GetHeight();
             if (!powerOfTwo(GetWidth() / myWidthFactor) || !powerOfTwo(GetHeight() / myHeightFactor)) {
                myTargetWidth  = nextPowerOfTwo(GetWidth() / myWidthFactor) * myWidthFactor;
                myTargetHeight = nextPowerOfTwo(GetHeight() / myHeightFactor) * myHeightFactor;
            }
            AC_DEBUG << "myTargetWidth = " << myTargetWidth;
            AC_DEBUG << "myTargetHeight = " << myTargetHeight;

            while (myTextureSizeLimit &&
                ((myTargetWidth / myWidthFactor) > myTextureSizeLimit ||
                 (myTargetHeight / myHeightFactor)  > myTextureSizeLimit) )
            {
                myTargetWidth  = (myTargetWidth / myWidthFactor)/2 * myWidthFactor;
                myTargetHeight = (myTargetHeight / myHeightFactor)/2 * myHeightFactor;
            }
            AC_DEBUG << "GetWidth() = " << myTargetWidth;
            AC_DEBUG << "GetHeight() = " << myTargetHeight;
            AC_DEBUG << "myTargetWidth = " << myTargetWidth;
            AC_DEBUG << "myTargetHeight = " << myTargetHeight;

            if (myTextureSizeLimit &&
                (myTargetWidth != GetWidth() || myTargetHeight != GetHeight())) {
                AC_WARNING << "Texture size exceeds "<<Y60_TEXTURE_SIZE_LIMIT_ENV<<"="<<
                    myTextureSizeLimit<<", resizing to "<<
                    myTargetWidth<<"x"<<myTargetHeight<<endl;
                ApplyFilter(PLFilterResizeBilinear(myTargetWidth, myTargetHeight));
            }

            if (!powerOfTwo(GetWidth() / myWidthFactor) || !powerOfTwo(GetHeight() / myHeightFactor)) {

                unsigned myWidth  = nextPowerOfTwo(GetWidth() / myWidthFactor) * myWidthFactor;
                unsigned myHeight = nextPowerOfTwo(GetHeight() / myHeightFactor) * myHeightFactor;

                if (theResizeMode == IMAGE_RESIZE_SCALE) {
                    AC_INFO << "Resizing bitmap " << _myFilename << " to next power of two: " << myWidth << "x" << myHeight << ".";
                    ApplyFilter(PLFilterResizeBilinear(myWidth, myHeight));
                } else if (theResizeMode == IMAGE_RESIZE_PAD) {
                    //TODO: make padding for for cube maps and 3D texture strips
                    AC_INFO << "Padding bitmap '" << _myFilename << "' to next power of two: " << myWidth << "x" << myHeight << ".";
                    ApplyFilter(PLFilterResizePadded(myWidth, myHeight));
                    _myImageMatrix.scale(Vector3f(float(myPreScaleWidth) / GetWidth(),
                            float(myPreScaleHeight) / GetHeight(), 1.0f));
                } else {
                    throw ImageLoaderException(string("ensurePowerOfTwo: Unknown resize mode: ")
                            + theResizeMode, PLUS_FILE_LINE);
                }
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
