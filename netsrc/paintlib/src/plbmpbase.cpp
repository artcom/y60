/*
/--------------------------------------------------------------------
|
|      $Id: plbmpbase.cpp,v 1.13 2005/07/12 13:10:34 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plbmpbase.h"
#include "planybmp.h"
#include "Filter/plfilter.h"
#include "Filter/plfilterquantize.h"

#include <cstring> // memcpy() & co.
#include <iostream>

using namespace std;

template<class DestPixelC>
void createTrueColorCopy (PLBmp & DestBmp, const PLBmpBase & SrcBmp,
                                      DestPixelC Dummy);

// Creates an empty bitmap.
PLBmpBase::PLBmpBase ()
  : m_pClrTab(0),
    m_pLineArray(0),
    m_DitherType(PLDTH_ORDERED),
    m_DitherPaletteType(PLDTHPAL_MEDIAN)   // defaults for 8Bit color quantization
    
{
}


PLBmpBase::~PLBmpBase
    ()
{
}

bool const PLBmpBase::operator ==
    ( PLBmpBase const &Other
    )
{
  return AlmostEqual (Other, 0);
}

#ifdef _DEBUG
void PLBmpBase::AssertValid () const
{
  if (m_Size != PLPoint(0,0))
  {
    // Color table only if 8 bpp.
    PLASSERT ((GetBitsPerPixel() > 8) == (m_pClrTab == NULL));
    PLASSERT (m_Size.y >= 0);
    PLASSERT (m_Size.x >= 0);

    // Greyscale only if 8 bpp
    //PLASSERT ((GetBitsPerPixel() <= 8) || !IsGreyscale()); // needlessly blows on valid 16 bit greyscale images
  }
}
#endif

/////////////////////////////////////////////////////////////////////
// PLBmpBase manipulation

void PLBmpBase::SetQuantizationMode (int DitherType, int DitherPaletteType)
{
  PLASSERT_VALID (this);
  m_DitherType = DitherType;
  m_DitherPaletteType = DitherPaletteType;
}

// The dummy parameter is nessesary to work around an MSVC bug. If it isn't
// there, the function will only get instantiated once :-(.
template<class DestPixelC>
void createTrueColorCopy (PLBmpBase & DestBmp, const PLBmpBase & SrcBmp,
                          DestPixelC /*Dummy*/)
{
  // Make sure we're using the right template.
  PLASSERT (DestBmp.GetBitsPerPixel() == sizeof (DestPixelC)*8);

  int BPPSrc = SrcBmp.GetBitsPerPixel();
  PLBYTE ** pSrcLines = SrcBmp.GetLineArray();
  DestPixelC ** pDstLines = (DestPixelC**) DestBmp.GetLineArray();
  int Height = min(SrcBmp.GetHeight(), DestBmp.GetHeight());
  int Width = min(SrcBmp.GetWidth(), DestBmp.GetWidth());
  for (int y = 0; y<Height; ++y)
  { // For each line
    DestPixelC * pDstPixel = pDstLines[y];
    switch (BPPSrc)
    {
      case 32:
        {
          PLPixel32 * pSrcPixel = (PLPixel32 *)pSrcLines[y];
          for (int x = 0; x < Width; ++x)
          { // For each pixel
            *pDstPixel = *pSrcPixel;
            ++pSrcPixel;
            ++pDstPixel;
          }
        }
        break;
      case 24:
        {
          PLPixel24 * pSrcPixel = (PLPixel24 *)pSrcLines[y];
          for (int x = 0; x < Width; ++x)
          { // For each pixel
            *pDstPixel = *pSrcPixel;
            ++pSrcPixel;
            ++pDstPixel;
          }
        }
        break;
      case 16:
        {
          PLPixel16 * pSrcPixel = (PLPixel16 *)pSrcLines[y];
          for (int x = 0; x < Width; ++x)
          { // For each pixel
            *pDstPixel = (DestPixelC)*pSrcPixel;
            ++pSrcPixel;
            ++pDstPixel;
          }
        }
        break;
      case 8:
        {
          PLPixel32 *pPal = SrcBmp.GetPalette();

          PLBYTE * pSrcPixel = pSrcLines[y];

          for (int x = 0; x < Width; ++x)
          { // For each pixel
            *pDstPixel = pPal[*pSrcPixel];
            ++pSrcPixel;
            ++pDstPixel;
          }
        }
        break;
      case 1:
        {
          PLPixel32 * pPal = SrcBmp.GetPalette();
          DestPixelC blackDot(0,0,0);
          DestPixelC whiteDot(255,255,255);
          // if bi-tonal "palette" exists, use it...
          if (pPal)
          {
            whiteDot = pPal[0];
            blackDot = pPal[1];
          }

          // assume msb is leftmost
          PLBYTE * pSrcPixel = pSrcLines[y];

          for (int x = 0; x < Width; ++x)
          { // For each pixel
            if (pSrcPixel[x / 8] & (128 >> (x & 7)))  // black pixel
              *pDstPixel = blackDot;
            else
              *pDstPixel = whiteDot;
            pDstPixel++;
          }
        }
        break;
      default:
        PLASSERT (false);
    }
  }
}

void PLBmpBase::CopyPixels
    ( const PLBmpBase& SrcBmp
    )
{
  if (SrcBmp.GetPixelFormat() == GetPixelFormat())
  {
    if (&SrcBmp != this)
    {
      PLBYTE ** pSrcLines = SrcBmp.GetLineArray();
      PLBYTE ** pDstLines = GetLineArray();
      int Height = min(SrcBmp.GetHeight(), GetHeight());
      int Width = min(SrcBmp.GetWidth(), GetWidth());
      int LineLen = Width*GetBitsPerPixel()/8;
      for (int y = 0; y<Height; y++) 
      {
        memcpy (pDstLines[y], pSrcLines[y], LineLen);
      }
    }
  }
  else
  {
    switch (GetBitsPerPixel())
    {
      case 32:
        createTrueColorCopy (*this, SrcBmp, PLPixel32());
        break;

      case 24:
        createTrueColorCopy (*this, SrcBmp, PLPixel24());
        break;

      case 16:
        createTrueColorCopy (*this, SrcBmp, PLPixel16());
        break;

      case 8:
        create8BPPCopy (SrcBmp);
        break;

      case 1:
        create1BPPCopy (SrcBmp);
        break;
      default:
        PLASSERT(false);
    }
    PLASSERT_VALID (this);

  }
}
    

void PLBmpBase::SetGrayPalette ()
// Fills the color table with a grayscale palette.
{
  PLASSERT (m_pClrTab); // Bitmap must contain a palette!

  int NumColors = GetNumColors();
  double ColFactor = 255/(NumColors-1);

  for (int i=0; i<NumColors; i++)
    SetPaletteEntry (PLBYTE(i), PLBYTE(i*ColFactor), PLBYTE(i*ColFactor), PLBYTE(i*ColFactor), 0xFF);
}

void PLBmpBase::SetPalette (PLPixel32 * pPal)
{
  PLASSERT (m_pClrTab); // Bitmap must contain a palette!

  memcpy (m_pClrTab, pPal, GetNumColors() * sizeof(PLPixel32));
}

void PLBmpBase::SetHasAlpha
    (bool b
    )
{
  PLASSERT_VALID (this);

  if (!HasAlpha() && b)
  {
    PLASSERT (GetBitsPerPixel() == 32);
    for (int y=0; y < GetHeight(); y++)
    {
      PLPixel32 * pLine = GetLineArray32()[y];
      for (int x=0; x < GetWidth(); x++)
        pLine[x].SetA(0xFF);
    }
  }  
  m_pf = m_pf.UseAlpha(b);
}

void PLBmpBase::SetAlphaChannel (PLBmpBase * pAlphaBmp)
// Replaces the alpha channel with a new one.
{
  PLBYTE * pAlphaLine;
  int x,y;
  PLPixel32 ** pLineArray;
  PLBYTE ** pAlphaLineArray;

  PLASSERT_VALID (this);
  // Current bitmap must be 32 bpp.
  PLASSERT (GetBitsPerPixel() == 32);

  PLASSERT_VALID (pAlphaBmp);
  // Alpha channel must be 8 bpp.
  PLASSERT (pAlphaBmp->GetBitsPerPixel() == 8);

  // The two bitmaps must have the same dimensions
  PLASSERT (pAlphaBmp->GetWidth() == GetWidth());
  PLASSERT (pAlphaBmp->GetHeight() == GetHeight());

  pLineArray = GetLineArray32();
  pAlphaLineArray = pAlphaBmp->GetLineArray();

  for (y=0; y < GetHeight(); y++)
  {
    PLPixel32 * pLine = pLineArray[y];
    pAlphaLine = pAlphaLineArray[y];
    for (x=0; x < GetWidth(); x++)
    {
      pLine[x].SetA(pAlphaLine[x]);
    }
  }
  m_pf = m_pf.UseAlpha(true);
  PLASSERT_VALID (this);
}

PLBYTE PLBmpBase::FindNearestColor (PLPixel32 col)
{
  PLPixel32 * pPalette = GetPalette();
  PLASSERT (pPalette);

  int dMin = 100000;
  PLBYTE index = static_cast<PLBYTE>(-1);
  for (int i = 0; i<GetNumColors(); i++)
  {
    int d1 = col.BoxDist (pPalette[i]);
    if (d1 < dMin)
    {
      dMin = d1;
      index = static_cast<PLBYTE>(i);
    }
  }
  return index;
}

bool PLBmpBase::AlmostEqual
    ( const PLBmpBase& Bmp,
      int epsilon
    ) const
    // TODO: Fix 16 bpp version!
{
  if (GetWidth() != Bmp.GetWidth() ||
      GetHeight() != Bmp.GetHeight() ||
      HasAlpha() != Bmp.HasAlpha() ||
      IsGreyscale() != Bmp.IsGreyscale() ||
      GetBitsPerPixel() != Bmp.GetBitsPerPixel())
    return false;

  if (m_Resolution != Bmp.GetResolution())
    return false;

  PLBYTE ** ppLines1 = GetLineArray();
  PLBYTE ** ppLines2 = Bmp.GetLineArray();
  int y,x;
  for (y=0; y<GetHeight(); y++)
    for (x=0; x<GetWidth(); x++)
      switch (GetBitsPerPixel())
      {
        case 8:
          if (abs (ppLines1[y][x] - ppLines2[y][x]) > epsilon)
            return false;
          break;
        case 16:
          if (((PLPixel16*)(ppLines1[y]))[x] != ((PLPixel16*)(ppLines2[y]))[x])
            return false;
          break;
        case 24:
          if (abs (ppLines1[y][x*3+PL_RGBA_RED] - ppLines2[y][x*3+PL_RGBA_RED]) > epsilon ||
              abs (ppLines1[y][x*3+PL_RGBA_GREEN] - ppLines2[y][x*3+PL_RGBA_GREEN]) > epsilon ||
              abs (ppLines1[y][x*3+PL_RGBA_BLUE] - ppLines2[y][x*3+PL_RGBA_BLUE]) > epsilon)
            return false;
          break;
        case 32:
          if (abs (ppLines1[y][x*4+PL_RGBA_RED] - ppLines2[y][x*4+PL_RGBA_RED]) > epsilon ||
              abs (ppLines1[y][x*4+PL_RGBA_GREEN] - ppLines2[y][x*4+PL_RGBA_GREEN]) > epsilon ||
              abs (ppLines1[y][x*4+PL_RGBA_BLUE] - ppLines2[y][x*4+PL_RGBA_BLUE]) > epsilon)
            return false;
          if (HasAlpha() &&
              abs (ppLines1[y][x*4+3] - ppLines2[y][x*4+3]) > epsilon)
            return false;
          break;
        default:
          // Unsupported BPP.
          PLASSERT (false);
      }

  // Test if the palettes are the same for paletted bitmaps.
  if (GetBitsPerPixel() == 8)
  {
    int i;
    PLPixel32 * pPal1 = GetPalette();
    PLPixel32 * pPal2 = Bmp.GetPalette();
    for (i=0; i<255; i++)
    {
      if (abs (pPal1[i].GetR() - pPal2[i].GetR()) > epsilon ||
          abs (pPal1[i].GetG() - pPal2[i].GetG()) > epsilon ||
          abs (pPal1[i].GetB() - pPal2[i].GetB()) > epsilon)
        return false;
    }
  }

  return true;
}


void PLBmpBase::Dump ()
{
    cerr << AsString() << endl;
    cerr << "  Pixel data: \n";
    
    PLBYTE ** ppLines = GetLineArray();
    int y,x;
    for (y=0; y<GetHeight(); y++)
    {
        cerr << "    ";
        for (x=0; x<GetWidth(); x++)
        {
            PLPixel32 pix;
            switch (GetBitsPerPixel())
            {
                case 32:
                    pix = *((PLPixel32*)&(ppLines [y][x*4]));
                    cerr << "(" << int(pix.GetR()) << "," << int(pix.GetG()) << "," << int(pix.GetB ())
                        << "," << int(pix.GetA ()) << ")";
                    break;
                case 24:
                    pix= *((PLPixel24*)&(ppLines [y][x*3]));
                    cerr << "(" << int(pix.GetR()) << "," << int(pix.GetG()) << "," << int(pix.GetB ());
                    break;
                case 16:
                    pix = *((PLPixel16*)&(ppLines [y][x*4]));
                    cerr << "(" << int(pix.GetR()) << "," << int(pix.GetG()) << "," << int(pix.GetB ());
                    break;
                case 8:
                    cerr << ppLines[y][x];
                    break;
                default:
                    PLASSERT (false);
            }
        }
        cerr << "\n";
    }
    if (GetBitsPerPixel() <= 8)
    {
        cerr << "    Palette:";
        cerr << "      ";
        PLPixel32 * pPal = GetPalette();
        int i;
        for (i=0; i<2; i++)
            cerr << "(" << pPal[i].GetR() << "," << pPal[i].GetG() << "," <<
                pPal[i].GetB () << "," << pPal[i].GetA ();
        cerr << "..." ;
        for (i=254; i<256; i++)
            cerr << "(" << pPal[i].GetR() << "," << pPal[i].GetG() << "," <<
                pPal[i].GetB () << "," << pPal[i].GetA ();
    }
    cerr << "\n";
}


void PLBmpBase::initLocals 
    ( PLLONG Width,
      PLLONG Height,
      const PLPixelFormat & pf
	)
{
  SetBmpInfo (PLPoint (Width, Height), PLPoint(0,0), pf);

  if (GetBitsPerPixel() <= 8)
  { // Color table exists
    SetGrayPalette ();
  }

  PLASSERT_VALID (this);
}

void PLBmpBase::create8BPPCopy (const PLBmpBase & rSrcBmp)
{
  int BPPSrc = rSrcBmp.GetBitsPerPixel();
  int Height = min(rSrcBmp.GetHeight(), GetHeight());
  int Width = min(rSrcBmp.GetWidth(), GetWidth());

  if (BPPSrc == 32) // Conversion 32->8 BPP
  {
    PLFilterQuantize Filter (m_DitherPaletteType, m_DitherType);
    PLAnyBmp TempBmp;
    Filter.Apply (&(const_cast<PLBmpBase &>(rSrcBmp)), &TempBmp);
    SetPalette(TempBmp.GetPalette());
    CopyPixels(TempBmp);
  }
  else // 1 -> 8
  {
    PLPixel32 *pPal = rSrcBmp.GetPalette();
    // if bi-tonal "palette" exists, use it...
    if (pPal)
    {
      PLBYTE *pWhite = (PLBYTE *) pPal;
      PLBYTE *pBlack = (PLBYTE *) (pPal+1);
      SetPaletteEntry(0,
                      pWhite[PL_RGBA_RED], pWhite[PL_RGBA_GREEN],
                      pWhite[PL_RGBA_BLUE], 255);
      SetPaletteEntry(1,
                      pBlack[PL_RGBA_RED], pBlack[PL_RGBA_GREEN],
                      pBlack[PL_RGBA_BLUE], 255);
    }
    else
    {
      SetPaletteEntry(0,255,255,255,255);
      SetPaletteEntry(1,0,0,0,255);
    }

    // assume msb is leftmost
    for (int y = 0; y<Height; ++y)
    { // For each line
      PLBYTE * pSrcPixel = rSrcBmp.GetLineArray()[y];
      PLBYTE * pDstPixel = GetLineArray()[y];
      for (int x = 0; x < Width; ++x)  // For each pixel
        pDstPixel[x] = pSrcPixel[x / 8] & (128 >> (x & 7)) ? 1 : 0;
    }
  }
}

void PLBmpBase::create1BPPCopy (const PLBmpBase & rSrcBmp)
{
  int BPPSrc = rSrcBmp.GetBitsPerPixel();
  PLBYTE ** pSrcLines = rSrcBmp.GetLineArray();
  PLBYTE ** pDstLines = GetLineArray();
  int Height = min(rSrcBmp.GetHeight(), GetHeight());
  int Width = min(rSrcBmp.GetWidth(), GetWidth());

  SetPaletteEntry(0,255,255,255,255);
  SetPaletteEntry(1,0,0,0,255);

  // downgrade to monochrome
  PLPixel32 *pPal = rSrcBmp.GetPalette();
  PLBYTE *pRGBA;
  int DstLineLen = (Width+7)/8;

  for (int y = 0; y < Height; ++y)
  { // For each line
    PLBYTE * pSrcPixel = pSrcLines[y];
    PLBYTE * pDstPixel = pDstLines[y];
    // fill with background (index 0) color
    memset(pDstPixel,0,DstLineLen);

    for (int x = 0; x < Width; ++x)  // For each pixel
    {
      pRGBA = BPPSrc == 8 ? (PLBYTE*) &pPal[*pSrcPixel] : pSrcPixel;
      // the following criterion supposedly finds "dark" pixels; it may
      // need some twiddling and maybe use the alpha channel as well
      if (pRGBA[PL_RGBA_RED] < 128 &&
          pRGBA[PL_RGBA_GREEN] < 128 &&
          pRGBA[PL_RGBA_BLUE] < 128 )
        pDstPixel[x / 8] |= 128 >> (x & 7);
      pSrcPixel += BPPSrc == 8 ? 1 : sizeof(PLPixel32);
    }
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plbmpbase.cpp,v $
|      Revision 1.13  2005/07/12 13:10:34  artcom
|      64 bit-fixes, png encoder fix.
|
|      Revision 1.12  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.11  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.10  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.9  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.8  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.7  2004/08/04 14:53:29  uzadow
|      Added PLFilterColorize.
|
|      Revision 1.6  2004/08/03 13:26:50  uzadow
|      Fixed copy bug when bitmaps have different sizes.
|
|      Revision 1.5  2004/06/20 16:59:34  uzadow
|      Added PLBmpBase::CopyPixels() and PLInPlaceFilter
|
|      Revision 1.4  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.3  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.2  2004/06/15 11:18:17  uzadow
|      First working version of PLBmpBase.
|
|      Revision 1.1  2004/06/15 10:46:41  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|
\--------------------------------------------------------------------
*/

