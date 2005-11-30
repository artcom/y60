/*
/--------------------------------------------------------------------
|
|      $Id: pltestcountedpointer.h,v 1.1 2003/04/13 20:13:21 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTCOUNTEDPOINTER
#define INCL_PLTESTCOUNTEDPOINTER

#include "pltest.h"

class PLBmp;

class PLTestCountedPointer: public PLTest
{

public:
  PLTestCountedPointer ();

  virtual ~PLTestCountedPointer();

  virtual void RunTests ();

private:
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: pltestcountedpointer.h,v $
|      Revision 1.1  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|
|
\--------------------------------------------------------------------
*/
