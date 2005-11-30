/*
/--------------------------------------------------------------------
|
|      $Id: pltest.cpp,v 1.10 2004/09/11 14:15:38 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "pltest.h"
#include "pldebug.h"

#include <iostream>

using namespace std;

PLTest::PLTest(void)
: m_bOk(true),
  m_NumSucceeded(0),
  m_NumFailed(0)
{
}

PLTest::~PLTest(void)
{
}

// TODO: Make this into a macro that prints out the test that failed.
void PLTest::Test (bool b)
{
  if (b)
  {
    m_NumSucceeded++;
  }
  else
  {
    cerr << "           ---->> failed\n";
    m_bOk = false;
    m_NumFailed++;
  }
// Comment in to stop when a test fails
//  PLASSERT (b);
}


bool PLTest::IsOk ()
{
  return m_bOk;
}

void PLTest::SetFailed ()
{
  m_NumFailed++;
  m_bOk = false;
}

int PLTest::GetNumSucceeded () const
{
  return m_NumSucceeded;
}

int PLTest::GetNumFailed() const
{
  return m_NumFailed;
}

void PLTest::AggregateStatistics (const PLTest& ChildTest)
{
  m_NumSucceeded += ChildTest.GetNumSucceeded();
  m_NumFailed += ChildTest.GetNumFailed();
}

/*
/--------------------------------------------------------------------
|
|      $Log: pltest.cpp,v $
|      Revision 1.10  2004/09/11 14:15:38  uzadow
|      Comitted testimages, resized most of them.
|
|      Revision 1.9  2004/09/11 12:41:36  uzadow
|      removed plstdpch.h
|
|      Revision 1.8  2004/08/20 19:53:15  uzadow
|      no message
|
|      Revision 1.7  2003/02/24 22:10:35  uzadow
|      Linux version of MakeBmpFromURL() tests
|
|      Revision 1.6  2003/02/24 20:59:40  uzadow
|      Added windows url support, url support tests.
|
|      Revision 1.5  2002/10/20 22:11:53  uzadow
|      Partial doc update.
|      Fixed tests when images were not available.
|
|      Revision 1.4  2002/08/07 20:32:31  uzadow
|      Updated Makefile.in, fixed psd decoder layer stuff.
|
|      Revision 1.3  2002/08/06 20:13:37  uzadow
|      Cross-platform stuff.
|
|      Revision 1.2  2002/08/04 21:20:42  uzadow
|      no message
|
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
