/*
/--------------------------------------------------------------------
|
|      $Id: pldirectfbbmp.cpp,v 1.9 2004/06/12 14:52:46 uzadow Exp $
|      Bitmap class for SDL surfaces.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plstdpch.h"
#include "pldirectfbbmp.h"
#include "plexcept.h"

#include <iostream>

using namespace std;

IDirectFB * PLDirectFBBmp::s_pDirectFB = 0;


void PLDirectFBBmp::SetDirectFB
  ( IDirectFB * pDirectFB
  )
{
  s_pDirectFB = pDirectFB;
}

PLDirectFBBmp::PLDirectFBBmp
  ()
    // Creates an empty bitmap.
{
  PLASSERT (s_pDirectFB);

  m_pSurface=0;
  internalCreate(16, 16, 32, false, false);

  PLASSERT_VALID(this);
}


PLDirectFBBmp::~PLDirectFBBmp
    ()
{
  // Free the memory.
  freeMembers();
}

void PLDirectFBBmp::CreateFromSurface
    ( IDirectFBSurface * pSurface,
      bool bOwnsSurface
    )
{
  freeMembers();

  m_pSurface = pSurface;
  m_bOwnsSurface = bOwnsSurface;

  DFBSurfacePixelFormat PixelFormat;
  m_pSurface->GetPixelFormat(m_pSurface, &PixelFormat); 
  int w, h;
  m_pSurface->GetSize(m_pSurface, &w, &h);

  bool bAlphaChannel = false;
  int bpp;
  bool bIsGreyscale = false;
  switch (PixelFormat) 
  {
    case DSPF_ARGB:
        bAlphaChannel = true;
        bpp = 32;
        break;
    case DSPF_RGB32:
        bpp = 32;
        break;
    case DSPF_RGB24:
        bpp = 24;
        break;
    case DSPF_RGB16:
        bpp = 16;
        break;
    case DSPF_A8:
        bpp = 8;
        bIsGreyscale = true;
        break;
    default:
        throw PLTextException (PL_ERRDFB, 
                "Unsupported pixel format in CreateFromSurface.");
  }

  if (bpp <= 8)
    m_pClrTab = new PLPixel32 [1 << bpp];
   else
    m_pClrTab = NULL;
  initLocals (w, h, bpp, bAlphaChannel, bIsGreyscale);
}


#ifdef _DEBUG
void PLDirectFBBmp::AssertValid () const
{
  PLBmp::AssertValid();
  PLASSERT (m_bpp == 32 || m_bpp == 24 || m_bpp == 16 || m_bpp == 8);
  DFBSurfacePixelFormat PixelFormat;
  m_pSurface->GetPixelFormat(m_pSurface, &PixelFormat);
  PLASSERT (PixelFormat == DSPF_ARGB || 
            PixelFormat == DSPF_A8 ||
            PixelFormat == DSPF_RGB16 || 
            PixelFormat == DSPF_RGB24 || 
            PixelFormat == DSPF_RGB32);
  int w, h;
  m_pSurface->GetSize(m_pSurface, &w, &h);
  PLASSERT (w == m_Size.x);
  PLASSERT (h == m_Size.y);
}
#endif

IDirectFBSurface * PLDirectFBBmp::GetSurface
    ()
{
  return m_pSurface;
}


long PLDirectFBBmp::GetMemUsed
    ()
    // Returns the memory used by the object.
{
  PLASSERT_VALID (this);

  return GetBytesPerLine()*GetHeight()+sizeof(IDirectFBSurface)+sizeof(*this);
}


long PLDirectFBBmp::GetBytesPerLine
    ()
    // Returns number of bytes used per line.
{
  return m_Size.x*m_bpp/8;
}


/////////////////////////////////////////////////////////////////////
// Static functions

long PLDirectFBBmp::GetBitsMemNeeded
    ( PLLONG width,
      PLLONG height,
      PLWORD BitsPerPixel
    )
    // Returns memory needed by bitmap bits. This is only approximate 
    // and doesn't take the stride into account.
{
  // Calculate memory per line.
  int LineMem = width*BitsPerPixel/8;

  // Multiply by number of lines
  return LineMem*height;
}


long PLDirectFBBmp::GetMemNeeded
    ( PLLONG width,
      PLLONG height,
      PLWORD BitsPerPixel
    )
    // Returns memory needed by a bitmap with the specified attributes.
    // This is only approximate and doesn't take the stride into account.
    // GetMemUsed is more accurate.
{
  int HeaderMem = sizeof (PLDirectFBBmp);
  if (BitsPerPixel < 16)
  { // Palette memory
    HeaderMem += (1 << BitsPerPixel)*sizeof (PLPixel32);
  }

  return HeaderMem+GetBitsMemNeeded (width, height, BitsPerPixel);
}


/////////////////////////////////////////////////////////////////////
// Local functions


void PLDirectFBBmp::internalCreate
    ( PLLONG Width,
      PLLONG Height,
      PLWORD BitsPerPixel,
      bool bAlphaChannel,
      bool bIsGreyscale
    )
    // Create a new empty bitmap. Bits are uninitialized.
    // Assumes that no memory is allocated before the call.
    // The bitmap is stored in a DirectFB system memory surface.
{
  PLASSERT (BitsPerPixel==32 || BitsPerPixel==24 ||
            BitsPerPixel==16 || BitsPerPixel==8);
  DFBSurfaceDescription Desc;
  Desc.flags = DFBSurfaceDescriptionFlags (DSDESC_CAPS | DSDESC_WIDTH | 
            DSDESC_HEIGHT | DSDESC_PIXELFORMAT);
  Desc.caps = DSCAPS_SYSTEMONLY;
  Desc.width = Width;
  Desc.height = Height;
  switch(BitsPerPixel) 
  {
    case 32:
      if (bAlphaChannel) 
        Desc.pixelformat = DSPF_ARGB;
      else
        Desc.pixelformat = DSPF_RGB32;
      break;
    case 24:
      Desc.pixelformat = DSPF_RGB24;
      break;
    case 16:
      Desc.pixelformat = DSPF_RGB16;
      break;
    case 8:
      Desc.pixelformat = DSPF_A8;
      break;
  }
  DFBResult err = s_pDirectFB->CreateSurface(s_pDirectFB, &Desc, &m_pSurface);
  if (err)
    throw PLTextException(PL_ERRDFB, "pDFB->CreateSurface() failed."); 

  if (BitsPerPixel <= 8)
    m_pClrTab = new PLPixel32 [1 << BitsPerPixel];
   else
    m_pClrTab = NULL;
  initLocals (Width, Height, BitsPerPixel, bAlphaChannel, bIsGreyscale);
  m_bOwnsSurface = true;
}


void PLDirectFBBmp::initLineArray
    ()
    // Note that this function unlocks the surface after getting the pixel offsets,
    // which is probably ok for system memory surfaces but will definitely break 
    // for video memory surfaces.
{
  void * pPixels;
  int Pitch;
  m_pSurface->Lock(m_pSurface, DFBSurfaceLockFlags (DSLF_READ | DSLF_WRITE), 
            &pPixels, &Pitch); 
  m_pLineArray = new PLBYTE * [m_Size.y];
  for (int y=0; y<m_Size.y; y++)
    m_pLineArray[y] = (PLBYTE*)(pPixels) + y*Pitch;
  m_pSurface->Unlock(m_pSurface);
}

void PLDirectFBBmp::freeMembers
    ()
{
  if (m_bOwnsSurface) {
    DFBResult err = m_pSurface->Release(m_pSurface);
    if (err) {
      // This should definitely throw an exception...
      cerr << __FILE__ << __LINE__ << ": " << DirectFBErrorString(err) << endl;
    }
    m_pSurface = 0;
  }

  delete [] m_pLineArray;
  m_pLineArray = NULL;
}

/*
/--------------------------------------------------------------------
|
|      $Log: pldirectfbbmp.cpp,v $
|      Revision 1.9  2004/06/12 14:52:46  uzadow
|      Added CreateFromSurface function.
|
|      Revision 1.8  2004/06/09 21:34:53  uzadow
|      Added 16 bpp support to plbitmap, planybmp and pldirectfbbmp
|
|      Revision 1.7  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.6  2004/04/16 20:31:17  uzadow
|      Added 24 bpp support.
|
|      Revision 1.5  2004/02/15 22:43:31  uzadow
|      Added 8-bit-support to DFBBitmap and PLFilterFill
|
|      Revision 1.4  2003/11/21 23:35:44  uzadow
|      Removed files built by the autotools.
|
|      Revision 1.3  2003/07/29 21:27:41  uzadow
|      Fixed PLDirectFBBmp::GetBytesPerLine(), im2* Makefiles
|
|      Revision 1.2  2003/07/27 18:08:38  uzadow
|      Added plfilterfliprgb
|
|      Revision 1.1  2003/07/27 13:50:48  uzadow
|      Added support for DirectFB surfaces.
|
|
|
\--------------------------------------------------------------------
*/
