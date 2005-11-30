/*
/--------------------------------------------------------------------
|
|      $Id: pltgadec.cpp,v 1.9 2004/09/11 12:41:35 uzadow Exp $
|      Targa Decoder Class
|
|      Targa file decoder. Decodes 8, 15, 16, 24 and 32 bpp
|      targa files (compressed and uncompressed) and returns a 32
|      bpp bitmap. Preserves the alpha channel.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltgadec.h"
#include "plexcept.h"


PLTGADecoder::PLTGADecoder
    ()
    : PLPicDecoder()
    // Creates a decoder
{
}


PLTGADecoder::~PLTGADecoder
    ()
{
}

void PLTGADecoder::Open (PLDataSource * pDataSrc)
{
  Trace (2, "Decoding TGA.\n");

  readTgaHeader (&m_TgaHead, pDataSrc);

  PLPixelFormat pf = PLPixelFormat::DONTCARE;

  switch (m_TgaHead.PixelDepth)
  {
    case 8:
      if (m_TgaHead.ImageType == TGA_Mono ||
          m_TgaHead.ImageType == TGA_RLEMono)
        pf = PLPixelFormat::L8;
       else
        pf = PLPixelFormat::I8;
      break;           
    case 16:
    case 32:
      pf = PLPixelFormat::A8R8G8B8;
      break;
    case 15:
    case 24:
      pf = PLPixelFormat::X8R8G8B8;
      break;
    default:
      raiseError (PL_ERRFORMAT_UNKNOWN,
                "TGA decoder: Unknown bits per pixel in file.");

  }
  SetBmpInfo (PLPoint (m_TgaHead.ImageWidth, m_TgaHead.ImageHeight), 
              PLPoint(0, 0), pf);
}

void PLTGADecoder::GetImage (PLBmpBase & Bmp)
{
  // Read the color map data (Version 1.0 and 2.0).
  if (m_TgaHead.CmapType != 0)
    readPalette (m_TgaHead.CmapIndex,
                 m_TgaHead.CmapLength,
                 m_TgaHead.CmapEntrySize,
                 &Bmp,
                 m_pDataSrc);

  readImage (&m_TgaHead, &Bmp, m_pDataSrc);
}


void PLTGADecoder::readTgaHeader
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      PLDataSource * pDataSrc
    )
{
  // Read the TGA header (Version 1.0 and 2.0).
  pTgaHead->IdLength      = ReadByte (pDataSrc);
  pTgaHead->CmapType      = ReadByte (pDataSrc);
  pTgaHead->ImageType     = ReadByte (pDataSrc);
  pTgaHead->CmapIndex     = ReadIWord (pDataSrc);
  pTgaHead->CmapLength    = ReadIWord (pDataSrc);
  pTgaHead->CmapEntrySize = ReadByte (pDataSrc);
  pTgaHead->X_Origin      = ReadIWord (pDataSrc);
  pTgaHead->Y_Origin      = ReadIWord (pDataSrc);
  pTgaHead->ImageWidth    = ReadIWord (pDataSrc);
  pTgaHead->ImageHeight   = ReadIWord (pDataSrc);
  pTgaHead->PixelDepth    = ReadByte (pDataSrc);
  pTgaHead->ImagDesc      = ReadByte (pDataSrc);

  // Skip image ID
  pDataSrc->Skip (pTgaHead->IdLength);
}


void PLTGADecoder::readPalette
    ( int StartIndex,           // Index of first palette entry.
      int Length,               // Number of palette entries stored.
      int EntrySize,            // Size of palette entries in bits.
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    )
    // Reads the TGA palette and creates a windows palette.
{
  int   i;

  for (i=StartIndex; i<StartIndex+Length; i++)
  {
    PLPixel32 CurEntry = readPixel32 (EntrySize, pDataSrc);
    pBmp->SetPaletteEntry (i, CurEntry);
  }
}


void PLTGADecoder::readImage
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    )
{
  bool bCompressed;

  if (pTgaHead->ImageType == TGA_Mono ||
      pTgaHead->ImageType == TGA_RLEMono)
    pBmp->SetGrayPalette ();

  switch (pTgaHead->ImageType)
  {
    case TGA_Map:
    case TGA_RGB:
    case TGA_Mono:
      bCompressed = false;
      break;
    case TGA_RLEMap:
    case TGA_RLERGB:
    case TGA_RLEMono:
      bCompressed = true;
      break;
    default:
      raiseError (PL_ERRFORMAT_UNKNOWN, "Unknown TGA image type.");
  }
  readData (pTgaHead, bCompressed, pBmp, pDataSrc);
}


void PLTGADecoder::readData
    ( TGAHEADER * pTgaHead,       // Pointer to TGA header structure
      bool bCompressed,
      PLBmpBase * pBmp,
      PLDataSource * pDataSrc
    )
{
  int Width = pTgaHead->ImageWidth;
  int Height = pTgaHead->ImageHeight;
  int bpp = pTgaHead->PixelDepth;

  // Bits 4 & 5 of the Image Descriptor byte control the ordering of
  // the pixels.
  bool bXReversed = ((pTgaHead->ImagDesc & 16) == 16);
  bool bYReversed = ((pTgaHead->ImagDesc & 32) == 32);

  PLBYTE * pDest;
  PLBYTE ** pLineArray = pBmp->GetLineArray();

  int y;

  for (y=0; y < Height; y++)
  {
    if (bYReversed)
      pDest = pLineArray[y];
      else
      pDest = pLineArray[Height-y-1];

    if (!bCompressed)
      expandUncompressedLine (pDest, Width, bXReversed, bpp, pDataSrc);
      else
      expandCompressedLine (pDest, Width, bXReversed, bpp, pDataSrc);
  }
}


void PLTGADecoder::expandUncompressedLine
    ( PLBYTE * pDest,
      int Width,
      bool bReversed,
      int bpp,
      PLDataSource * pDataSrc
    )
{
  int x;

  for (x=0; x<Width; x++)
  {
    if (bpp > 8)
    {
      *((PLPixel32 *)pDest) = readPixel32 (bpp, pDataSrc);
      pDest += 4;
    }
    else
    {
      *pDest = readPixel8 (bpp, pDataSrc);
      pDest ++;
    }
  }
}


void PLTGADecoder::expandCompressedLine
    ( PLBYTE * pDest,
      int Width,
      bool bReversed,
      int bpp,
      PLDataSource * pDataSrc
    )
{
  int  x;
  int  i;
  PLBYTE Count;

  for (x=0; x<Width; )
  {
    Count = ReadByte (pDataSrc);
    if (Count & 128)
    { // RLE-Encoded packet
      Count -= 127; // Calculate real repeat count.
      if (bpp > 8)
      {
        *((PLPixel32 *)pDest) = readPixel32 (bpp, pDataSrc);
        for (i=1; i<Count; i++)
          *((PLPixel32 *)(pDest+i*4)) = *(PLPixel32 *)pDest;
      }
      else
      {
        *pDest = readPixel8 (bpp, pDataSrc);
        for (i=1; i<Count; i++)
          *(pDest+i) = *pDest;
      }
    }
    else
    { // Raw packet
      Count += 1; // Calculate real repeat count.
      for (i=0; i<Count; i++)
      {
        if (bpp > 8)
          *((PLPixel32 *)(pDest+i*4)) = readPixel32 (bpp, pDataSrc);
        else
          *(pDest+i) = readPixel8 (bpp, pDataSrc);
      }
    }
    if (bpp > 8)
      pDest += Count*4;
    else
      pDest += Count;
    x += Count;
  }
}


PLPixel32 PLTGADecoder::readPixel32
    ( int bpp,
      PLDataSource * pDataSrc
    )
    // The decoder could be made much faster if the big switch (bpp)
    // statement was taken out of the inner loop. On the other hand,
    // doing that would cause a lot of code to be repeated half a
    // dozen times...
{
  PLPixel32 Dest;
  PLWORD Src;
  PLBYTE * pCurEntry;

  switch (bpp)
  {
    case 15:
    case 16:
      Src = ReadIWord(pDataSrc);
      if (bpp == 16)
        Dest.Set (( Src >> 7 ) & 0x0F8,     // red
                  ( Src >> 2 ) & 0x0F8,     // green
                  ( Src & 0x1F ) * 8,       // blue
                  (PLBYTE)(Src & 32786 >> 8));
       else
        Dest.Set (( Src >> 7 ) & 0x0F8,     // red
                  ( Src >> 2 ) & 0x0F8,     // green
                  ( Src & 0x1F ) * 8,       // blue
                  0xFF);
      break;
    case 24:
      pCurEntry = pDataSrc->ReadNBytes (3);
      Dest.Set (*(pCurEntry+2), *(pCurEntry+1), *(pCurEntry), 0xFF);
      break;
    case 32:
      pCurEntry = pDataSrc->ReadNBytes (4);
      Dest.Set (*(pCurEntry+2), *(pCurEntry+1), *(pCurEntry), *(pCurEntry+3));
      break;
  }
  return Dest;
}

PLBYTE PLTGADecoder::readPixel8
    ( int bpp,
      PLDataSource * pDataSrc
    )
{
  PLBYTE Dest;

  PLASSERT (bpp == 8);

  Dest = ReadByte(pDataSrc);

  return Dest;
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltgadec.cpp,v $
|      Revision 1.9  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.8  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.7  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.6  2002/08/04 21:20:41  uzadow
|      no message
|
|      Revision 1.5  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.3  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.7  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.6  2000/10/24 23:02:28  uzadow
|      Fixed exception handling.
|
|      Revision 1.5  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.4  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/10/03 18:50:52  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
--------------------------------------------------------------------
*/
