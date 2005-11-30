/*
/--------------------------------------------------------------------
|
|      $Id: DocMan.h,v 1.6 2000/03/28 21:05:03 Ulrich von Zadow Exp $
|
\--------------------------------------------------------------------
*/

#if !defined(AFX_DOCMANAGER_H__188308B4_0AAD_11D2_8A47_0000E81D3D27__INCLUDED_)
#define AFX_DOCMANAGER_H__188308B4_0AAD_11D2_8A47_0000E81D3D27__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CDocManagerEx

class CDocManagerEx : public CDocManager
{
    DECLARE_DYNAMIC(CDocManagerEx)

    // Construction
public:
    CDocManagerEx()
	{} ;
    virtual ~CDocManagerEx()
	{} ;

    // Overrides
    // helper for standard commdlg dialogs
    virtual BOOL DoPromptFileName(CString& fileName, UINT nIDSTitle,
                                  DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif
/*
/--------------------------------------------------------------------
|
|      $Log: DocMan.h,v $
|      Revision 1.6  2000/03/28 21:05:03  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.5  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.4  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.3  1999/10/19 21:32:53  Ulrich von Zadow
|      no message
|
|
--------------------------------------------------------------------
*/
