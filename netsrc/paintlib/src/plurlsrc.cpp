/*
/--------------------------------------------------------------------
|
|      $Id: plurlsrc.cpp,v 1.6 2004/09/11 12:41:35 uzadow Exp $
|      URL Data Source Class
|
|      This is a class which takes a URL as a source of
|      picture data.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plurlsrc.h"
#include "plexcept.h"

#include <stdlib.h>

using namespace std;

PLCurlInitializer PLURLSource::s_CurlInitializer;

PLURLSource::PLURLSource()
  : PLDataSource (NULL) // Progress notification not implemented.
{
  m_RawData.reserve(32786);  // Save a few reallocations.
}

PLURLSource::~PLURLSource
    ()
{
}

size_t PLURLSource::WriteCurlData(void *ptr, size_t size, size_t nmemb, void *stream)
{
  PLURLSource * This = (PLURLSource *)stream;
  This->m_RawData.append ((char*)ptr, size*nmemb);
  return size*nmemb;
}
 
int PLURLSource::Open (const char *pURL)
{
  // Easy implementation: get everything on open.
  CURL * CurlHandle = s_CurlInitializer.getHandle();

  curl_easy_setopt(CurlHandle, CURLOPT_URL, pURL);
  curl_easy_setopt(CurlHandle, CURLOPT_NOPROGRESS, 1);

  curl_easy_setopt(CurlHandle, CURLOPT_WRITEFUNCTION, WriteCurlData);
  curl_easy_setopt(CurlHandle, CURLOPT_FILE, (void*)this);  // New curl version wants CURLOPT_WRITEDATA here.
  char szErrBuf[CURL_ERROR_SIZE];
  curl_easy_setopt(CurlHandle, CURLOPT_ERRORBUFFER, szErrBuf);

  CURLcode res = curl_easy_perform(CurlHandle);
  if (res != CURLE_OK) {
    m_ErrCode = res;
    m_sErrStr = szErrBuf;
    return PL_ERRURL_SOURCE;
  } else {
    long HTTPCode;
    CURLcode res = curl_easy_getinfo(CurlHandle, CURLINFO_HTTP_CODE, &HTTPCode);
    if (HTTPCode > 399) {
        m_ErrCode = HTTPCode;
        m_sErrStr = httpErr2Str(HTTPCode);
        return PL_ERRURL_SOURCE;
    }

    m_CurPos = 0;
    PLDataSource::Open(pURL, m_RawData.length());
    return 0;
  }
}

void PLURLSource::Close
    ()
{
  m_RawData = "";
  PLDataSource::Close();
}


PLBYTE * PLURLSource::ReadNBytes
    ( int n
    )
{
  PLDataSource::ReadNBytes(n);
  
  m_CurPos += n;
  
  return (PLBYTE*)m_RawData.data()+m_CurPos-n;
}


PLBYTE * PLURLSource::ReadEverything
    ()
{
  return (PLBYTE*)(m_RawData.data()+m_CurPos);
}

void PLURLSource::Seek
    ( int n
    )
{
  m_CurPos = n;
}

// Read but don't advance file pointer.
PLBYTE * PLURLSource::GetBufferPtr
    ( int MinBytesInBuffer
    )
{
  return (PLBYTE*)(m_RawData.data()+m_CurPos);
}

const char * PLURLSource::GetCurlErrStr()
{
  return m_sErrStr.c_str();
}

int PLURLSource::GetCurlErrCode()
{
  return m_ErrCode;
}

const string PLURLSource::httpErr2Str(int HTTPCode)
{
  switch (HTTPCode) {
    case 400: return "Bad Request";
    case 401: return "Unauthorized";
    case 402: return "Payment Required";
    case 403: return "Forbidden";
    case 404: return "Not Found";
    case 405: return "Method Not Allowed";
    case 406: return "Not Acceptable";
    case 407: return "Proxy Authentication Required";
    case 408: return "Request Time-out";
    case 409: return "Conflict";
    case 410: return "Gone";
    case 411: return "Length Required";
    case 412: return "Precondition Failed";
    case 413: return "Request Entity Too Large";
    case 414: return "Request-URI Too Large";
    case 415: return "Unsupported Media Type";
    case 416: return "Requested range not satisfiable";
    case 417: return "Expectation Failed";
    case 500: return "Internal Server Error";
    case 501: return "Not Implemented";
    case 502: return "Bad Gateway";
    case 503: return "Service Unavailable";
    case 504: return "Gateway Time-out";
    case 505: return "HTTP Version not supported";
    default: {
       char sz[10];
#ifdef WIN32       
      _itoa(HTTPCode, sz, 10);
#else
      sprintf (sz, "%d", HTTPCode);
#endif      
      return string("Unknown error")+sz;
    }
  }
}

PLCurlInitializer::PLCurlInitializer () 
{
  curl_global_init(CURL_GLOBAL_ALL);
  m_CurlHandle = curl_easy_init();
  
  
}

PLCurlInitializer::~PLCurlInitializer ()
{
  finalize();
}

CURL * PLCurlInitializer::getHandle()
{
  return m_CurlHandle;
}

void PLCurlInitializer::finalize()
{
  curl_easy_cleanup(m_CurlHandle);
  curl_global_cleanup();
}


/*
/--------------------------------------------------------------------
|
|      $Log: plurlsrc.cpp,v $
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2004/08/20 14:39:38  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.4  2004/04/18 12:34:45  uzadow
|      Changes for new libcurl version.
|
|      Revision 1.3  2003/04/14 10:48:19  uzadow
|      Added connection reuse capability to PLURLSource.
|
|      Revision 1.2  2003/02/24 16:19:49  uzadow
|      Added url source (linux only), improved configure support for libungif detection.
|
|      Revision 1.1  2003/02/15 21:26:58  uzadow
|      Added win32 version of url data source.
|
|
\--------------------------------------------------------------------
*/
