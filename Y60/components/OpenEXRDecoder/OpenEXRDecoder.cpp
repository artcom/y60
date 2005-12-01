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
#include "PLDataSourceStreamAdapter.h"
#include <paintlib/plexcept.h>
#include <iostream>
#include <asl/file_functions.h>

using namespace asl;
using namespace std;
using namespace Imf;

OpenEXRDecoder::OpenEXRDecoder() : PLPicDecoder() {
}


OpenEXRDecoder::~OpenEXRDecoder() {
    delete _myOpenEXRFile;
}

void
OpenEXRDecoder::Open(PLDataSource * pDataSrc) {
    if (pDataSrc) {
        PLDataSourceStreamAdapter myDataStream(*pDataSrc);

        _myOpenEXRFile = new RgbaInputFile(myDataStream);
        Imath::Box2i myDataWindow = _myOpenEXRFile->dataWindow();
        int myWidth  = myDataWindow.max.x - myDataWindow.min.x + 1;
        int myHeight = myDataWindow.max.y - myDataWindow.min.y + 1;

        RgbaChannels myChannels = _myOpenEXRFile->channels();
        cerr << "rgba channels: " << myChannels << endl;

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
        cerr << "Pixelformat: " << myPixelFormat.GetName() << endl;

        SetBmpInfo(PLPoint (myWidth, myHeight), PLPoint(0, 0), myPixelFormat);
        cerr << "SetBmpInfo" << endl;
    } else {
        throw OpenEXRException("PLDataSource is null", PLUS_FILE_LINE);
    }
}

void
OpenEXRDecoder::Close() {
    PLPicDecoder::Close();
}

void
OpenEXRDecoder::GetImage(PLBmpBase & Bmp) {
    //pixels.resizeErase(height, width);
    Imath::Box2i myDataWindow = _myOpenEXRFile->dataWindow();
    cerr << "data window min: " << myDataWindow.min.x << " " << myDataWindow.min.y << endl;
    cerr << "data window may: " << myDataWindow.max.x << " " << myDataWindow.max.y << endl;
    //_myOpenEXRFile->setFrameBuffer(&pixels[0][0] - myDataWindow.min.x - myDataWindow.min.y * myWidth, 1, myWidth);
    //_myOpenEXRFile->readPixels(myDataWindow.min.y, myDataWindow.max.y);


/*
    cerr << "OpenEXRDecoder::GetImage" << endl;
    int myBpp= Bmp.GetBitsPerPixel();
    cerr << myBpp << endl;
    PLBYTE ** pLineArray = Bmp.GetLineArray();
    unsigned long myBufferSize = m_di->getOutputDataSize(myBpp);
    vector<char> myBuffer(myBufferSize);
    int myDataSize = m_di->getOutputData(
        &myBuffer[0],
        myBufferSize,
        0, //myBpp,
        0
    );

    int y;
    int myHeight = GetHeight();
    int myWidth = GetWidth();
    for (y=0; y < myHeight; y++)
    {
        memcpy(pLineArray[y], &myBuffer[y * myBufferSize/myHeight], myBufferSize/myHeight);
    }
    if (myBpp == 8) {
        Bmp.SetGrayPalette ();
    }
*/
}

