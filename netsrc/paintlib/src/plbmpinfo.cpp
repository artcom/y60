/*
/--------------------------------------------------------------------
|
|      $Id: plbmpinfo.cpp,v 1.7 2004/10/02 22:23:12 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plbmpinfo.h"

#include <stdio.h>
#ifndef WIN32
#include <strings.h>
#include <string.h>
#endif

#include <vector>

using namespace std;

const char szInfoFormatString[] =
    "{ Size: (%i, %i), Resolution: (%i, %i), Pixelformat: %s }";

PLBmpInfo::PLBmpInfo (const PLPoint& Size, const PLPoint& Resolution,
                      const PLPixelFormat & pf)
  : m_pf(PLPixelFormat::DONTCARE)
{
  SetBmpInfo (Size, Resolution, pf);
}

PLBmpInfo::PLBmpInfo (const char * pszInfo)
  : m_pf(PLPixelFormat::DONTCARE)
{
  vector<char> pf(128);
  int NumFields = sscanf (pszInfo, szInfoFormatString,
          &m_Size.x, &m_Size.y, &m_Resolution.x, &m_Resolution.y,
           &pf[0]);
  
  m_pf = PLPixelFormat::FromName(&pf[0]);
 
  PLASSERT (NumFields == 5);
}

void PLBmpInfo::SetBmpInfo (const PLPoint& Size, const PLPoint& Resolution,
                            const PLPixelFormat & pf)
{
  m_Size = Size;
  m_pf = pf;
  m_Resolution = Resolution;  
}

void PLBmpInfo::SetBmpInfo (const PLBmpInfo& SrcInfo)
{
  *this = SrcInfo;
}

void PLBmpInfo::AsString (char * psz, int len) const
{
  char sz[1024];
  sprintf (sz, szInfoFormatString,
           m_Size.x, m_Size.y,m_Resolution.x, m_Resolution.y,
           m_pf.GetName().c_str());
  PLASSERT ((int)strlen (sz) < len);
  strcpy (psz, sz);
}

string PLBmpInfo::AsString () const
{
    char sz[1024];
    AsString(sz, 1023);
    return sz;
}

bool PLBmpInfo::operator == (const PLBmpInfo & Other) const
{
  return (m_Size == Other.GetSize() && 
          m_Resolution == Other.GetResolution() && m_pf == Other.m_pf);
}

/*
/--------------------------------------------------------------------
|
|      $Log: plbmpinfo.cpp,v $
|      Revision 1.7  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.6  2004/09/11 12:41:35  uzadow
|      removed plstdpch.h
|
|      Revision 1.5  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.4  2002/11/27 21:48:45  uzadow
|      Added include for more portability.
|
|      Revision 1.3  2002/11/06 22:43:00  uzadow
|      Win32 compatibility
|
|      Revision 1.2  2002/11/04 22:40:13  uzadow
|      Updated for gcc 3.1
|
|      Revision 1.1  2002/08/04 20:17:57  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|      Update to VS .NET
|
|
\--------------------------------------------------------------------
*/

