/*
/--------------------------------------------------------------------
|
|      $Id: testpsddecoder.h,v 1.3 2002/10/20 22:11:53 uzadow Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTESTPSDDECODER
#define INCL_PLTESTPSDDECODER

#include <string>

class PLTester;
class PLBmp;

class PLTestPSDDecoder
{

public:
  PLTestPSDDecoder ();

  virtual ~PLTestPSDDecoder();

  void RunTests ();

private:
  void test (const std::string& sFName);
};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: testpsddecoder.h,v $
|      Revision 1.3  2002/10/20 22:11:53  uzadow
|      Partial doc update.
|      Fixed tests when images were not available.
|
|      Revision 1.2  2001/10/21 17:54:40  uzadow
|      Linux compatibility
|
|      Revision 1.1  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|
\--------------------------------------------------------------------
*/
