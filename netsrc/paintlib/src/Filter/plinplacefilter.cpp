/*
/--------------------------------------------------------------------
|
|      $Id: plinplacefilter.cpp,v 1.2 2004/09/11 12:41:36 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plinplacefilter.h"
#include "plbmpbase.h"
#include "plbitmap.h"
#include "planybmp.h"

PLInPlaceFilter::PLInPlaceFilter() : PLFilter()
{

}

PLInPlaceFilter::~PLInPlaceFilter()
{

}

void PLInPlaceFilter::ApplyInPlace(PLBmp * pBmp) const
{
  ApplyInPlace((PLBmpBase*)pBmp);
}

/*
/--------------------------------------------------------------------
|
|      $Log: plinplacefilter.cpp,v $
|      Revision 1.2  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.1  2004/06/20 16:59:38  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|
\--------------------------------------------------------------------
*/
