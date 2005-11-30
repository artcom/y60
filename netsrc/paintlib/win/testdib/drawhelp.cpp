/*
/--------------------------------------------------------------------
|
|      $Id: drawhelp.cpp,v 1.4 2002/03/31 13:36:42 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#include "stdafx.h"

#include "drawhelp.h"
#include "dibgrit.h"


void DrawOnBmp
    ( PLBmp * pDestBmp,
      CRect * pDestRect,
      PLBmp * pSrPLBmp,
      PLBYTE Opacity
    )
{
  CDIBGrItem DIBItem (pDestRect->left, pDestRect->top,
                      pDestRect->Width(), pDestRect->Height(),
                      0, Opacity, pSrPLBmp);

  DIBItem.Draw (pDestBmp, pDestRect);
}
/*
/--------------------------------------------------------------------
|
|      $Log: drawhelp.cpp,v $
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
