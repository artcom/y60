/*
/--------------------------------------------------------------------
|
|      $Id: plfilterlightness.h,v 1.3 2004/06/15 10:26:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERLIGHTNESS)
#define INCL_PLFILTERLIGHTNESS

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "plfilter.h"

class PLBmp;

//! Filter that changes the lightness of a 32 bpp bitmap. lightness
//! values must be in the range -100..100. A value of 0 leaves the 
//! image unchanged.
class PLFilterLightness : public PLFilter  
{
public:
  //!
	PLFilterLightness (int lightness);
  
  //!
	virtual ~PLFilterLightness();

  //!
  virtual void Apply(PLBmpBase *pBmpSource, PLBmp* pBmpDest) const;

protected:
    int m_lightness;
};
#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterlightness.h,v $
|      Revision 1.3  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.2  2002/02/24 13:00:47  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.4  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.3  2001/01/12 23:29:38  uzadow
|      Lightness 0 now means "don't change anything".
|
|      Revision 1.2  2000/12/04 23:57:13  uzadow
|      Cleaned up code.
|
|
|
\--------------------------------------------------------------------
*/
