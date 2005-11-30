/*
/--------------------------------------------------------------------
|
|      $Id: plurlsrc.h,v 1.4 2004/08/20 14:39:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLURLSRC
#define INCL_PLURLSRC

#ifndef INCL_PLDATASRC
#include "pldatasrc.h"
#endif

#include <string>
#include <curl/curl.h>

// Internal singleton class that initializes and deinitializes curl
class PLCurlInitializer
{
public:
  CURL * getHandle();
  void finalize();

private:
  friend class PLURLSource;
  PLCurlInitializer();
  ~PLCurlInitializer();

  CURL * m_CurlHandle;

};

// This is a class which takes a URL as a source of
// picture data.
class PLURLSource : public PLDataSource
{
public:
  //!
  PLURLSource
    ();

  //!
  virtual ~PLURLSource
    ();

  //!
  virtual int Open(const char *pURL);

  //!
  virtual void Close
    ();

  virtual PLBYTE * ReadNBytes
    ( int n
    );

  //! Read but don't advance file pointer.
  virtual PLBYTE * GetBufferPtr
    ( int MinBytesInBuffer
    );

  //! This is a legacy routine that interferes with progress notifications.
  //! Don't call it!
  virtual PLBYTE * ReadEverything
    ();

  virtual void Seek
    ( int n
    );

  const char * GetCurlErrStr ();
  int GetCurlErrCode ();

private:
  static size_t WriteCurlData(void *ptr, size_t size, size_t nmemb, void *stream);
  const std::string httpErr2Str(int HTTPCode);
  // So curl gets initialized once only.
  static PLCurlInitializer s_CurlInitializer;
  std::string m_RawData;
  int m_CurPos;

  std::string m_sErrStr;
  int m_ErrCode;
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plurlsrc.h,v $
|      Revision 1.4  2004/08/20 14:39:38  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.3  2003/04/20 12:44:47  uzadow
|      Added EXIF documentation.
|
|      Revision 1.2  2003/04/14 10:48:19  uzadow
|      Added connection reuse capability to PLURLSource.
|
|      Revision 1.1  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|
\--------------------------------------------------------------------
*/
