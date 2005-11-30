/*
/--------------------------------------------------------------------
|
|      $Id: plpicenc.h,v 1.7 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPICENC
#define INCL_PLPICENC

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

#ifndef INCL_PLDATASNK
#include "pldatasink.h"
#endif

#ifndef AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_
#include "plobject.h"
#endif

// the details of the bitmap class are not needed here
class PLBmp;

//! Base class for image encoders.
//! PLPicEncoder is an abstract base class. It defines common routines
//! for all encoders. Encoders for specific file formats can be
//! derived from this class. Objects of this class interact with a
//! PLDataSink to encode bitmaps.
class PLPicEncoder : public PLObject
{

public:
  //! Empty constructor. The actual initialization takes place in a
  //! derived class.
  PLPicEncoder
    ();

  //! Destructor. Frees memory allocated.
  virtual ~PLPicEncoder
    ();

  //! Encodes a bitmap to a file and stores.
  virtual void MakeFileFromBmp(
    const char *,
    PLBmpBase*
    );

#ifdef _WIN32
  virtual void MakeFileFromBmpW(
    const wchar_t *,
    PLBmpBase*
    );
#endif

  //! Encodes a picture to a pre-existing data destination (sink).
  virtual void SaveBmp( PLBmpBase*, PLDataSink* );

  //!
  void SetTraceConfig( int Level, char * pszFName );

  // This function is needed by callbacks outside of any object,
  // so it's public and static. It should not be called from
  // outside of the library. 
  static void raiseError( int Code, char * pszErr );

  //!
  void Trace( int TraceLevel, const char * pszMessage );

protected:
  //! Implements the actual encoding process. Uses variables local to the
  //! object to retrieve and store the data. Implemented in derived classes.
  //!
  virtual void DoEncode
    ( PLBmpBase* pBmp,
      PLDataSink* pDataSrc
      ) = 0;
};
#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plpicenc.h,v $
|      Revision 1.7  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.6  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.5  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.4  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.3  2002/02/24 13:00:26  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.7  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/08 15:51:30  Ulrich von Zadow
|      Misc. modifications to png encoder.
|
|      Revision 1.5  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.4  1999/10/19 21:28:05  Ulrich von Zadow
|      Added jpeg encoder
|
|      Revision 1.3  1999/10/03 18:50:50  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
\--------------------------------------------------------------------
*/
