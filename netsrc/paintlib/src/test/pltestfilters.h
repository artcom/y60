/*
/--------------------------------------------------------------------
|
|      $Id: pltestfilters.h,v 1.6 2004/10/02 22:23:13 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTFILTERS
#define INCL_PLTESTFILTERS

#include "pltest.h"

class PLFilter;
class PLBmp;

class PLTestFilters: public PLTest
{
public:
  PLTestFilters(PLBmp* pBmp32, PLBmp* pBmp24, PLBmp* pBmp16, PLBmp* pBmp8);
  virtual ~PLTestFilters();

  virtual void RunTests ();

private:
  void testFilterWithBmp (const PLFilter& Filter, PLBmp& SrcBmp);
  void testDoubleFilter (const PLFilter& Filter);
  void testQuant (int DthPal, int Dth);
  void testFillRect ();
  void testFill ();
  void testRotate ();
  void testColorize ();
  void testFlipRGB (); 

  PLBmp * m_pBmp32;  
  PLBmp * m_pBmp24;
  PLBmp * m_pBmp16;
  PLBmp * m_pBmp8;

};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pltestfilters.h,v $
|      Revision 1.6  2004/10/02 22:23:13  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.5  2004/09/21 13:03:05  uzadow
|      Reenabled testFlipRGB, added 32-bit version of PLFilterColorize.
|
|      Revision 1.4  2004/08/04 14:53:36  uzadow
|      Added PLFilterColorize.
|
|      Revision 1.3  2003/07/27 18:08:38  uzadow
|      Added plfilterfliprgb
|
|      Revision 1.2  2003/02/27 22:46:59  uzadow
|      Added plfilterrotate.
|
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
