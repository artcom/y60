/*
/--------------------------------------------------------------------
|
|      $Id: plfilesrc.cpp,v 1.17 2004/11/09 15:55:06 artcom Exp $
|      File Data Source Class
|
|      This is a class which takes a file as a source of picture data.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plfilesrc.h"
#include "plexcept.h"
#include "pldebug.h"

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
  #define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
  #include <windows.h>
#endif

#ifdef HAVE_MMAP
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#endif

PLFileSource::PLFileSource
    ( PLIProgressNotification * pNotification
    )
  : PLDataSource (pNotification),
#ifdef _WIN32
    m_hf (INVALID_HANDLE_VALUE),    // File handle.
    m_hm (NULL),    // Handle to file-mapping object.
#else
#ifndef HAVE_MMAP    
    m_pFile (NULL),
    m_pBuffer (NULL),
    m_pReadPos (NULL),
    m_BytesReadFromFile(0),
#endif
#endif
    m_pStartData (NULL),
    m_pCurPos (NULL)
{
}

PLFileSource::~PLFileSource
    ()
{
#ifdef _WIN32
  if (m_hf)
    Close();
#else
#ifndef HAVE_MMAP    
  if (m_pFile)
    Close();
#endif
#endif
}

int PLFileSource::Open
    ( const char * pszFName
    )
{
  int FileSize;
#ifdef _WIN32
  try
  {
    m_hf = CreateFileA (pszFName, GENERIC_READ, FILE_SHARE_READ, NULL,
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                        NULL);

    if (m_hf == INVALID_HANDLE_VALUE)
    {
      m_hf = NULL;
      return getLastPLError();
    }

    FileSize = ::GetFileSize (m_hf, NULL);

    int rc = mapFileHandle();
    if (rc)
    {
      if (m_hf) CloseHandle (m_hf);
      return rc;
    }

    // We've got the file mapped to memory.
    PLDataSource::Open (pszFName, FileSize);
  }
  catch (PLTextException)
  {
    // Clean up on error
    if (m_hf) CloseHandle (m_hf);
    throw;
  }
  return 0;

#else
#ifdef HAVE_MMAP
  int fd = open(pszFName, O_RDONLY);
  if (fd == -1) {
      PLTRACE("Open failed");
      return PL_ERRFILE_NOT_FOUND;
  }
  
  struct stat FileInfo;
  fstat(fd, &FileInfo);
  if (!S_ISREG(FileInfo.st_mode)) {
      close (fd);
      PLTRACE("S_ISREG failed");
      return PL_ERRACCESS_DENIED;    // File isn't a regular file.
  }
  FileSize = FileInfo.st_size;
  m_pStartData = (PLBYTE*)(mmap (0, FileSize, PROT_READ, MAP_SHARED, fd, 0));
  if (m_pStartData == MAP_FAILED) {
      PLTRACE("mmap failed");
      return PL_ERRACCESS_DENIED;
  }
  m_pCurPos = m_pStartData;
  close(fd);
  PLDataSource::Open(pszFName, FileSize);
  return 0;
#else
  // Generic code assuming memory mapped files are not available.
  m_pFile = NULL;
  if (strcmp (pszFName, ""))
    m_pFile = fopen (pszFName, "rb");

  if (m_pFile == NULL)
  { // Crude...
    m_pFile = 0;
    return PL_ERRFILE_NOT_FOUND;
  }

  // Determine file size. Can this be done in an easier way using ANSI C?
  fseek (m_pFile, 0, SEEK_END);
  FileSize = ftell (m_pFile);
  fseek (m_pFile, 0, SEEK_SET);

  // Create a buffer for the file.
  m_pBuffer = new PLBYTE[FileSize];
  //  this only works if our implementation of "new" does not throw...
  if (m_pBuffer == 0)
  {
    fclose (m_pFile);
    return PL_ERRNO_MEMORY;
  }

  m_pReadPos = m_pBuffer;
  m_pCurPos = m_pBuffer;
  m_BytesReadFromFile = 0;
  PLDataSource::Open (pszFName, FileSize);
  fillBuffer ();
  return 0;
#endif
#endif
}

#ifdef _WIN32
int PLFileSource::OpenW
    ( const wchar_t * pszwFName
    )
{
  try
  {
    m_hf = CreateFileW (pszwFName, GENERIC_READ, FILE_SHARE_READ, NULL,
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
                       NULL);

    if (m_hf == INVALID_HANDLE_VALUE)
    {
      m_hf = NULL;
      return getLastPLError();
    }

    int FileSize;
    FileSize = ::GetFileSize (m_hf, NULL);

    int rc = mapFileHandle();
    if (rc)
    {
      if (m_hf) CloseHandle (m_hf);
      return rc;
    }

    // We've got the file mapped to memory.
    PLDataSource::OpenW (pszwFName, FileSize);
  }
  catch (PLTextException)
  {
    if (m_hf) CloseHandle (m_hf);
    throw;
  }
  return 0;

}
#endif


void PLFileSource::Close
    ()
{
#ifdef _WIN32
  UnmapViewOfFile (m_pStartData);
  PLDataSource::Close ();
  CloseHandle (m_hm);
  CloseHandle (m_hf);
  m_hm = NULL;
  m_hf = NULL;
#else
  #ifdef HAVE_MMAP
  munmap(m_pStartData, GetFileSize());
  #else
  delete [] m_pBuffer;
  m_pBuffer = NULL;
  PLDataSource::Close ();
  fclose (m_pFile);
  m_pFile = NULL;
  #endif
#endif
}

PLBYTE * PLFileSource::ReadNBytes
    ( int n
    )
{
  isBufferOk( n);
  PLDataSource::ReadNBytes(n);
#ifndef _WIN32
#ifndef HAVE_MMAP
  if (!bytesAvailable(n))
    fillBuffer(n);
#endif  
#endif
  m_pCurPos += n;
  return m_pCurPos-n;
}

//! Read but don't advance file pointer.
PLBYTE * PLFileSource::GetBufferPtr
    ( int MinBytesInBuffer
    )
{
  PLASSERT (MinBytesInBuffer < 4096);
#ifndef _WIN32
#ifndef HAVE_MMAP
  if (!bytesAvailable(MinBytesInBuffer))
    fillBuffer();
#endif
#endif
  isBufferOk (MinBytesInBuffer);
  return m_pCurPos;
}

PLBYTE * PLFileSource::ReadEverything
    ()
{
#if _WIN32 || HAVE_MMAP
  isBufferOk(1);
  return m_pCurPos;
#else
  int BytesToRead = GetFileSize()-m_BytesReadFromFile;
  int i = fread (m_pReadPos, 1, BytesToRead, m_pFile);
  PLASSERT (i==BytesToRead);
  m_BytesReadFromFile += BytesToRead;
  m_pReadPos += BytesToRead;
  return m_pCurPos;
#endif
}

void PLFileSource::Seek
    ( int n
    )
{
#if _WIN32 || HAVE_MMAP
  isBufferOk (1);
  m_pCurPos = m_pStartData+n;
#else
  fseek (m_pFile, n, SEEK_SET);
#endif  
  PLDataSource::Seek(n);
}

// If we're using a memory mapped file, an unexpected end of file condition results
// in an access violation. We trap that access violation here and turn it
// into a paintlib exception.
// TODO: Make this work for non-windows systems.
void PLFileSource::isBufferOk (int NumBytes)
{
#ifdef _WIN32
  try 
  {
    PLBYTE byte = m_pCurPos[NumBytes-1];
  } 
  catch (...)
  {
    throw PLTextException(PL_ERREND_OF_FILE, "End of file reached while decoding.");
  }
#endif
}


#if !_WIN32 && !HAVE_MMAP
void PLFileSource::fillBuffer
    ( int n
    )
{
  int BytesToRead = GetFileSize() - m_BytesReadFromFile;
  if ( BytesToRead > n )
    BytesToRead = n;
  int i = fread (m_pReadPos, 1, BytesToRead, m_pFile);
  PLASSERT (i==BytesToRead);
  m_BytesReadFromFile += BytesToRead;
  m_pReadPos += BytesToRead;
}

bool PLFileSource::bytesAvailable
    ( int n
    )
{
  if (m_pReadPos-m_pCurPos >= n)
    return true;
  else
    return false;
}
#endif

#ifdef _WIN32
int PLFileSource::mapFileHandle()
{
  PLBYTE * pBuffer = NULL;
  try 
  {
    m_hm = CreateFileMapping (m_hf, NULL, PAGE_READONLY, 0, 0, NULL);

    // This happens if the file is empty.
    if (m_hm == NULL)
    {  // raiseError (PL_ERRACCESS_DENIED, "CreateFileMapping failed.");
      if (m_hf) CloseHandle (m_hf);
      m_hf = NULL;
      return PL_ERRACCESS_DENIED;
    }

    pBuffer = (PLBYTE *) MapViewOfFile (m_hm, FILE_MAP_READ, 0, 0, 0);

    if (pBuffer == NULL)
      // raiseError (PL_ERRACCESS_DENIED, "MapViewOfFile failed.");
      return PL_ERRACCESS_DENIED;
    m_pStartData = pBuffer;
    m_pCurPos = pBuffer;
  }
  catch (PLTextException)
  {
    if (pBuffer) UnmapViewOfFile (pBuffer);
    if (m_hm) CloseHandle (m_hm);
    throw;
  }
  return 0;
}


int PLFileSource::getLastPLError() 
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
|      $Log: plfilesrc.cpp,v $
|      Revision 1.17  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.16  2004/09/15 15:26:21  uzadow
|      Linux compatibility changes, doc update.
|
|      Revision 1.15  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.14  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.13  2004/08/20 14:39:37  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.12  2004/04/15 16:39:30  uzadow
|      Initial Mac OS X port
|
|      Revision 1.11  2004/03/04 18:05:55  uzadow
|      Fixed linux file not found exception bug.
|
|      Revision 1.10  2004/02/15 22:33:34  uzadow
|      Fixed strange close() bug.
|
|      Revision 1.9  2003/11/22 13:11:34  uzadow
|      Added memory-mapped file support for linux
|
|      Revision 1.8  2003/08/03 12:54:10  uzadow
|      Fixed broken linux build.
|
|      Revision 1.7  2003/08/03 12:36:56  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.6  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.5  2003/04/20 22:37:54  uzadow
|      Fixed bug in non-memory mapped file data source.
|
|      Revision 1.4  2003/04/20 22:20:47  uzadow
|      Added check for errors reading memory-mapped files.
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
|      Revision 1.10  2001/09/13 20:46:45  uzadow
|      Removed 4096-byte limit for fillBuffer that was causing PLPNGEncoder
|      to fail under Linux.
|
|      Revision 1.9  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.8  2001/01/21 14:28:21  uzadow
|      Changed array cleanup from delete to delete[].
|
|      Revision 1.7  2000/09/01 13:27:07  Administrator
|      Minor bugfixes
|
|      Revision 1.6  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.4  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
