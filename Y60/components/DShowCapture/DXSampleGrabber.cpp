#include "DXSampleGrabber.h"
#include <dshow.h>

namespace y60 {

static const GUID CLSID_ColorSpaceConverter = 
{0x1643E180,0x90F5,0x11CE, {0x97, 0xD5, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A }};


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