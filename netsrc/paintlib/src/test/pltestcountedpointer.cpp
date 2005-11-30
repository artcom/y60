/*
/--------------------------------------------------------------------
|
|      $Id: pltestcountedpointer.cpp,v 1.3 2004/09/11 12:41:36 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltestcountedpointer.h"
#include "pltester.h"

#include <iostream>
#include <string>
#include <fstream>

#include "plpaintlibdefs.h"

#include "plcountedpointer.h"

PLTestCountedPointer::PLTestCountedPointer ()
{
}

PLTestCountedPointer::~PLTestCountedPointer ()
{
}

void PLTestCountedPointer::RunTests ()
{
  PLTRACE ("  Testing basic counted pointer...\n");
  int j;
  PLCountedPointer<int> IntPtr(new int(3));
  j = *IntPtr;
  PLASSERT(j==3);
  
  PLCountedPointer<int> IntPtr2(IntPtr);
  PLASSERT(*IntPtr2==3);
  PLASSERT(IntPtr2==IntPtr);
  
  IntPtr = PLCountedPointer<int>(NULL);
  PLASSERT(*IntPtr2==3);
  
  IntPtr2 = PLCountedPointer<int>(new int(4));
  PLASSERT(*IntPtr2!=3);
  
  PLTRACE ("  Testing counted array pointer...\n");
  PLCountedArrayPointer<PLBYTE> ByteArrayPtr(new PLBYTE[5]);
  ByteArrayPtr[0] = 23;
  PLASSERT (ByteArrayPtr[0] == 23);
  PLASSERT (ByteArrayPtr[0] != 24);
  
  PLCountedArrayPointer<PLBYTE> ByteArrayPtr2(ByteArrayPtr);
  PLASSERT(ByteArrayPtr2[0]==23);
  PLASSERT(ByteArrayPtr2==ByteArrayPtr);
  PLASSERT((*ByteArrayPtr2)==23);
  
  ByteArrayPtr = PLCountedArrayPointer<PLBYTE>(NULL);
  PLASSERT(ByteArrayPtr2[0]==23);
  
  ByteArrayPtr2 = PLCountedArrayPointer<PLBYTE>(new PLBYTE[2]);
  PLASSERT(ByteArrayPtr2[0]!=23);
  
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltestcountedpointer.cpp,v $
|      Revision 1.3  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.2  2004/09/11 10:30:40  uzadow
|      Linux build fixes, automake dependency fixes.
|
|      Revision 1.1  2003/04/13 20:13:21  uzadow
|      Added counted pointer classes (windows ver.)
|
|
|
\--------------------------------------------------------------------
*/
