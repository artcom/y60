/*
/--------------------------------------------------------------------
|
|      $Id: MainFrm.h,v 1.7 2001/09/16 19:03:23 uzadow Exp $
|      highest-level frame window class declaration
|
|      Copyright (c) 1998 Bernard Delmée
|
\--------------------------------------------------------------------
*/

#ifndef INCL_MFRM
#define INCL_MFRM

#ifndef __AFXEXT_H__
#include <afxext.h>      // for access to CToolBar and CStatusBar
#endif

class CMainFrame : public CMDIFrameWnd
{
    DECLARE_DYNAMIC(CMainFrame)
public:
    CMainFrame();

    // Implementation
public:
    virtual ~CMainFrame();
    void SetProgressInfo(UINT part);


protected:  // control bar embedded members
    CStatusBar  m_wndStatusBar;
    CToolBar  m_wndToolBar;

    // Generated message map functions
protected:
    //{{AFX_MSG(CMainFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
    afx_msg void OnActivate(UINT nState, CWnd* pWndOther, bool bMinimized);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
    afx_msg void OnUpdatePanels(CCmdUI *pCmdUI);
};

#endif  // INCL_MFRM
/*
/--------------------------------------------------------------------
|
|      $Log: MainFrm.h,v $
|      Revision 1.7  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.6  2000/03/28 21:05:04  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.5  2000/01/10 23:53:02  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.4  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
