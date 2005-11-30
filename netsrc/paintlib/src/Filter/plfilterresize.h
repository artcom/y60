/*
/--------------------------------------------------------------------
|
|      $Id: plfilterresize.h,v 1.2 2002/03/31 13:36:42 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTERResize_H
#define FILTERResize_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

//! Base class for filters that resize the image.
class PLFilterResize : public PLFilter  
{
public:
  //! 
	PLFilterResize(int NewXSize, int NewYSize);
  //! 
	virtual ~PLFilterResize();
  //! 
  void SetNewSize(int NewXSize, int NewYSize);

protected:
  int m_NewXSize;
  int m_NewYSize;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterresize.h,v $
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  1999/11/27 18:45:49  Ulrich von Zadow
|      Added/Updated doc comments.
|
|      Revision 1.1  1999/10/21 16:05:18  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:44  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
