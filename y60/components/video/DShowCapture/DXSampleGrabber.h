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

#ifndef _ac_DShowCapture_DXSampleGrabber_h_
#define _ac_DShowCapture_DXSampleGrabber_h_



// this is a workaround, because ms somehow decided not to ship
// dxtrans.h, which is part of DirectX7 August/2007 SDK and included in qedit.h
// the include qedit.h is still in newer sdk files, but dxtrans.h is not
// ms is 'working' on this, use this workaround for compiler/linker-massage (05/2010 VS)
#pragma include_alias( "dxtrans.h", "qedit.h" )
#define __IDxtCompositor_INTERFACE_DEFINED__
#define __IDxtAlphaSetter_INTERFACE_DEFINED__
#define __IDxtJpeg_INTERFACE_DEFINED__
#define __IDxtKey_INTERFACE_DEFINED__
//  end-workaround
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
