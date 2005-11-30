/*
/--------------------------------------------------------------------
|
|      $Id: plfilesink.h,v 1.8 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/
// not quite ready for prime-time; bdelmee; 2/99

#ifndef INCL_PLFILESNK
#define INCL_PLFILESNK

#ifndef INCL_PLDATASNK
#include "pldatasink.h"
#endif

#ifdef _WIN32
#define PL_FILE_MAPPING
#endif

#ifdef _WIN32
  typedef void *HANDLE;
#endif

#include <stdio.h>

//! This is a data sink class which takes a file as a destination of picture data.
//!
//! If PL_FILE_MAPPING is defined (this is the case under windows), the data is
//! written directly to the mapped file. If not, the class allocates a buffer 
//! large enough to hold a complete
//! image file. But we don't know its size before it's actually encoded, so
//! we have to be generous or "accidents will happen"
//! TODO: Use mapped files for the *nix version as well.
class PLFileSink : public PLDataSink
{

public:
  //! Create an uninitialized file sink.
  PLFileSink
    ();

  //!
  virtual ~PLFileSink
    ();

  //! Open a file sink. Allocates MaxFileSize bytes.
  virtual int Open
    ( const char * pszFName,
      int MaxFileSize
    );
#ifdef _WIN32
    //! Same as open, just for windows wide-char types.
  virtual int OpenW
    ( const wchar_t * pszwFName,
      int MaxFileSize
    );
#endif

  //! Flushes the data and closes the file.
  virtual void Close
    ();

private:
#ifdef _WIN32
  int getLastPLError();
#endif  

#ifdef PL_FILE_MAPPING
  HANDLE m_hf;    // File handle.
  HANDLE m_hm;    // Handle to file-mapping object.
#else
  FILE * m_pFile;
#endif
  PLBYTE * m_pDataBuf;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilesink.h,v $
|      Revision 1.8  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.7  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.6  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.5  2003/08/03 12:36:56  uzadow
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
|      Revision 1.5  2001/09/15 21:02:44  uzadow
|      Cleaned up stdpch.h and config.h to make them internal headers.
|
|      Revision 1.4  2000/01/16 20:43:13  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  2000/01/08 15:56:12  Ulrich von Zadow
|      Made sure change logging works in every file.
|
|
\--------------------------------------------------------------------
*/
