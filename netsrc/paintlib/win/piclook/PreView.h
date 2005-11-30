/////////////////////////////////////////////////////////////////////////////
// Copyright (C) 1998 by Jorge Lodos
// All rights reserved
//
// Distribute and use freely, except:
// 1. Don't alter or remove this notice.
// 2. Mark the changes you made
//
// Send bug reports, bug fixes, enhancements, requests, etc. to:
//    lodos@cigb.edu.cu
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_)
#define AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PreviewFileDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPreviewFileDlg dialog

class CPreviewFileDlg : public CFileDialog
{
    DECLARE_DYNAMIC(CPreviewFileDlg)

public:
    CPreviewFileDlg(BOOL bOpenFileDialog, // true for FileOpen, false for FileSaveAs
                    LPCTSTR lpszDefExt = NULL,
                    LPCTSTR lpszFileName = NULL,
                    DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
                    LPCTSTR lpszFilter = NULL,
                    CWnd* pParentWnd = NULL);

    // Attributes
private:
    bool m_bPreview;
    CDIBStatic m_DIBStaticCtrl;

protected:
    //{{AFX_MSG(CPreviewFileDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnPreview();
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
    afx_msg void OnSetFocus(CWnd* pOldWnd);
    //}}AFX_MSG
    virtual void OnFileNameChange();

    // Implementation
#ifdef _DEBUG
    virtual void Dump(CDumpContext& dc) const;
#endif
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PREVIEWFILEDLG_H__1D054314_0872_11D2_8A46_0000E81D3D27__INCLUDED_)
/*
/--------------------------------------------------------------------
|
|      $Log: PreView.h,v $
|      Revision 1.7  2001/10/06 15:31:10  uzadow
|      Fixed open file dialog bug.
|
|      Revision 1.6  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/03/28 21:05:04  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.4  2000/01/10 23:53:02  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/12/02 17:07:36  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
