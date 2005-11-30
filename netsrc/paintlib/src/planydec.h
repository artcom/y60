/*
/--------------------------------------------------------------------
|
|      $Id: planydec.h,v 1.13 2004/09/15 21:10:45 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLANYDEC
#define INCL_PLANYDEC

#ifndef INCL_PLPICDEC
#include "plpicdec.h"
#endif

#include "pldecoderfactory.h"

#include <vector>

class CBitmap;

class PLPictDecoder;
class PLTGADecoder;
class PLTIFFDecoder;
class PLJPEGDecoder;
class PLPNGDecoder;
class PLWEMFDecoder;
class PLPCXDecoder;
class PLPGMDecoder;
class PLGIFDecoder;
class PLBmpDecoder;
class PLPPMDecoder;
class PLPSDDecoder;
class PLSGIDecoder;
class PLIFF85Decoder;

#define PL_FT_UNKNOWN 0
#define PL_FT_WINBMP  1
#define PL_FT_MACPICT 2
#define PL_FT_TARGA   3
#define PL_FT_TIFF    4
#define PL_FT_JPEG    5
#define PL_FT_PNG     6
#define PL_FT_EPSTIFF 7
#define PL_FT_WMF     8
#define PL_FT_EMF     9
#define PL_FT_PCX    10
#define PL_FT_PGM    11
#define PL_FT_GIF    12
#define PL_FT_PPM    13
#define PL_FT_PSD    14
#define PL_FT_SGI    15
#define PL_FT_IFF85  16


//! Class which decodes pictures with any known format. 
//! It auto-detects the format to use and delegates the work to one of
//! the other decoder classes.
//! The interface to PLAnyPicDecoder is the same as PLPicDecoder.
class PLAnyPicDecoder : public PLPicDecoder
{

public:
  //! Creates a decoder.
  PLAnyPicDecoder
    ();

  //! Destroys the decoder.
  ~PLAnyPicDecoder
    ();

  //! Returns the actual file type loaded. Can be called after open. 
  int GetFileFormat ();

  //! Opens a data source and prepares decoding.
  virtual void Open (PLDataSource * pDataSrc);

  //! Decodes the actual image
  virtual void GetImage (PLBmpBase & Bmp);

  //! Terminates decoding.
  virtual void Close ();

  static void RegisterDecoder(PLDecoderFactory * pFactory);
  
private:
  // Check for file-format-specific data & return the file type if
  // something fits.
  int getFileType
    ( PLBYTE * pData,
      int DataLen
    );

  long epsLongVal
    ( unsigned char *p
    );

  PLPicDecoder * findPluggedDecoder(PLBYTE * pData, int DataLen);

  ///////////////////////////////////////////////////////////////////
  // Member variables.
  int m_Type;
  PLPicDecoder  * m_pCurDec;
  bool m_bUsePluggedDecoder;

  PLBmpDecoder  * m_pBmpDec;
  PLPictDecoder * m_pPictDec;
  PLTGADecoder  * m_pTGADec;
  PLTIFFDecoder * m_pTIFFDec;
  PLJPEGDecoder * m_pJPEGDec;
  PLPNGDecoder  * m_pPNGDec;
  PLWEMFDecoder * m_pWEMFDec;
  PLPCXDecoder  * m_pPCXDec;
  PLPGMDecoder  * m_pPGMDec;
  PLGIFDecoder  * m_pGIFDec;
  PLPPMDecoder  * m_pPPMDec;
  PLPSDDecoder  * m_pPSDDec;
  PLSGIDecoder  * m_pSGIDec;
  PLIFF85Decoder* m_pIFF85Dec;  

  static std::vector<PLDecoderFactory*> s_DecoderFactories;
};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: planydec.h,v $
|      Revision 1.13  2004/09/15 21:10:45  uzadow
|      Added decoder plugin interface to PLAnyPicDecoder.
|
|      Revision 1.12  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.11  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.10  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.9  2004/03/13 19:40:23  uzadow
|      Added Rupert Welchs iff decoder.
|
|      Revision 1.8  2003/06/01 21:12:06  uzadow
|      Added sgi rgb file format.
|
|      Revision 1.7  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from 
|      images without loading the whole image and proper greyscale support.
|
|      Revision 1.6  2002/03/06 22:46:54  uzadow
|      Fixed major PLAnyDec bug
|
|      Revision 1.5  2001/10/21 17:12:39  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added 
|      PLFilterPixel.
|
|      Revision 1.4  2001/10/16 17:51:32  uzadow
|      Added ppm support (Todd Harris)
|
|      Revision 1.3  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.2  2001/10/03 13:58:21  uzadow
|      Removed references to config.h
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.12  2001/09/15 21:02:44  uzadow
|      Cleaned up stdpch.h and config.h to make them internal headers.
|
|      Revision 1.11  2001/01/14 13:36:15  uzadow
|      Added PLAnyPicDecoder::GetFileFormat()
|
|      Revision 1.10  2000/12/08 12:32:00  uzadow
|      Added gif decoder by Michael Salzlechner.
|
|      Revision 1.9  2000/03/31 11:53:29  Ulrich von Zadow
|      Added quantization support.
|
|      Revision 1.8  2000/03/16 13:56:37  Ulrich von Zadow
|      Added pgm decoder by Jose Miguel Buenaposada Biencinto
|
|      Revision 1.7  2000/01/16 20:43:12  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  1999/12/08 15:39:45  Ulrich von Zadow
|      Unix compatibility changes
|
|
\--------------------------------------------------------------------
*/
