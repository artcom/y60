/*
/--------------------------------------------------------------------
|
|      $Id: pltiffencex.cpp,v 1.4 2004/09/11 12:41:35 uzadow Exp $
|      TIFF Encoder Class
|
|      TIFF file encoder. Uses LIBTIFF to do the actual conversion.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

// Not quite ready for prime-time; bdelmee; 2/99
// I mostly needed monochrome TIFF writing, which seems robust enough.
// Palette or RGB writing seems to work but has not been sufficiently tested.

#include <stdarg.h>
#include "pltiffenc.h"
#include "pltiffencex.h"
#include "plbitmap.h"
#include "plexcept.h"

extern "C"
{
#include "tiffio.h"   // for the tags definitions
#include "tif_msrc.h"
}


/////////////////////////////////////////////////////////////////////
// more format-specific encoder


PLTIFFEncoderEx::PLTIFFEncoderEx() 
  : PLTIFFEncoder(),
    m_TiffToken(0)
{}



PLTIFFEncoderEx::~PLTIFFEncoderEx()
{
  Dissociate();
}


bool PLTIFFEncoderEx::Associate( PLDataSink* pDataSnk )
{
  m_TiffToken = TIFFOpenMem (pDataSnk->m_pStartData,
                             pDataSnk->m_nMaxFileSize,
                             &(pDataSnk->m_nCurPos));
  return m_TiffToken != 0;
}


void PLTIFFEncoderEx::Dissociate()
{
  if ( m_TiffToken )
  {
    TIFFClose( m_TiffToken );
    m_TiffToken = 0;
  }
}


void PLTIFFEncoderEx::DoEncode (PLBmpBase * pBmp, PLDataSink* /* pDataSnk */)
{
  PLASSERT( m_TiffToken );
  // call base version on open tiff descriptor
  PLTIFFEncoder::DoTiffEncode( pBmp, m_TiffToken );
}


// The following two calls make their base class equivalent usable,
// without requiring the user to know about the libtiff internals (TIFF*)
int PLTIFFEncoderEx::SetBaseTags(PLBmpBase* pBmp )
{
  return PLTIFFEncoder::SetBaseTags( m_TiffToken, pBmp );
}


int PLTIFFEncoderEx::SetField( int tag_id, ... )
{
  int retv;
  va_list marker;

  va_start( marker, tag_id );     /* Initialize variable arguments. */
  retv = TIFFVSetField( m_TiffToken, tag_id, marker );
  va_end( marker );              /* Reset variable arguments.      */

  return retv;
}
/*
/--------------------------------------------------------------------
|
|      $Log: pltiffencex.cpp,v $
|      Revision 1.4  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.3  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.4  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  2000/01/10 23:53:00  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.2  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.1  1999/10/19 21:30:42  Ulrich von Zadow
|      B. Delmee - Initial revision
|
|
\--------------------------------------------------------------------
*/
