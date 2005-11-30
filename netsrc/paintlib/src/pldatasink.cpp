/*
/--------------------------------------------------------------------
|
|      $Id: pldatasink.cpp,v 1.7 2004/11/09 15:55:06 artcom Exp $
|      Data Destination Base Class
|
|      This is a base class for a destination of picture data.
|      It defines methods to open, write to, and close data sources.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pldatasink.h"
#include "pldebug.h"

#include <string.h>

PLDataSink::PLDataSink() 
: PLObject(),
  m_pStartData(NULL),
  m_nCurPos(0),
  m_pszName(NULL),
#ifdef _WIN32
  m_pszwName(NULL),
  m_bNameIsWide(false),
#endif
  m_nMaxFileSize(0)
{  
}


PLDataSink::~PLDataSink
    ()
{
  if (m_pStartData)
    Close();
}


// the actual data buffer is allocated in the derived classes
void PLDataSink::Open
    ( const char * pszName,
      PLBYTE*  pData,
      size_t MaxFileSize
    )
{
  // Data source may not be open already!
  PLASSERT (! m_pStartData);
  PLASSERT (MaxFileSize > 0);

  m_nMaxFileSize = MaxFileSize;
  // unchecked memory allocation, here!
  m_pszName = new char [strlen (pszName)+1];
  strcpy (m_pszName, pszName);
  m_pStartData = pData;
  m_nCurPos = 0;
#ifdef _WIN32
  m_bNameIsWide = false;
#endif
}

#ifdef _WIN32

void PLDataSink::OpenW
    ( const wchar_t * pszwName,
      PLBYTE*  pData,
      size_t MaxFileSize
    )
{
  // Data source may not be open already!
  PLASSERT (! m_pStartData);
  PLASSERT (MaxFileSize > 0);

  m_nMaxFileSize = MaxFileSize;
  // unchecked memory allocation, here!
  m_pszwName = new wchar_t [wcslen (pszwName)+1];
  m_pszName = new char[strlen("Wide")+1];
  strcpy(m_pszName,"Wide");
  wcscpy (m_pszwName, pszwName);
  m_pStartData = pData;
  m_nCurPos = 0;
  m_bNameIsWide = true;
}

#endif

void PLDataSink::Close
    ()
{
  if (m_pszName)
  { delete [] m_pszName; m_pszName = NULL; }
#ifdef _WIN32
  delete[] m_pszwName;
  m_pszwName = NULL;
  m_bNameIsWide = false;
#endif
  m_pStartData = NULL;
  m_nCurPos = 0;
}


char * PLDataSink::GetName
    ()
{
  return m_pszName;
}

#ifdef _WIN32
wchar_t * PLDataSink::GetNameW
    ()
{
  return m_pszwName;
}
#endif

size_t PLDataSink::WriteNBytes
    ( size_t   n,
    PLBYTE* pData
    )
{
  if (m_nCurPos+n > m_nMaxFileSize)
    throw PLTextException (PL_ERREND_OF_FILE,
          "Buffer overflow while encoding.\n");

  memcpy(m_pStartData + m_nCurPos, pData, n);
  m_nCurPos += n;
  return n;
}

void PLDataSink::WriteByte
    ( PLBYTE Data
    )
{
  *(m_pStartData + m_nCurPos) = Data;
  m_nCurPos ++;
}

/*
/--------------------------------------------------------------------
|
|      $Log: pldatasink.cpp,v $
|      Revision 1.7  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.4  2002/02/24 13:00:19  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.3  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.2  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.7  2001/01/21 14:28:21  uzadow
|      Changed array cleanup from delete to delete[].
|
|      Revision 1.6  2000/12/20 19:17:46  uzadow
|      FileSink::Close() now calls the base class Close() so
|      the file sink can be reopened.
|
|      Revision 1.5  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  1999/10/19 21:23:23  Ulrich von Zadow
|      Martin Skinner: Added WriteNBytes()
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
