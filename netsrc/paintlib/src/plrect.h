/*
/--------------------------------------------------------------------
|
|      $Id: plrect.h,v 1.11 2004/06/06 12:56:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLRECT
#define INCL_PLRECT

#include "plpoint.h"

//! Simple rectangle class.
//! Contains all points from tl up to but not including br.
class PLRect
{
public:
  //!
  PLPoint tl;
  //!
  PLPoint br;

  //!
  PLRect
  ();

  //!
  PLRect
  ( int left,
    int top,
    int right,
    int bottom
  );

  //!
  PLRect
  ( const PLPoint& TL,
    const PLPoint& BR
  );

  //!
  bool operator ==
  ( const PLRect & rect
  ) const;

  //!
  bool operator !=
  ( const PLRect & rect
  ) const;

  //!
  int Width 
  () const;

  //!
  int Height
  () const;

  //!
  void SetWidth
  (int width
  );
 
  //!
  void SetHeight
  (int height
  );

  //!
  bool Contains 
  ( const PLPoint& pt
  ) const;

  //!
  bool Contains 
  ( const PLRect& rect
  ) const;

  //!
  bool Intersects
  ( const PLRect& rect
  ) const;

  //!
  void Expand
  ( const PLRect& rect
  );

  //!
  void Intersect
  ( const PLRect& rect
  );
};

inline PLRect::PLRect
()
{}

inline PLRect::PLRect
  ( const PLPoint& TL,
    const PLPoint& BR
  ): tl(TL), br(BR)
{}

inline PLRect::PLRect
  ( int left,
    int top,
    int right,
    int bottom
  ) : tl(left, top), 
      br (right, bottom)
{}

inline bool PLRect::operator ==
( const PLRect & rect
) const
{
  return (tl == rect.tl && br == rect.br);
}

inline bool PLRect::operator !=
( const PLRect & rect
) const
{
  return !(rect==*this);
}

inline int PLRect::Width 
() const
{
  return br.x-tl.x;
}

inline int PLRect::Height
() const
{
  return br.y-tl.y;
}

inline void PLRect::SetWidth
(int width
)
{
    br.x = tl.x+width;
}
 
inline void PLRect::SetHeight
(int height
)
{
    br.y = tl.y+height;
}

inline bool PLRect::Contains 
( const PLPoint& pt
) const
{
    return (pt.x >= tl.x && pt.x < br.x &&
        pt.y >= tl.y && pt.y < br.y);
}

inline bool PLRect::Contains 
( const PLRect& rect
) const
{
    PLPoint brpt (rect.br.x-1, rect.br.y-1);
    return Contains(rect.tl) && Contains(brpt);
}

inline bool PLRect::Intersects
  ( const PLRect& rect
  ) const
{   
    if (rect.br.x <= tl.x || rect.tl.x >= br.x ||
        rect.br.y <= tl.y || rect.tl.y >= br.y)
      return false;
    else
      return true;
}

#ifndef  min
#define min(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef  max
#define max(a, b)       ((a) < (b) ? (b) : (a))
#endif

inline void PLRect::Expand
  ( const PLRect& rect
  )
{
    tl.x = min(tl.x, rect.tl.x);
    tl.y = min(tl.y, rect.tl.y);
    br.x = max(br.x, rect.br.x);
    br.y = max(br.y, rect.br.y);
}

inline void PLRect::Intersect
  ( const PLRect& rect
  )
{
    tl.x = max(tl.x, rect.tl.x);
    tl.y = max(tl.y, rect.tl.y);
    br.x = min(br.x, rect.br.x);
    br.y = min(br.y, rect.br.y);
}

#undef min
#undef max

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plrect.h,v $
|      Revision 1.11  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.10  2004/02/21 18:37:08  uzadow
|      Fixed rect.Intersects() again.
|
|      Revision 1.8  2004/02/21 16:48:24  uzadow
|      Fixed rect.Intersects()
|
|      Revision 1.7  2003/10/15 21:24:04  uzadow
|      *** empty log message ***
|
|      Revision 1.6  2003/08/17 18:04:38  uzadow
|      Added PLRect::Intersect()
|
|      Revision 1.5  2003/08/17 15:04:30  uzadow
|      Added PLRect::Intersects() and Expand()
|
|      Revision 1.4  2003/03/19 14:33:14  uzadow
|      Added Rect.Contains
|
|      Revision 1.3  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.1  2001/09/24 14:24:52  uzadow
|      Added PLRect.
|
|      Revision 1.4  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2000/11/21 20:20:36  uzadow
|      Changed bool to bool.
|
|      Revision 1.2  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.1  1999/12/09 16:35:58  Ulrich von Zadow
|      Added PLRect.
|
|
\--------------------------------------------------------------------
*/
