/*
|
|      $Id: plpicenc.cpp,v 1.10 2004/11/09 15:55:06 artcom Exp $
|      Generic Picture Encoder Class
|
|      Abstract base class to dump picture data to memory or file.
|      Classes derived from this class implement concrete encoders
|      for specific file formats.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plpicenc.h"
#include "plfilesink.h"
#include "plexcept.h"
// only for the tracing facility. This really shouldn't be here.
#include "plpicdec.h"

#include <errno.h>

PLPicEncoder::PLPicEncoder()
  : PLObject()
// Creates an encoder
{}


PLPicEncoder::~PLPicEncoder()
{}


// Encodes a picture by creating a file data sink and
// calling SaveBmp with this data sink.

void PLPicEncoder::MakeFileFromBmp (const char * pszFName, PLBmpBase * pBmp)
{
  PLFileSink FileSink;
  int err;

  char sz[1024];
  sprintf (sz, "--- Encoding file %s. ---\n", pszFName);
  Trace (1, sz);

  // We allocate a buffer large enough to hold the raw bitmap
  // plus some overhead.In most cases this should be enough to
  // hold the uncompressed data in any format, plus headers, etc...
  // Some "pathological" cases however may end up with a CODEC
  // producing more data than the uncompressed version!
  int bufsize = pBmp->GetWidth()*pBmp->GetHeight()*(pBmp->GetBitsPerPixel()/8) + 65536;
  /* This worked for years, but it's probably not enough for exif.
  int bufsize = pBmp->GetMemUsed();
  bufsize = bufsize < 20000 ? bufsize + 4096 : int(1.2 * bufsize);
  */
  err = FileSink.Open( pszFName, bufsize );
  if (err)
  {
    sprintf (sz, "Opening %s failed: %s", pszFName, strerror(errno));
    raiseError (err, sz);
  }

  SaveBmp ( pBmp, &FileSink );
  FileSink.Close ();
}
#ifdef _WIN32
void PLPicEncoder::MakeFileFromBmpW (const wchar_t * pszwFName, PLBmpBase * pBmp)
{
  PLFileSink FileSink;
  int err;

  char sz[1024];
  char sz2[900];

  // First calculate how much room the result will take up
  int neededSize = ::WideCharToMultiByte(CP_ACP,
        0,
              pszwFName,
              -1,
              NULL,
              0,
              NULL,
              NULL);
  if (neededSize < 900){  // Prevent overflow of our buffer
    ::WideCharToMultiByte(CP_ACP,
        0,
              pszwFName,
              -1,
              sz2,
              900,
              NULL,
              NULL);
  }
  else{
      strcpy(sz2,"{{Filename too long}}");
  }
  sprintf (sz, "--- Encoding file %s. ---\n", sz2);
  Trace (1, sz);

  // We allocate a buffer large enough to hold the raw bitmap
  // plus some overhead.In most cases this should be enough to
  // hold the uncompressed data in any format, plus headers, etc...
  // Some "pathological" cases however may end up with a CODEC
  // producing more data than the uncompressed version!
  int bufsize = pBmp->GetWidth()*pBmp->GetHeight()*(pBmp->GetBitsPerPixel()/8) + 65536;
  err = FileSink.OpenW( pszwFName, bufsize );
  if (err)
  {
    sprintf (sz, "Opening %s failed: %s", sz2, strerror(errno));
    raiseError (err, sz);
  }

  SaveBmp ( pBmp, &FileSink );
  FileSink.Close ();
}
#endif


// Encodes a picture by getting the encoded data from pDataSrc.
// Saves the results to pDataSrc
// Actually, a wrapper so thin around "DoEncode", you could see through...
void PLPicEncoder::SaveBmp (PLBmpBase* pBmp, PLDataSink* pDataSnk)
{
  DoEncode( pBmp, pDataSnk );
}

////////////////////
// As long as the tracing code lives in the base decoder,
// we'll just forward everything to it without bothering the user

void PLPicEncoder::SetTraceConfig( int Level, char * pszFName )
{
  PLPicDecoder::SetTraceConfig( Level, pszFName );
}

void PLPicEncoder::raiseError( int Code, char * pszErr )
{
  PLPicDecoder::raiseError( Code, pszErr );
}

void PLPicEncoder::Trace( int TraceLevel, const char * pszMessage )
{
  PLPicDecoder::Trace( TraceLevel, pszMessage );
}
/*
/--------------------------------------------------------------------
|
|      $Log: plpicenc.cpp,v $
|      Revision 1.10  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.9  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.8  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.7  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.6  2003/04/19 19:03:52  uzadow
|      Exif save (windows)
|
|      Revision 1.5  2002/08/05 20:43:12  uzadow
|      cygwin compatibility changes
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/02/24 13:00:24  uzadow
|      Documentation update; removed buggy PLFilterRotate.
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
|      Revision 1.7  2001/02/04 14:07:24  uzadow
|      Changed max. filename length.
|
|      Revision 1.6  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.4  2000/01/08 15:51:30  Ulrich von Zadow
|      Misc. modifications to png encoder.
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
