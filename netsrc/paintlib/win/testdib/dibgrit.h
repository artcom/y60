/*
/--------------------------------------------------------------------
|
|      $Id: dibgrit.h,v 1.6 2002/02/24 13:00:57 uzadow Exp $
|      Bitmap Graphic item class
|
|      A bitmap on a canvas.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_DIBGRIT
#define INCL_DIBGRIT

#include "gritem.h"

class PLBmp;

class CDIBGrItem : public CGrItem
{

DECLARE_DYNAMIC (CDIBGrItem);

public:
  CDIBGrItem
    ( int x,            // Position on the canvas
      int y,
      int w,            // Width
      int h,            // Height
      int z,            // Position in z-Order
      BYTE Opacity,     // Opacity of the object. 255 is completely
                        // opaque, 0 is completely transparent.
      PLBmp * pBmp      // The bitmap.
    );

  ~CDIBGrItem
    ();

  virtual void Draw
    ( PLBmp * pCanvas,
      CRect * pUpdateRect
    );
    // Responsible for drawing the object on the canvas.

  PLBmp * GetBmp
    ()
  {
    return m_pBmp;
  }

private:
  void drawClippedNoScale
    ( PLBmp * pCanvas,
      CRect * pRect
    );
    // Draws the object. pRect must have been clipped already.
    // Assumes that no scaling is nessesary.

  void drawAlphaLine
    ( BYTE * pDest,
      BYTE * pSrc,
      CRect * pRect
    );

  void drawFadeLine
    ( BYTE * pDest,
      BYTE * pSrc,
      CRect * pRect
    );

  void drawClipped
    ( PLBmp * pCanvas,
      CRect * pRect
    );

  void drawClippedNoAlpha
    ( PLBmp * pCanvas,
      CRect * pRect
    );

  // Member variables.

  PLBmp * m_pBmp;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: dibgrit.h,v $
|      Revision 1.6  2002/02/24 13:00:57  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.5  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2000/08/13 12:11:44  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.3  2000/01/16 20:43:19  anonymous
|      Removed MFC dependencies
|
|
\--------------------------------------------------------------------
*/
