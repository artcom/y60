/*
/--------------------------------------------------------------------
|
|      $Id: pliff85dec.cpp,v 1.6 2004/09/11 12:41:35 uzadow Exp $
|      Electronic Arts IFF 85 Decoder Class
|
|       REFERENCES :
|           Jerry Morrison, Electronic Arts; "EA IFF 85" Standard for
|           Interchange Format Files; January 14, 1985
|           Available from http://www.wotsit.org as iff.zip
|
|           Jerry Morrison, Electronic Arts; "ILBM" IFF Interleaved Bitmap;
|           January 17, 1986
|           Available from http://www.wotsit.org as ilbm.zip
|
|           Carolyn Scheppner - Commodore Amiga Technical Support;
|           "Intro to Amiga IFF ILBM Files and Amiga Viewmodes";
|           USENET posting to comp.graphics, comp.sys.amiga;
|           August 25th, 1988
| 
|      Rupert Welch (rafw@mindless.com) - December 2003
|
|      Copyright (c) 2003-2004 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if defined(__BCPLUSPLUS__)
#pragma hdrstop
#endif

#include "pliff85dec.h"
#include "pliff85.h"
#include "plexcept.h"

#if defined(_MSC_VER) // && (_MSC_VER <= 1200)
#pragma warning(push, 3)
#endif
#include <algorithm>
#include <vector>
#if defined(_MSC_VER) // && (_MSC_VER <= 1200)
#pragma warning(pop)
#endif

// Enabling detailed tracing may slow things down considerably.
// #define DETAILED_TRACE 0
#if DETAILED_TRACE
#include <sstream>
#endif

#ifndef _MSC_VER
#include <math.h>
#endif

// Creates a decoder
PLIFF85Decoder::PLIFF85Decoder()
  : PLPicDecoder(),
    m_formType(0),
    m_bitmapHeader(),
    m_pal(),
    m_viewMode(0)
{}


PLIFF85Decoder::~PLIFF85Decoder()
{}


void PLIFF85Decoder::Open(PLDataSource * pDataSrc)
{

  // Check if the file is a valid IFF-85 file or not
  Trace(2, "Decoding IFF-85 header.\n");

  PLIFF85::Chunk chunk;

  if (PLIFF85::MakeID(reinterpret_cast<const char*> (pDataSrc->GetBufferPtr(sizeof chunk))) !=
        PLIFF85::ID_FORM)
  {
    raiseError(PL_ERRWRONG_SIGNATURE, "File is not a single-form IFF.");
  }

  readChunkHeader(chunk, pDataSrc);

  size_t totalSize = chunk.ckSize + sizeof chunk;
  size_t lumpIndex = sizeof chunk;

  // Now read the form type - we only know how to handle PBM and ILBM.
  chunk.ckID   = ReadMLong(pDataSrc);
  if (chunk.ckID == PLIFF85::ID_PBM)
  {
    Trace(2, "Form type: PBM\n");
  }
  else if (chunk.ckID == PLIFF85::ID_ILBM)
  {
    Trace(2, "Form type: ILBM\n");
  }
  else
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Unknown form type.");
  }

  m_formType = chunk.ckID;
  lumpIndex += sizeof chunk.ckID;

  int numCMapElements = 0;

  m_viewMode = 0;

  // Now loop through the chunks, processing the header and cmap.
  // Stop when we find the body, so the data source is left in the correct
  // position to read it.
  bool  readBitmapHeader    = false;
  bool  readPalette         = false;
  bool  foundBody           = false;
  bool  done                = false;

  PLIFF85::LONG bodySize    = 0;

  while (!done)
  {
    readChunkHeader(chunk, pDataSrc);
    lumpIndex += sizeof chunk;

    if (chunk.ckID == PLIFF85::ID_BMHD)
    {
      if (size_t(chunk.ckSize) != sizeof m_bitmapHeader)
      {
        raiseError(PL_ERRFORMAT_UNKNOWN, "Unexpected header size.");
      }

      m_bitmapHeader.w                  = ReadMWord(pDataSrc);
      m_bitmapHeader.h                  = ReadMWord(pDataSrc);
      m_bitmapHeader.x                  = ReadMWord(pDataSrc);
      m_bitmapHeader.y                  = ReadMWord(pDataSrc);
      m_bitmapHeader.nPlanes            = ReadByte(pDataSrc);
      m_bitmapHeader.masking            = ReadByte(pDataSrc);
      m_bitmapHeader.compression        = ReadByte(pDataSrc);
      m_bitmapHeader.pad1               = ReadByte(pDataSrc);
      m_bitmapHeader.transparentColor   = ReadMWord(pDataSrc);
      m_bitmapHeader.xAspect            = ReadByte(pDataSrc);
      m_bitmapHeader.yAspect            = ReadByte(pDataSrc);
      m_bitmapHeader.pageWidth          = ReadMWord(pDataSrc);
      m_bitmapHeader.pageHeight         = ReadMWord(pDataSrc);

      switch (m_bitmapHeader.masking)
      {
        case PLIFF85::mskNone:
          Trace(2, "No masking.\n");
          break;
        case PLIFF85::mskHasMask:
          Trace(2, "Has mask plane.\n");
          break;
        case PLIFF85::mskHasTransparentColor:
          Trace(2, "Has transparent colour.\n");
          break;
        case PLIFF85::mskLasso:
          Trace(2, "Lasso");
          break;
        default:
          raiseError(PL_ERRFORMAT_UNKNOWN, "Unknown masking technique.");
          break;
      }

      switch (m_bitmapHeader.compression)
      {
        case PLIFF85::cmpNone:
          Trace(2, "No compression.\n");
          break;
        case PLIFF85::cmpByteRun1:
          Trace(2, "Byte run encoding.\n");
          break;
        default:
          raiseError(PL_ERRFORMAT_UNKNOWN, "Unknown compression method.");
          break;
      }
      readBitmapHeader = true;
    }
    else if (chunk.ckID == PLIFF85::ID_CMAP)
    {
      // The palette.
      PLCOMPILER_ASSERT(sizeof(PLIFF85::ColorRegister) == 3);

      numCMapElements = chunk.ckSize / 3;

      for (int i = 0; i < numCMapElements; ++i)
      {
        const PLIFF85::ColorRegister * pElement =
          reinterpret_cast<const PLIFF85::ColorRegister *> (pDataSrc->ReadNBytes(3));
        m_pal[i].Set(pElement->red, pElement->green, pElement->blue, 0xFF);
      }
      readPalette = true;
    }
    else if (chunk.ckID == PLIFF85::ID_CAMG)
    {
      // The viewport mode.
      if (size_t(chunk.ckSize) != sizeof m_viewMode)
      {
        raiseError(PL_ERRFORMAT_UNKNOWN, "Unexpected CAMG size.");
      }
      m_viewMode = ReadMLong(pDataSrc);
      #if DETAILED_TRACE
        std::ostringstream strTrace;
        strTrace << "View mode " << std::hex << std::showbase << m_viewMode << ".\n";
        Trace(2, strTrace.str().c_str());
      #endif
    }
    else if (chunk.ckID == PLIFF85::ID_BODY)
    {
      bodySize      = chunk.ckSize;
      foundBody     = true;
      done          = true;
    }
    else
    {
      // Some other chunk type - ignore it.
      pDataSrc->Skip(chunk.ckSize);
    }

    if (!done)
    {
      lumpIndex += chunk.ckSize;
    }

    if (lumpIndex >= totalSize - 1)
    {
      done = true;
    }
  }

  // We must have seen a header, cmap and body at this point.
  if (!readBitmapHeader)
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Header not found.");
  }

  if ((!readPalette) && (m_bitmapHeader.nPlanes <= 8))
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Palette not found.");
  }

  if (!foundBody)
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Body not found.");
  }

  // Check the body is the expected size ...
  if (m_bitmapHeader.compression == PLIFF85::cmpNone)
  {
    if (bodySize != getBytesPerRow() * m_bitmapHeader.h)
    {
      raiseError(PL_ERRFORMAT_UNKNOWN, "Unexpected body size.");
    }
  }

  // ... and that it does not extend beyond the end of the form.
  if (lumpIndex + bodySize > totalSize)
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Body extends beyond form.");
  }

  // Too many CMAP entries probably indicates an error.
  const int maxExpectedCMapElements = int(((m_viewMode & PLIFF85::viewHAM) != 0) ?
    pow(2, m_bitmapHeader.nPlanes - 2) :
    pow(2, m_bitmapHeader.nPlanes));

  if (numCMapElements > maxExpectedCMapElements)
  {
    raiseError(PL_ERRFORMAT_UNKNOWN, "Too many CMAP entries.");
  }

  PLPixelFormat pf;
  int DestBPP = m_bitmapHeader.nPlanes;
  bool bAlpha = (m_bitmapHeader.masking == PLIFF85::mskHasTransparentColor);

  if ((DestBPP > 8)
  ||  ((m_viewMode & PLIFF85::viewHAM) != 0)
  ||  (m_bitmapHeader.masking == PLIFF85::mskHasTransparentColor))
  {
    if (bAlpha) {
      pf = PLPixelFormat::A8R8G8B8;
    } else {
      pf = PLPixelFormat::X8R8G8B8;
    }
  }
  else 
  {
    pf = PLPixelFormat::I8;
  }

  SetBmpInfo(PLPoint(m_bitmapHeader.w, m_bitmapHeader.h), PLPoint(0, 0), pf);
} // Open

void PLIFF85Decoder::GetImage(PLBmpBase & Bmp)
{
  Trace(2, "Decoding IFF-85 body.\n");

  if (GetBitsPerPixel() == 8)
  {
    Bmp.SetPalette(&m_pal[0]);
  }

  // Decode each row into local storage, further processing depends on
  // form type.
  const int bytes_per_row = getBytesPerRow();

  std::vector<PLBYTE> buf(bytes_per_row);

  for (int row = 0; row < m_bitmapHeader.h; ++row)
  {
    #if DETAILED_TRACE
      std::ostringstream strTrace;
      strTrace << "#Row " << row << ".\n";
      Trace(3, strTrace.str().c_str());
    #endif

    if (m_bitmapHeader.compression == PLIFF85::cmpByteRun1)
    {
      readCompressedRow(&buf[0], m_pDataSrc, bytes_per_row);
    }
    else
    {
      PLASSERT(m_bitmapHeader.compression == PLIFF85::cmpNone);
      readUncompressedRow(&buf[0], m_pDataSrc, bytes_per_row);
    }

    std::vector<PLBYTE> decodedBuf(m_bitmapHeader.w * GetBitsPerPixel() / 8);

    if (m_formType == PLIFF85::ID_PBM)
    {
      // The number of bytes we want to output may be less than the number
      // we have read in, as the input must have an even number of bytes per
      // plane (which can produce a lot of unnecessary padding for a PBM).
      const int bytes_per_row = m_bitmapHeader.w * GetBitsPerPixel() / 8;
      decodedBuf.assign(buf.begin(), buf.begin() + bytes_per_row);
    }
    else
    {
      PLASSERT(m_formType == PLIFF85::ID_ILBM);

      for (int bp = 0; bp < m_bitmapHeader.nPlanes; ++bp)
      {
        const int start_index = bp * bytes_per_row / m_bitmapHeader.nPlanes;
        for (int i = 0; i < m_bitmapHeader.w; ++i)
        {
          // Get the byte we're interested in.
          PLBYTE the_byte = buf[start_index + i / 8];
          // Isolate the bit we're interested in.
          the_byte &= static_cast<PLBYTE> (1 << (7 - (i % 8)));
          // Now shift the bit to the correct position for this plane.
          if ((7 - (i % 8)) > bp)
          {
            the_byte >>= 7 - (i % 8) - bp;
          }
          else
          {
            the_byte <<= bp - (7 - (i % 8));
          }
          decodedBuf[i] |= the_byte;
        }
      }
    }

    std::fill_n(Bmp.GetLineArray()[row], m_bitmapHeader.w * GetBitsPerPixel() / 8, 0);

    if ((m_viewMode & PLIFF85::viewHAM) != 0)
    {
      PLPixel32 prevPixel(0, 0, 0);

      for (int i = 0; i < m_bitmapHeader.w; ++i)
      {
        // Get the byte we're interested in.
        PLBYTE the_byte = decodedBuf[i];
        PLBYTE mod = static_cast<PLBYTE> (the_byte >> (m_bitmapHeader.nPlanes - 2));
        the_byte &= static_cast<PLBYTE> (~(3 << (m_bitmapHeader.nPlanes - 2)));

        // The top two bits tell us how to proceed.  The comments for each
        // case below assume a total of six planes.
        // Note: R=2, B=1, contrary to reference 3 above.  I think where it
        // refers to the last two bits xy, y is bit 6, x is bit 7.
        PLPixel32* pThePixel = &Bmp.GetLineArray32()[row][i];
        switch (mod)
        {
          case 0:
            // No modification.  Use planes 0-3 as normal color register index.
            *pThePixel = m_pal[the_byte];
            break;
          case 1:
            // Hold previous, replacing Blue component with bits from planes 0-3.
            pThePixel->Set(
              prevPixel.GetR(),
              prevPixel.GetG(),
              static_cast<PLBYTE> (the_byte << 4),
              prevPixel.GetA());
            break;
          case 2:
            // Hold previous, replacing Red component with bits from planes 0-3.
            pThePixel->Set(
              static_cast<PLBYTE> (the_byte << 4),
              prevPixel.GetG(),
              prevPixel.GetB(),
              prevPixel.GetA());
            break;
          case 3:
            // Hold previous, replacing Green component with bits from planes 0-3.
            pThePixel->Set(
              prevPixel.GetR(),
              static_cast<PLBYTE> (the_byte << 4),
              prevPixel.GetB(),
              prevPixel.GetA());
            break;
          default:
            PLASSERT(!"Impossible HAM mod value.");
            break;
        }
        prevPixel = *pThePixel;
      }
    }
    else if (m_bitmapHeader.masking == PLIFF85::mskHasTransparentColor)
    {
      for (int i = 0; i < m_bitmapHeader.w; ++i)
      {
        // Get the byte we're interested in.
        PLBYTE the_byte = decodedBuf[i];
        PLPixel32* pThePixel = &Bmp.GetLineArray32()[row][i];
        pThePixel->Set(
          m_pal[the_byte].GetR(),
          m_pal[the_byte].GetG(),
          m_pal[the_byte].GetB(),
          static_cast<PLBYTE> ((the_byte == m_bitmapHeader.transparentColor) ? 0 : 0xFF));
      }
    }
    else
    {
      std::copy(
        decodedBuf.begin(),
        decodedBuf.end(),
        Bmp.GetLineArray()[row]);
    }
  }
} // GetImage

void PLIFF85Decoder::readChunkHeader(PLIFF85::Chunk& chunk, PLDataSource * pDataSrc)
{
  #if DETAILED_TRACE
    char szID[5];
    std::memcpy(szID, pDataSrc->GetBufferPtr(4), 4);
    szID[4] = '\0';
  #endif

  chunk.ckID   = ReadMLong(pDataSrc);
  chunk.ckSize = ReadMLong(pDataSrc);

  #if DETAILED_TRACE
    std::ostringstream strTrace;
    strTrace << szID << ", size: " << chunk.ckSize << '\n';
    Trace(2, strTrace.str().c_str());
  #endif
} // ReadChunkHeader

void PLIFF85Decoder::readCompressedRow(PLBYTE * pRow, PLDataSource * pDataSrc, int numBytes)
{
  int bytesOutput = 0;
  while (bytesOutput < numBytes)
  {
    signed char n = static_cast<signed char> (*pDataSrc->Read1Byte());
    if (n >= 0)
    {
      // Copy the next n+1 bytes literally.
      const PLBYTE * pData = m_pDataSrc->ReadNBytes(n + 1);
      std::copy(pData, pData + n + 1, &pRow[bytesOutput]);
      bytesOutput += n + 1;
      #if DETAILED_TRACE
        std::ostringstream strTrace;
        strTrace << "Copying " << (n + 1) << " bytes literally.\n";
        Trace(3, strTrace.str().c_str());
      #endif
    }
    else if ((n >= -127) && (n <= -1))
    {
      // Replicate the next byte -n+1 times.
      PLBYTE d = *pDataSrc->Read1Byte();
      std::fill_n(&pRow[bytesOutput], -n + 1, d);
      bytesOutput += -n + 1;
      #if DETAILED_TRACE
        std::ostringstream strTrace;
        strTrace << "Duplicating " << static_cast<int> (d) << " " << (-n + 1) << " times.\n";
        Trace(3, strTrace.str().c_str());
      #endif
    }
    else
    {
      // nop
      PLASSERT(n == -128);
    }
  }
  PLASSERT(bytesOutput == numBytes);
} // readCompressedRow

void PLIFF85Decoder::readUncompressedRow(PLBYTE * pRow, PLDataSource * pDataSrc, int numBytes)
{
  const PLBYTE * pRowData = pDataSrc->ReadNBytes(numBytes);
  std::copy(pRowData, pRowData + numBytes, pRow);
} // readUncompressedRow

int PLIFF85Decoder::getBytesPerRow() const
{
  int bytes_per_plane = ((m_bitmapHeader.w + (m_bitmapHeader.w & 1)) + 7) / 8;
  bytes_per_plane += (bytes_per_plane & 1);
  int bytes_per_row = bytes_per_plane * m_bitmapHeader.nPlanes;
  bytes_per_row += (bytes_per_row & 1);
  return bytes_per_row;
} // getBytesPerRow

/*
/--------------------------------------------------------------------
|
|      $Log: pliff85dec.cpp,v $
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.4  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.3  2004/04/15 20:22:05  uzadow
|      - All test output goes to cerr now.
|      - Fixed behaviour when test images were not available.
|
|      Revision 1.2  2004/03/13 21:09:08  uzadow
|      Linux compatibility changes.
|
|      Revision 1.1  2004/03/13 19:40:23  uzadow
|      Added Rupert Welchs iff decoder.
|
|
|
\--------------------------------------------------------------------
*/

