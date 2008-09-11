//============================================================================
// Copyright (C) 2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================

#ifndef _ac_DShowCapture_DXSampleGrabber_h_
#define _ac_DShowCapture_DXSampleGrabber_h_

#include <qedit.h>
#include <asl/dom/Value.h>
#include <asl/base/ThreadLock.h>
//#include <afxmt.h>

namespace y60 {
/*! @addtogroup Y60componentsDShowCapture */
/* @{ */


class DXSampleGrabber :  public ISampleGrabberCB {
public:
    DXSampleGrabber();
    ~DXSampleGrabber() {}

    // We have to set the mediaType as initialization, we can not get it 
    // through the pSample->GetMediaType(), it will be NULL if the media
    // type did not change.
    void SetGrabMediaType( AM_MEDIA_TYPE mt ) {m_mediaType = mt;}

    // The function that called when every frame is being processed
    // So we implement our tracking algorithm here now. It is like
    // the Transform() in Transform filter.
    STDMETHODIMP SampleCB(double SampleTime, IMediaSample *pSample);
    // Another callback function. I am not using it now.
    STDMETHODIMP BufferCB(double SampleTime, BYTE *pBuffer, long BufferLen)
    {return E_NOTIMPL;}
    // Control the tracking parameters
    void SetParams(int *params);
    void GetParams(int *params);

public: // The interface needed by ISampleGrabber Filter
    void unlockImage();
    unsigned char* lockImage();
    int getHeight() const { return _myHeight; }
    int getWidth() const { return _myWidth; }
    // fake out any COM ref counting
    STDMETHODIMP_(ULONG) AddRef() { return 2; }
    STDMETHODIMP_(ULONG) Release() { return 1; }
    // fake out any COM QI'ing
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv) {
        if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) {
            *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
            return NOERROR;
        }    
        return E_NOINTERFACE;
    }


    
protected:
    // The image dimension. We need them for change the coordinate of windows 
    // (left_top is (0, 0) to that of the image (left_bottom is (0,0))
    int  _myWidth, _myHeight;

    // For calculating the Frame rate
    double m_dFrameRate, m_dPrevTime;

    // The media type we are processing
    AM_MEDIA_TYPE m_mediaType;

    bool    _myImageFlag;
    unsigned char* _myLastImage;
    
    asl::ThreadLock _myLock;
public:
//    CEvent        m_imageReady;
};

/* @} */
}

#endif // _ac_DShowCapture_DXSampleGrabber_h_
