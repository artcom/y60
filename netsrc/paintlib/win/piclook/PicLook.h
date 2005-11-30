/*
/--------------------------------------------------------------------
|
|      $Id: PicLook.h,v 1.6 2001/09/16 19:03:23 uzadow Exp $
|      application class declaration
|
|      PaintLib MDI MFC graphic files viewer
|
|      Copyright (c) 1998 Bernard Delmée courtesy of Dilys bvba.
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PICLOOK
#define INCL_PICLOOK

#ifndef __AFXWIN_H__
#   error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"    // main symbols
#include "planydec.h"        // PaintLib multi-format decoder class

//  private message for palette negotiations
#define WM_DOREALIZE   (WM_USER + 0)

/////////////////////////////////////////////////////////////////////////////
// CPaintLibViewerApp :
// See PicLook.cpp for the implementation of this class
//

class CPaintLibViewerApp : public CWinApp
{
public:
    CPaintLibViewerApp();
    PLAnyPicDecoder *GetDecoder()
    {
        return &m_theDecoder;
    }

    // Overrides
    virtual BOOL InitInstance();
    
    int GetScreenBPP() const;

protected:
    // global decoder object (embedded in unique app instance)
    PLAnyPicDecoder  m_theDecoder;

    // Implementation

    //{{AFX_MSG(CPaintLibViewerApp)
    afx_msg void OnAppAbout();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // INCL_PICLOOK
/*
/--------------------------------------------------------------------
|
|      $Log: PicLook.h,v $
|      Revision 1.6  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/03/28 21:05:04  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.4  2000/01/10 23:53:02  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
