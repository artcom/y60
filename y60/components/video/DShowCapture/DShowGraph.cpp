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

#include "DShowGraph.h"
#include "DShowHelper.h"

#include <initguid.h>

DEFINE_GUID(PROPSETID_VIDCAP_AVT,
  0xdec6f81a, 0x8c51, 0x4279, 0xa4, 0x5f, 0x26, 0x23, 0x82, 0x6b, 0xa8, 0x92);

#import "AVTPropSet.tlb" no_namespace

#include <atlcomcli.h>
#include <asl/base/Logger.h>
#include <asl/base/Assure.h>
#include <asl/base/string_functions.h>

#include <iostream>

using namespace std;


namespace y60 {


static const GUID CLSID_ColorSpaceConverter =
{0x1643E180,0x90F5,0x11CE, {0x97, 0xD5, 0x00, 0xAA, 0x00, 0x55, 0x59, 0x5A }};

DShowGraph::DShowGraph()
{
    // Put the pointers to NULL to indicate no initialization yet
	m_pCaptureGraphBuilder2 = NULL;
    m_pGraphBuilder  = NULL;
    m_pMediaControl  = NULL;
    m_pFilterGraph   = NULL;
	m_pSrcFilter	 = NULL;
	// Default setup for the render methods
	m_fIsScaling    = false;	// no scaling
	m_dwGraphRegister = 1;		// init it to indicate if it is registered.

	_myDesiredFrameRate = 25;
	_myDesiredHeight = 320;
	_myDesiredWidth = 240;
    _myDesiredBits = 24;
    _myDesiredVideoStandard = AnalogVideo_PAL_B;

    m_pColorConv = NULL;
	m_pGrabFilter = NULL;
	m_pSampleGrabber = NULL;
	m_pTrackingCB = NULL;
}

DShowGraph::~DShowGraph()
{
    stopGraph();
    destroyFilterGraph();
	if (m_pTrackingCB) {
	    delete m_pTrackingCB;
	}
}

void DShowGraph::setAnalogVideoFormat(void)
{
    HRESULT hr;
    IAMAnalogVideoDecoder *pIAmAVD = NULL;
    long lValue = 0;

    hr = m_pCaptureGraphBuilder2->FindInterface(NULL,
        &MEDIATYPE_Video, m_pSrcFilter,
        IID_IAMAnalogVideoDecoder, (void **)&pIAmAVD);
    if(FAILED(hr))
    {
        // this interface is only supported by WDM drivers.
        // If this failed, we're probably looking at a VfW
        // driver. No harm, no foul.
        return;
    }

    hr = pIAmAVD->get_AvailableTVFormats(&lValue);
    if(SUCCEEDED(hr))
    {
        if (lValue & _myDesiredVideoStandard) {
            // Desired Standard is available
            hr = pIAmAVD->put_TVFormat(_myDesiredVideoStandard);
            if(FAILED(hr)) {
                AC_ERROR << "Failure setting analog format for WDM video capture in setAnalogVideoFormat()";
            }
        } else {
            AC_WARNING << "WDM video capture card does not appear to support the desired analog video format in setAnalogVideoFormat()";
        }
    }
    else {
        AC_WARNING << "Failure getting available analog TV formats for WDM video capture card in setAnalogVideoFormat()";
    }

    if (pIAmAVD)
    {
        pIAmAVD->Release();
        pIAmAVD = NULL;
    }
}

bool DShowGraph::buildCaptureGraph(IBaseFilter * pSrcFilter, int theIndex) {
	HRESULT hr = CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &m_pCaptureGraphBuilder2);
	if (FAILED(hr))	{
	    checkForDShowError(hr, "CoCreateInstance(m_pCaptureGraphBuilder2) failed", PLUS_FILE_LINE);
	    return false;
	}
	// Attach the filter graph to the capture graph

	hr = m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraphBuilder);
	if (FAILED(hr))	{
	    checkForDShowError(hr, "SetFiltergraph(m_pGraphBuilder) failed", PLUS_FILE_LINE);
	    return false;
	}

	if (pSrcFilter == 0) {
		// Use the system device enumerator and class enumerator to find
		// a video capture/preview device, such as a desktop USB video camera.
		hr = findCaptureDevice(&pSrcFilter, theIndex);
	    if (FAILED(hr))	{
	        checkForDShowError(hr, "findCaptureDevice failed", PLUS_FILE_LINE);
	        return false;
	    }

	}
	m_pSrcFilter = pSrcFilter;
    
	// Add Capture filter to our graph.
	hr = m_pGraphBuilder->AddFilter(pSrcFilter, L"Video Capture");
	if (FAILED(hr))	{
	    checkForDShowError(hr, "AddFilter(\"Video Capture\") failed", PLUS_FILE_LINE);
	    return false;
	}

	setAnalogVideoFormat();

	addExtraFilters();

    hr = selectVideoFormat();

	if (FAILED(hr)) {
	    checkForDShowError(hr, "selectVideoFormat() failed", PLUS_FILE_LINE);
    	//return false;  // unable to build graph
    }

/*
    // test
    IBaseFilter* ppf;
    IFileSinkFilter* pSink;
    m_pCaptureGraphBuilder2->SetOutputFileName(
        &MEDIASUBTYPE_Avi, L"C:\\Example.avi", &ppf, &pSink);
*/

    // Connect the extra filters into the graph
    IPin * pOut = get_pin( m_pSrcFilter, PINDIR_OUTPUT );
    IPin * pIn  = get_pin( m_pGrabFilter, PINDIR_INPUT);
    hr = m_pGraphBuilder->Connect(pOut, pIn);
    SafeRelease(pOut);
    SafeRelease(pIn);
    if (FAILED(hr))    {
        checkForDShowError(hr, "m_pGraphBuilder->Connect() failed", PLUS_FILE_LINE);
        return false;
    }

    pOut = get_pin(m_pGrabFilter, PINDIR_OUTPUT);
    hr = m_pGraphBuilder->Render( pOut );
    SafeRelease(pOut);
    if (FAILED(hr))    {
        checkForDShowError(hr, "m_pGraphBuilder->Render() failed", PLUS_FILE_LINE);
        //return false;
    }

    // Now that the filter has been added to the graph and we have
    // rendered its stream, we can release this reference to the filter.


    // Don't release the source filter, it will be released in the DTOR
    //SafeRelease( pSrcFilter );
    return true;
}


bool DShowGraph::createFilterGraph(int theIndex, unsigned theInputPinNumber)
{
    HRESULT hr;

    hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                      IID_IGraphBuilder, (void **)&m_pGraphBuilder);
    if (FAILED(hr))    {
        checkForDShowError(hr, "CoCreateInstance(m_pGraphBuilder) failed", PLUS_FILE_LINE);
        return false;
    }
    m_pGraphBuilder->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
    m_pGraphBuilder->QueryInterface(IID_IFilterGraph, (void**)&m_pFilterGraph);

    if (!buildCaptureGraph(NULL, theIndex)) {
        return false;
    }

    configCrossbar(theInputPinNumber);
    // Do some initialization of the extra filters
    initExtraFilters();

    return true; // success build the graph
}

void DShowGraph::destroyFilterGraph()
{

    this->m_fstartGraph= false;
    stopGraph();
    // release the DirectShow Objects
    SafeRelease( m_pGraphBuilder );
    SafeRelease( m_pCaptureGraphBuilder2 );
    SafeRelease( m_pMediaControl );
    SafeRelease( m_pFilterGraph  );
    SafeRelease( m_pSrcFilter );
    // release the extra filters resource
    releaseExtraFilters();
}

void DShowGraph::startGraph()
{
    HRESULT hr;
    if( m_pMediaControl )
    {
        // Add the graph to Rot so that the graphedit can view it
        if (m_dwGraphRegister) {
            hr = addGraphToRot(m_pGraphBuilder, &m_dwGraphRegister);
            if (FAILED(hr))
            {
                m_dwGraphRegister = 0;
            }
        }
        //hr = m_pMediaControl->Run();

    }
}

void DShowGraph::stopGraph()
{
    if( m_pMediaControl )
    {
        m_pMediaControl->Stop();
    }

    if (m_dwGraphRegister)
        removeGraphFromRot(m_dwGraphRegister);
}

HRESULT DShowGraph::findCaptureDevice(IBaseFilter ** ppSrcFilter, int theDeviceIndex)
{
    HRESULT hr;
    IBaseFilter * pSrc = NULL;
    CComPtr <IMoniker> pMoniker =NULL;
    ULONG cFetched;

    // Create the system device enumerator
    CComPtr <ICreateDevEnum> pDevEnum =NULL;

    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
        IID_ICreateDevEnum, (void ** ) &pDevEnum);

    // Create an enumerator for the video capture devices
    CComPtr <IEnumMoniker> pClassEnum = NULL;

    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);

    if (theDeviceIndex > 0) {
        pClassEnum->Skip(theDeviceIndex);
    }
    // Use the first video capture device on the device list.
    // Note that if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.
    if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched)))
    {
        // Bind Moniker to a filter object
        hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
        if (FAILED(hr))
        {
            AC_ERROR << "Couldn't bind moniker to filter object!";
            return hr;
        } else {
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                (void**)(&pPropBag));
            // Find the description or friendly name.
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"Description", &varName, 0);
            if (FAILED(hr))
            {
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            }
            if (SUCCEEDED(hr))
            {
                // Add it to the application's list box.
                _myDevice = std::string(asl::convertBSTRToLPSTR(varName.bstrVal));
                AC_DEBUG << "Capture Device is: " << _myDevice;
                VariantClear(&varName);
            }
            pPropBag->Release();
        }
    }
    else
    {
        AC_ERROR << "Unable to access video capture device!";
        return E_FAIL;
    }
    // Copy the found filter pointer to the output parameter.
    // Do NOT Release() the reference, since it will still be used
    // by the calling function.
    *ppSrcFilter = pSrc;
    return hr;
}

void DShowGraph::setCameraParams(unsigned long theWhiteBalanceU, unsigned long theWhiteBalanceV,
        unsigned long theShutter, unsigned long theGain)
{
    CComQIPtr<IAVTDolphinPropSet, &PROPSETID_VIDCAP_AVT> pAVTFilter(m_pSrcFilter);

    if(pAVTFilter) {
        AC_DEBUG << "Found AVT camera interface." << endl;
        HRESULT hr;
        hr = pAVTFilter->SetWhitebalanceU(theWhiteBalanceU, 0, 0);
        checkForDShowError(hr, "SetWhiteBalanceU", PLUS_FILE_LINE);
        hr = pAVTFilter->SetWhitebalanceV(theWhiteBalanceV, 0, 0);
        checkForDShowError(hr, "SetWhiteBalanceV", PLUS_FILE_LINE);
        hr = pAVTFilter->SetGain(theGain, 0, 0);
        checkForDShowError(hr, "SetGain", PLUS_FILE_LINE);
        hr = pAVTFilter->SetExposure(theShutter, 0, 0);
        checkForDShowError(hr, "SetExposure", PLUS_FILE_LINE);
    } else {
        AC_DEBUG << "AVT camera interface not found." << endl;
    }


// [CH+CM]: This code works, but the parameter passing is not implemented, yet.
//          So please leave this block, for further improvement.
#if 0
    IAMVideoProcAmp * pProcAmp = 0;
    m_pSrcFilter->QueryInterface(IID_IAMVideoProcAmp, (void**)&pProcAmp);

    if (pProcAmp) {
        AC_DEBUG << "Found AMVideoProcAmp interface";

        long Min, Max, Step, Default, Flags, Val;
        pProcAmp->GetRange(VideoProcAmp_Gain, &Min, &Max, &Step, &Default, &Flags);
        AC_DEBUG << "get gain returns: " << Min << " " << Max << " " << Step << " " << Default << " " << Flags;

        pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
        AC_DEBUG << "get brightness before returns: " << Val << " " << Flags;


        pProcAmp->Set(VideoProcAmp_Brightness, 40, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Contrast, 40, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Hue, 18, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Saturation, 100, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Sharpness, 1, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_WhiteBalance, 1, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Gamma, 1, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_ColorEnable, 1, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_BacklightCompensation, 0, VideoProcAmp_Flags_Manual);
        pProcAmp->Set(VideoProcAmp_Gain, 20, VideoProcAmp_Flags_Manual);

        pProcAmp->Get(VideoProcAmp_Brightness, &Val, &Flags);
        AC_DEBUG << "get brightness returns: " << Val << " " << Flags;

    } else {
        AC_DEBUG << "AMVideoProcAmp interface not found.";
    }

    IAMCameraControl * pCamContr = 0;
    m_pSrcFilter->QueryInterface(IID_IAMCameraControl , (void**)&pCamContr);

    if (pCamContr) {
        long Min, Max, Step, Default, Flags, Val;
        AC_DEBUG << "AMCameraControl interface found.";
        pCamContr->Set(CameraControl_Exposure, 25, CameraControl_Flags_Manual);
        pCamContr->Set(CameraControl_Iris, 40, CameraControl_Flags_Manual);
        pCamContr->GetRange(CameraControl_Iris, &Min, &Max, &Step, &Default, &Flags);
        AC_DEBUG << "get range IRIS: " << Min << " " << Max << " " << Step << " " << Default << " " << Flags;

    } else {
        AC_DEBUG << "AMCameraControl interface not found.";
    }
#endif
}


IPin* DShowGraph::get_pin( IBaseFilter* pFilter, PIN_DIRECTION dir )
{
    IEnumPins*  pEnumPins = 0;
    IPin*       pPin = 0;

    if( pFilter )
    {
        pFilter->EnumPins( &pEnumPins );
        if( pEnumPins != 0 )
        {
            for(;;)
            {
                ULONG  cFetched = 0;
                PIN_DIRECTION pinDir = PIN_DIRECTION(-1);
                pPin = 0;

                pEnumPins->Next( 1, &pPin, &cFetched );
                if( cFetched == 1 && pPin != 0 )
                {
                    pPin->QueryDirection( &pinDir );
                    if( pinDir == dir ) break;
                    pPin->Release();
                }
            }
            pEnumPins->Release();
        }
    }

    return pPin;
}

HRESULT DShowGraph::addGraphToRot(IUnknown *pUnkGraph, DWORD *pdwRegister)
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    WCHAR wsz[128];
    HRESULT hr;

    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }

    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph,
              GetCurrentProcessId());

    hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(0, pUnkGraph, pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

void DShowGraph::removeGraphFromRot(DWORD pdwRegister)
{
    IRunningObjectTable *pROT;

    if (SUCCEEDED(GetRunningObjectTable(0, &pROT))) {
        pROT->Revoke(pdwRegister);
        pROT->Release();
    }
}

void DShowGraph::CaptureLive(int theIndex, unsigned theInputPinNumber)
{
    destroyFilterGraph();
    if (createFilterGraph(theIndex, theInputPinNumber)) {
        startGraph();
        this->m_fstartGraph = true;
    } else {
        destroyFilterGraph();
        AC_ERROR << "Unable to build Filter Graph";
    }
}

unsigned char* DShowGraph::lockImage()
{
    if (m_pTrackingCB) {
        return m_pTrackingCB->lockImage();
    } else {
        return 0;
    }
}

void DShowGraph::Play(){
    if (!this->m_fstartGraph)
        this->startGraph();
    this->m_fstartGraph = true;
    m_pMediaControl->Run();
}


HRESULT DShowGraph::selectVideoFormat() {
    IAMStreamConfig *pSC;
    // Get the Media Stream config interface
    HRESULT hr = m_pCaptureGraphBuilder2->FindInterface(&PIN_CATEGORY_CAPTURE,
            &MEDIATYPE_Video, m_pSrcFilter, IID_IAMStreamConfig, (void **)&pSC);

    if (FAILED(hr)) {
        checkForDShowError(hr, "Could not find interface 'IID_IAMStreamConfig'", PLUS_FILE_LINE);
    }

    // get format being used NOW
    AM_MEDIA_TYPE *pmt;

    hr = pSC->GetFormat(&pmt);
    if (FAILED(hr)) {
        checkForDShowError(hr, "GetFormat() failed", PLUS_FILE_LINE);
    }

    //check the media subtype
    if ( !IsEqualGUID( pmt->subtype,  MEDIASUBTYPE_RGB24) ||
         !((VIDEOINFO *)pmt->pbFormat) )
    {
        AC_WARNING << "Video capture device does not use selected media subtype 'RGB24' by default.";
    }

    //check the resolution
    VIDEOINFOHEADER *pVih = reinterpret_cast<VIDEOINFOHEADER*>(pmt->pbFormat);
    if ((pVih->bmiHeader.biWidth != _myDesiredWidth) ||
        (pVih->bmiHeader.biHeight != _myDesiredHeight))
    {
        AC_INFO << "Video capture device resolution is '"
                 << pVih->bmiHeader.biWidth << "x" << pVih->bmiHeader.biHeight
                 << "'. Should be '" << _myDesiredWidth << "x" << _myDesiredHeight << "'!";
    }
    BITMAPINFOHEADER bih;
    bih.biBitCount = static_cast<WORD>(_myDesiredBits);
    bih.biClrImportant = 0;
    bih.biClrUsed = 0;
    bih.biCompression = 0;
    bih.biHeight = _myDesiredHeight;
    bih.biWidth = _myDesiredWidth;
    bih.biPlanes = 1;
    bih.biSize = sizeof(BITMAPINFOHEADER);
    bih.biSizeImage =  _myDesiredHeight * _myDesiredWidth * _myDesiredBits / 8;
    bih.biWidth = _myDesiredWidth;
    bih.biXPelsPerMeter = 0;
    bih.biYPelsPerMeter = 0;

    VIDEOINFOHEADER vih;
    vih.rcSource.top = 0;
    vih.rcSource.left = 0;
    vih.rcSource.bottom = 0;
    vih.rcSource.right = 0;
    vih.rcTarget.top = 0;
    vih.rcTarget.left = 0;
    vih.rcTarget.bottom = 0;
    vih.rcTarget.right = 0;
    vih.dwBitRate = _myDesiredFrameRate* _myDesiredHeight * _myDesiredBits / 8; //15 * 640* 480* 24/ 8;
    vih.dwBitErrorRate = 0;
    vih.AvgTimePerFrame = 10000000 / _myDesiredFrameRate;
    vih.bmiHeader = bih;

    pmt->bFixedSizeSamples = TRUE;
    pmt->bTemporalCompression = FALSE;
    pmt->cbFormat = sizeof(VIDEOINFOHEADER);
    pmt->formattype = FORMAT_VideoInfo;
    pmt->lSampleSize = _myDesiredHeight * _myDesiredWidth * _myDesiredBits / 8; //UYVY format //640* 480* 24 / 8;
    pmt->majortype = MEDIATYPE_Video;
    pmt->subtype = MEDIASUBTYPE_RGB24;
    //pmt->subtype = MEDIASUBTYPE_UYVY;
    pmt->pbFormat = (unsigned char *)&vih;
    pmt->pUnk = 0;

    AC_DEBUG << "w=" << _myDesiredWidth << ", h=" << _myDesiredHeight << ", framerate=" << _myDesiredFrameRate << ", bits=" << _myDesiredBits;

    hr = pSC->SetFormat(pmt);
    if (FAILED(hr)) {
        checkForDShowError(hr, "Set stream format 'SetFormat()' failed", PLUS_FILE_LINE);
    }

    pSC->Release();

    return hr;
}

void DShowGraph::setDesiredVideoFormat(int theWidth, int theHeight, int theFps, int theBits) {
    _myDesiredFrameRate = theFps;
    _myDesiredWidth = theWidth;
    _myDesiredHeight = theHeight;
    _myDesiredBits = theBits;
}


void DShowGraph::addExtraFilters()
{
    // Instanciate the ISampleGrabber filter
    HRESULT hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
                                       IID_IBaseFilter, (LPVOID *)&m_pGrabFilter);
    m_pGrabFilter->QueryInterface(IID_ISampleGrabber, (void **)&m_pSampleGrabber);
    if (FAILED(hr)) {
        MessageBox(NULL, "Fail to init ISampleGrabber", NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
        return;
    }

    // Specify what media type to process, to make sure it is connected correctly
    // We now process full decompressed RGB24 image data
    AM_MEDIA_TYPE   mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;
    mt.formattype = FORMAT_VideoInfo;
    hr = m_pSampleGrabber->SetMediaType(&mt);
    // Set working mode as continuous with no buffer
    m_pSampleGrabber->SetOneShot(FALSE);
    m_pSampleGrabber->SetBufferSamples(FALSE);
    // add the grabber into the graph
    hr = m_pFilterGraph->AddFilter(m_pGrabFilter, L"Grabber");
    if (FAILED(hr)) {
        checkForDShowError(hr, "Add filter 'Grabber' failed", PLUS_FILE_LINE);
    }

    // Sometimes, we may need the color converter to add in the SampleGrabber
    // Filter. So just add one into the graph. If it is needed, it will be used
    // Otherwise, it will be idle.
    hr = CoCreateInstance( CLSID_ColorSpaceConverter, NULL, CLSCTX_INPROC_SERVER,
                 IID_IBaseFilter, (void **)&m_pColorConv );
    m_pFilterGraph->AddFilter(m_pColorConv, L"Color Space Converter");
    if (FAILED(hr)) {
        checkForDShowError(hr, "Add filter 'Color Space Converter' failed", PLUS_FILE_LINE);
    }

    // Normally, we donot need to connect the PIN by ourselves.
    // it will be correctly connected when using Render().
    // So we do not connect these two filters together.
    // XXX !(show Video)?
    if (true) {
        IBaseFilter * pNull;
        hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER,
                              IID_IBaseFilter, (LPVOID*) &pNull);
        m_pFilterGraph->AddFilter(pNull, L"NullRender");

        // we can release the reference to the null render
        // after it has been added to the video graph.
        SafeRelease(pNull);

    }
}

void DShowGraph::releaseExtraFilters() {
    // Stop the tracking

    // Release the resourse
    SafeRelease(m_pSampleGrabber);
    SafeRelease(m_pColorConv);
    SafeRelease(m_pGrabFilter);
}

void DShowGraph::initExtraFilters() {
    // Init the grabber filter
    AM_MEDIA_TYPE   mt;
    // Set the CallBack Interface
    if (!m_pTrackingCB)
        m_pTrackingCB = new DXSampleGrabber();
    m_pSampleGrabber->SetCallback(m_pTrackingCB, 0);

    // Set the media type (needed to get the bmp header info
    m_pSampleGrabber->GetConnectedMediaType(&mt);
    m_pTrackingCB->SetGrabMediaType(mt);
}

//
// Converts a PinType into a String
//
std::string DShowGraph::pinToString(long lType)
{
    std::string myString;
    switch (lType)
    {
        case PhysConn_Video_Tuner:           myString = std::string("Video Tuner\0");          break;
        case PhysConn_Video_Composite:       myString = std::string("Video Composite\0");      break;
        case PhysConn_Video_SVideo:          myString = std::string("Video SVideo\0");         break;
        case PhysConn_Video_RGB:             myString = std::string("Video RGB\0");            break;
        case PhysConn_Video_YRYBY:           myString = std::string("Video YRYBY\0");          break;
        case PhysConn_Video_SerialDigital:   myString = std::string("Video SerialDigital\0");  break;
        case PhysConn_Video_ParallelDigital: myString = std::string("Video ParallelDigital\0");break;
        case PhysConn_Video_SCSI:            myString = std::string("Video SCSI\0");           break;
        case PhysConn_Video_AUX:             myString = std::string("Video AUX\0");            break;
        case PhysConn_Video_1394:            myString = std::string("Video 1394\0");           break;
        case PhysConn_Video_USB:             myString = std::string("Video USB\0");            break;
        case PhysConn_Video_VideoDecoder:    myString = std::string("Video Decoder\0");        break;
        case PhysConn_Video_VideoEncoder:    myString = std::string("Video Encoder\0");        break;

        case PhysConn_Audio_Tuner:           myString = std::string("Audio Tuner\0");          break;
        case PhysConn_Audio_Line:            myString = std::string("Audio Line\0");           break;
        case PhysConn_Audio_Mic:             myString = std::string("Audio Mic\0");            break;
        case PhysConn_Audio_AESDigital:      myString = std::string("Audio AESDigital\0");     break;
        case PhysConn_Audio_SPDIFDigital:    myString = std::string("Audio SPDIFDigital\0");   break;
        case PhysConn_Audio_SCSI:            myString = std::string("Audio SCSI\0");           break;
        case PhysConn_Audio_AUX:             myString = std::string("Audio AUX\0");            break;
        case PhysConn_Audio_1394:            myString = std::string("Audio 1394\0");           break;
        case PhysConn_Audio_USB:             myString = std::string("Audio USB\0");            break;
        case PhysConn_Audio_AudioDecoder:    myString = std::string("Audio Decoder\0");        break;

        default:
            myString = std::string("Unknown\0");
            break;
    }
    return myString;
}

void DShowGraph::traceCrossbarInfo(IAMCrossbar *pXBar)
{
    HRESULT hr;
    long cOutput = -1, cInput = -1;
    hr = pXBar->get_PinCounts(&cOutput, &cInput);

    for (long i = 0; i < cOutput; i++)
    {
        long lRelated = -1, lType = -1;
        hr = pXBar->get_CrossbarPinInfo(FALSE, i, &lRelated, &lType);
        AC_DEBUG << "Output pin " << i << ": " << pinToString(lType);
    }

    for (long i = 0; i < cInput; i++)
    {
        long lRelated = -1, lType = -1;
        hr = pXBar->get_CrossbarPinInfo(TRUE, i, &lRelated, &lType);
        AC_DEBUG << "Input pin " << i << " - " << pinToString(lType);
    }
}

void DShowGraph::configCrossbar(unsigned theInputPinNumber) {
    // Search upstream for a crossbar.
    IAMCrossbar *pXBar1 = NULL;
    HRESULT hr = m_pCaptureGraphBuilder2->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, m_pSrcFilter,
        IID_IAMCrossbar, (void**)&pXBar1);
    if (SUCCEEDED(hr))
    {
        AC_DEBUG << "Routing Crossbar1 input=" << theInputPinNumber;
        pXBar1->Route(0, theInputPinNumber);
        traceCrossbarInfo(pXBar1);

        // Found one crossbar. Get its IBaseFilter interface.
        IBaseFilter *pFilter = NULL;
        hr = pXBar1->QueryInterface(IID_IBaseFilter, (void**)&pFilter);
        if (SUCCEEDED(hr))
        {
            // Search upstream for another crossbar.
            IAMCrossbar *pXBar2 = NULL;
            hr = m_pCaptureGraphBuilder2->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, pFilter,
                IID_IAMCrossbar, (void**)&pXBar2);
            pFilter->Release();

            if (SUCCEEDED(hr))
            {
                AC_DEBUG << "Routing Crossbar2 input=" << theInputPinNumber;
                pXBar2->Route(0, theInputPinNumber);
                traceCrossbarInfo(pXBar2);
                /* ... */
                pXBar2->Release();
            }
        }
        pXBar1->Release();
    }
}

std::vector<std::string> DShowGraph::enumDevices() {
    //IMoniker *pMoniker = NULL;
    std::vector<std::string> myDeviceList;
    ICreateDevEnum *pDevEnum = NULL;
    IEnumMoniker *pEnum = NULL;

    // Create the System Device Enumerator.
    HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL,
        CLSCTX_INPROC_SERVER, IID_ICreateDevEnum,
        reinterpret_cast<void**>(&pDevEnum));
    if (SUCCEEDED(hr))
    {
        // Create an enumerator for the video capture category.
        hr = pDevEnum->CreateClassEnumerator(
            CLSID_VideoInputDeviceCategory,
            &pEnum, 0);
        ASSURE(pEnum);
        while (true)
        {
            IMoniker *pMoniker = NULL;
            ULONG ulFetched = 0;
            hr = pEnum->Next(1, &pMoniker, &ulFetched);
            if( hr != S_OK ) {
                break;
            }
            IPropertyBag *pPropBag;
            hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag,
                (void**)(&pPropBag));
            if (FAILED(hr))
            {
                pMoniker->Release();
                continue;  // Skip this one, maybe the next one will work.
            }
            // Find the description or friendly name.
            VARIANT varName;
            VariantInit(&varName);
            hr = pPropBag->Read(L"Description", &varName, 0);
            if (FAILED(hr))
            {
                hr = pPropBag->Read(L"FriendlyName", &varName, 0);
            }
            if (SUCCEEDED(hr))
            {
                // Add it to the application's list box.
                myDeviceList.push_back(std::string(asl::convertBSTRToLPSTR(varName.bstrVal)));
                AC_DEBUG << "Found Device " << myDeviceList.size()-1 << ": " << myDeviceList[myDeviceList.size()-1];
                VariantClear(&varName);
            }
            pPropBag->Release();
            pMoniker->Release();
        }
        pEnum->Release();
        pDevEnum->Release();
    }
    return myDeviceList;
}

}
