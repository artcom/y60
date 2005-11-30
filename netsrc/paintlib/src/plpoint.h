/*
/--------------------------------------------------------------------
|
|      $Id: plpoint.h,v 1.10 2004/10/02 22:23:12 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPOINT
#define INCL_PLPOINT

//! Simple point class for 2d integer coordinate manipulations. This class
//! contains only inline functions and nothing virtual. Its member variables
//! are public.
class PLPoint
{
public:
  //!
  int x;
  //! 
  int y;

  //!
  PLPoint
  ();

  //!
  PLPoint
  ( int X,
    int Y
  );

  //!
  bool operator ==
  ( const PLPoint & pt
  ) const;

  //!
  bool operator !=
  ( const PLPoint & pt
  ) const;

  //!
  void operator +=
  ( const PLPoint & pt
  );

  //!
  void operator -=
  ( const PLPoint & pt
  );

  //!
  PLPoint operator -
  () const;

  //!
  PLPoint operator +
  ( const PLPoint & pt
  ) const;

  //!
  PLPoint operator -
  ( const PLPoint & pt
  ) const;

  //!
  PLPoint operator /
  ( double f
  ) const;
  
  //!
  PLPoint operator *
  ( double f
  ) const;
};

inline PLPoint::PLPoint
()
{}


inline PLPoint::PLPoint
( int X,
  int Y
)
{
  x = X;
  y = Y;
}

inline bool PLPoint::operator ==
( const PLPoint & pt
) const
{
  return (x == pt.x && y == pt.y);
}

inline bool PLPoint::operator !=
( const PLPoint & pt
) const
{
  return (x != pt.x || y != pt.y);
}

inline void PLPoint::operator +=
( const PLPoint & pt
)
{
  x += pt.x;
  y += pt.y;
}

inline void PLPoint::operator -=
( const PLPoint & pt
)
{
  x -= pt.x;
  y -= pt.y;
}

inline PLPoint PLPoint::operator -
() const
{
  return PLPoint(-x, -y);
}

inline PLPoint PLPoint::operator +
( const PLPoint & pt
) const
{
  return PLPoint(x + pt.x, y + pt.y);
}

inline PLPoint PLPoint::operator -
( const PLPoint & pt
) const
{
  return PLPoint(x - pt.x, y - pt.y);
}

inline PLPoint PLPoint::operator /
  ( double f
  ) const
{
  return PLPoint (int(x/f), int(y/f));
}

inline PLPoint PLPoint::operator *
  ( double f
  ) const
{
  return PLPoint (int(x*f), int(y*f));
}

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpoint.h,v $
|      Revision 1.10  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.9  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.8  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.7  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.6  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.5  2001/09/24 14:18:42  uzadow
|      Added operator -, improved const-correctness.
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
|      Added PLPoint.
|
|
\--------------------------------------------------------------------
*/
