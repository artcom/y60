//============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#include "OpenEXRDecoder.h"

#include <iostream>

#if defined(_MSC_VER)
#    pragma warning(push,1)
#endif
#include <paintlib/plexcept.h>
#if defined(_MSC_VER)
#    pragma warning(pop)
#endif

#include <asl/base/file_functions.h>

#include "PLDataSourceStreamAdapter.h"

using namespace asl;
using namespace std;
using namespace Imf;

OpenEXRDecoder::OpenEXRDecoder() : PLPicDecoder(), _myDataStream(), _myOpenEXRFile() {
}


OpenEXRDecoder::~OpenEXRDecoder() {
    delete _myOpenEXRFile;  // note: order matters here as '_myDataStream' is accessed by 
    delete _myDataStream;   //       '_myOpenEXRFile's dtor!
}

void
OpenEXRDecoder::Open(PLDataSource * pDataSrc) {
    if (pDataSrc) {
        _myDataStream = new PLDataSourceStreamAdapter(*pDataSrc);
        _myOpenEXRFile = new RgbaInputFile(*_myDataStream);

        RgbaChannels myChannels = _myOpenEXRFile->channels();
        //cerr << "rgba channels: " << myChannels << endl;

        PLPixelFormat myPixelFormat = PLPixelFormat::DONTCARE;
        switch (myChannels) {
            case WRITE_RGB:
                myPixelFormat = PLPixelFormat::FromName("B16G16R16"); // PLPixelFormats are backwards
                break;
            case WRITE_RGBA:
                myPixelFormat = PLPixelFormat::FromName("A16B16G16R16"); // PLPixelFormats are backwards
                break;
            default:
                throw OpenEXRException(string("Pixel format not supported: ") + as_string(myChannels), PLUS_FILE_LINE);
        }
        //cerr << "Pixelformat: " << myPixelFormat.GetName() << endl;

        Imath::Box2i myDataWindow = _myOpenEXRFile->dataWindow();
        unsigned myWidth  = myDataWindow.max.x - myDataWindow.min.x + 1;
        unsigned myHeight = myDataWindow.max.y - myDataWindow.min.y + 1;

        SetBmpInfo(PLPoint (myWidth, myHeight), PLPoint(0, 0), myPixelFormat);
    } else {
        throw OpenEXRException("PLDataSource is null", PLUS_FILE_LINE);
    }
}

void
OpenEXRDecoder::Close() {
    PLPicDecoder::Close();
}

void
OpenEXRDecoder::GetImage(PLBmpBase & Bmp)
{
    unsigned int myWidth  = Bmp.GetWidth();
    //unsigned int myHeight = Bmp.GetHeight();

    PLBYTE ** myBmpLines = Bmp.GetLineArray();
    _myOpenEXRFile->setFrameBuffer((Rgba*)myBmpLines[0], 1, myWidth);

    Imath::Box2i myDataWindow = _myOpenEXRFile->dataWindow();
    _myOpenEXRFile->readPixels(myDataWindow.min.y, myDataWindow.max.y);
}
