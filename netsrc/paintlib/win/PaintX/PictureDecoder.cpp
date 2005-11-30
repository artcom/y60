/*
/--------------------------------------------------------------------
|
|      $Id: PictureDecoder.cpp,v 1.14 2004/09/11 12:41:36 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "stdafx.h"
#include "PaintX.h"
#include "PictureDecoder.h"
#include "plBmpEnc.h"
#include "plWinBmp.h"
#include "plpixelformat.h"

/////////////////////////////////////////////////////////////////////////////
// CPictureDecoder

STDMETHODIMP CPictureDecoder::InterfaceSupportsErrorInfo(REFIID riid)
{
  static const IID* arr[] =
    {
      &IID_IPictureDecoder,
    };
  for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
  {
    if (InlineIsEqualGUID(*arr[i],riid))
      return S_OK;
  }
  return S_FALSE;
}

STDMETHODIMP CPictureDecoder::LoadPicture(BSTR Filename, IUnknown ** ppUnk)
{
  HRESULT hResult=S_OK;
  PLWinBmp * pWinBmp=new PLWinBmp;
  IPictureDisp * pIPictureDisp = NULL;

  // Big try..catch wrapper to catch any exceptions before they reach COM.
  try
  {
      USES_CONVERSION;
	  m_AnyPicDecoder.MakeBmpFromFile(W2A(Filename), pWinBmp);
	  pIPictureDisp = BitmapToPicture(pWinBmp);
  }

  catch (PLTextException e)
  {
    // Handle PaintLib exceptions
    hResult = this->CreateErrorInfo(e, IID_IPictureDecoder);
  }

  // Get rid of the bitmap, if it exists
  if (pWinBmp)
	{
      delete pWinBmp;
	  pWinBmp = NULL;
	}
  // We have an error condition
  // AND an (allocated) Picture, so we must
  // get rid of the Picture Object
  if (FAILED(hResult) && pIPictureDisp)
  {
    (pIPictureDisp)->Release();
    (pIPictureDisp)=NULL;
  }
  else
    *ppUnk = pIPictureDisp;

  return hResult;
}

STDMETHODIMP CPictureDecoder::LoadResPicture(int ResourceID, IUnknown ** ppUnk)

{
	HRESULT hResult=S_OK;
	PLWinBmp * pWinBmp=new PLWinBmp;
	IPictureDisp * pIPictureDisp = NULL;
  
	// Big try..catch wrapper to catch any exceptions before they reach COM.
	try {
		try {
			m_AnyPicDecoder.MakeBmpFromResource(GetModuleHandle(NULL), ResourceID, pWinBmp);
		} catch (PLTextException e) {
			// try again, this time with resource type "PAINTX"
			m_AnyPicDecoder.MakeBmpFromResource(GetModuleHandle(NULL), ResourceID, pWinBmp, PLPixelFormat::DONTCARE, "PAINTX");
		}
		pIPictureDisp = BitmapToPicture(pWinBmp);
	} catch (PLTextException e) {
		// Handle PaintLib exceptions
		hResult = this->CreateErrorInfo(e, IID_IPictureDecoder);
	}

	// Get rid of the bitmap, if it exists
	if (pWinBmp) {
		delete pWinBmp;
		pWinBmp = NULL;
	}
	// if we have an error condition
	// AND a (allocated) picture, we must
	// get rid of the picture object
	if (FAILED(hResult) && pIPictureDisp)
	{
		pIPictureDisp->Release();
		pIPictureDisp=NULL;
	} else {
		*ppUnk = pIPictureDisp;
	}

	return hResult;
}

STDMETHODIMP CPictureDecoder::LoadMemPicture(VARIANT* vByteArray, IUnknown ** ppUnk)

{
  HRESULT hResult=S_OK;
  PLWinBmp * pWinBmp=new PLWinBmp;
  IPictureDisp * pIPictureDisp = NULL;
  SAFEARRAY FAR* psa = NULL;
  BYTE* ppvData = NULL;

  // Big try..catch wrapper to catch any exceptions before they reach COM.
  try 
  {
    USES_CONVERSION;

    // Type check VARIANT parameter. It should contain a byte array
    if (V_VT(vByteArray) != (VT_ARRAY | VT_UI1))
      throw PLTextException (PL_ERRINTERNAL,"Type Mismatch in Parameter. Pass a byte array by reference");

    psa = V_ARRAY(vByteArray);
	    
    // Check dimensions of the array.
    if (SafeArrayGetDim(psa) != 1)
      throw PLTextException (PL_ERRINTERNAL,"Type Mismatch in Parameter. Pass a one-dimensional array");

    long lBound, uBound;
    hResult = SafeArrayGetLBound(psa, 1, &lBound);
    if (FAILED(hResult)) {
      throw PLTextException (PL_ERRINTERNAL,"Could not get lbound of array");
    }

    hResult = SafeArrayGetUBound(psa, 1, &uBound);
    if (FAILED(hResult)) {
      throw PLTextException (PL_ERRINTERNAL,"Could not get ubound of array");
    }

    hResult = SafeArrayAccessData(psa, reinterpret_cast<void HUGEP* FAR*>(&ppvData));
    if (FAILED(hResult)) {
      throw PLTextException (hResult,"Could not read array");
    }

    m_AnyPicDecoder.MakeBmpFromMemory(ppvData, uBound-lBound, pWinBmp);
    SafeArrayUnaccessData(psa);
    ppvData = NULL;

    pIPictureDisp = BitmapToPicture(pWinBmp);
    
  } 
  catch (PLTextException e) 
  {
    // Handle PaintLib exceptions
	hResult = this->CreateErrorInfo(e, IID_IPictureDecoder);
  }

  // unlock array if locked
  if (ppvData) 
  {
    SafeArrayUnaccessData(psa);
    ppvData = NULL;
  }

  // Get rid of the bitmap, if it exists
  if (pWinBmp)
	{
      delete pWinBmp;
	  pWinBmp = NULL;
	}
  // We have an error condition
  // AND an (allocated) Picture, so we must
  // get rid of the Picture Object
  if (FAILED(hResult) && pIPictureDisp)
  {
    (pIPictureDisp)->Release();
    (pIPictureDisp)=NULL;
  }
  else
    *ppUnk = pIPictureDisp;
	
  return hResult;
}


IPictureDisp * CPictureDecoder::BitmapToPicture(PLBmp* pBmp)
{ 
  // We allocate a buffer large enough to hold the raw bitmap
  // plus some overhead.In most cases this should be enough to
  // hold the uncompressed data in any format, plus headers, etc...
  // Some "pathological" cases however may end up with a CODEC
  // producing more data than the uncompressed version!
  int bufsize = pBmp->GetMemUsed();
  bufsize = bufsize < 20000 ? bufsize + 4096 : int(1.2 * bufsize);
  PLStreamSink MyStreamSink;
  MyStreamSink.Open( bufsize );

  // Now save BMP encoded to the stream
  PLBmpEncoder MyBmpEncoder;
  MyBmpEncoder.SaveBmp( pBmp, &MyStreamSink );
  MyStreamSink.Close ();

  // Create a COM Picture Object from this stream
  IPictureDisp * pIPictureDisp = NULL;
  HRESULT hResult = OleLoadPicture(MyStreamSink.GetIStream(),0,false, IID_IPictureDisp, (void**)&pIPictureDisp);

  // If the picture is ok, then call IPicture::PictureChanged
  if (SUCCEEDED(hResult))
    hResult = SignalPictureChanged(pIPictureDisp);
  else
    throw PLTextException (PL_ERRINTERNAL,"OleLoadPicture failed.\n");

  return pIPictureDisp;
}

HRESULT CPictureDecoder::SignalPictureChanged(IUnknown * pUnknown)
{
  HRESULT hResult = S_OK;

  // Get Picture Interface (only the Picture Interface has the PictureChanged method)
  IPicture* pPicture;
  if(SUCCEEDED(pUnknown->QueryInterface(IID_IPicture, (LPVOID *) &pPicture)))
  {
    hResult = pPicture->PictureChanged();
    // release the reference
    pPicture->Release();
  };

  return hResult;
}

HRESULT CPictureDecoder::CreateErrorInfo(PLTextException  TextException,REFIID riidSource)
{
  // create an error info object
  ICreateErrorInfo * pcerrinfo;
  HRESULT hr = ::CreateErrorInfo(&pcerrinfo);
  // if we succeeded
  if (SUCCEEDED(hr))
  {
    int ErrCode = TextException.GetCode();

    switch(ErrCode)
    {
        // Map PaintLib Error Codes to Win32 Codes to COM HRESULT Codes
      case PL_ERRPATH_NOT_FOUND:
        hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND);
        break;
      case PL_ERRFILE_NOT_FOUND:
        hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_FILE_NOT_FOUND);
        break;
      case  PL_ERRACCESS_DENIED:
        hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_WIN32, ERROR_ACCESS_DENIED);
        break;
        // Map from PaintLib Error Codes to COM Interface-Independent Error Codes
      case PL_ERRNO_MEMORY:
        hr = E_OUTOFMEMORY;
        break;
        // Map From PaintLib Error Codes to COM Interface-Dependent HRESULT Codes
      case PL_ERRINTERNAL:
      case PL_ERRFORMAT_NOT_SUPPORTED:
      case PL_ERRWRONG_SIGNATURE:
      case PL_ERRFORMAT_UNKNOWN:
      case PL_ERRUNKNOWN_FILE_TYPE:
      case PL_ERREND_OF_FILE:
      default:
        hr = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, ErrCode);
    }

    // Set up ErrInfo object
    pcerrinfo->SetGUID(riidSource);

    CComBSTR bstrSource(L"PaintX");
    pcerrinfo->SetSource(bstrSource);

    CComBSTR bstrDescription(TextException);
    pcerrinfo->SetDescription(bstrDescription);

    // QI for the IErrorInfo interface
    IErrorInfo * perrinfo;
    if(SUCCEEDED(pcerrinfo->QueryInterface(IID_IErrorInfo, (LPVOID *) &perrinfo)))
    {
      // set the error info object
      ::SetErrorInfo(0, perrinfo);
      // release the reference
      perrinfo->Release();
    }
    // release the reference
    pcerrinfo->Release();
  }
  // return the error value
  return hr;
}


/*
/--------------------------------------------------------------------
|
|      $Log: PictureDecoder.cpp,v $
|      Revision 1.14  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.13  2003/03/18 22:44:29  mskinner
|      added LoadMemPicture\nadded IPictureDecoder2
|
|      Revision 1.12  2002/07/11 19:47:11  mskinner
|      replaced _bstr_t with CComBSTR
|
|      Revision 1.11  2002/03/03 16:29:56  uzadow
|      Re-added BPPWanted.
|
|      Revision 1.10  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.9  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2000/09/29 20:00:51  Administrator
|      VBPaintX added
|
|      Revision 1.8  2000/09/26 14:13:49  Martin Skinner
|      Added ResType "PAINTX" to LoadResPicture.
|
|      Revision 1.7  2000/09/01 14:13:49  Administrator
|      Removed MFC from paintX, added MSCV paintX sample.
|
|      Revision 1.6  2000/01/16 20:43:17  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  2000/01/10 23:53:00  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.4  1999/12/31 18:13:47  Ulrich von Zadow
|      Fixed error handling.
|
|
\--------------------------------------------------------------------
*/
