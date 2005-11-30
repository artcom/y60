/*
/--------------------------------------------------------------------
|
|      $Id: plpicdec.cpp,v 1.18 2004/11/09 15:55:06 artcom Exp $
|      Generic Picture Decoder Class
|
|      Abstract base class to construct PLBmps from picture data in
|      memory or in a file. Classes derived from this class implement
|      concrete decoders for specific file formats. The data is
|      returned in 8- or 32-bit DIBs with a valid alpha channel.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plpicdec.h"
#include "plfilesrc.h"
#ifdef _WIN32
#include "plressrc.h"
#endif
#include "plmemsrc.h"
#ifdef PL_SUPPORT_CURL
#include "plurlsrc.h"
#endif
#include "plexcept.h"
#include "planybmp.h"

#include <stdio.h>

// Set default Trace configuration here. The defined levels are
// explained in picdec.h.
int PLPicDecoder::m_TraceLevel = 0;
char * PLPicDecoder::m_pszTraceFName = NULL;


PLPicDecoder::PLPicDecoder
  () : m_pDataSrc (0)
    // Creates a decoder
{
}


PLPicDecoder::~PLPicDecoder
    ()
{
  if (m_pszTraceFName)
    delete [] m_pszTraceFName;
  m_pszTraceFName = NULL;
}


void PLPicDecoder::MakeBmpFromFile
    ( const char * pszFName,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted,
      PLIProgressNotification * pProgNot
    )
    // Decodes a picture in a file by creating a file data source and
    // calling MakeBmp with this data source.
{
  try
  {
    OpenFile (pszFName, pProgNot);
    MakeBmp (pBmp, pfWanted);
    Close();
  }
  catch (PLTextException)
  {
    // Clean up on error
    if (m_pDataSrc)
      Close();
    throw;
  }
}


#ifdef _WIN32
void PLPicDecoder::MakeBmpFromFileW
    ( const wchar_t * pszwFName,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted,
      PLIProgressNotification * pProgNot
    )
    // Decodes a picture in a file by creating a file data source and
    // calling MakeBmp with this data source.
{
  try
  {
    OpenFileW (pszwFName, pProgNot);
    MakeBmp (pBmp, pfWanted);
    Close();
  }
  catch (PLTextException)
  {
    // Clean up on error
    if (m_pDataSrc)
      Close();
    throw;
  }
}
#endif


void PLPicDecoder::MakeBmpFromURL
    ( const char * pszURL,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted,
      PLIProgressNotification * pProgNot
    )
{
#ifdef PL_SUPPORT_CURL
  int err;

  char sz[256];
  sprintf (sz, "--- Decoding URL %s. ---\n", pszURL);
  Trace (1, sz);

  PLURLSource * pSrc;
  try
  {
    pSrc = new PLURLSource ();
    m_pDataSrc = pSrc;
    err = pSrc->Open (pszURL);
    if (err)
    {
      sprintf (sz, "Reading URL %s failed, reason: %s", pszURL, pSrc->GetCurlErrStr());
      raiseError (err, sz);
    }

    Open (pSrc);
    m_pDataSrc = pSrc;
    MakeBmp (pBmp, pfWanted);
    Close ();
  }
  catch (PLTextException e)
  {
    // Clean up on error
    int MCode = pSrc->GetCurlErrCode();
    delete pSrc;
    if (e.GetCode() == PL_ERRURL_SOURCE)
    {
      throw PLTextException(e.GetCode(), MCode, (const char*)e);
    } 
    else 
    {
      throw;
    } 
  }
#else
  throw PLTextException(PL_ERRFORMAT_NOT_COMPILED, 
                        "URL support not compiled into library.");
#endif			

}

#ifdef _WIN32
void PLPicDecoder::MakeBmpFromResource
    ( HINSTANCE hInstResource, int ResourceID,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted,
      const char * ResType,
      HMODULE hResModule
    )
    // Decodes a picture in a resource by creating a resource data
    // source and calling MakeBmp with this data source.
{
  int err;

  char sz[256];
  sprintf (sz, "--- Decoding resource ID %i. ---\n", ResourceID);
  Trace (1, sz);

  PLResourceSource * pResSrc;
  try
  {
    pResSrc = new PLResourceSource ();
    m_pDataSrc = pResSrc;
    err = pResSrc->Open (hInstResource, ResourceID, ResType);
    if (err)
    {
      sprintf (sz, "Opening resource %i failed", ResourceID);
      raiseError (err, sz);
    }

    Open (pResSrc);
    m_pDataSrc = pResSrc;
    MakeBmp (pBmp, pfWanted);
    Close ();
  }
  catch (PLTextException)
  {
    // Clean up on error
    delete pResSrc;
    throw;
  }
}
#endif


void PLPicDecoder::MakeBmpFromMemory
    ( unsigned char * ucMemSrc,
      int MemSrcSize,
      PLBmp * pBmp,
      const PLPixelFormat& pfWanted,
      PLIProgressNotification * pProgNot
    )
    // Decodes a picture from memory directly resembling the image file by
    // creating a memory data source and calling MakeBmp with this data source.
{
  PLMemSource * pMemSrc = NULL;
  int err;

  char sz[256];
  sprintf (sz, "--- Decoding from memory at %p. ---\n", ucMemSrc);
  Trace (1, sz);

  try
  {
    PLMemSource * pMemSrc = new PLMemSource ();
    err = pMemSrc->Open (ucMemSrc, MemSrcSize);
    if (err)
    {
      sprintf (sz, "Reading from memory at %p failed", ucMemSrc);
      raiseError (err, sz);
    }

    Open (pMemSrc);
    m_pDataSrc = pMemSrc;
    MakeBmp (pBmp, pfWanted);
    Close ();
  }
  catch (PLTextException)
  {
    // Clean up on error
    delete pMemSrc;
    throw;
  }
}


void PLPicDecoder::MakeBmp
    ( PLBmp * pBmp,
      const PLPixelFormat& pfWanted
    )
    // Decodes a picture by getting the encoded data from m_pDataSrc.
    // Stores the results in pBmp.
{
  PLBmp * pTempBmp;

  if (pfWanted == PLPixelFormat::DONTCARE ||
      pfWanted == GetPixelFormat())
    pTempBmp = pBmp;
  else if (pfWanted.GetBitsPerPixel() > GetBitsPerPixel())
    pTempBmp = new PLAnyBmp;
  else
    throw (PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Image bit depth doesn't match request."));

  pTempBmp->Create (*this);
  GetImage (*pTempBmp);

  if (pfWanted != PLPixelFormat::DONTCARE &&
      pfWanted != GetPixelFormat())
  {
    pBmp->CreateCopy(*pTempBmp, pfWanted);
    delete pTempBmp;
  }
}

void PLPicDecoder::SetTraceConfig
    ( int Level,
      char * pszFName
    )
{
  // Level has to be between 0 and 3.
  PLASSERT (Level < 4);

  m_TraceLevel = Level;

  if (m_pszTraceFName)
    delete [] m_pszTraceFName;

  if (pszFName)
  {
    m_pszTraceFName = new char[strlen (pszFName)+1];
    strcpy (m_pszTraceFName, pszFName);

    // Delete any old Trace file with the same name.
    remove (m_pszTraceFName);
  }
  else
    m_pszTraceFName = NULL;
}

#ifdef _WIN32
void PLPicDecoder::OpenFileW
    ( const wchar_t * pszwFName,
      PLIProgressNotification * pProgNot
    )
{
  int err;

  char sz[1024];
  char sz2[900];

  // First calculate how much room the result will take up
  int neededSize = ::WideCharToMultiByte(CP_ACP,
              0,
              pszwFName,
              -1,
              NULL,
              0,
              NULL,
              NULL);
  if (neededSize < 900){  // Prevent overflow of our buffer
    ::WideCharToMultiByte(CP_ACP,
        0,
              pszwFName,
              -1,
              sz2,
              900,
              NULL,
              NULL);
  }
  else{
      strcpy(sz2,"{{Filename too long}}");
  }

  sprintf (sz, "--- Decoding file %s. ---\n", sz2);
  Trace (1, sz);

  PLFileSource * pFileSrc = 0;
  try
  {
    pFileSrc = new PLFileSource (pProgNot);
    err = pFileSrc->OpenW (pszwFName);
    if (err)
    {
      sprintf (sz, "Opening %s failed", sz2);
      raiseError (err, sz);
    }
    Open (pFileSrc);
    m_pDataSrc = pFileSrc;
  }
  catch (PLTextException)
  {
    if (pFileSrc != 0)
        pFileSrc->Close();
    delete pFileSrc;
    throw;
  }
}
#endif

void PLPicDecoder::OpenFile
    ( const char * pszFName,
      PLIProgressNotification * pProgNot
    )
{
  int err;

  char sz[1024];
  sprintf (sz, "--- Decoding file %s. ---\n", pszFName);
  Trace (1, sz);

  PLFileSource * pFileSrc = 0;
  try
  {
    pFileSrc = new PLFileSource (pProgNot);
    err = pFileSrc->Open (pszFName);
    if (err)
    {
      sprintf (sz, "Opening %s failed", pszFName);
      raiseError (err, sz);
    }

    Open (pFileSrc);
    m_pDataSrc = pFileSrc;
  }
  catch (PLTextException)
  {
    if (pFileSrc != 0)
        pFileSrc->Close();
    delete pFileSrc;
    throw;
  }
}

void PLPicDecoder::SetDataSrc (PLDataSource * pDataSrc)
{
  PLASSERT (m_pDataSrc == 0);
  m_pDataSrc = pDataSrc;
}

void PLPicDecoder::Close
    ()
{
  if (m_pDataSrc)
  {
    m_pDataSrc->Close ();
    delete m_pDataSrc;
    m_pDataSrc = 0;
  }
}

void PLPicDecoder::raiseError
    ( int Code,
      char * pszErr
    )
    // This function is needed by callbacks outside of any object,
    // so it's public and static. It should not be called from
    // outside of the library.
{
  char sz[256];
  sprintf (sz, "Decoder error: %s\n", pszErr);
  Trace (0, sz);
  throw (PLTextException (Code, sz));
}

void PLPicDecoder::Trace
    ( int TraceLevel,
      const char * pszMessage
    )
    // Outputs debugging data to a file or to the MSVC debug console.
{
  if (TraceLevel <= m_TraceLevel)
  {
    if (m_pszTraceFName)
    {
      // The file is closed after every call so no data is lost
      // if the program crashes.
      FILE * pFile = fopen (m_pszTraceFName, "a+t");
      if (pFile != (FILE *)0)
      {
        fprintf (pFile, pszMessage);
        fclose (pFile);
      }
      else
      { // No permission? File locked? Filename nonsense?
        PLTRACE ("Error opening Trace file!\n");
      }
    }
    else
      PLTRACE (pszMessage);
  }
}

PLBYTE * PLPicDecoder::unpackPictRow
    ( PLBYTE * pLineBuf,
      PLDataSource * pDataSrc,
      int Width,
      int rowBytes,
      int SrcBytes
    )
{
  PLBYTE * pRawLine = pLineBuf;

  if (rowBytes < 8)
  { // Ah-ha!  The bits aren't actually packed.  This will be easy.
    pRawLine = pDataSrc->ReadNBytes (rowBytes);
  }
  else
  {
    PLBYTE * pSrcLine = pDataSrc->ReadNBytes(SrcBytes);
    PLBYTE * pCurPixel = pRawLine;

    // Unpack RLE. The data is packed bytewise.
    for (int j = 0; j < SrcBytes; )
    {
      PLBYTE FlagCounter = pSrcLine[j];
      if (FlagCounter & 0x80)
      {
        if (FlagCounter == 0x80)
          // Special case: repeat value of 0.
          // Apple sais ignore.
          j++;
        else
        { // Packed data.
          int len = ((FlagCounter ^ 255) & 255) + 2;

          memset (pCurPixel, *(pSrcLine+j+1), len);
          pCurPixel += len;
          j += 2;
        }
      }
      else
      { // Unpacked data
        int len = (FlagCounter & 255) + 1;
        memcpy (pCurPixel, pSrcLine+j+1, len);
        pCurPixel += len;
        j += len + 1;
      }
    }
  }
  return pRawLine;
}


/*
/--------------------------------------------------------------------
|
|      $Log: plpicdec.cpp,v $
|      Revision 1.18  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.17  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.16  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.15  2004/07/28 11:41:07  uzadow
|      PNG decoder now decodes directly to 24 bpp
|
|      Revision 1.14  2004/07/01 10:10:29  artcom
|      Library now compiles when libcurl is not installed.
|
|      Revision 1.13  2003/08/03 12:54:10  uzadow
|      Fixed broken linux build.
|
|      Revision 1.12  2003/08/03 12:48:50  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.11  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.10  2003/02/24 22:10:27  uzadow
|      Linux version of MakeBmpFromURL() tests
|
|      Revision 1.9  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.8  2002/08/05 19:06:30  uzadow
|      no message
|
|      Revision 1.7  2002/08/04 21:20:41  uzadow
|      no message
|
|      Revision 1.6  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.5  2002/03/03 16:29:55  uzadow
|      Re-added BPPWanted.
|
|      Revision 1.4  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.3  2001/10/16 17:12:26  uzadow
|      Added support for resolution information (Luca Piergentili)
|
|      Revision 1.2  2001/10/06 22:37:08  uzadow
|      Linux compatibility.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.13  2001/09/15 21:02:44  uzadow
|      Cleaned up stdpch.h and config.h to make them internal headers.
|
|      Revision 1.12  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.11  2001/02/04 14:07:24  uzadow
|      Changed max. filename length.
|
|      Revision 1.10  2001/01/21 14:28:21  uzadow
|      Changed array cleanup from delete to delete[].
|
|      Revision 1.9  2000/12/18 22:42:52  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.8  2000/03/30 21:24:15  Ulrich von Zadow
|      Added MakeBmpFromMemory() function by Markus Ewald
|
|      Revision 1.7  2000/01/16 20:43:14  anonymous
|      Removed MFC dependencies
|
|      Revision 1.6  2000/01/11 21:40:30  Ulrich von Zadow
|      Added instance handle parameter to LoadFromResource()
|
|      Revision 1.5  2000/01/08 15:51:30  Ulrich von Zadow
|      Misc. modifications to png encoder.
|
|      Revision 1.4  1999/11/08 22:12:51  Ulrich von Zadow
|      Andreas Koepf: Added resource type as parameter to
|      MakeBmpFromResource
|
|      Revision 1.3  1999/10/03 18:50:51  Ulrich von Zadow
|      Added automatic logging of changes.
|
|
\--------------------------------------------------------------------
*/
