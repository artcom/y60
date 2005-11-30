/*
/--------------------------------------------------------------------
|
|      $Id: pldecoderfactory.h,v 1.2 2004/09/28 16:14:04 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLDECODERFACTORY
#define INCL_PLDECODERFACTORY

#include "plpaintlibdefs.h"

class PLPicDecoder;

class PLDecoderFactory
{
public:
  //!
  virtual ~PLDecoderFactory
      ()
  {}

  //!
  virtual PLPicDecoder * CreateDecoder() const = 0;

  //!
  virtual bool CanDecode(PLBYTE * pData, int DataLen) const = 0;

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pldecoderfactory.h,v $
|      Revision 1.2  2004/09/28 16:14:04  artcom
|      added some missing forward decls
|
|      Revision 1.1  2004/09/15 21:10:46  uzadow
|      Added decoder plugin interface to PLAnyPicDecoder.
|
|
|
\--------------------------------------------------------------------
*/
