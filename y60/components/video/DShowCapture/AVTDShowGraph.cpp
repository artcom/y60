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

#include "AVTActiveCam.h"

#include "AVTDShowGraph.h"
#include "DShowHelper.h"

DEFINE_GUID(PROPSETID_VIDCAP_AVT,
  0xdec6f81a, 0x8c51, 0x4279, 0xa4, 0x5f, 0x26, 0x23, 0x82, 0x6b, 0xa8, 0x92);

//#include "AVTCameraPropset.h"
//#import "AVTPropSet.tlb" no_namespace
#include <atlcomcli.h>


using namespace std;


namespace y60 {

    AVTDShowGraph::AVTDShowGraph():_myAVTCameraIndex(0)
{}

AVTDShowGraph::~AVTDShowGraph()
{
}

void AVTDShowGraph::CaptureLive(int theIndex, unsigned theInputPinNumber)
{
    _myAVTCameraIndex = theIndex;
    destroyFilterGraph();
    if (createFilterGraph(0, theInputPinNumber)) {
        startGraph();
        this->m_fstartGraph = true;
    } else {
        destroyFilterGraph();
        AC_ERROR << "Unable to build Filter Graph";
    }
}

bool AVTDShowGraph::buildCaptureGraph(IBaseFilter * pSrcFilter, int theIndex) {
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
    
    setCameraParams(_myAVTCameraIndex);

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
    }
    return true;
}

bool AVTDShowGraph::setCameraParams(int theIndex)
{
	CComPtr<IAVTActiveCam>			m_AVTActiveCam;
	//m_AVTActiveCam = NULL;
    const IID IID_IAVTActiveCam =
    {0xF762827C,0xCF21,0x4350,{0x99,0x6F,0xA4,0xBC,0x01,0x2B,0x8D,0x39}};

    HRESULT hr;
    hr =  m_pSrcFilter->QueryInterface(IID_IAVTActiveCam,(void **) &m_AVTActiveCam);
    if(hr==S_OK) {
    	m_AVTActiveCam->put_Camera(theIndex);
        AC_INFO << "Avtivated AVT-Camera #: " << theIndex;
    }

    m_AVTActiveCam->put_Rate(static_cast<float>(_myDesiredFrameRate));
    float myRate;
    m_AVTActiveCam->get_Rate(&myRate);
    if (myRate != _myDesiredFrameRate) {
        AC_INFO << "Sorry, could not set desired framerate #"<< _myDesiredFrameRate << ", camera has: " <<  myRate;
    }
    AC_INFO << "Set camera framerate to #" << myRate;

    return true;
}
}
