/*
/--------------------------------------------------------------------
|
|      $Id: plpngdec.cpp,v 1.14 2005/07/12 13:10:34 artcom Exp $
|      PNG Decoder Class
|
|      PNG file decoder. Uses LibPng to do the actual decoding.
|      PNG supports many pixel formats not supported by paintlib.
|      These pixel formats are converted to the nearest paintlib
|      equivalent. Images with less or more than 8 bits per channel
|      are converted to 8 bits per channel. Images with 16-bit
|      palettes or grayscale images with an alpha channel are
|      returned as full 32-bit RGBA bitmaps.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plpngdec.h"
#include "plexcept.h"
#include "png.h"


PLPNGDecoder::PLPNGDecoder
    ()
    : PLPicDecoder()
    // Creates a decoder
{
}


PLPNGDecoder::~PLPNGDecoder
    ()
{
}



void my_read_data(png_structp png_ptr, png_bytep data, png_size_t length)
{
  // todo : check data erasing
  PLBYTE *ptr;
  PLDataSource* pSourceInfo=(PLDataSource*)png_get_io_ptr(png_ptr);

  ptr = pSourceInfo->ReadNBytes(length);
  memcpy(data,ptr,length);
}

void PLPNGDecoder::Open (PLDataSource * pDataSrc)
{
  png_uint_32 width, height;
  int interlace_type;

  m_png_ptr = png_create_read_struct
              (PNG_LIBPNG_VER_STRING,
               (void *)NULL,
               user_error_fn,
               user_warning_fn);
  PLASSERT (m_png_ptr);

  m_info_ptr = png_create_info_struct(m_png_ptr);
  PLASSERT (m_info_ptr);

  png_set_read_fn(m_png_ptr, (void*)pDataSrc, my_read_data);

  /* The call to png_read_info() gives us all of the information from the
   * PNG file before the first IDAT (image data chunk).
   */
  png_read_info(m_png_ptr, m_info_ptr);

  png_get_IHDR(m_png_ptr, m_info_ptr, &width, &height, &m_bit_depth, &m_color_type,
               &interlace_type, NULL, NULL);

  PLPixelFormat pf;
  
  switch (m_color_type)
  {
    case PNG_COLOR_TYPE_RGB:  
      pf = PLPixelFormat::R8G8B8;
      break;
    case PNG_COLOR_TYPE_RGB_ALPHA:      
      pf = PLPixelFormat::A8R8G8B8;
      break;
    case PNG_COLOR_TYPE_GRAY:
      pf = PLPixelFormat::L8;
      break;
    case PNG_COLOR_TYPE_GRAY_ALPHA:      
      png_set_gray_to_rgb(m_png_ptr);
      png_set_expand(m_png_ptr);      
      pf = PLPixelFormat::A8R8G8B8;
      break;
    case PNG_COLOR_TYPE_PALETTE:
      if (m_bit_depth != 16)        
        pf = PLPixelFormat::I8;
      else
      { // 16-bit palette image
        png_set_expand(m_png_ptr);        
        pf = PLPixelFormat::R8G8B8;
      }
      break;
  }

  if (pf.GetBitsPerPixel() == 32 || pf.GetBitsPerPixel() == 24)
  { 
    png_set_bgr (m_png_ptr);
  }

  SetBmpInfo (PLPoint (width, height), PLPoint(0,0), pf);

  png_uint_32 XRes, YRes;
  int UnitType;
  png_get_pHYs (m_png_ptr, m_info_ptr, &XRes, &YRes,
                &UnitType);
  if (UnitType == PNG_RESOLUTION_METER)
    m_Resolution = PLPoint (int (XRes/39.37f+0.5), int (YRes/39.37f+0.5));
}

void PLPNGDecoder::GetImage (PLBmpBase & Bmp)
{
  if (m_color_type == PNG_COLOR_TYPE_GRAY)
  {
    int NumColors = 1<<(m_bit_depth);
    for (int i=0; i<NumColors; i++)
    {
      PLBYTE CurColor = static_cast<PLBYTE>((i*255)/(NumColors-1));
      Bmp.SetPaletteEntry(PLBYTE(i), CurColor, CurColor, CurColor, 0xFF);
    }
  }

  if (m_color_type == PNG_COLOR_TYPE_PALETTE)
  {
    png_color* ppng_color_tab=NULL;

    int   nbColor=0;

    png_get_PLTE(m_png_ptr, m_info_ptr, &ppng_color_tab, &nbColor);

    for (int i=0; i<nbColor; i++)
    {
      Bmp.SetPaletteEntry(i,
                            (*(ppng_color_tab+i)).red,
                            (*(ppng_color_tab+i)).green,
                            (*(ppng_color_tab+i)).blue,
                            static_cast<PLBYTE>(0xFF));
    }
  }

  if (m_bit_depth == 16)
    png_set_strip_16(m_png_ptr);
  if (m_bit_depth < 8)
    png_set_packing(m_png_ptr);

  PLBYTE ** pLineArray = Bmp.GetLineArray();
  png_read_image(m_png_ptr, pLineArray);

  /* read rest of file, and get additional chunks in info_ptr - REQUIRED */
  png_read_end(m_png_ptr, m_info_ptr);

  /* clean up after the read, and free any memory allocated - REQUIRED */
  png_destroy_read_struct(&m_png_ptr, &m_info_ptr, (png_infopp)NULL);
}


void PLPNGDecoder::user_error_fn
    ( png_structp /*png_ptr*/,
      png_const_charp error_msg
    )
{
  raiseError (PL_ERRFORMAT_UNKNOWN, (char *)error_msg);
}

void PLPNGDecoder::user_warning_fn
    ( png_structp /*png_ptr*/,
      png_const_charp warning_msg
    )
{
  PLTRACE ((char *)warning_msg);
}

/*
/--------------------------------------------------------------------
|
|      $Log: plpngdec.cpp,v $
|      Revision 1.14  2005/07/12 13:10:34  artcom
|      64 bit-fixes, png encoder fix.
|
|      Revision 1.13  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.12  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.11  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.10  2004/07/28 11:41:07  uzadow
|      PNG decoder now decodes directly to 24 bpp
|
|      Revision 1.9  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.8  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.7  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.6  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.5  2001/10/16 18:00:19  uzadow
|      Linux compatibility
|
|      Revision 1.4  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.3  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.2  2001/09/28 19:50:56  uzadow
|      Added some 24 bpp stuff & other minor features.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.11  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.10  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.9  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.8  2000/07/11 17:11:01  Ulrich von Zadow
|      Added support for RGBA pixel ordering (Jose Miguel Buenaposada Biencinto).
|
|      Revision 1.7  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/04 18:35:23  Ulrich von Zadow
|      no message
|
|      Revision 1.5  1999/12/08 16:31:40  Ulrich von Zadow
|      Unix compatibility
|
|      Revision 1.4  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
