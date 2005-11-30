/*
/--------------------------------------------------------------------
|
|      $Id: testfilterresizebilinear.h,v 1.6 2005/03/08 14:22:46 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTFILTERRESIZEBILINEAR
#define INCL_PLTESTFILTERRESIZEBILINEAR

#include "pltest.h"

class PLBmp;

class PLTestFilterResizeBilinear: public PLTest
{

public:
  PLTestFilterResizeBilinear ();

  virtual ~PLTestFilterResizeBilinear();

  virtual void RunTests ();

private:
  void createBmp(PLBmp& Bmp);
  void createMonoBmp(PLBmp& Bmp);
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: testfilterresizebilinear.h,v $
|      Revision 1.6  2005/03/08 14:22:46  artcom
|      Added resizing of Luminace Bitmaps
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/21 17:08:41  uzadow
|      no message
|
|      Revision 1.2  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|
\--------------------------------------------------------------------
*/
