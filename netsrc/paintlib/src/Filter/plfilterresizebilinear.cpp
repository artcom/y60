/*
/--------------------------------------------------------------------
|
|      $Id: plfilterresizebilinear.cpp,v 1.16 2005/04/29 10:16:42 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include <iostream>
#include "plfilterresizebilinear.h"
#include "plbitmap.h"
#include "plexcept.h"
#include "pl2passscale.h"

#include <cstring>

using namespace std;

PLFilterResizeBilinear::PLFilterResizeBilinear (int NewXSize, int NewYSize)
  : PLFilterResize (NewXSize, NewYSize)
{
}

void PLFilterResizeBilinear::Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) const
{
    // Create a new Bitmap 
    pBmpDest->Create(m_NewXSize,
        m_NewYSize,
        pBmpSource->GetPixelFormat(),
        NULL, 0, 
        pBmpSource->GetResolution());

    // Create a Filter Class from template
    PLBilinearContribDef f(0.64);
    if (pBmpSource->GetPixelFormat() == PLPixelFormat::A8R8G8B8 ||
        pBmpSource->GetPixelFormat() == PLPixelFormat::A8B8G8R8 || 
        pBmpSource->GetPixelFormat() == PLPixelFormat::X8R8G8B8 || 
        pBmpSource->GetPixelFormat() == PLPixelFormat::X8B8G8R8) 
    {
        C2PassScale <CDataRGBA_UBYTE> sS(f);
        sS.Scale ((CDataRGBA_UBYTE::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGBA_UBYTE::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::R8G8B8 ||
               pBmpSource->GetPixelFormat() == PLPixelFormat::B8G8R8)
    {
        C2PassScale <CDataRGB_UBYTE> sS(f);
        sS.Scale ((CDataRGB_UBYTE::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGB_UBYTE::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::L8) {
        C2PassScale <CDataL_UBYTE> sS(f);
        sS.Scale ((CDataL_UBYTE::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataL_UBYTE::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::L16) {
        C2PassScale <CDataL_UWORD> sS(f);
        sS.Scale ((CDataL_UWORD::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataL_UWORD::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::L16S) {
        C2PassScale <CDataL_SWORD> sS(f);
        sS.Scale ((CDataL_SWORD::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataL_SWORD::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::A16B16G16R16) {
        C2PassScale <CDataRGBA_HALF> sS(f);
        sS.Scale ((CDataRGBA_HALF::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGBA_HALF::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::B16G16R16) {
        C2PassScale <CDataRGB_HALF> sS(f);
        sS.Scale ((CDataRGB_HALF::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGB_HALF::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::A32B32G32R32) {
        C2PassScale <CDataRGBA_FLOAT> sS(f);
        sS.Scale ((CDataRGBA_FLOAT::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGBA_FLOAT::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else if (pBmpSource->GetPixelFormat() == PLPixelFormat::B32G32R32) {
        C2PassScale <CDataRGB_FLOAT> sS(f);
        sS.Scale ((CDataRGB_FLOAT::_RowType *) pBmpSource->GetLineArray(), 
                pBmpSource->GetWidth(), 
                pBmpSource->GetHeight(), 
                (CDataRGB_FLOAT::_RowType *) pBmpDest->GetLineArray(),
                pBmpDest->GetWidth(),
                pBmpDest->GetHeight());
    } else {
        // non-supported pixel format
        throw PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, 
                    string(pBmpSource->GetPixelFormat().GetName()+
                    " unsupported in FilterResizeBilinear").c_str());
        //PLASSERT(false);
    }
}

/*
/--------------------------------------------------------------------
|
|      $Log: plfilterresizebilinear.cpp,v $
|      Revision 1.16  2005/04/29 10:16:42  artcom
|      - fixed 32-bit XRGB format support
|
|      Revision 1.15  2005/04/21 09:17:45  artcom
|      added 16-bit signed greyscale pixel type (nice for dicom)
|      bilinear resize now supported 16-bit signed & unsigned greyscale formats
|      png encoder now supports 16-bit greyscale format
|
|      Revision 1.14  2005/03/08 14:22:37  artcom
|      Added resizing of Luminace Bitmaps
|
|      Revision 1.13  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.12  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.11  2004/09/09 16:52:45  artcom
|      refactored PixelFormat
|
|      Revision 1.10  2004/07/28 13:55:48  artcom
|      Added 24 bpp support to plfilterresizebilinear.
|
|      Revision 1.9  2004/06/15 10:26:13  uzadow
|      Initial nonfunctioning version of plbmpbase.
|
|      Revision 1.8  2003/11/05 15:17:26  artcom
|      Added ability to specify initial data in PLBitmap::Create()
|
|      Revision 1.7  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.6  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.5  2001/10/16 17:12:27  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.4  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|      Revision 1.3  2001/09/30 16:57:25  uzadow
|      Improved speed of 2passfilter.h, code readability changes.
|
|      Revision 1.2  2001/09/16 20:57:17  uzadow
|      Linux version name prefix changes
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2001/01/15 15:05:31  uzadow
|      Added PLBmp::ApplyFilter() and PLBmp::CreateFilteredCopy()
|
|      Revision 1.4  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/12/08 16:31:40  Ulrich von Zadow
|      Unix compatibility
|
|      Revision 1.2  1999/12/08 15:39:46  Ulrich von Zadow
|      Unix compatibility changes
|
|      Revision 1.1  1999/10/21 16:05:18  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:45  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
