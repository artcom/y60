/*
/--------------------------------------------------------------------
|
|      $Id: plsubbmp.cpp,v 1.5 2004/09/11 12:41:35 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plsubbmp.h"
#include "plrect.h"
#include "plexcept.h"

#include <iostream>

using namespace std;

PLSubBmp::PLSubBmp
  ()
{
  m_pLineArray=0;
  m_pClrTab=0;
}


PLSubBmp::~PLSubBmp
    ()
{
  // Free the memory.
  freeMembers();
}
  
void PLSubBmp::Create
    ( PLBmpBase & SrcBmp,
      const PLRect & SrcRect
    )
{
  PLASSERT (!m_pLineArray);

  int bpp = SrcBmp.GetBitsPerPixel();
  if (bpp <= 8)
    m_pClrTab = new PLPixel32 [1 << bpp];
  else
    m_pClrTab = NULL;
  
  initLocals (SrcRect.Width(), SrcRect.Height(), 
          SrcBmp.GetPixelFormat());
  if (bpp <= 8)
    SetPalette (SrcBmp.GetPalette());
              
  m_pLineArray = new PLBYTE * [m_Size.y];
  PLBYTE** ppSrcLines = SrcBmp.GetLineArray();
  int XOfs = SrcRect.tl.x*(bpp/8);
  for (int y=0; y<m_Size.y; y++)
    m_pLineArray[y] = ppSrcLines[SrcRect.tl.y+y]+XOfs;
    
  PLASSERT_VALID(this);
}    
    
void PLSubBmp::Create
    ( int Width,
      int Height,
      const PLPixelFormat& pf,
      PLBYTE * pBits,
      int Stride
    )
{
  if (m_pLineArray) 
  {
    if (Width != GetWidth() || Height != GetHeight() ||
        pf != GetPixelFormat())
    {
      freeMembers();
    } 
  }
  
  if (!m_pLineArray) 
  {  // freeMembers sets m_pLineArray to 0. 
    if (pf.GetBitsPerPixel() <= 8)
      m_pClrTab = new PLPixel32 [1 << pf.GetBitsPerPixel()];
    else
      m_pClrTab = NULL;
    initLocals (Width, Height, pf);
    if (pf.GetBitsPerPixel() <= 8)
      SetGrayPalette ();
    m_pLineArray = new PLBYTE * [m_Size.y];
  }

  PLBYTE * pCurLine = m_pLineArray[0];
  for (int y=0; y<m_Size.y; y++) 
  {
    m_pLineArray[y] = pBits;
    pBits += Stride;
  }

  PLASSERT_VALID(this);
}    


/////////////////////////////////////////////////////////////////////
// Local functions


void PLSubBmp::freeMembers
    ()
{
  if (m_pClrTab)
  {
    delete [] m_pClrTab;
    m_pClrTab = NULL;
  }

  if (m_pLineArray)
  {
    delete [] m_pLineArray;
    m_pLineArray = NULL;
  }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plsubbmp.cpp,v $
|      Revision 1.5  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.4  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.3  2004/06/19 15:39:45  uzadow
|      Bugfix
|
|      Revision 1.2  2004/06/16 16:38:01  artcom
|      PLSubBmp now allows multiple Create() calls.
|
|      Revision 1.1  2004/06/15 14:17:11  uzadow
|      First working version of PLSubBmp.
|
|
\--------------------------------------------------------------------
*/
