/*
|
|      $Id: plfilesink.cpp,v 1.8 2004/11/09 15:55:06 artcom Exp $
|      File "Data Sink" Class
|
|      This class a file as a destination for picture data.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfilesink.h"
#include "pldebug.h"
#include "plexcept.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
  #define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
  #include <windows.h>
#endif


PLFileSink::PLFileSink ()
: PLDataSink (),
#ifdef PL_FILE_MAPPING
  m_hf (INVALID_HANDLE_VALUE),    // File handle.
  m_hm (NULL),    // Handle to file-mapping object.
#else
  m_pFile (NULL),
#endif
  m_pDataBuf (NULL)
{
}

PLFileSink::~PLFileSink ()
{
#ifdef PL_FILE_MAPPING
  if (m_hf && m_hf != INVALID_HANDLE_VALUE)
    Close();
#else
  if (m_pFile)
    Close();
#endif
}


int PLFileSink::Open (const char * pszFName, int MaxFileSize)
{
#ifdef PL_FILE_MAPPING
  // Use memory-mapped files
  m_hf = CreateFileA (pszFName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);

  if (m_hf == INVALID_HANDLE_VALUE)
  {
    m_hf = NULL;
    return getLastPLError();
  }
  m_hm = CreateFileMapping (m_hf, NULL, PAGE_READWRITE, 0, MaxFileSize, NULL);
  if (m_hm == NULL)
  {
    if (m_hf) CloseHandle (m_hf);
    m_hf = NULL;
    return PL_ERRACCESS_DENIED;
  }
  m_pDataBuf = (PLBYTE *) MapViewOfFile (m_hm, FILE_MAP_WRITE, 0, 0, 0);
  PLDataSink::Open(pszFName, m_pDataBuf, MaxFileSize);
  return 0;
  
#else
  // Generic code assuming memory mapped files are not available.
  // we could actually open the file in "Close()",
  // but if e.g we cannot create it, it's pointless to proceed
  if ((m_pFile = fopen (pszFName, "wb")) &&
      (m_pDataBuf = new PLBYTE [MaxFileSize]))
  {
    PLDataSink::Open(pszFName, m_pDataBuf, MaxFileSize);
    return 0;
  }
  else
    return -1;
#endif
}

#ifdef _WIN32
// Opening wide filenames can't be done in ANSI-C++ so we 
// require WinAPI file-mappings. 
int PLFileSink::OpenW (const wchar_t * pszwFName, int MaxFileSize)
{

  m_hf = CreateFileW (pszwFName, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                       CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);

  if (m_hf == INVALID_HANDLE_VALUE)
  {
    m_hf = NULL;
    return getLastPLError();
  }
  m_hm = CreateFileMapping (m_hf, NULL, PAGE_READWRITE, 0, MaxFileSize, NULL);
  if (m_hm == NULL)
  {
    if (m_hf) CloseHandle (m_hf);
    m_hf = NULL;
    return PL_ERRACCESS_DENIED;
  }
  m_pDataBuf = (PLBYTE *) MapViewOfFile (m_hm, FILE_MAP_WRITE, 0, 0, 0);
  PLDataSink::OpenW(pszwFName, m_pDataBuf, MaxFileSize);
  return 0;

}
#endif


// now flush the data to disk
void PLFileSink::Close ()
{
#ifdef PL_FILE_MAPPING
  UnmapViewOfFile (m_pStartData);
  CloseHandle (m_hm);
  m_hm = NULL;
  ::SetFilePointer(m_hf,GetDataSize(),0,FILE_BEGIN);
  ::SetEndOfFile(m_hf); //Truncate the file to the right size
  CloseHandle (m_hf);
  m_hf = NULL;
#else
  int towrite = GetDataSize();
  int written = fwrite( m_pStartData, 1, towrite, m_pFile );
  PLASSERT( written == towrite );
  fclose( m_pFile );
  m_pFile = 0;

  if (m_pDataBuf)
  {
    delete[] m_pDataBuf;
    m_pDataBuf = NULL;
  }
#endif

  PLDataSink::Close();
}

#ifdef _WIN32
int PLFileSink::getLastPLError() 
{
  switch (GetLastError())
  {
    case ERROR_PATH_NOT_FOUND:
      // sprintf (sz, "Path not found.", pszFName);
      return PL_ERRPATH_NOT_FOUND;
    case ERROR_FILE_NOT_FOUND:
      // sprintf (sz, "File not found.", pszFName);
      return PL_ERRFILE_NOT_FOUND;
    case ERROR_ACCESS_DENIED:
      // sprintf (sz, "Access denied.", pszFName);
      return PL_ERRACCESS_DENIED;
    case ERROR_SHARING_VIOLATION:
      // sprintf (sz, "Sharing violation.", pszFName);
      return PL_ERRACCESS_DENIED;
    default:
      // sprintf (sz, "CreateFile returned %d.",
      //          pszFName, GetLastError());
      return PL_ERRFILE_NOT_FOUND;
  }
}
#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilesink.cpp,v $
|      Revision 1.8  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.7  2004/09/11 12:50:52  uzadow
|      Linux build fixes.
|
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2003/08/03 12:36:55  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.4  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.3  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.7  2000/12/20 19:17:46  uzadow
|      FileSink::Close() now calls the base class Close() so
|      the file sink can be reopened.
|
|      Revision 1.6  2000/12/09 12:16:26  uzadow
|      Fixed several memory leaks.
|
|      Revision 1.5  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
