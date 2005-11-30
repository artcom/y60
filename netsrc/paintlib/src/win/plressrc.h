/*
/--------------------------------------------------------------------
|
|      $Id: plressrc.h,v 1.7 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_RESSRC
#define INCL_RESSRC

#ifndef INCL_PLDATASRC
#include "pldatasrc.h"
#endif
#include "plpaintlibdefs.h"

#ifdef _WIN32
  // TODO: This is really just to define HANDLE. 
  #define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
  #define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
  #include <windows.h>
#endif

//! This is a class which takes a windows resource as a source of
//! picture data.
class PLResourceSource : public PLDataSource
{
public:
  //!
  PLResourceSource
    ();

  //!
  virtual ~PLResourceSource
    ();

  //!
  virtual int Open
    ( HINSTANCE lh_ResInst, int ResourceID,
      const char * pResType = NULL
    );

  //!
  virtual void Close
    ();

  virtual PLBYTE * ReadNBytes
    ( int n
    );

  //! Read but don't advance file pointer.
  virtual PLBYTE * GetBufferPtr
    ( int MinBytesInBuffer
    );

  //! This is a legacy routine that interferes with progress notifications.
  //! Don't call it!
  virtual PLBYTE * ReadEverything
    ();

  //!
  virtual void Seek
    ( int n
    );
    
private:
  HRSRC   m_hRsrc;
  HGLOBAL m_hGlobal;
  PLBYTE * m_pCurPos;
  PLBYTE * m_pStartPos;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plressrc.h,v $
|      Revision 1.7  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.6  2004/09/11 12:41:37  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2004/08/20 19:54:45  uzadow
|      no message
|
|      Revision 1.4  2003/08/03 12:03:22  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.3  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  2000/01/11 22:07:11  Ulrich von Zadow
|      Added instance handle parameter.
|
|      Revision 1.3  1999/11/02 21:20:14  Ulrich von Zadow
|      AfxFindResourceHandle statt AfxGetInstanceHandle
|      verwendet.
|
|
\--------------------------------------------------------------------
*/
