/*
/--------------------------------------------------------------------
|
|      $Id: WinBmpEx.h,v 1.3 2001/09/16 19:03:23 uzadow Exp $
|	     PaintLib windows DrawDib bitmap class implementation
|
|      See Paul DiLascia's excellent january and march ´97 MSJ
|      articles. This version optionally draws using the 'video for
|      windows' DrawDib API. This is supposedly faster, and offers
|      dithering, too.
|
|      Copyright (c) 1998-2000 Bernard Delmée
|
\--------------------------------------------------------------------
*/

#ifndef INCL_DRAWDIB
#define INCL_DRAWDIB

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


#include "pldibsect.h"

class PLWinBmpEx: public PLDIBSection
{
public:
    PLWinBmpEx();
    virtual ~PLWinBmpEx();

    //! Extended Draw function; can use DrawDib or not.
    //! Using any stretch-mode (but the default) forces GDI usage
    BOOL DrawEx(HDC dc, const RECT* rcDst=NULL, const RECT* rcSrc=NULL,
                HPALETTE hPal=NULL, bool bForeground=true,
		int StretchMode=-1, DWORD rop=SRCCOPY);
    
    //! Draws the bitmap with top left corner at specified location.
    virtual void Draw( HDC dc, int x, int y, DWORD rop = -1 );

    //! Draws the bitmap on the given device context.
    //! Scales the bitmap by Factor.
    virtual void StretchDraw (HDC hDC, int x, int y, 
	double Factor, DWORD rop = -1);

    //! Draws the bitmap on the given device context.
    //! Scales the bitmap so w is the width and h the height.
    virtual void StretchDraw (HDC hDC, int x, int y, 
	int w, int h, DWORD rop = -1);
    
    //! Draws a portion of the bitmap on the given device context
    virtual BOOL DrawExtract( HDC hDC, POINT pntDest, RECT rcSrc );

    //! Call this whenever recycling the bitmap
    void BuildLogPalette();

    //! Retrieve windows logical palette.
    HPALETTE GetLogPalette();

private:
    HPALETTE m_pal;    // logical palette

    HPALETTE CreatePalette();

// if we ever wanted to build without pulling in vfw 
#ifndef _WITHOUT_VFW_
    HDRAWDIB m_hdd;    // for DrawDib API
#endif
};

#endif // INCL_DRAWDIB

/*
/--------------------------------------------------------------------
|
|      $Log: WinBmpEx.h,v $
|      Revision 1.3  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.2  2000/03/30 21:47:41  Ulrich von Zadow
|      Added zoom-in mode, PLWinBmpEx, conditional use of DrawDIB
|      and some other nice stuff by Bernard Delmée.
|
|
|
\--------------------------------------------------------------------
*/
