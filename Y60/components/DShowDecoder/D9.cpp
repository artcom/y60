#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#include <stdio.h>

#include <streams.h>
#include <mmreg.h>
#include <commctrl.h>

#include <atlbase.h>

// VMR9 Headers
#include <d3d9.h>
#include <vmr9.h>


HRESULT CreateVMR9Renderer(IFilterGraph *pGraph)
{
    IBaseFilter* pBF = NULL;
    HRESULT hRes = CoCreateInstance(CLSID_VideoMixingRenderer9, NULL, CLSCTX_INPROC,
                                    IID_IBaseFilter, (LPVOID *)&pBF);
    if(SUCCEEDED(hRes))
    {
        hRes = pGraph->AddFilter(pBF, L"Video Mixing Renderer 9");
	}
	return hRes;
}

