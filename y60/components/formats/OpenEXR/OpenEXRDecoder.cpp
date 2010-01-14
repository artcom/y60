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
