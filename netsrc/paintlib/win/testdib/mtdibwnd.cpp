/*
/----------------------------------------------------------------
|
|      $Id: mtdibwnd.cpp,v 1.3 2002/01/28 20:19:51 uzadow Exp $
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

#include "stdafx.h"
#include "mtdibwnd.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CMTDIBWnd

IMPLEMENT_DYNAMIC (CMTDIBWnd, CDIBWnd);

CMTDIBWnd::CMTDIBWnd
    ( PLWinBmp * pBmp,
      CRITICAL_SECTION * pCritical
    )
  : CDIBWnd (pBmp)
{
  m_pCritical = pCritical;
}

/////////////////////////////////////////////////////////////////////////////
// CMTDIBWnd operations

void CMTDIBWnd::NewDIBNotify()
{
  CRect rect;
  GetClientRect (&rect);
  int cx = rect.right;
  int cy = rect.bottom;

  EnterCriticalSection (m_pCritical);
  m_BmpSize =  m_pBmp->GetSize();
  LeaveCriticalSection (m_pCritical);
  initScrollBars (cx, cy);
  InvalidateRect (false);
}

BEGIN_MESSAGE_MAP(CMTDIBWnd, CDIBWnd)
  //{{AFX_MSG_MAP(CMTDIBWnd)
  ON_WM_PAINT()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMTDIBWnd message handlers

void CMTDIBWnd::OnPaint()
{
  EnterCriticalSection (m_pCritical);
  CDIBWnd::OnPaint();
  LeaveCriticalSection (m_pCritical);
}
/*
/--------------------------------------------------------------------
|
|      $Log: mtdibwnd.cpp,v $
|      Revision 1.3  2002/01/28 20:19:51  uzadow
|      no message
|
|
--------------------------------------------------------------------
*/
