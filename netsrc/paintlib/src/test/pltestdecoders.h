/*
/--------------------------------------------------------------------
|
|      $Id: pltestdecoders.h,v 1.5 2004/03/04 18:05:58 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTDECODERS
#define INCL_PLTESTDECODERS

#include "pltest.h"
#include <stdio.h>

#include <string>

class PLBmp;

class PLTestDecoders: public PLTest
{
public:
  PLTestDecoders();
  virtual ~PLTestDecoders();

  virtual void RunTests ();

private:
  void getDirInfo (const char * pszDirName);
  void addToInfoFile (const char * pszDirName, const char * pszFName);

  void testImage (std::string sLine);
  void testFileNotFound();
  std::string makeURLFromFName(const std::string& sFName);

  FILE * m_InfoFile;

};

#endif


/*
/--------------------------------------------------------------------
|
|      $Log: pltestdecoders.h,v $
|      Revision 1.5  2004/03/04 18:05:58  uzadow
|      Fixed linux file not found exception bug.
|
|      Revision 1.4  2003/02/24 20:59:40  uzadow
|      Added windows url support, url support tests.
|
|      Revision 1.3  2002/11/02 22:38:28  uzadow
|      Fixed \\r handling in TestBmpList.txt, removed libtool from cvs
|
|      Revision 1.2  2002/08/06 20:13:37  uzadow
|      Cross-platform stuff.
|
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
