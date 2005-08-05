/*
/--------------------------------------------------------------------
|
|      $Id: plfiltersubtract.h,v 1.2 2004/08/03 14:44:36 martin Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERSUBTRACT_H
#define FILTERSUBTRACT_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <paintlib/plbitmap.h>
#include <paintlib/Filter/plfilter.h>

//! Subtracts one bitmap from another 
class PLFilterSubtract : public PLFilter  
{
public:
  //! 
  PLFilterSubtract(PLBmpBase * pBmpSubtrahend);
  //! 
  virtual ~PLFilterSubtract();
  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) 
    const;
private:
	PLBmpBase * m_pBmpSubtrahend;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltersubtract.h,v $
|      Revision 1.2  2004/08/03 14:44:36  martin
|      PL include bug workaround
|
|      Revision 1.1  2004/08/03 14:30:09  martin
|      better image compare algorithem
|
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
