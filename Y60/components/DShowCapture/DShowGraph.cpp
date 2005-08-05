#include "DShowGraph.h"
#include <atlcomcli.h>
#include <asl/Logger.h>

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
	m_pMediaSeeking  = NULL;
	m_pSrcFilter	 = NULL;
	// Default setup for the render methods
	m_fIsScaling    = false;	// no scaling
	m_dwGraphRegister = 0;		// init it to indicate if it is registered.

	// desired default video format: 720*576 * 15 fps
	_myDesiredFrameRate = 25; 
	_myDesiredHeight = 320;
	_myDesiredWidth = 240; 
    _myDesiredBits = 24;
    _myDesiredVideoStandard = AnalogVideo_PAL_B;

    //_myDesiredVideoStandard = AnalogVideo_NTSC_M;

    m_pColorConv = NULL;
	m_pGrabFilter = NULL;
	m_pSampleGrabber = NULL;
	m_pTrackingCB = NULL;
}

DShowGraph::~DShowGraph()
{
	// release resouses
    destroyFilterGraph();
	if (m_pTrackingCB)	delete m_pTrackingCB; 
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
                AC_ERROR << "Failure setting analog NTSC format for WDM video capture in setAnalogVideoFormat()";
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
	if (FAILED(hr))	return false;  // unable to build graph 
	// Attach the filter graph to the capture graph

	hr = m_pCaptureGraphBuilder2->SetFiltergraph(m_pGraphBuilder);
	if (FAILED(hr))	return false;  // unable to build graph 

	if (pSrcFilter == 0) {
		// Use the system device enumerator and class enumerator to find
		// a video capture/preview device, such as a desktop USB video camera.
		hr = findCaptureDevice(&pSrcFilter, theIndex);
		if (FAILED(hr))	return false;  // unable to build graph 
		
	}
	m_pSrcFilter = pSrcFilter;

	// Add Capture filter to our graph.
	hr = m_pGraphBuilder->AddFilter(pSrcFilter, L"Video Capture");
    setAnalogVideoFormat();
	
	addExtraFilters();
    

/*	///////////////////////////////////////////////////////////
	// Seems I have no need for all this efforts to connect these Pins
	// together. Just render the Pin pSourceOut, the render() will 
	// automaticly connect these filters together. So the following
	// is not necessary anymore. Cool!!!
	// **** well this DOES NOT always work
	// **** good luck with the null render;
	///////////////////////////////////////////////////////////
	// if (pGrabIn && pGrabOut) { // if need extra filters
	//	// connect source filter with the input Pin
	//	hr = m_pGraphBuilder->Connect( pSourceOut, pGrabIn );
	//	if (FAILED(hr))	return false;  // unable to build graph 
	//	// then, render output Pin with default rendering graph
	//	hr = m_pGraphBuilder->Render( pGrabOut );
	//}
	//else // No extra filters to add in, render the source out Pin
	//    hr = m_pGraphBuilder->Render( pSourceOut );
*/

	if (FAILED(selectVideoFormat()))	return false;  // unable to build graph 
	///////////////////////////////////////////////////////////////////
	// test test
//	IBaseFilter* ppf;
//	IFileSinkFilter* pSink;
//	m_pCaptureGraphBuilder2->SetOutputFileName(
//		&MEDIASUBTYPE_Avi, L"C:\\Example.avi", &ppf, &pSink);

	// Connect the extra filters into the graph
    IPin* pOut = get_pin( pSrcFilter, PINDIR_OUTPUT );
	IPin * pIn = get_pin( m_pGrabFilter, PINDIR_INPUT);

	hr = m_pGraphBuilder->Connect(pOut, pIn);
	SafeRelease(pOut); SafeRelease(pIn);
	if (FAILED(hr))	return false;  // unable to build graph 

	pOut = get_pin(m_pGrabFilter, PINDIR_OUTPUT);
	hr = m_pGraphBuilder->Render( pOut );
	SafeRelease(pOut);
	if (FAILED(hr))	return false;  // unable to build graph 
	

	// Now that the filter has been added to the graph and we have
	// rendered its stream, we can release this reference to the filter.
    

	// Don't release the source filter, it will be released in the DTOR
	//SafeRelease( pSrcFilter );
	return true;
}

bool DShowGraph::createFilterGraph(int theIndex)
{
    IPin* pGrabIn = NULL, *pGrabOut = NULL;
	HRESULT hr;

    hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
                      IID_IGraphBuilder, (void **)&m_pGraphBuilder );
	if (FAILED(hr)) return false; // unable to build graph 
	else {
        m_pGraphBuilder->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
        m_pGraphBuilder->QueryInterface(IID_IFilterGraph, (void**)&m_pFilterGraph);

        buildCaptureGraph(NULL, theIndex);
	}
    configCrossbar();
	// Do some initialization of the extra filters
	initExtraFilters();
	// release the Pin
    SafeRelease( pGrabIn );
    SafeRelease( pGrabOut );

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
	SafeRelease( m_pMediaSeeking );
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
				// Msg(TEXT("Failed to register filter graph with ROT!  hr=0x%x"), hr);
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
                USES_CONVERSION;
                _myDevice = std::string(OLE2A(varName.bstrVal));
                AC_INFO << "Capture Device is: " << _myDevice;
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

void DShowGraph::CaptureLive(int theIndex)
{
	destroyFilterGraph();

    if (createFilterGraph(theIndex))
		startGraph();
	else {
		destroyFilterGraph();
		AC_ERROR << "Unable to build Filter Graph";
	}
}

LONGLONG DShowGraph::Seek(int offset)
{
	LONGLONG cur_pos = 0;
	if (m_pMediaSeeking) {
		m_pMediaControl->Pause();
		m_pMediaSeeking->GetCurrentPosition(&cur_pos);
		cur_pos += offset;
		m_pMediaSeeking->SetPositions(&cur_pos, AM_SEEKING_AbsolutePositioning,
			NULL, AM_SEEKING_NoPositioning);
		m_pMediaControl->StopWhenReady();
	}
	return cur_pos;
};

bool DShowGraph::createFilterGraph(IMoniker *videoDevice)
{
    IPin* pGrabIn = NULL, *pGrabOut = NULL;
    HRESULT hr;
    IBaseFilter * psrcFilter = 0;

    hr = CoCreateInstance( CLSID_FilterGraph, NULL, CLSCTX_INPROC, 
        IID_IGraphBuilder, (void **)&m_pGraphBuilder );
    if (FAILED(hr)) {
        return false; // unable to build graph 
    }
    m_pGraphBuilder->QueryInterface(IID_IMediaControl,(void**)&m_pMediaControl);
    m_pGraphBuilder->QueryInterface(IID_IFilterGraph, (void**)&m_pFilterGraph);


    // Create the capture graph builder
    if(videoDevice != 0)
    {
        IPropertyBag *pBag;

        hr = videoDevice->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pBag);
        if(SUCCEEDED(hr))
        {
            VARIANT var;
            var.vt = VT_BSTR;
            hr = pBag->Read(L"FriendlyName", &var, NULL);
            if (hr == NOERROR) {
                // var.bstrVal is a wide string
                //_myAVName = std::string(var.bstrVal);
                SysFreeString(var.bstrVal);
            }
            pBag->Release();
        }
        hr = videoDevice->BindToObject(0, 0, IID_IBaseFilter, (void**)&psrcFilter);
    }
    buildCaptureGraph(psrcFilter);
    configCrossbar();

    // Do some initialization of the extra filters
    initExtraFilters();
    // release the Pin
    SafeRelease( pGrabIn );
    SafeRelease( pGrabOut );

    this->m_fstartGraph = false;
    return true; // success build the graph
}

bool DShowGraph::initCaptureLive (IMoniker * videoDevice) {
	destroyFilterGraph();

    if (!createFilterGraph(videoDevice)) {
		destroyFilterGraph();
		MessageBox(NULL, "Unable to build Filter Graph", NULL, MB_OK|MB_ICONEXCLAMATION|MB_TASKMODAL);
		return false;
	}

	return true;
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

	if (true) { // default: 320 * 240 422, 30f/s

		// get format being used NOW
		AM_MEDIA_TYPE *pmt;
		
		
		hr = pSC->GetFormat(&pmt);

        BITMAPINFOHEADER bih;
        bih.biBitCount = _myDesiredBits;
        bih.biClrImportant = 0;
        bih.biClrUsed = 0;
        bih.biCompression = 0;
        bih.biHeight = _myDesiredHeight;
        bih.biPlanes = 1;
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biSizeImage =  _myDesiredHeight * _myDesiredWidth * _myDesiredBits/ 8;
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
		vih.dwBitRate =  _myDesiredFrameRate* _myDesiredHeight * _myDesiredBits/ 8; //15 * 640* 480* 24/ 8;
		vih.dwBitErrorRate = 0;
		vih.AvgTimePerFrame = 10000000 / _myDesiredFrameRate;
		vih.bmiHeader = bih;

		
		pmt->bFixedSizeSamples = TRUE;
		pmt->bTemporalCompression = FALSE;
		pmt->cbFormat = sizeof(VIDEOINFOHEADER);
		pmt->formattype = FORMAT_VideoInfo;
		pmt->lSampleSize = _myDesiredHeight * _myDesiredWidth*_myDesiredBits/ 8; //UYVY format //640* 480* 24 / 8;
		pmt->majortype = MEDIATYPE_Video;
		pmt->pbFormat = (unsigned char *)&vih;
		pmt->pUnk = 0;
	
		//pmt->subtype = MEDIASUBTYPE_UYVY; //MEDIASUBTYPE_RGB24;
		
		hr = pSC->SetFormat(pmt);
	}
	else { // set capture config by property page
        // XXX
		ISpecifyPropertyPages *pSpec;
		CAUUID cauuid;
		hr = pSC->QueryInterface(IID_ISpecifyPropertyPages,	(void **)&pSpec);
		if (hr == S_OK) { // show dialog
   			hr = pSpec->GetPages(&cauuid);            
			hr = OleCreatePropertyFrame(NULL, 30, 30, NULL, 1,
					(IUnknown **)&pSC, cauuid.cElems, (GUID *)cauuid.pElems, 0, 0, NULL);
			// Release the memory
			CoTaskMemFree(cauuid.pElems);
			pSpec->Release();
		}
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

	// Sometimes, we may need the color converter to add in the SampleGrabber
	// Filter. So just add one into the graph. If it is needed, it will be used
	// Otherwise, it will be idle. 
    hr = CoCreateInstance( CLSID_ColorSpaceConverter, NULL, CLSCTX_INPROC_SERVER, 
                 IID_IBaseFilter, (void **)&m_pColorConv );
	m_pFilterGraph->AddFilter(m_pColorConv, L"ColorChange");

	// Normally, we donot need to connect the PIN by ourselves. 
	// it will be correctly connected when using Render().
	// So we do not connect these two filters together.
    // XXX !(show Video)?
	if (true) {
		IBaseFilter * pNull;
		hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, 
	                                   IID_IBaseFilter, (LPVOID*) &pNull);
		m_pFilterGraph->AddFilter(pNull, L"NullRender");
		
		// we could release the reference to the null render
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
        long lRelated = -1, lType = -1, lRouted = -1;
        hr = pXBar->get_CrossbarPinInfo(FALSE, i, &lRelated, &lType);
        AC_INFO << "Output pin " << i << ": " << pinToString(lType);
    }

    for (i = 0; i < cInput; i++)
    {
        long lRelated = -1, lType = -1;
        hr = pXBar->get_CrossbarPinInfo(TRUE, i, &lRelated, &lType);
        AC_INFO << "Input pin " << i << " - " << pinToString(lType);
    }
}

void DShowGraph::configCrossbar() {
    // Search upstream for a crossbar.
    IAMCrossbar *pXBar1 = NULL;
    HRESULT hr = m_pCaptureGraphBuilder2->FindInterface(&LOOK_UPSTREAM_ONLY, NULL, m_pSrcFilter,
        IID_IAMCrossbar, (void**)&pXBar1);
    if (SUCCEEDED(hr)) 
    {
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
                traceCrossbarInfo(pXBar2);
                /* ... */
                pXBar2->Release();
            }
        }
        pXBar1->Release();
    }
}

std::vector<std::string> DShowGraph::enumDevices() {
    IMoniker *pMoniker = NULL;
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
        while (pEnum->Next(1, &pMoniker, NULL) == S_OK)
        {
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
                USES_CONVERSION;
                myDeviceList.push_back(std::string(OLE2A(varName.bstrVal)));
                AC_INFO << "Found Device " << myDeviceList.size()-1 << ": " << myDeviceList[myDeviceList.size()-1]; 
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
