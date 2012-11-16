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
*/

#include "DXSampleGrabber.h"
#include <dshow.h>

namespace y60 {

DXSampleGrabber::DXSampleGrabber() {


    //m_pImage = new CVisRGBAByteImage;

    //m_pSequence = new CVisSequence<CVisRGBABytePixel>;
    //InitializeCamera();


    m_dFrameRate = 0;
    _myLastImage = NULL;
    _myImageFlag = 0;

}

// This callback function will be called every time the new image frame is obtained
// So we implement our tracking algorithm here now.
STDMETHODIMP DXSampleGrabber::SampleCB(double SampleTime, IMediaSample *pSample)
{
    VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER *) m_mediaType.pbFormat;
    BYTE *pData;                // Pointer to the actual image buffer
    pSample->GetPointer(&pData);


    // Get the image properties from the BITMAPINFOHEADER
    _myWidth  = pvi->bmiHeader.biWidth;
    _myHeight = pvi->bmiHeader.biHeight;


    _myLock.lock();
    if (!_myLastImage) {
        _myLastImage = new unsigned char[_myWidth*_myHeight*3];
    }
    int size = pSample->GetSize();
    memcpy(_myLastImage, pData, size);
    _myImageFlag = 1;

    _myLock.unlock();

    // display the debug info into the m_pDisplayWnd
    m_dFrameRate = 0.8 * m_dFrameRate + 0.2 / (SampleTime - m_dPrevTime);
    m_dPrevTime = SampleTime;

    return S_OK;
}

unsigned char* DXSampleGrabber::lockImage()
{
    if (!_myImageFlag) return 0;
    _myLock.lock();
    return _myLastImage;
}

void DXSampleGrabber::unlockImage()
{
    _myImageFlag = 0;
    _myLock.unlock();
}


}