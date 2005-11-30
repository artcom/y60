/*
/--------------------------------------------------------------------
|
|      $Id: plfilterrotate.h,v 1.5 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERROTATE_H
#define FILTERROTATE_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

//! Rotates a bitmap 90, 180 or 270 degress
class PLFilterRotate : public PLFilter  
{
public:
  enum AngleType {ninety, oneeighty, twoseventy};
  //! 
  PLFilterRotate(AngleType);
  //! 
  virtual ~PLFilterRotate();
  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) 
    const;
private:
	AngleType m_Angle;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterrotate.h,v $
|      Revision 1.5  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.4  2003/02/27 23:01:33  uzadow
|      Linux version of PLFilterRotate, nicer test output
|
|      Revision 1.3  2003/02/27 22:46:57  uzadow
|      Added plfilterrotate.
|
|      Revision 1.1  2002/02/05 09:40:54  david
|      pavel added rotate90 filter
|
\--------------------------------------------------------------------
*/
