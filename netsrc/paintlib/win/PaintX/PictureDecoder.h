/*
/--------------------------------------------------------------------
|
|      $Id: PictureDecoder.h,v 1.8 2003/03/18 22:44:32 mskinner Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef __PICTUREDECODER_H_
#define __PICTUREDECODER_H_

#include "resource.h"       // main symbols
#include "planydec.h"
#include "plbitmap.h"
#include "plexcept.h"
#include "plstreamsink.h"

extern CComModule _Module;

/////////////////////////////////////////////////////////////////////////////
// CPictureDecoder
class ATL_NO_VTABLE CPictureDecoder : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CPictureDecoder, &CLSID_PictureDecoder>,
	public ISupportErrorInfo,
	public IDispatchImpl<IPictureDecoder2, &IID_IPictureDecoder2, &LIBID_PaintX>
{
public:
	CPictureDecoder()
	{
	}

DECLARE_REGISTRY_RESOURCEID(IDR_PICTUREDECODER)

BEGIN_COM_MAP(CPictureDecoder)
	COM_INTERFACE_ENTRY(IPictureDecoder)
	COM_INTERFACE_ENTRY(IPictureDecoder2)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// PictureDecoder
public:
	STDMETHOD(LoadMemPicture)(/*[in]*/ VARIANT* vByteArray, /*[out, retval]*/ IUnknown ** ppUnk);
	STDMETHOD(LoadResPicture)(/*[in]*/ int ResourceID, /*[out, retval]*/ IUnknown ** ppUnk);
	STDMETHOD(LoadPicture)(/*[in]*/ BSTR Filename, /*[out, retval]*/ IUnknown ** ppUnk);
protected:
    IPictureDisp * BitmapToPicture(PLBmp* pBmp);
	HRESULT SignalPictureChanged(IUnknown * pUnknown);
	HRESULT CreateErrorInfo(PLTextException  TextException,REFIID riidSource);
	PLAnyPicDecoder m_AnyPicDecoder;
};

#endif //__PICTUREDECODER_H_
/*
/--------------------------------------------------------------------
|
|      $Log: PictureDecoder.h,v $
|      Revision 1.8  2003/03/18 22:44:32  mskinner
|      added LoadMemPicture\nadded IPictureDecoder2
|
|      Revision 1.7  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.6  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
