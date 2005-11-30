/*
/--------------------------------------------------------------------
|
|      $Id: plpicdec.h,v 1.18 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPICDEC
#define INCL_PLPICDEC

#ifndef INCL_PLBITMAP
#include "plbitmap.h"
#endif

#ifndef INCL_PLDATASRC
#include "pldatasrc.h"
#endif

#ifdef _WIN32
#include <windows.h>
#endif

//! PLPicDecoder is an abstract base class. It defines common routines
//! for all decoders. Decoders for specific file formats can be
//! derived from this class. Objects of this class interact with a
//! PLDataSource to decode bitmaps.
class PLPicDecoder : public PLBmpInfo
{

public:
  //! Empty constructor. The actual initialization takes place in a
  //! derived class.
  PLPicDecoder
    ();

  //! Destructor. Frees memory allocated.
  virtual ~PLPicDecoder
    ();

  //! Decodes a picture in a file and stores the results in pBmp.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 16, 24, 32 and 0.
  //! 0 will result in either an 8 or a 32 bpp bitmap being returned,
  //! depending on the source bitmap. Of course, if BPPWanted is less than the
  //! bpp of the file, some data is lost.
  //! Note that internally, all data is decoded to 8 or 32 bpp first in all 
  //! cases, so using 16 or 24 as BPPWanted will cause in a copy and format 
  //! conversion operation.
  //!
  //! As an exception, BPPWanted can be 1 for b/w tiff files.
  virtual void MakeBmpFromFile
    ( const char * pszFName,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted = PLPixelFormat::DONTCARE,
      PLIProgressNotification * pProgNot = NULL
    );

#ifdef _WIN32  
  //! Decodes a picture in a file and stores the results in pBmp.
  //! Wide-Character version (windows only). 
  virtual void MakeBmpFromFileW
    ( const wchar_t * pszwFName,
      PLBmp * pBmp,
      const PLPixelFormat& pf = PLPixelFormat::DONTCARE,
      PLIProgressNotification * pProgNot = NULL
    );
#endif


  //! Decodes a picture in an url (http://, ftp:://, ...) and stores the results in pBmp.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 16, 24, 32 and 0.
  //! 0 will result in either an 8 or a 32 bpp bitmap being returned,
  //! depending on the source bitmap. Of course, if BPPWanted is less than the
  //! bpp of the file, some data is lost.
  //! Note that internally, all data is decoded to 8 or 32 bpp first in all 
  //! cases, so using 16 or 24 as BPPWanted will cause in a copy and format 
  //! conversion operation.
  //!
  //! As an exception, BPPWanted can be 1 for b/w tiff files.
  virtual void MakeBmpFromURL
    ( const char * pszURL,
      PLBmp * pBmp,
      const PLPixelFormat& pf = PLPixelFormat::DONTCARE,
      PLIProgressNotification * pProgNot = NULL
    );

#ifdef _WIN32
  //! Decodes a picture in a windows resource by creating a resource data
  //! source and calling MakeBmp with this data source.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 16, 24, 32 and 0.
  //! 0 will result in either an 8 or a 32 bpp bitmap being returned,
  //! depending on the source bitmap. Of course, if BPPWanted is less than the
  //! bpp of the file, some data is lost.
  //! Note that internally, all data is decoded to 8 or 32 bpp first in all 
  //! cases, so using 16 or 24 as BPPWanted will cause in a copy and format 
  //! conversion operation.
  //!
  //! As an exception, BPPWanted can be 1 for b/w tiff files.
  //!
  //! ResType is the windows resource type (e.g. "JPEG") to be used.
  //! If a module handle is passed as hResModule, then the resource
  //! will be loaded from this module, otherwise, the resource
  //! will be searched for by AfxFindResourceHandle (! MFC dependent!)
  virtual void MakeBmpFromResource
    ( HINSTANCE hInstResource, int ResourceID,
      PLBmp * pBmp,
      const PLPixelFormat& pf = PLPixelFormat::DONTCARE,
      const char* ResType = NULL,
      HMODULE hResModule = 0
	  );
#endif

  //! Decodes a picture from a memory location which directly resembles
  //! the image file as it would be on disc. The result is stored in pBmp.
  //! BPPWanted is the number of bits per pixel in the bitmap
  //! returned. Valid values for BPPWanted are 8, 16, 24, 32 and 0.
  //! 0 will result in either an 8 or a 32 bpp bitmap being returned,
  //! depending on the source bitmap. Of course, if BPPWanted is less than the
  //! bpp of the file, some data is lost.
  //! Note that internally, all data is decoded to 8 or 32 bpp first in all 
  //! cases, so using 16 or 24 as BPPWanted will cause in a copy and format 
  //! conversion operation.
  //!
  //! As an exception, BPPWanted can be 1 for b/w tiff files.
  virtual void MakeBmpFromMemory
    ( unsigned char * ucMemSrc,
      int MemSrcSize,
      PLBmp * pBmp,
      const PLPixelFormat& pf = PLPixelFormat::DONTCARE,
      PLIProgressNotification * pProgNot = NULL
    );

  //!
  void OpenFile
    ( const char * pszFName, 
      PLIProgressNotification * pProgNot = NULL
    );
  
#ifdef _WIN32
  //!
  void OpenFileW
    ( const wchar_t * pszwFName, 
      PLIProgressNotification * pProgNot = NULL
    );
#endif
  //! Decodes an image in an already-opened data source and 
  //! stores the results in pBmp. 
  //! Open should be called before, close after this routine is called.
    virtual void MakeBmp
    ( PLBmp * pBmp,
      const PLPixelFormat& pf = PLPixelFormat::DONTCARE
    );

  //!
  virtual void Close
    ();

  //! Sets the amount and destination of debug traces output by the
  //! debug version. pszFName contains either a valid file name or is
  //! NULL. If it contains a file name, this file is used to store
  //! debug information. If pszFName is NULL, the destination is
  //! either the MSVC debug console or stderr depending on the
  //! version of the library. Valid values for Level are: <BR>
  //! <BR>
  //! 0: Trace only errors.<BR>
  //! 1: Trace top-level calls.<BR>
  //! 2: Trace picture format information<BR>
  //! 3: Trace all miscellaneous info.<BR>
  //! <BR>
  //! The trace configuration is global to all decoders.
  static void SetTraceConfig
    ( int Level,
      char * pszFName
    );

  //!
  virtual void Open (PLDataSource * pDataSrc)=0;

  //! Decodes an image. Assumes that Open() and Bmp.Create() have already
  //! been called.
  virtual void GetImage (PLBmpBase & Bmp) = 0;

  // Used only by PLAnyPicDecoder.
  void SetDataSrc (PLDataSource * pDataSrc);

  // This function is needed by callbacks outside of any object,
  // so it's public and static. It should not be called from
  // outside of the library.
  static void raiseError
    ( int Code,
      char * pszErr
    );

  //! Called to output status messages to the current debug console
  static void Trace
    ( int TraceLevel,
      const char * pszMessage
    );

protected:
  // TODO: What is this doing here?!
  PLBYTE * unpackPictRow
    ( PLBYTE * pLineBuf,
      PLDataSource * pDataSrc,
      int Width,
      int rowBytes,
      int SrcBytes
    );

  PLBYTE ReadByte
    ( PLDataSource * pDataSrc
    );

  // Machine-independent routines for byte-order conversion.

  PLWORD ReadIWord
    ( PLDataSource * pDataSrc
    );

  PLWORD ReadMWord
    ( PLDataSource * pDataSrc
    );

  PLLONG ReadILong
    ( PLDataSource * pDataSrc
    );

  PLLONG ReadMLong
    ( PLDataSource * pDataSrc
    );

  PLDataSource * m_pDataSrc;

private:
  ///////////////////////////////////////////////////////////////////
  // Member variables.

  static int    m_TraceLevel;    // 0: Trace only errors.
                                 // 1: Trace top-level calls.
                                 // 2: Trace picture format info.
                                 // 3: Trace misc. info.
  static char * m_pszTraceFName; // Name of trace file. NULL if trace to
                                 // MSVC debug console.
};


inline PLBYTE PLPicDecoder::ReadByte
    ( PLDataSource * pDataSrc
    )
{
  return *(pDataSrc->ReadNBytes (1));
}


inline PLWORD PLPicDecoder::ReadIWord
    ( PLDataSource * pDataSrc
    )
{
  PLBYTE * pData = pDataSrc->Read2Bytes ();
  // This should work regardless of the destination byte order ;-)
  return pData[0] + (pData[1]<<8);
}


inline PLWORD PLPicDecoder::ReadMWord
    ( PLDataSource * pDataSrc
    )
{
  PLBYTE * pData = pDataSrc->Read2Bytes ();
  return pData[1] + (pData[0]<<8);
}


inline PLLONG PLPicDecoder::ReadILong
    ( PLDataSource * pDataSrc
    )
{
  PLBYTE * pData = pDataSrc->Read4Bytes ();
  return pData[0] + (pData[1]<<8) + (pData[2]<<16) + (pData[3]<<24);
}

inline PLLONG PLPicDecoder::ReadMLong
    ( PLDataSource * pDataSrc
    )
{
  PLBYTE * pData = pDataSrc->Read4Bytes ();
  return pData[3] + (pData[2]<<8) + (pData[1]<<16) + (pData[0]<<24);
}

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plpicdec.h,v $
|      Revision 1.18  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.17  2004/09/15 15:26:21  uzadow
|      Linux compatibility changes, doc update.
|
|      Revision 1.16  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.15  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.14  2004/06/19 20:13:17  uzadow
|      Documentation update.
|
|      Revision 1.13  2004/06/19 17:34:27  uzadow
|      Documentation update
|
|      Revision 1.12  2004/06/19 16:49:07  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.11  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.10  2003/08/03 12:54:10  uzadow
|      Fixed broken linux build.
|
|      Revision 1.9  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.8  2003/03/19 14:33:13  uzadow
|      Added Rect.Contains
|
|      Revision 1.7  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.6  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.5  2002/03/06 22:46:54  uzadow
|      Fixed major PLAnyDec bug
|
|      Revision 1.4  2002/03/03 16:29:55  uzadow
|      Re-added BPPWanted.
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
|      Revision 1.8  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.7  2000/03/30 21:24:15  Ulrich von Zadow
|      Added MakeBmpFromMemory() function by Markus Ewald
|
|      Revision 1.6  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.5  2000/01/11 21:40:30  Ulrich von Zadow
|      Added instance handle parameter to LoadFromResource()
|
|      Revision 1.4  2000/01/08 15:51:30  Ulrich von Zadow
|      Misc. modifications to png encoder.
|
|      Revision 1.3  1999/11/08 22:12:51  Ulrich von Zadow
|      Andreas Koepf: Added resource type as parameter to
|      MakeBmpFromResource
|
|
\--------------------------------------------------------------------
*/
