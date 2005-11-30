/*
/--------------------------------------------------------------------
|
|      $Id: plsdlbmp.cpp,v 1.3 2003/05/29 21:08:34 uzadow Exp $
|      Bitmap class for SDL surfaces.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plstdpch.h"
#include "plsdlbmp.h"
#include "plexcept.h"


PLSDLBmp::PLSDLBmp
  ( int SurfFlags
  ) : m_pSurface(NULL),
      m_SurfFlags(SurfFlags)
    // Creates an empty bitmap.
{
  internalCreate(16, 16, 32, false, false);

  PLASSERT_VALID(this);
}


PLSDLBmp::~PLSDLBmp
    ()
{
  // Free the memory.
  freeMembers();
}

void PLSDLBmp::Attach
    ( SDL_Surface * pSurface
    )
{
  PLASSERT_VALID (this);

  freeMembers ();
  m_pSurface = pSurface;
  m_SurfFlags = m_pSurface->flags;
  m_pClrTab = NULL;
  bool hasAlpha = false;
  if (m_SurfFlags & SDL_SRCALPHA) {
      hasAlpha = true;
  }
  initLocals (m_pSurface->w, m_pSurface->h, 32, hasAlpha, false);

  PLASSERT_VALID (this);

}

#ifdef _DEBUG
void PLSDLBmp::AssertValid () const
{
  PLBmp::AssertValid();
  PLASSERT (m_bpp == 32);
  PLASSERT (m_pSurface->format->BitsPerPixel == m_bpp);
  PLASSERT (m_pSurface->w == m_Size.x);
  PLASSERT (m_pSurface->h == m_Size.y);
  PLASSERT (((m_SurfFlags & SDL_SRCALPHA) == SDL_SRCALPHA) == HasAlpha());
}
#endif

SDL_Surface * PLSDLBmp::GetSurface
    ()
{
  return m_pSurface;
}


long PLSDLBmp::GetMemUsed
    ()
    // Returns the memory used by the object.
{
  PLASSERT_VALID (this);

  return GetBytesPerLine()*GetHeight()*4+sizeof(SDL_Surface)+sizeof(*this);
}


long PLSDLBmp::GetBytesPerLine
    ()
    // Returns number of bytes used per line.
{
  return m_pSurface->pitch;
}


/////////////////////////////////////////////////////////////////////
// Static functions

long PLSDLBmp::GetBitsMemNeeded
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


long PLSDLBmp::GetMemNeeded
    ( PLLONG width,
      PLLONG height,
      PLWORD BitsPerPixel
    )
    // Returns memory needed by a bitmap with the specified attributes.
    // This is only approximate and doesn't take the stride into account.
    // GetMemUsed is more accurate.
{
  int HeaderMem = sizeof (PLSDLBmp);
  if (BitsPerPixel < 16)
  { // Palette memory
    HeaderMem += (1 << BitsPerPixel)*sizeof (PLPixel32);
  }

  return HeaderMem+GetBitsMemNeeded (width, height, BitsPerPixel);
}


/////////////////////////////////////////////////////////////////////
// Local functions


void PLSDLBmp::internalCreate
    ( PLLONG Width,
      PLLONG Height,
      PLWORD BitsPerPixel,
      bool bAlphaChannel,
      bool bIsGreyscale
    )
    // Create a new empty bitmap. Bits are uninitialized.
    // Assumes that no memory is allocated before the call.
{
  PLASSERT (BitsPerPixel==32);
  // Create SDL memory surface.
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  int rmask = 0xff000000;
  int gmask = 0x00ff0000;
  int bmask = 0x0000ff00;
  int amask = 0x000000ff;
#else
  int rmask = 0x000000ff;
  int gmask = 0x0000ff00;
  int bmask = 0x00ff0000;
  int amask = 0xff000000;
#endif
  if (bAlphaChannel) {
    m_SurfFlags |= SDL_SRCALPHA;
  } else {
      amask = 0;
  }
  m_pSurface = SDL_CreateRGBSurface (m_SurfFlags, Width, Height, 32,
          rmask, gmask, bmask, amask);
  m_pClrTab = NULL;
  initLocals (Width, Height, BitsPerPixel, bAlphaChannel, bIsGreyscale);

  PLASSERT_VALID (this);
}


void PLSDLBmp::initLineArray
    ()
{
  m_pLineArray = new PLBYTE * [m_Size.y];
  int LineLen = GetBytesPerLine();

  for (int y=0; y<m_Size.y; y++)
    m_pLineArray[y] = (PLBYTE*)(m_pSurface->pixels) + y*LineLen;
}

void PLSDLBmp::freeMembers
    ()
{
  SDL_FreeSurface (m_pSurface);

  delete [] m_pLineArray;
  m_pLineArray = NULL;
}

/*
/--------------------------------------------------------------------
|
|      $Log: plsdlbmp.cpp,v $
|      Revision 1.3  2003/05/29 21:08:34  uzadow
|      Fixed SDL autoconf bug; fixed PLExif dependency on gcc > 3.0
|
|      Revision 1.2  2003/03/08 15:15:51  uzadow
|      Added PLSDLBmp::Attach().
|
|      Revision 1.1  2003/03/08 14:32:08  uzadow
|      Added support for bitmaps in SDL surface format.
|
|
\--------------------------------------------------------------------
*/
