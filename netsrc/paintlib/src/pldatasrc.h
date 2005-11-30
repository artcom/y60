/*
/--------------------------------------------------------------------
|
|      $Id: pldatasrc.h,v 1.9 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLDATASRC
#define INCL_PLDATASRC

#include "plexcept.h"
#include "plpaintlibdefs.h"

#ifndef AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_
#include "plobject.h"
#endif

#ifdef _WIN32
#include "stddef.h"
#endif

class PLIProgressNotification;

//! This is a base class for a source of picture data.
//! It defines methods to open, close, and read from data sources.
//! Does byte-order-conversions in the ReadByte, ReadWord, and
//! ReadLong routines.
class PLDataSource : public PLObject
{

public:

  //! Constructs a new data source. pNotification points to an object that 
  //! reacts to progress notification messages.
  PLDataSource
    ( PLIProgressNotification * pNotification = NULL
    );

  //!
  virtual ~PLDataSource
    ();

  //! 
  virtual void Open
    ( const char * pszName,
      int    FileSize
    );

#ifdef _WIN32
  //!
  virtual void OpenW
    ( const wchar_t * pszwName,
      int    FileSize
    );
#endif

  //!
  virtual void Close
    ();

  //!
  char * GetName
    ();

#ifdef _WIN32
  //!
  wchar_t * GetNameW
    ();

  //!
  bool NameIsWide
    ();
#endif

  //! Read but don't advance file pointer.
  virtual PLBYTE * GetBufferPtr
    ( int MinBytesInBuffer
    ) = 0;

  //! This needs to be overridden in derived classes.
  virtual PLBYTE * ReadNBytes
    ( int n
    );

  //!
  int GetFileSize
    ();

  //! This is a legacy routine that interferes with progress notifications.
  //! Don't call it!
  virtual PLBYTE * ReadEverything
    () = 0;

  //!
  PLBYTE * Read1Byte
    ();

  //!
  PLBYTE * Read2Bytes
    ();

  //!
  PLBYTE * Read4Bytes
    ();

  //! handles progress notification from other libs
  void OProgressNotification
    ( double part
    );

  //!
  void AlignToWord
    ();

  //!
  void Skip
    ( int n
    );

  //!
  virtual void Seek
    ( int n
    );

  //! Test to see if we didn't go past the end of the file
  void CheckEOF
    ();

protected:

private:
  char * m_pszName;        // Name of the data source for diagnostic
                           // purposes.
#ifdef _WIN32
  wchar_t * m_pszwName;       // Name in wide characters.
  bool      m_bNameIsWide;    // Is the name wide or not?
#endif
  int    m_FileSize;
  int    m_BytesRead;
  bool   m_bSrcLSBFirst;   // Source byte order: true for intel order,
                           // false for Motorola et al. (MSB first).
  PLIProgressNotification * m_pNotification;
};


#ifdef _WIN32
inline bool PLDataSource::NameIsWide
      ()
{
  return m_bNameIsWide;
}
#endif

inline int PLDataSource::GetFileSize
    ()
{
  return m_FileSize;
}

inline PLBYTE * PLDataSource::Read1Byte
    ()
{
  return ReadNBytes (1);
}


inline PLBYTE * PLDataSource::Read2Bytes
    ()
{
  return ReadNBytes (2);
}


inline PLBYTE * PLDataSource::Read4Bytes
    ()
{
  return ReadNBytes (4);
}

inline void PLDataSource::AlignToWord
    ()
{
  if (m_BytesRead & 1)
    Read1Byte();
}


inline void PLDataSource::Skip
    ( int n
    )
{
  ReadNBytes (n);
}

inline void PLDataSource::CheckEOF
    ()
{

  if (m_FileSize < m_BytesRead)
  {
    throw PLTextException (PL_ERREND_OF_FILE,
          "End of file reached while decoding.\n");
  }
}

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pldatasrc.h,v $
|      Revision 1.9  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.8  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.7  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.6  2004/08/20 14:39:37  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.5  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.4  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.3  2002/02/24 13:00:21  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.2  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/10/24 22:59:34  uzadow
|      no message
|
|
\--------------------------------------------------------------------
*/
