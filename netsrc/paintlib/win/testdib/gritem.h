/*
/--------------------------------------------------------------------
|
|      $Id: gritem.h,v 1.5 2002/02/24 13:00:57 uzadow Exp $
|      Graphic item class
|
|      Abstract base class for an item on a canvas. Saves position
|      and size information as well as opacity of the item. The
|      actual draw method is implemented in derived classes.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_GRITEM
#define INCL_GRITEM

class PLBmp;

class CGrItem : public CObject
{

DECLARE_DYNAMIC (CGrItem);

public:
  CGrItem
    ( int x,            // Position on the canvas
      int y,
      int w,            // Width
      int h,            // Height
      int z,            // Position in z-Order
      BYTE Opacity      // Opacity of the object. 255 is completely
                        // opaque, 0 is completely transparent.
    );
    // Create and initialize a GrItem.

  ~CGrItem
    ();

  virtual void Draw
    ( PLBmp * pCanvas,
      CRect * pUpdateRect
    ) = 0;
    // Responsible for drawing the object on the canvas. Implemented
    // in a derived class.


  void GetRect
    ( CRect * pRect
    );
    // Returns dimensions as a rectangle.

  // Member variables.

  int    m_x, m_y;     // Position on the canvas
  int    m_w, m_h;     // Width & Height
  int    m_z;          // Position in z-Order
  BYTE   m_Opacity;    // Opacity of the object. 255 is completely
                       // opaque, 0 is completely transparent.

protected:
  void clip
    ( PLBmp * pDestBmp,
      CRect * pUpdRect
    );
    // Clips pUpdRect to the current item and canvas.

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: gritem.h,v $
|      Revision 1.5  2002/02/24 13:00:57  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.4  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|
--------------------------------------------------------------------
*/
