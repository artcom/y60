/*
|
|      $Id: plmemsink.cpp,v 1.7 2004/11/09 15:55:06 artcom Exp $
|      Memory "Data Sink" Class
|
|      This class uses memory as a destination for picture data.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plmemsink.h"
#include "plexcept.h"
#include "plpaintlibdefs.h"
#include "plexcept.h"

PLMemSink::PLMemSink ()
  : PLDataSink(),    
    m_pDataBuf (NULL)
{

}

PLMemSink::~PLMemSink ()
{  
  Close();
}

int PLMemSink::Open (const char * pszFName, int MaxFileSize)
{
  if (m_pDataBuf = new PLBYTE [MaxFileSize])
  {
    PLDataSink::Open(pszFName, m_pDataBuf, MaxFileSize);
    return 0;
  }
  else
    return -1;
}

#ifdef _WIN32
int PLMemSink::OpenW (const wchar_t * pszwFName, int MaxFileSize)
{
  if (m_pDataBuf = new PLBYTE [MaxFileSize])
  {
    PLDataSink::OpenW(pszwFName, m_pDataBuf, MaxFileSize);
    return 0;
  }
  else
    return -1;
}
#endif

void PLMemSink::Close ()
{
  if (m_pDataBuf)
  {
    delete [] m_pDataBuf;
    m_pDataBuf = NULL;
  }

  PLDataSink::Close();
}

PLBYTE* PLMemSink::GetBytes()
{
	if (m_pDataBuf)
		return m_pDataBuf;
	else
		throw PLTextException(PL_ERRINTERNAL, "Open was not called for CMemSink");
}


/*
/--------------------------------------------------------------------
|
|      $Log: plmemsink.cpp,v $
|      Revision 1.7  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.2  2001/01/21 14:28:21  uzadow
|      Changed array cleanup from delete to delete[].
|
|      Revision 1.1  2000/10/12 21:57:26  uzadow
|      no message
|
|
|
\--------------------------------------------------------------------
*/
