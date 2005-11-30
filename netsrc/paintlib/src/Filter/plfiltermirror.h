/*
/--------------------------------------------------------------------
|
|      $Id: plfiltermirror.h,v 1.3 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|      Original code by Richard Hollis
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLFILTERMIRROR
#define INCL_PLFILTERMIRROR

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

//! Creates a mirror image of a bitmap.
class PLFilterMirror: public PLFilter  
{
public:
  //! 
	PLFilterMirror();
  //! 
	virtual ~PLFilterMirror();
  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

protected:
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfiltermirror.h,v $
|      Revision 1.3  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.2  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.1  2001/01/13 20:06:16  uzadow
|      Added Flip and Mirror filters.
|
|
|
\--------------------------------------------------------------------
*/
