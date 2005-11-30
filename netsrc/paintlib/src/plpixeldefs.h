/*
/--------------------------------------------------------------------
|
|      $Id: plpixeldefs.h,v 1.5 2004/11/09 15:55:06 artcom Exp $
|
|      Pixel format definitions for MS Visual C++
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/


#ifndef INCL_PLPIXELDEFS
#define INCL_PLPIXELDEFS

#undef PL_PIXEL_BGRA_ORDER  // MS Windows ordering
#undef PL_PIXEL_RGBA_ORDER // GNU/Linux (at least)

// Define the pixel format for RGB-bitmaps. Change this if you change
// the order of the color components. 

// We have this pixel byte ordering
#define PL_PIXEL_BGRA_ORDER

#ifdef PL_PIXEL_RGBA_ORDER
 // To access in RGBA order
 #define PL_RGBA_RED    0
 #define PL_RGBA_GREEN  1
 #define PL_RGBA_BLUE   2
 #define PL_RGBA_ALPHA  3
#elif defined(PL_PIXEL_BGRA_ORDER)
 #define PL_RGBA_BLUE   0
 #define PL_RGBA_GREEN  1
 #define PL_RGBA_RED    2
 #define PL_RGBA_ALPHA  3
#else
  #error Pixel RGB bytes order not selected
#endif

// Note that the preceeding #defines aren't static const ints because
// this breaks preprocessor tests in several places.

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: plpixeldefs.h,v $
|      Revision 1.5  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.2  2000/11/09 22:23:36  uzadow
|      no message
|
|
\--------------------------------------------------------------------
*/
