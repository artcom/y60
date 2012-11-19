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

#ifndef _ac_DShowCapture_DShowGraph_h_
#define _ac_DShowCapture_DShowGraph_h_

#define NO_DSHOW_STRSAFE // avoid a bunch of useless warnings and an error that <dshow.h> drags in

#include <dshow.h>

#include "qedit.h"

#include <string>
#include <vector>
#include "DXSampleGrabber.h"

namespace y60 {
/*! @addtogroup Y60componentsDShowCapture */
/* @{ */
class DShowGraph {
public:
    DShowGraph();
    virtual ~DShowGraph();

    void setDesiredVideoFormat(int theWidth, int theHeight, int theFps, int theBits);

    // Automatically find a capturing device to capture video
    virtual void CaptureLive(int theIndex, unsigned theInputPinNumber = 0);

    // Destroy the whole Graph
    void Destroy() { destroyFilterGraph(); };

    // Pause the video
    void Pause() { m_pMediaControl->Pause(); }
    // Continue play the video
    void Play() ;
    // Stop the video
    void Stop()  { m_pMediaControl->Stop(); }

    void setCameraParams(unsigned long theWhiteBalanceU, unsigned long theWhiteBalanceV,
            unsigned long theShutter, unsigned long theGain);

    void unlockImage() {m_pTrackingCB->unlockImage();}
    unsigned char* lockImage();
    int getWidth() const {return m_pTrackingCB->getWidth();}
    int getHeight() const {return m_pTrackingCB->getHeight();}
    bool isRunning() const { return m_fstartGraph; }
    void setAnalogVideoFormat(void);
    void traceCrossbarInfo(IAMCrossbar *pXBar);
    void configCrossbar(unsigned theInputPinNumber = 0);
    std::vector<std::string> enumDevices();
protected:
    // Create the filter graph to render the video (capture or playback)
    virtual bool createFilterGraph(int theIndex = 0, unsigned theInputPinNumber = 0);
    bool setActiveCamera(int theIndex);

    // Release the filter graph
    virtual void destroyFilterGraph();
    // Start the playing
    void startGraph();
    // Stop playing
    void stopGraph();
    // automatically find a usable capturing device
    HRESULT findCaptureDevice(IBaseFilter ** ppSrcFilter, int theDeviceIndex = 0);

    ///////////////////////////////////////////////////////////////////
    // Build the capture graph
    virtual bool buildCaptureGraph(IBaseFilter * filter = 0, int theIndex = 0);

    // create and add extra filters, called in createFilterGraph() to
    // create extra filters into the graph
    virtual void    addExtraFilters();
    // release the resource related to the extra filters
    virtual void    releaseExtraFilters();
    // initialization the extra filters after the graph is finished
    virtual void    initExtraFilters();

    // Get the input or output pin to connect filters
    IPin* get_pin( IBaseFilter* pFilter, PIN_DIRECTION dir );

    // DirectShow interface pointers
    IGraphBuilder *         m_pGraphBuilder; // build render graph
    ICaptureGraphBuilder2 * m_pCaptureGraphBuilder2; // for capture
    IMediaControl*          m_pMediaControl; // MediaControl
    IFilterGraph*           m_pFilterGraph;  // Filter Graph
    // The interface to set callback function
    // ColorConverter is needed to convert other color to RGB24
    IBaseFilter *           m_pColorConv;
    // Grab filter which get image data for each frame
    IBaseFilter *           m_pGrabFilter;
    // The interface to set callback function
    ISampleGrabber *        m_pSampleGrabber;
    DXSampleGrabber *       m_pTrackingCB;

    bool            m_fIsScaling;   // scale the video to the display window
    //bool            m_fShowCaptureProperties; // Setup dialog before capture
    bool            m_fstartGraph; // has the video graph started;
    DWORD           m_dwGraphRegister;
    void            removeGraphFromRot(DWORD pdwRegister);
    HRESULT         addGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);

    int             _myDesiredWidth, _myDesiredHeight, _myDesiredBits;
    int             _myDesiredFrameRate;
    std::string     _myDevice;
    AnalogVideoStandard _myDesiredVideoStandard;
    std::string     pinToString(long lType);

    HRESULT         selectVideoFormat();
    IBaseFilter *   m_pSrcFilter;
};
/* @} */
}

#endif // _ac_DShowCapture_DShowGraph_h_
