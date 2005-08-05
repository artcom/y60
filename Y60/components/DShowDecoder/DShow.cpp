#define _WIN32_DCOM

#include "DShow.h"

#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>

#include <mmreg.h>
#include <commctrl.h>

#include <atlbase.h>


//private functions

HRESULT CreateSampleGrabber(IGraphBuilder *pGB, IBaseFilter **pFilter);
HRESULT CreateNullRenderer(IGraphBuilder *pGB, IBaseFilter **pFilter);

HRESULT GetUnconnectedPin(
						  IBaseFilter *pFilter,   // Pointer to the filter.
						  PIN_DIRECTION PinDir,   // Direction of the pin to find.
						  IPin **ppPin);          // Receives a pointer to the pin.

HRESULT ConnectFilters(
            IGraphBuilder *pGraph, // Filter Graph Manager.
            IPin *pOut,            // Output pin on the upstream filter.
            IBaseFilter *pDest);    // Downstream filter.

HRESULT RemoveFilterByName( IGraphBuilder *pGB, LPCWSTR pName);
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister);

//in D9.cpp
HRESULT CreateVMR9Renderer(IFilterGraph *pGraph);

#ifndef JIF
#define JIF(x) if (FAILED(hr=(x))) {return hr;}
#endif

//end private


HRESULT CreateGraph(const char * theFileName, IGraphBuilder **theGraph, 
        ISampleGrabber **theSampleGrabber) 
{
	HRESULT hr;
	IUnknown *pUnk;
	IGraphBuilder *pGraph;

    USES_CONVERSION;
	WCHAR *wFileName = A2W(theFileName);

    JIF(CoInitialize(NULL));

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
                            IID_IGraphBuilder, (LPVOID *)&pUnk);	

    hr = pUnk->QueryInterface(IID_IGraphBuilder, (LPVOID *)&pGraph);

    JIF(CreateVMR9Renderer(pGraph));

	IBaseFilter *pSampleGrabber;
	JIF(CreateSampleGrabber(pGraph, &pSampleGrabber));

	JIF(pGraph->RenderFile(wFileName, NULL)); 
	//now we have a nice graph, and we need to remove the video renderer...

	JIF(RemoveFilterByName(pGraph, L"Video Mixing Renderer 9"));
	JIF(RemoveFilterByName(pGraph, L"Color Space Converter"));
	
	IBaseFilter *pNullRenderer;
	JIF(CreateNullRenderer(pGraph, &pNullRenderer));

	IPin *myOutPin;
	JIF(GetUnconnectedPin(pSampleGrabber, PINDIR_OUTPUT, &myOutPin));
	JIF(ConnectFilters(pGraph, myOutPin, pNullRenderer));    

#if PUBLISH_GRAPH_IN_GRAPH_EDIT
	DWORD dwRegister;
	hr = AddToRot(pGraph, &dwRegister);
#endif

	*theGraph = pGraph;
	pSampleGrabber->QueryInterface(IID_ISampleGrabber, (void**)theSampleGrabber);

	return hr;
}


HRESULT CreateSampleGrabber(IGraphBuilder *pGB, IBaseFilter **pFilter) {
	HRESULT hr;

	// Create the Sample Grabber.
	JIF(CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC_SERVER,
		IID_IBaseFilter, (void**)pFilter));

	JIF(pGB->AddFilter(*pFilter, L"Sample Grabber"));

	ISampleGrabber *pGrabber;
	(*pFilter)->QueryInterface(IID_ISampleGrabber, (void**)&pGrabber);

	AM_MEDIA_TYPE mt;
	ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
	mt.majortype = MEDIATYPE_Video;
	mt.subtype = MEDIASUBTYPE_RGB24;
	pGrabber->SetMediaType(&mt);

	hr = pGrabber->SetBufferSamples(TRUE);
	//hr = pGrabber->SetCallback(&ourSampleGrabberCB, 0);

	return hr;
}

HRESULT CreateNullRenderer(IGraphBuilder *pGB, IBaseFilter **pFilter) {
	IBaseFilter * pNullRenderer;
	HRESULT hr;
	JIF(CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, 
	                                   IID_IBaseFilter, (LPVOID*) &pNullRenderer));
    
	JIF(pGB->AddFilter(pNullRenderer, L"NullRender"));

	*pFilter = pNullRenderer;
	return hr;
}

HRESULT GetUnconnectedPin(IBaseFilter *pFilter,   // Pointer to the filter.
						  PIN_DIRECTION PinDir,   // Direction of the pin to find.
						  IPin **ppPin)           // Receives a pointer to the pin.
{
	IEnumPins *pEnum = 0;
	IPin *pPin = 0;
	HRESULT hr;

	if (!ppPin)
		return E_POINTER;
	*ppPin = 0;

	// Get a pin enumerator
	JIF(pFilter->EnumPins(&pEnum));

	// Look for the first unconnected pin
	while (pEnum->Next(1, &pPin, NULL) == S_OK)
	{
		PIN_DIRECTION ThisPinDir;

		pPin->QueryDirection(&ThisPinDir);
		if (ThisPinDir == PinDir)
		{
			IPin *pTmp = 0;

			hr = pPin->ConnectedTo(&pTmp);
			if (SUCCEEDED(hr))  // Already connected, not the pin we want.
			{
				pTmp->Release();
			}
			else  // Unconnected, this is the pin we want.
			{
				pEnum->Release();
				*ppPin = pPin;
				return S_OK;
			}
		}
		pPin->Release();
	}

	// Release the enumerator
	pEnum->Release();

	// Did not find a matching pin
	return E_FAIL;
}

HRESULT ConnectFilters(
    IGraphBuilder *pGraph, // Filter Graph Manager.
    IPin *pOut,            // Output pin on the upstream filter.
    IBaseFilter *pDest)    // Downstream filter.
{
	HRESULT hr;

    if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

#if 0
        PIN_DIRECTION PinDir;
        pOut->QueryDirection(&PinDir);
        _ASSERTE(PinDir == PINDIR_OUTPUT);
#endif

    // Find an input pin on the downstream filter.
    IPin *pIn = 0;
    JIF(GetUnconnectedPin(pDest, PINDIR_INPUT, &pIn));
    
    // Try to connect them.
    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    return hr;
}

HRESULT RemoveFilterByName( IGraphBuilder *pGB, LPCWSTR pName) 
{
	HRESULT hr;
	IBaseFilter * pFilter;
	hr = pGB->FindFilterByName(pName, &pFilter);
	if (pFilter) {
		hr = pGB->RemoveFilter(pFilter);
	}
	return hr;
}

//debug w/ GraphEdit
HRESULT AddToRot(IUnknown *pUnkGraph, DWORD *pdwRegister) 
{
    IMoniker * pMoniker;
    IRunningObjectTable *pROT;
    if (FAILED(GetRunningObjectTable(0, &pROT))) {
        return E_FAIL;
    }
    WCHAR wsz[256];
    wsprintfW(wsz, L"FilterGraph %08x pid %08x", (DWORD_PTR)pUnkGraph, GetCurrentProcessId());
    HRESULT hr = CreateItemMoniker(L"!", wsz, &pMoniker);
    if (SUCCEEDED(hr)) {
        hr = pROT->Register(ROTFLAGS_REGISTRATIONKEEPSALIVE, pUnkGraph,
            pMoniker, pdwRegister);
        pMoniker->Release();
    }
    pROT->Release();
    return hr;
}

