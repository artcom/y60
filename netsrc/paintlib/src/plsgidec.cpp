/*
/--------------------------------------------------------------------
|
|      $Id: plsgidec.cpp,v 1.7 2004/09/11 12:41:35 uzadow Exp $
|      SGI rgb Decoder Class
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plsgidec.h"
#include "plexcept.h"

const PLWORD SGI_MAGIC = 474;

const int STORAGE_VERBATIM = 0;
const int STORAGE_RLE      = 1;

const int CMAP_NORMAL      = 0;
const int CMAP_DITHERED    = 1;  // not supported
const int CMAP_SCREEN      = 2;  // not supported
const int CMAP_COLORMAP    = 3;  // not supported


PLSGIDecoder::PLSGIDecoder
    ()
    : PLPicDecoder()
{
}


PLSGIDecoder::~PLSGIDecoder
    ()
{
}

void PLSGIDecoder::Open (PLDataSource * pDataSrc)
{
  Trace (2, "Decoding SGI.\n");
  PLPixelFormat pf;
  readHeader (&m_Header, pDataSrc);

  bool bGreyscale = (m_Header.ZSize == 1 && m_Header.Colormap == CMAP_NORMAL);

  switch (m_Header.ZSize)
  {
    case 1:
      if (bGreyscale) {
        pf = PLPixelFormat::L8;
      } else {
        pf = PLPixelFormat::I8;
      }
      break;
    case 2:
      throw PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Two-channel SGI RGB files are not supported.");
      break;
    case 3:
      pf = PLPixelFormat::X8R8G8B8;
      break;
    case 4:
      // Image has alpha channel
      pf = PLPixelFormat::A8R8G8B8;      
  }
  if (m_Header.Dimension == 1)
  {
    throw PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "One-dimensional SGI RGB files are not supported.");
  }
  
  
  SetBmpInfo (PLPoint (m_Header.XSize, m_Header.YSize), PLPoint(0, 0), pf);
}

void PLSGIDecoder::GetImage (PLBmpBase & Bmp)
{
  switch (m_Header.Storage) 
  {
    case STORAGE_VERBATIM:
      readUncompressed(Bmp);
      break;
    case STORAGE_RLE:
      readRLE(Bmp);
      break;
    default:
      throw PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, 
            "Unknown SGI RGB file compression format.");
  }
}


void PLSGIDecoder::readHeader
    ( SGIHeader * pHeader,
      PLDataSource * pDataSrc
    )
{
    pHeader->Magic     = ReadMWord (pDataSrc);
    pHeader->Storage   = ReadByte (pDataSrc);
    pHeader->bpc       = ReadByte (pDataSrc);
    pHeader->Dimension = ReadMWord (pDataSrc);
    pHeader->XSize     = ReadMWord (pDataSrc);
    pHeader->YSize     = ReadMWord (pDataSrc);
    pHeader->ZSize     = ReadMWord (pDataSrc);
    pHeader->PixMin    = ReadMLong (pDataSrc);
    pHeader->PixMax    = ReadMLong (pDataSrc);
    ReadMLong(pDataSrc);
    memcpy (pHeader->Name, (char *)pDataSrc->ReadNBytes(80), 80);
    pHeader->Colormap  = ReadMLong (pDataSrc);
    pDataSrc->Skip(404);

    if (pHeader->Magic != SGI_MAGIC)
    {
      throw PLTextException (PL_ERRWRONG_SIGNATURE, "SGI decoder invoked for non-SGI file.");
    }
}

void PLSGIDecoder::readUncompressed
    ( PLBmpBase & Bmp
    )
{
  if (GetBitsPerPixel() == 8) 
  {
    PLBYTE ** ppLine = Bmp.GetLineArray();
    for (int y = 0; y < GetHeight(); y++) 
    {
      memcpy(ppLine[GetHeight()-y-1], m_pDataSrc->ReadNBytes(GetWidth()), GetWidth());
    }
  } 
  else
  { 
    PLBYTE ** ppLine = Bmp.GetLineArray();
    for (int z = 0; z < m_Header.ZSize; z++)
    {
      for (int y = GetHeight()-1; y >=0 ; y--) 
      {
        PLBYTE * pLine = ppLine[y];
        for (int x = 0; x < GetWidth(); x++)
        {
          switch (z)
          {
            case 0:
              pLine[x*4+PL_RGBA_RED] = ReadByte (m_pDataSrc);
              break;
            case 1:
              pLine[x*4+PL_RGBA_GREEN] = ReadByte (m_pDataSrc);
              break;
            case 2:
              pLine[x*4+PL_RGBA_BLUE] = ReadByte (m_pDataSrc);
              break;
            case 3:
              pLine[x*4+PL_RGBA_ALPHA] = ReadByte (m_pDataSrc);
              break;
          }
        }
      }
    }
  }
}

void PLSGIDecoder::readRLE
    ( PLBmpBase & Bmp
    )
{
  int xsize = m_Header.XSize;
  int ysize = m_Header.YSize;
  int zsize = m_Header.ZSize;

  /* size of rle offset and length tables */
  int tablen = ysize * zsize * sizeof(long);

  unsigned long * RowOffsets = (unsigned long *)malloc(tablen);
  for (int i = 0; i<ysize*zsize; i++)
    RowOffsets[i] = ReadMLong(m_pDataSrc);
  
  /* read run length table */ 
  unsigned long * RowLengths = (unsigned long *)malloc(tablen);
  for (int i = 0; i<ysize*zsize; i++)
    RowLengths[i] = ReadMLong(m_pDataSrc);

  for (int channel = 0; channel < zsize; channel++)
  {
    int ChannelOffset;
    if (zsize == 1)
      ChannelOffset = 0;
    else
      switch(channel) 
      {
        case 0:
          ChannelOffset = PL_RGBA_RED;
          break;
        case 1:
          ChannelOffset = PL_RGBA_GREEN;
          break;
        case 2:
          ChannelOffset = PL_RGBA_BLUE;
          break;
        case 3:
          ChannelOffset = PL_RGBA_ALPHA;
          break;
      }
    for (int y = 0; y < ysize; y++) 
    {
      unsigned long SourceOffset = RowOffsets[y+channel*ysize];
      PLBYTE * pDestPixels = Bmp.GetLineArray()[Bmp.GetHeight()-y-1];
      m_pDataSrc->Seek(SourceOffset);
      int RowLen = RowLengths[y+channel*ysize];
      PLBYTE * pSrcLine = m_pDataSrc->ReadNBytes(RowLen);
      PLBYTE * pSrcData = pSrcLine;
      bool bDone = false;
      while (!bDone && pSrcData-pSrcLine != RowLen)
      {
        PLBYTE Count = *pSrcData++;
        if (Count == 0) 
          bDone = true;
        else
        {
          if (Count & 0x80)
          {
            Count &= 0x7F;
            while (Count--)
            {
              *(pDestPixels+ChannelOffset) = *pSrcData++;
              pDestPixels+=Bmp.GetBitsPerPixel()/8;
            }
          }
          else
          {
            PLBYTE Pixel = *pSrcData++;
            while (Count--)
            {
              *(pDestPixels+ChannelOffset) = Pixel;
              pDestPixels+=Bmp.GetBitsPerPixel()/8;
            }
          }
        }
      }
    }
  }
  delete[] RowOffsets;
  delete[] RowLengths;
}

/*
/--------------------------------------------------------------------
|
|      $Log: plsgidec.cpp,v $
|      Revision 1.7  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.6  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.5  2004/08/20 20:13:55  uzadow
|      SGI decoder fixed for 8bpp images.
|
|      Revision 1.4  2004/08/20 19:51:53  uzadow
|      Now works.
|
|      Revision 1.3  2004/08/20 14:39:38  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.2  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.1  2003/06/01 21:12:06  uzadow
|      Added sgi rgb file format.
|
|
|
\--------------------------------------------------------------------
*/
