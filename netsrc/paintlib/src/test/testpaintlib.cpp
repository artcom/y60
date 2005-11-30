/*
/--------------------------------------------------------------------
|
|      $Id: testpaintlib.cpp,v 1.11 2004/11/09 15:55:06 artcom Exp $
|
|      Test suite for paintlib.
|
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifdef _WIN32
#include <windows.h>
#else
#include "plpaintlibdefs.h"
#endif

#include "pltester.h"

int main(int nargs, char** args)
{
  PLTester* pTester;
  pTester = new PLTester();
  pTester->RunTests();
  bool bOK = pTester->IsOk();
  delete pTester;

  if (bOK)
    return 0;
  else
    return 1;
}

/*
/--------------------------------------------------------------------
|
|      $Log: testpaintlib.cpp,v $
|      Revision 1.11  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.10  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.9  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.8  2002/11/04 22:40:14  uzadow
|      Updated for gcc 3.1
|
|      Revision 1.7  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.6  2002/02/24 13:00:50  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.5  2001/10/06 20:44:45  uzadow
|      Linux compatibility
|
|      Revision 1.4  2001/09/16 20:57:17  uzadow
|      Linux version name prefix changes
|
|      Revision 1.3  2001/01/15 15:05:31  uzadow
|      Added CBmp::ApplyFilter() and CBmp::CreateFilteredCopy()
|
|      Revision 1.2  2000/12/09 12:16:26  uzadow
|      Fixed several memory leaks.
|
|      Revision 1.1  2000/11/21 20:29:39  uzadow
|      Added test project.
|
|
|
\--------------------------------------------------------------------
*/
