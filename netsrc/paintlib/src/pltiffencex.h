/*
/--------------------------------------------------------------------
|
|      $Id: pltiffencex.h,v 1.4 2004/06/19 18:16:33 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTIFFENCEX
#define INCL_PLTIFFENCEX

#include "pltiffenc.h"

//! Expanded tiff encoder class.
//! Basic tiff output is available via "PLTIFFEncoder::MakeFileFromBmp()".
//! This does not compress data or allow any informative tag to be set.
//! For those who need greater control, PLTIFFDecoderEx is provided.
class PLTIFFEncoderEx : public PLTIFFEncoder
{

public:
  //! Creates an encoder
  PLTIFFEncoderEx();

  //! Destroys an encoder
  virtual ~PLTIFFEncoderEx();

  //! Bind DataSink to Encoder (the link is the TIFF* member)
  bool Associate( PLDataSink* );

  //! Flush the output
  void Dissociate( void );

  //! SetBaseTabs and SetField make their base class equivalent usable,
  //! without requiring the user to know about the libtiff internals (TIFF*)
  int SetBaseTags( PLBmpBase* );

  //! SetBaseTabs and SetField make their base class equivalent usable,
  //! without requiring the user to know about the libtiff internals (TIFF*)
  int SetField( int tag_id, ... );

protected:
  //! Sets up LIBTIFF environment and calls LIBTIFF to encode an image.
  virtual void DoEncode( PLBmpBase*, PLDataSink* = 0 );

private:
  TIFF* m_TiffToken;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltiffencex.h,v $
|      Revision 1.4  2004/06/19 18:16:33  uzadow
|      Documentation update
|
|      Revision 1.3  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.2  1999/11/27 18:45:48  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/19 21:30:42  Ulrich von Zadow
|      B. Delmee - Initial revision
|
|
--------------------------------------------------------------------
*/
