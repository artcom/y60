
/*
/--------------------------------------------------------------------
|
|      $Id: plexcept.h,v 1.12 2004/09/15 14:52:09 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLEXCEPT
#define INCL_PLEXCEPT

#ifndef AFX_PLOBJECT_H__E40881E3_C809_11D3_97BC_0050046F615E__INCLUDED_
#include "plobject.h"
#endif


// Error codes
#define PL_ERROK              0            // No error
#define PL_ERRWRONG_SIGNATURE 1            // Expected file signature
                                           // not found.
#define PL_ERRFORMAT_UNKNOWN  2            // Sanity check failed.

#define PL_ERRPATH_NOT_FOUND  3            // <--
#define PL_ERRFILE_NOT_FOUND  4            // <-- Problems with the file
#define PL_ERRACCESS_DENIED   5            // <-- system.

#define PL_ERRFORMAT_NOT_SUPPORTED 6       // Known but unsupported
                                           // format.
#define PL_ERRINTERNAL        7            // Internal error of some sort.
#define PL_ERRUNKNOWN_FILE_TYPE    8       // Couldn't recognize the
                                           // file type.
#define PL_ERRDIB_TOO_LARGE   9            // Maximum size for 1 bmp was
                                           // exceeded.
#define PL_ERRNO_MEMORY      10            // Out of memory.
#define PL_ERREND_OF_FILE    11            // End of file reached before
                                           // end of image.
#define PL_ERRFORMAT_NOT_COMPILED 12       // Support for this file format
                                           // was #ifdef'ed out.
#define PL_ERRURL_SOURCE     13            // Curl returned error.
#define PL_ERRBAD_EXIF       14            //


//! An object of this class is thrown by other classes when an error
//! occurs. It contains an error code and a string describing the
//! error. The error code is meant to be used internally in the
//! program; the descriptive string can be output to the user. Error
//! codes and strings do not correspond 1:1. The strings are more
//! precise.
class PLTextException : public PLObject
{

public:
  //! Creates an exception.
  PLTextException
    ( int Code,
      const char * pszErr
    );

  //! Creates an exception.
  PLTextException
    ( int Code,
      int MinorCode,
      const char * pszErr
    );

  //! Copy constructor.
  PLTextException
    ( const PLTextException& ex
    );

  //! Destroys an exception
  virtual ~PLTextException
    ();

  //! Returns the code of the error that caused the exception. Valid
  //! error codes are:
  //!
  //! PL_ERRWRONG_SIGNATURE (1): Expected file signature not found.
  //!
  //! PL_ERRFORMAT_UNKNOWN (2): Unexpected data encountered. This
  //! probably indicates a corrupt file or an unknown file
  //! sub-format.
  //!
  //! PL_ERRPATH_NOT_FOUND (3), PL_ERRFILE_NOT_FOUND (4),
  //! PL_ERRACCESS_DENIED (5): Problems with the file system.
  //!
  //! PL_ERRFORMAT_NOT_SUPPORTED (6): Known but unsupported format.
  //!
  //! PL_ERRINTERNAL (7): Kaputt. Tell me about it.
  //!
  //! PL_ERRUNKNOWN_FILE_TYPE (8): Couldn't recognize the file type.
  //!
  //! PL_ERRDIB_TOO_LARGE (9): Maximum size for 1 bmp was exceeded.
  //! (See MAX_BITMAP_SIZE above for an explanation).
  //!
  //! PL_ERRNO_MEMORY (10): Out of memory.
  //!
  //! PL_ERREND_OF_FILE (11): End of file reached before end of image.
  //!
  //! PL_ERRFORMAT_NOT_COMPILED (12): Support for this file format
  //! was #ifdef'ed out.
  //!
  //! PL_ERRURL_SOURCE (13): Curl returned error.
  //!
  //! PL_ERRBAD_EXIF (14): Error decoding EXIF data.
  virtual int GetCode
    ()
    const;

  //! In case of PL_ERRURL_SOURCE, the actual curl error code (from curl.h) is
  //! placed here.
  int GetMinorCode
    ()
    const;

  //! This operator allows the exception to be treated as a string
  //! whenever needed. The string contains the error message.
  virtual operator const char *
    ()
    const;

private:
  int     m_Code;
  int     m_MinorCode;
  char *  m_pszErr;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plexcept.h,v $
|      Revision 1.12  2004/09/15 14:52:09  artcom
|      - added PLPixelformatTest
|      - added DICOM Decoder
|
|      Revision 1.11  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.10  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.9  2003/11/21 23:35:44  uzadow
|      Removed files built by the autotools.
|
|      Revision 1.8  2003/08/03 12:03:20  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.7  2003/04/20 12:44:47  uzadow
|      Added EXIF documentation.
|
|      Revision 1.6  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|      Revision 1.5  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|      Revision 1.4  2002/11/18 14:44:46  uzadow
|      Added PNG compression support, fixed docs.
|
|      Revision 1.3  2002/08/07 18:50:38  uzadow
|      More cygwin compatibility changes.
|
|      Revision 1.2  2002/03/03 16:29:55  uzadow
|      Re-added BPPWanted.
|
|      Revision 1.1  2001/09/16 19:03:22  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2000/01/17 23:38:07  Ulrich von Zadow
|      MFC removal aftermath.
|
|
\--------------------------------------------------------------------
*/
