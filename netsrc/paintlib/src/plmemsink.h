/*
/--------------------------------------------------------------------
|
|      $Id: plmemsink.h,v 1.6 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLMEMSINK
#define INCL_PLMEMSINK

#include "pldatasink.h"
#include "plpaintlibdefs.h"

//! This is a class which takes a memory region as a destination of
//! picture data. The result is an encoded (for example, jpeg-compressed) 
//! image in memory.
class PLMemSink : public PLDataSink
{

public:
  //!
  PLMemSink
    ();

  //!
  virtual ~PLMemSink
    ();

  //!
  virtual int Open
    ( const char * pszFName,
      int MaxFileSize
    );
#ifdef _WIN32
  //!
  virtual int OpenW
    ( const wchar_t * pszwFName,
      int MaxFileSize
    );
#endif

  //!
  virtual void Close
    ();

  //! Returns a pointer to the compressed image.
  virtual PLBYTE* GetBytes();

private:
  PLBYTE * m_pDataBuf;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plmemsink.h,v $
|      Revision 1.6  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.5  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.4  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.3  2002/02/24 13:00:22  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.1  2000/10/12 21:57:26  uzadow
|      no message
|
|
|
\--------------------------------------------------------------------
*/
