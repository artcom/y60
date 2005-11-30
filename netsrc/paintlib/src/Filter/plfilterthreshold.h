/*
/--------------------------------------------------------------------
|
|      $Id: plfilterthreshold.h,v 1.2 2004/06/15 10:26:13 uzadow Exp $
|   
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLFILTERTHRESHOLD)
#define INCL_PLFILTERTHRESHOLD

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "plfilter.h"

class PLBmp;

//! Returns an 8 bpp grayscale bitmap that contains only the channel
//! selected and is black wherever this channel has values <= threshold_min
//! or >= threshold_max.
class PLFilterThreshold : public PLFilter
{
public:
  //! 
	PLFilterThreshold(int threshold_min,int threshold_max, int channel);
  //! 
	virtual ~PLFilterThreshold();

  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const;

protected:
	int m_threshold_min;
	int m_threshold_max;
	int m_channel;


};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterthreshold.h,v $
|      Revision 1.2  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.1  2000/09/26 14:28:47  Administrator
|      Added Threshold filter
|
|
|
\--------------------------------------------------------------------
*/
