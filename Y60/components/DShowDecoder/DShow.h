
#ifndef _ac_DShow_h_
#define _ac_DShow_h_

#include <dshow.h>
#include <qedit.h>

HRESULT CreateGraph(const char * wFilename, 
		 IGraphBuilder **theGraph, ISampleGrabber **theSampleGrabber);

#endif

