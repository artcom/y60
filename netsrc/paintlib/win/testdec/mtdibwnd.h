/*
/----------------------------------------------------------------
|
|      $Id: mtdibwnd.h,v 1.3 2002/01/28 20:18:46 uzadow Exp $
|      Window containing a bitmap. Thread-safe.
|
|      Child window containing a bitmap. Handles
|      scrolling and resize.
|
|      A critical section is set whenever the window needs to
|      access the bitmap data.
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\----------------------------------------------------------------
*/

#ifndef INCL_MTDIBWND
#define INCL_MTDIBWND

#include "dibwnd.h"

class CMTDIBWnd : public CDIBWnd
{

DECLARE_DYNAMIC (CMTDIBWnd);
// Construction
public:
  CMTDIBWnd
    ( PLWinBmp * pBmp,
      CRITICAL_SECTION * pCritical
    );

// Implementation
public:
  virtual void NewDIBNotify();

protected:
// Overrides
  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CMTDIBWnd)
  //}}AFX_VIRTUAL

  // Generated message map functions
  //{{AFX_MSG(CMTDIBWnd)
  afx_msg void OnPaint();
  //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  CRITICAL_SECTION * m_pCritical;

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: mtdibwnd.h,v $
|      Revision 1.3  2002/01/28 20:18:46  uzadow
|      no message
|
|
--------------------------------------------------------------------
*/
