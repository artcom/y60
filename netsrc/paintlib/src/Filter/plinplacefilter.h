/*
/--------------------------------------------------------------------
|
|      $Id: plinplacefilter.h,v 1.1 2004/06/20 16:59:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if !defined(INCL_PLINPLACEFILTER)
#define INCL_PLINPLACEFILTER

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class PLBmpBase;
class PLBmp;

#include "plfilter.h"

//! Base class for filters that can operate in-place on PLBmpBase objects.
class PLInPlaceFilter : public PLFilter  
{
public:
  //!
  PLInPlaceFilter();
  //!
  virtual ~PLInPlaceFilter() = 0;

  //! In-Place Apply. Applies the filter to pBmp. 
  virtual void ApplyInPlace(PLBmpBase* pBmp) const = 0;  

  //! In-Place Apply. Simply calls the version for PLBmpBase.
  virtual void ApplyInPlace(PLBmp* pBmp) const;  
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plinplacefilter.h,v $
|      Revision 1.1  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|
|
\--------------------------------------------------------------------
*/
