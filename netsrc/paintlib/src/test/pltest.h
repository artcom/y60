/*
/--------------------------------------------------------------------
|
|      $Id: pltest.h,v 1.1 2002/08/04 20:12:34 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLTEST
#define INCL_PLTEST

class PLTest
{
public:
  PLTest(void);
  virtual ~PLTest(void);

  bool IsOk ();
  virtual void RunTests () = 0;

  void Test (bool b);
  void SetFailed ();

  int GetNumSucceeded () const;
  int GetNumFailed () const;

  void AggregateStatistics (const PLTest& ChildTest);

private:
  bool m_bOk;
  int m_NumSucceeded;
  int m_NumFailed;

};

#endif

/*
/--------------------------------------------------------------------
|
|      $Log: pltest.h,v $
|      Revision 1.1  2002/08/04 20:12:34  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|
|
|
\--------------------------------------------------------------------
*/
