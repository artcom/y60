//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

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