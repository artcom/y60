/*
/--------------------------------------------------------------------
|
|      $Id: drawhelp.h,v 1.4 2002/03/31 13:36:42 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#ifndef INCL_DRAWHELP
#define INCL_DRAWHELP

#include "plbitmap.h"

void DrawOnBmp
    ( PLBmp * pDestBmp,
      CRect * pDestRect,
      PLBmp * pSrPLBmp,
      PLBYTE Opacity
    ); 
                
#endif
/*
/--------------------------------------------------------------------
|
|      $Log: drawhelp.h,v $
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
