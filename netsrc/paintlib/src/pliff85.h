/*
/--------------------------------------------------------------------
|
|      $Id: pliff85.h,v 1.2 2004/04/16 20:14:41 uzadow Exp $
|
|      Defines structures and constants present in Electronic Arts IFF-85
|      files, in particular those in ILBM and PBM files.
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

#if !defined(INCL_PLIFF85)
#define      INCL_PLIFF85

#include "pldebug.h"
#include "plpaintlibdefs.h"

// Standard IFF-85 constants and types.
namespace PLIFF85
{
  // Standard IFF types based on their paintlib equivalents (where available).
  // This allows the stuff below to follow the documentation more closely.
  typedef PLBYTE          UBYTE;  //< 8-bit unsigned
  typedef signed short    WORD;   //< 16-bit signed
  typedef PLWORD          UWORD;  //< 16-bit unsigned
  typedef PLLONG          LONG;   //< 32-bit signed

  typedef LONG ID;        //< 4 chars in ' ' through '~'.

  // Chunks are the building blocks in the IFF structure.
  struct Chunk
  {
    ID      ckID;       //< Chunk ID
    LONG    ckSize;     //< size of the chunk data following.
    // This is followed by ckSize UBYTEs.
  };

  // Converts a 4-character string to a IFF chunk ID format.
  inline ID MakeID(const char IDStr[4], bool validate = true)
  {
    if (validate)
    {
      PLASSERT(IDStr[0] >= ' ');
      PLASSERT(IDStr[0] <= '~');
      PLASSERT(IDStr[1] >= ' ');
      PLASSERT(IDStr[1] <= '~');
      PLASSERT(IDStr[2] >= ' ');
      PLASSERT(IDStr[2] <= '~');
      PLASSERT(IDStr[3] >= ' ');
      PLASSERT(IDStr[3] <= '~');
    }
    return ((UBYTE(IDStr[0]) << 24) |
            (UBYTE(IDStr[1]) << 16) |
            (UBYTE(IDStr[2]) << 8) |
            (UBYTE(IDStr[3])));
  }

  const ID ID_FORM    = MakeID("FORM");   //< All valid IFF files should start with this.
  const ID ID_ILBM    = MakeID("ILBM");   //< A ILBM image.
  const ID ID_PBM     = MakeID("PBM ");   //< A PBM image.
  const ID ID_BMHD    = MakeID("BMHD");   //< Bitmap header.
  const ID ID_CMAP    = MakeID("CMAP");   //< A colormap, or palette.
  const ID ID_CAMG    = MakeID("CAMG");   //< Commodore AMiGa viewmodes.
  const ID ID_BODY    = MakeID("BODY");   //< The body of the image.

  // BMHD types.

  typedef UBYTE Masking;  //< Choice of masking technique.

  // Designates an opaque rectangular image.
  const Masking mskNone                   = 0;

  // \brief This means that a mask plane is interleaved with the bitplanes
  // in the BODY chunk.
  const Masking mskHasMask                = 1;

  // \brief Indicates that pixels in the source planes matching
  // transparentColor are to be considered "transparent".
  const Masking mskHasTransparentColor    = 2;

  // Indicates the reader may construct a mask by lassoing the image.
  const Masking mskLasso                  = 3;

  // \brief Choice of compression algorithm applied to the rows of all
  // source and mask planes.
  // \note Do not compress across rows!
  typedef UBYTE Compression;

  const Compression cmpNone       = 0;    //< No compression.

  // This is the byte run encoding described in Appendix C of the ILBM definition.
  const Compression cmpByteRun1   = 1;

  // The required property "BMHD" holds a BitMapHeader as defined here.
  // It describes the dimensions and encoding of the image, including
  // data necessary to understand the BODY chunk to follow.
  struct BitMapHeader
  {
    UWORD       w;                      //< raster width in pixels.
    UWORD       h;                      //< raster height in pixels.
    WORD        x;                      //< pixel position for this image.
    WORD        y;                      //< pixel position for this image.
    UBYTE       nPlanes;                //< # source bitplanes
    Masking     masking;                //< masking type.
    Compression compression;            //< Compression type.
    UBYTE       pad1;                   //< unused; for consistency, put 0 here.
    UWORD       transparentColor;       //< transparent "color number" (sort of).
    UBYTE       xAspect;                //< pixel aspect, a ratio width : height.
    UBYTE       yAspect;                //< pixel aspect, a ratio width : height.
    WORD        pageWidth;              //< source "page" size in pixels.
    WORD        pageHeight;             //< source "page" size in pixels.
  };

  // The optional (but encouraged) property "CMAP" stores color map data
  // as triplets of red, green, and blue intensity values. The n color
  // map entries ("color registers") are stored in the order 0 through
  // n-1, totaling 3n bytes. Thus n is the ckSize/3. Normally, n would
  // equal 2 raised to the power of nPlanes.
  //
  // A CMAP chunk contains a ColorMap array as defined below. (These
  // typedefs assume a C compiler that implements packed arrays of
  // 3-byte elements.)
  struct ColorRegister
  {
    UBYTE red;                  //< red intensity 0..255.
    UBYTE green;                //< green intensity 0..255.
    UBYTE blue;                 //< blue intensity 0..255.
  };

//  typedef ColorRegister ColorMap[n];  /* size = 3n bytes */

  // Commodore Amiga viewport mode.
  typedef LONG Viewmode;

  // Viewmode masks.

  // Hold-and-Modify.  The bits in the two last planes describe an R G or B
  // modification to the color of the previous pixel on the line to create
  // the color of the current pixel.
  const Viewmode viewHAM    = 0x00000800;

} // namespace PLIFF85

#endif // !defined(INCL_PLIFF85)

/*
/--------------------------------------------------------------------
|
|      $Log: pliff85.h,v $
|      Revision 1.2  2004/04/16 20:14:41  uzadow
|      Changes to make cdoc work.
|
|      Revision 1.1  2004/03/13 19:40:23  uzadow
|      Added Rupert Welchs iff decoder.
|
|
|
\--------------------------------------------------------------------
*/
