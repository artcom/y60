/*
/--------------------------------------------------------------------
|
|      $Id: plbitmap.cpp,v 1.24 2004/09/11 12:41:35 uzadow Exp $
|      Device independent bitmap class
|
|      Manipulates uncompressed device independent bitmaps
|      of all color depths.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plbitmap.h"
#include "plexcept.h"
#include "plfilterquantize.h"

#include <iostream>

using namespace std;

// Creates an empty bitmap.
// Derived classes have to create a small bitmap here so the
// class can assume that a valid bitmap is available all the
// time.
PLBmp::PLBmp ()
{
}


PLBmp::~PLBmp
    ()
{
}

/////////////////////////////////////////////////////////////////////
// PLBmp creation

void PLBmp::Create (PLLONG Width, PLLONG Height, 
                    const PLPixelFormat& pf, PLBYTE * pBits, 
                    int Stride, const PLPoint& Resolution)
{
  PLASSERT_VALID (this);

  freeMembers ();
  internalCreate (Width, Height, pf);
  m_Resolution = Resolution;

  if (pBits) 
  {
    if (Stride == 0)
    {
      Stride = Width*pf.GetBitsPerPixel()/8;
    }
    for (int y = 0; y<Height; y++) 
    {
      memcpy(GetLineArray()[y], pBits+y*Stride, Width*pf.GetBitsPerPixel()/8);
    }
  }

  PLASSERT_VALID (this);
}

void PLBmp::Create (const PLBmpInfo& Info)
{
  Create (Info.GetWidth(), Info.GetHeight(), Info.GetPixelFormat(),
          NULL, 0, Info.GetResolution());
}

void PLBmp::CreateCopy (const PLBmpBase & rSrcBmp, 
    const PLPixelFormat& pfWanted)
{
  PLASSERT_VALID (&rSrcBmp);

  int BPPWanted = pfWanted.GetBitsPerPixel();

  PLASSERT (BPPWanted == 32 || BPPWanted == 24 || BPPWanted == 16 ||
          BPPWanted == 8 || BPPWanted == 1 || BPPWanted == 0);
  int BPPSrc = rSrcBmp.GetBitsPerPixel();
  PLASSERT (BPPSrc == 32 || BPPSrc == 24 || BPPSrc == 16 ||
          BPPSrc == 8 || BPPSrc == 1);

  if (pfWanted == rSrcBmp.GetPixelFormat() || 
      pfWanted == PLPixelFormat::DONTCARE)
  {
    if (&rSrcBmp != this)
    {
      // Create empty bitmap.
      freeMembers ();
      PLASSERT_VALID(&rSrcBmp);
      // Create empty bitmap.
      internalCreate (rSrcBmp.GetWidth(), rSrcBmp.GetHeight(),
              rSrcBmp.GetPixelFormat());
      if (GetBitsPerPixel() <= 8)
          SetPalette (rSrcBmp.GetPalette());
    } 
    else 
    {
      return;
    }
  }
  else
  {
    // Can't copy to self while changing bit depth.
    PLASSERT (&rSrcBmp != this);

    bool bDestAlpha = rSrcBmp.HasAlpha() && BPPWanted == 32;
    bool bDestGreyscale = rSrcBmp.IsGreyscale() && BPPWanted == 8;
    Create (rSrcBmp.GetWidth(), rSrcBmp.GetHeight(),
            pfWanted);
  }
  CopyPixels(rSrcBmp);
  SetResolution (rSrcBmp.GetResolution());
  PLASSERT_VALID (this);
}

void PLBmp::CreateFilteredCopy (PLBmpBase & rSrcBmp, const PLFilter & rFilter)
{
  PLASSERT_VALID (&rSrcBmp);
  rFilter.Apply (&rSrcBmp, this);
}


/////////////////////////////////////////////////////////////////////
// PLBmp manipulation

void PLBmp::ApplyFilter
    ( const PLFilter& Filter
    )
{
  Filter.ApplyInPlace (this);
}

/////////////////////////////////////////////////////////////////////
// Local functions

void PLBmp::initLocals (PLLONG Width, PLLONG Height, 
                        const PLPixelFormat& pf)
{
  SetBmpInfo (PLPoint (Width, Height), PLPoint(0,0), pf);
  initLineArray ();

  if (GetBitsPerPixel() <= 8)
  { // Color table exists
    SetGrayPalette ();
  }

  PLASSERT_VALID (this);
}


void PLBmp::internalCopy (const PLBmpBase & rSrcBmp)
{
  PLASSERT_VALID(&rSrcBmp);
  // Create empty bitmap.
  internalCreate (rSrcBmp.GetWidth(), rSrcBmp.GetHeight(), 
                  rSrcBmp.GetPixelFormat());
  PLBYTE ** pSrcLines = rSrcBmp.GetLineArray();
  PLBYTE ** pDstLines = GetLineArray();
  // Minimum possible line length.
  int LineLen = GetBytesPerLine();
  for (int y = 0; y<GetHeight(); y++) 
  {
    memcpy (pDstLines[y], pSrcLines[y], LineLen);
  }
  if (GetBitsPerPixel() <= 8)
    SetPalette (rSrcBmp.GetPalette());
  SetResolution (rSrcBmp.GetResolution());

  PLASSERT_VALID (this);
}


/*
/--------------------------------------------------------------------
|
|      $Log: plbitmap.cpp,v $
|      Revision 1.24  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.23  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.22  2004/06/20 16:59:34  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.21  2004/06/19 17:04:22  uzadow
|      Removed Lock(), Unlock(), PLDDrawBmp
|
|      Revision 1.20  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.19  2004/06/19 15:39:45  uzadow
|      Bugfix
|
|      Revision 1.18  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.17  2004/06/15 10:26:05  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.16  2004/06/09 21:34:53  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.15  2003/11/05 15:17:23  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.14  2003/07/29 21:27:41  uzadow
|      Fixed PLDirectFBBmp::GetBytesPerLine(), im2* Makefiles
|
|      Revision 1.13  2003/02/24 16:19:49  uzadow
|      Added url source (linux only), improved configure support for libungif detection.
|
|      Revision 1.12  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.11  2002/11/02 20:33:27  uzadow
|      Added gif transparency handling (Mark Richarme)
|
|      Revision 1.10  2002/08/05 19:06:30  uzadow
|      no message
|
|      Revision 1.9  2002/08/04 21:20:41  uzadow
|      no message
|
|      Revision 1.8  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.7  2002/03/31 13:36:41  uzadow
|      Updated copyright.
|
|      Revision 1.6  2001/10/21 17:12:39  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.5  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.4  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.3  2001/09/30 19:54:17  uzadow
|      Improved AlmostEqual();
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.31  2001/09/15 14:30:20  uzadow
|      Fixed PLPixel32 initialization bug.
|
|      Revision 1.30  2001/09/13 20:47:36  uzadow
|      Removed commented-out lines.
|
|      Revision 1.29  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.28  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.27  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.26  2000/12/03 21:15:43  uzadow
|      Fixed png handling in configure; test/ adapted to linux
|
|      Revision 1.25  2000/11/21 20:18:03  uzadow
|      Added operator ==
|
|      Revision 1.24  2000/11/02 21:28:47  uzadow
|      Fixed copy constructors.
|
|      Revision 1.23  2000/09/26 14:28:47  Administrator
|      Added Threshold filter
|
|      Revision 1.22  2000/09/26 12:14:51  Administrator
|      Refactored quantization.
|
|      Revision 1.21  2000/09/01 14:30:03  Administrator
|      no message
|
|      Revision 1.20  2000/09/01 14:19:46  Administrator
|      no message
|
|      Revision 1.19  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.18  2000/05/28 11:08:51  Ulrich von Zadow
|      Corrected alpha channel bug in CreateCopy.
|
|      Revision 1.17  2000/03/31 12:20:05  Ulrich von Zadow
|      Video invert filter (beta)
|
|      Revision 1.16  2000/03/31 11:53:29  Ulrich von Zadow
|      Added quantization support.
|
|      Revision 1.15  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.14  2000/01/10 23:52:59  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.13  1999/12/31 17:59:54  Ulrich von Zadow
|      Corrected error in PLBmp::SetPixel for 1 bpp.
|
|      Revision 1.12  1999/12/30 15:54:47  Ulrich von Zadow
|      Added PLWinBmp::FromClipBoard() and CreateFromHBitmap().
|
|      Revision 1.11  1999/12/10 01:27:26  Ulrich von Zadow
|      Added assignment operator and copy constructor to
|      bitmap classes.
|
|      Revision 1.10  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.9  1999/12/02 17:07:34  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.8  1999/11/08 22:10:53  Ulrich von Zadow
|      no message
|
|      Revision 1.7  1999/10/22 21:25:51  Ulrich von Zadow
|      Removed buggy octree quantization
|
|      Revision 1.6  1999/10/21 18:47:26  Ulrich von Zadow
|      Added Rotate, GetPixel, SetPixel and FindNearestColor
|
|      Revision 1.5  1999/10/21 16:05:43  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.4  1999/10/19 21:29:44  Ulrich von Zadow
|      Added filters.
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
\--------------------------------------------------------------------
*/

