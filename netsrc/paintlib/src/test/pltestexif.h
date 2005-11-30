/*
/--------------------------------------------------------------------
|
|      $Id: pltestexif.h,v 1.2 2003/04/19 19:04:00 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTEXIF
#define INCL_PLTESTEXIF

#include "pltest.h"

#include <string>

class PLTestExif: public PLTest
{

public:
  PLTestExif ();

  virtual ~PLTestExif();

  virtual void RunTests ();

private:
  void testImage
    ( const std::string& sFName, 
      const std::string& sMakeExpected, 
      int NumTags);
      
  void testEncode();
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltestexif.h,v $
|      Revision 1.2  2003/04/19 19:04:00  uzadow
|      Exif save (windows)
|
|      Revision 1.1  2003/04/13 21:51:43  uzadow
|      Added exif loading - windows ver.
|
|
\--------------------------------------------------------------------
*/
