/*
/----------------------------------------------------------------
|
|      $Id: dibwnd.h,v 1.4 2001/09/16 19:03:23 uzadow Exp $
|      Window containing a bitmap.
|
|      Child window containing a bitmap. Handles
|      scrolling and resize.
|
|      Copyright (c) 1996 Ulrich von Zadow
|
\----------------------------------------------------------------
*/

#ifndef INCL_DIBWND
#define INCL_DIBWND

#include "plwinbmp.h"

class CDIBWnd : public CWnd
{

DECLARE_DYNAMIC (CDIBWnd);
// Construction
public:
  CDIBWnd
    ( PLWinBmp * pBmp
    );

  virtual ~CDIBWnd
    ();

// Implementation
public:
  virtual void NewDIBNotify
    ();
    // Call this whenever the bitmap is changed outside of the object.

protected:
// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(CDIBWnd)
        //}}AFX_VIRTUAL

  // Generated message map functions
  //{{AFX_MSG(CDIBWnd)
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnSize(UINT nType, int cx, int cy);
  afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        afx_msg void OnPaint();
        afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
        //}}AFX_MSG
  DECLARE_MESSAGE_MAP()

  virtual BOOL PreCreateWindow (CREATESTRUCT& cs);

protected:
  void initScrollBars
    ( int cx,
      int cy
    );

  PLWinBmp   * m_pBmp;
  CSize       m_Size;
  CSize       m_BmpSize;
  CPoint      m_BmpPos;
  bool        m_bScrolling;

private:
  void initScrollBar
    ( int ID,
      int Page,
      int Range,
      int Pos
    );

  int handleScroll
    ( int ID,
      UINT nSBCode,
      UINT nPos,
      int Page,
      int Range,
      int Pos
    );

};

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: dibwnd.h,v $
|      Revision 1.4  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.3  2000/01/16 20:43:19  anonymous
|      Removed MFC dependencies
|
|
\--------------------------------------------------------------------
*/
