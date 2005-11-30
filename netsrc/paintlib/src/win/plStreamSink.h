/*
/--------------------------------------------------------------------
|
|      $Id: plStreamSink.h,v 1.2 2001/10/06 22:03:26 uzadow Exp $
|      Copyright (c) 1996-2000 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(AFX_STREAMSINK_H__5FCF8981_7C33_11D4_AD12_00010209C12B__INCLUDED_)
#define AFX_STREAMSINK_H__5FCF8981_7C33_11D4_AD12_00010209C12B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "plobject.h"
#include "pldatasink.h"
#include "plpaintlibdefs.h"

class PLStreamSink : public PLDataSink
{
public:
	IStream * GetIStream();
	PLStreamSink();
	virtual ~PLStreamSink();

  //!
  virtual int Open
    (
	  int MaxFileSize
    );

  //!
  virtual void Close
    ();

private:
  IStream * m_pIStream;
  PLBYTE * m_pDataBuf;
	HGLOBAL m_hMem;
protected:
};

#endif // !defined(AFX_STREAMSINK_H__5FCF8981_7C33_11D4_AD12_00010209C12B__INCLUDED_)

/*
/--------------------------------------------------------------------
|
|      $Log: plStreamSink.h,v $
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.1  2000/09/01 14:19:46  Administrator
|      no message
|
|
\--------------------------------------------------------------------
*/
