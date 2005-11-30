/*
/--------------------------------------------------------------------
|
|      $Id: plwinbmp.h,v 1.8 2005/07/12 17:44:51 artcom Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_WINBMP
#define INCL_WINBMP

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN  /* Prevent including <winsock*.h> in <windows.h> */
  #define VC_EXTRALEAN  // Exclude rarely-used stuff from Windows headers
  #include <windows.h>
#endif

//! This is the windows version of PLBmp. The internal storage format
//! is a windows DIB. It supports all color depths allowed by
//! windows: 1, 8, 16, 24, and 32 bpp. 
//!
//! The subset of the windows DIB format supported is as follows: The
//! DIB is stored so that header, palette, and bits are in one
//! buffer. The bottom line is stored first (biHeight must be > 0)
//! and the data is uncompressed (BI_RGB). Color tables for 16, 24,
//! and 32 bpp are not supported. biClrUsed is always 0. The palette
//! mode is DIB_RGB_COLORS. DIB_PAL_COLORS is not supported.
//!
//! Note that almost all real-life DIBs conform to this subset
//! anyway, so there shouldn't be any problems.
//!
//! <i>In the current version, some functions (notably CreateCopy) only
//! support 1, 8 and 32 bpp. Sorry!</i>
class PLWinBmp : public PLBmp
{

public:
  // Creates an empty bitmap.
  PLWinBmp ();

  //! Copy constructor
  PLWinBmp
    ( const PLWinBmp &Orig
    );

  //! Copy constructor
  PLWinBmp
    ( const PLBmpBase &Orig
    );

  //! Destroys the bitmap.
  virtual ~PLWinBmp ();

  //! Assignment operator.
  PLWinBmp &operator= (PLBmpBase const &Orig);

  //! Assignment operator.
  PLWinBmp &operator= (PLWinBmp const &Orig);

#ifdef _DEBUG
  virtual void AssertValid () const;    // Tests internal object state
#endif

  // PLWinBmp manipulation

  // Do a bitblt using the alpha channel of pSrPLBmp. Restricted to
  // 32 bpp.
  // Legacy routine. Use the Blt classes instead.
  void AlphaBlt (PLWinBmp * pSrPLBmp, int x, int y);

  // PLWinBmp information

  //! Returns the amount of memory used by the object.
  virtual long GetMemUsed ();

  //! Returns number of bytes used per line.
  virtual long GetBytesPerLine ();

  // Windows-specific interface

  //! Loads a bitmap from a windows resource (.rc or .res linked to
  //! the exe). Fails if the bitmap is compressed.
  virtual void CreateRes (HINSTANCE lh_ResInst, int ID);

  //! Takes a HBITMAP and converts it to a PLWinBmp.
  void CreateFromHBitmap (HBITMAP hBitMap);

  //! Takes an existing device-independent bitmap and converts it
  //! to a PLWinBmp.
  void CreateFromHDIBBitmap(BITMAPINFOHEADER* pBIH, HPALETTE hPal = NULL);

  //! Returns the size of the bitmap in pixels
  SIZE GetSize ();

  //! Access the windows bitmap structure. Using this structure, all
  //! standard DIB manipulations can be performed.
  BITMAPINFOHEADER * GetBMI ();

  // PLWinBmp output

  //! Draws the bitmap on the given device context using
  //! StretchDIBits.
  virtual void Draw (HDC hDC, int x, int y, DWORD rop = SRCCOPY);

  //! Draws the bitmap on the given device context using
  //! StretchDIBits. Scales the bitmap by Factor.
  virtual void StretchDraw (HDC hDC, int x, int y, double Factor, DWORD rop = SRCCOPY);

  //! Draws the bitmap on the given device context using
  //! StretchDIBits. Scales the bitmap so w is the width and
  //! h the height.
  virtual void StretchDraw (HDC hDC, int x, int y, int w, int h, DWORD rop = SRCCOPY);

  //! Draws a portion of the bitmap on the given device context
  virtual BOOL DrawExtract (HDC hDC, POINT pntDest, RECT rcSrc);

  //! Puts a copy of the bitmap in the clipboard
  void ToClipboard ();

  //! Reads the clipboard into the bitmap. uFormat can be either
  //! CF_BITMAP or CF_DIB.
  bool FromClipboard (UINT uFormat = CF_BITMAP);

  //! Gets a pointer to the bitmap bits. (Usually, using GetLineArray()
  //! is much easier!)
  BYTE * GetBits ();

  //! Copies the palette over from pSrPLBmp.
  void CopyPalette (PLWinBmp * pSrPLBmp);

  //! Traces the values in the palette via PLTRACE();
  void TracePalette();

  // Static functions

  //! Returns memory needed by a bitmap with the specified attributes.
  static long GetMemNeeded (LONG width, LONG height, WORD BitsPerPixel);

  //! Returns memory needed by bitmap bits.
  static long GetBitsMemNeeded (LONG width, LONG height, WORD BitsPerPixel);

  //! Returns memory needed by one line.
  static int GetLineMemNeeded (LONG width, WORD BitsPerPixel);

protected:

  // Protected callbacks

  //! Create a new empty DIB. Bits are uninitialized.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate (PLLONG Width, PLLONG Height, 
         const PLPixelFormat& pf);

  //! Creates a PLWinBmp from an existing bitmap pointer.
  //! Assumes that no memory is allocated before the call.
  virtual void internalCreate (BITMAPINFOHEADER* pBMI);

  //! Deletes memory allocated by member variables.
  virtual void freeMembers ();

  //! Initializes internal table of line addresses.
  virtual void initLineArray ();

  // Creates a copy of the current bitmap in a global memory block
  // and returns a handle to this block.
  virtual HANDLE createCopyHandle ();

  // Set color table pointer & pointer to bits based on m_pBMI.
  virtual void initPointers ();

  const PLPixelFormat& pixelFormatFromBMI(const BITMAPINFOHEADER * pBMI) const;

  // Member variables.

  BITMAPINFOHEADER * m_pBMI;  // Pointer to picture format information.
  BYTE * m_pBits;


private:
};


// Note that _both_ these copy constructors are needed. If only the 
// second one is there, the compiler generates a default copy 
// constructor anyway :-(.
inline PLWinBmp::PLWinBmp
    ( const PLWinBmp &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLWinBmp::PLWinBmp
    ( const PLBmpBase &Orig
    )
    : PLBmp ()
{
  internalCopy (Orig);
}

inline PLWinBmp & PLWinBmp::operator= ( PLBmpBase const &Orig)
{ 
  PLBmp::operator=(Orig);
  return *this;
}

inline PLWinBmp & PLWinBmp::operator= ( PLWinBmp const &Orig)
{
  PLBmp::operator=(Orig);
  return *this;
}


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plwinbmp.h,v $
|      Revision 1.8  2005/07/12 17:44:51  artcom
|      fixed win32 internalCreate
|
|      Revision 1.7  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.6  2004/09/11 14:15:40  uzadow
|      Comitted testimages, resized most of them.
|
|      Revision 1.5  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.4  2004/06/15 14:17:25  uzadow
|      First working version of PLSubBmp.
|
|      Revision 1.3  2003/08/03 12:03:22  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.2  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.14  2001/01/14 13:36:15  uzadow
|      Added PLAnyPicDecoder::GetFileFormat()
|
|      Revision 1.13  2000/11/21 20:29:39  uzadow
|      Added test project.
|
|      Revision 1.12  2000/11/02 21:28:47  uzadow
|      Fixed copy constructors.
|
|      Revision 1.11  2000/10/12 21:59:34  uzadow
|      Added CreateFromHDIBBitmap() and CopyPalette() to PLWinBmp
|      Added CF_DIB support to PLWinBmp::FromClipboard() (Richard Hollis)
|
|      Revision 1.10  2000/09/01 14:13:49  Administrator
|      Removed MFC from paintX, added MSCV paintX sample.
|
|      Revision 1.9  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.8  2000/01/17 23:37:12  Ulrich von Zadow
|      Corrected bug in assignment operator.
|
|      Revision 1.7  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
\--------------------------------------------------------------------
*/
