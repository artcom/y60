/*
/--------------------------------------------------------------------
|
|      $Id: DibCtl.h,v 1.8 2002/08/04 20:08:01 uzadow Exp $
|
\--------------------------------------------------------------------
*/
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

//  adapted to paintlib by bdelmee@skynet.be, 1998

#ifndef INCL_DIBCTL
#define INCL_DIBCTL

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "WinBmpEx.h"

/////////////////////////////////////////////////////////////////////////////
// CDIBStatic window

class CDIBStatic : public CStatic
{
    // Construction
public:
    CDIBStatic() : m_bHasFile(false)
	{} ;
    virtual ~CDIBStatic()
	{} ;

    // Attributes
private:
    PLWinBmpEx	m_DIB;
    bool	m_bHasFile;

    // Operations
public:
    bool IsValidDib() const
    {
        return m_bHasFile;
    }
    bool LoadDib(LPCTSTR lpszFileName);
    void UpdateDib();
    void RemoveDib();

    bool DoRealizePalette(bool bForceBackGround);

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CDIBStatic)
    //}}AFX_VIRTUAL

protected:
    void ClearDib();
    void PaintDib(bool bDibValid);

    // Generated message map functions
protected:
    //{{AFX_MSG(CDIBStatic)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnQueryNewPalette();
    afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // INCL_DIBCTL
/*
/--------------------------------------------------------------------
|
|      $Log: DibCtl.h,v $
|      Revision 1.8  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.7  2002/01/27 18:20:18  uzadow
|      Updated copyright message; corrected pcx decoder bug.
|
|      Revision 1.6  2000/03/28 21:05:03  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.5  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.4  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
