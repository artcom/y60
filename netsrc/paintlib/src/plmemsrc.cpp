/*
/--------------------------------------------------------------------
|
|      $Id: plmemsrc.cpp,v 1.6 2004/09/11 12:41:35 uzadow Exp $
|      Memory Data Source Class
|
|      This is a class which takes a memory region as a source of
|      picture data.
|      Original author: Patrick Strömstedt.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plmemsrc.h"
#include "plexcept.h"


PLMemSource::PLMemSource()
  : PLDataSource (NULL), // No progress notification nessesary when
                        // reading from memory.
    m_pCurPos(NULL),
    m_pStartPos(0)
{
}

PLMemSource::~PLMemSource
    ()
{
  if (m_pCurPos)
    Close();
}

int PLMemSource::Open (unsigned char *pek, int size)
{
  PLDataSource::Open ("Mem", size);
  m_pCurPos = pek;
  m_pStartPos = pek;
  return 0;
}

void PLMemSource::Close
    ()
{
  m_pCurPos = NULL;
  PLDataSource::Close();
}


PLBYTE * PLMemSource::ReadNBytes
    ( int n
    )
{
  PLDataSource::ReadNBytes(n);

  m_pCurPos += n;
  return m_pCurPos-n;
}


PLBYTE * PLMemSource::ReadEverything
    ()
{
  return m_pCurPos;  // ;-)
}

void PLMemSource::Seek
    ( int n
    )
{
    m_pCurPos = m_pStartPos+n;
}

// Read but don't advance file pointer.
PLBYTE * PLMemSource::GetBufferPtr
    ( int MinBytesInBuffer
    )
{
  return m_pCurPos;
}

/*
/--------------------------------------------------------------------
|
|      $Log: plmemsrc.cpp,v $
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2004/08/20 14:39:37  uzadow
|      Added rle format to sgi decoder. Doesn't work yet, though.
|
|      Revision 1.4  2002/03/31 13:36:42  uzadow
|      Updated copyright.
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
|      Revision 1.2  2001/02/04 14:31:52  uzadow
|      Member initialization list cleanup (Erik Hoffmann).
|
|      Revision 1.1  2000/03/17 10:51:38  Ulrich von Zadow
|      no message
|
|
\--------------------------------------------------------------------
*/
