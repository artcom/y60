/*
/--------------------------------------------------------------------
|
|      $Id: shadow.h,v 1.5 2002/03/31 13:36:42 uzadow Exp $
|      Shadow Graphic item class
|
|      A shadowed region on a canvas. The item is basically a
|      partially transparent region. Transparency is defined by an
|      alpha DIB and a global transparency value. Most of the code
|      was just copied from CDIBGrItem & changed a bit.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_SHADOW
#define INCL_SHADOW

#include "plbitmap.h"
#include "gritem.h"

class CShadowItem : public CGrItem
{

DECLARE_DYNAMIC (CShadowItem);

public:
  CShadowItem
    ( int x,             // Position on the canvas
      int y,
      int w,             // Width
      int h,             // Height
      int z,             // Position in z-Order
      PLBYTE Opacity,      // Opacity of the object. 255 is completely
                         // opaque, 0 is completely transparent.
      PLPixel32 * pColor, // Shadow color.
      PLBmp * pAlphaBmp   // Alpha channel (must be 8 bpp). Can be NULL.
    );

  ~CShadowItem
    ();

  virtual void Draw
    ( PLBmp * pCanvas,
      CRect * pUpdateRect
    );
    // Responsible for drawing the object on the canvas.

  // Member variables.

  PLPixel32  m_Color;        // Shadow color
  PLBmp    * m_pAlphaBmp;
  PLBYTE   ** m_pLineArray;

private:

  void drawClippedNoScale
    ( PLBmp * pCanvas,
      CRect * pRect
    );
    // Draws the object. pRect must have been clipped already.
    // Assumes that no scaling is nessesary.

  void drawAlphaLine
    ( PLBYTE * pDest,
      PLBYTE * pAlpha,
      CRect * pRect
    );

  void drawFadeLine
    ( PLBYTE * pDest,
      CRect * pRect
    );
    // Draws one line. No scaling. Assumes alpha channel doesn't
    // exist.

  void drawClipped
    ( PLBmp * pCanvas,
      CRect * pRect
    );
    // Draws the object. pRect must have been clipped already.

  void drawScaleLine
    ( PLBYTE * pDest,
      PLBYTE * pAlpha,
      CRect * pRect
    );

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: shadow.h,v $
|      Revision 1.5  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.4  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
