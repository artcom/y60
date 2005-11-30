/*
/--------------------------------------------------------------------
|
|      $Id: FilterDialog.h,v 1.6 2001/09/16 19:03:23 uzadow Exp $
|
\--------------------------------------------------------------------
*/

#if !defined(AFX_FILTERDIALOG_H__68D45661_80F7_11D3_9BDF_0020A9004404__INCLUDED_)
#define AFX_FILTERDIALOG_H__68D45661_80F7_11D3_9BDF_0020A9004404__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class PLBmp;

class PLFilterDialog : public CDialog
{
    // Construction
public:
    PLFilterDialog (CWnd* pParent, PLBmp * pBmp,
                   const char * pszFilterName, bool bRadiusAllowed);
    int GetWidth();
    int GetHeight();
    double GetRadius();

    // Dialog Data
    //{{AFX_DATA(PLFilterDialog)
	enum { IDD = IDD_FILTER };
    CButton  m_ConstrainCheck;
    CStatic  m_RadiusStatic;
    CEdit  m_RadiusEdit;
    CStatic  m_NameStatic;
    CEdit  m_HeightEdit;
    CEdit  m_WidthEdit;
    int    m_Height;
    int    m_Width;
    double  m_Radius;
	//}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PLFilterDialog)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(PLFilterDialog)
    virtual void OnOK();
    afx_msg void OnChangeHeightedit();
    afx_msg void OnChangeWidthedit();
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    PLBmp * m_pBmp;
    const char * m_pszFilterName;
    bool m_bRadiusAllowed;
    bool m_bChanging;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FILTERDIALOG_H__68D45661_80F7_11D3_9BDF_0020A9004404__INCLUDED_)

/*
/--------------------------------------------------------------------
|
|      $Log: FilterDialog.h,v $
|      Revision 1.6  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.5  2000/11/06 23:22:52  uzadow
|      Added dialogs for Contrast and Intensity
|
|      Revision 1.4  2000/03/30 21:47:41  Ulrich von Zadow
|      Added zoom-in mode, PLWinBmpEx, conditional use of DrawDIB
|      and some other nice stuff by Bernard Delmée.
|
|      Revision 1.3  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.2  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.1  1999/10/19 21:33:49  Ulrich von Zadow
|      Added filter support.
|
|
\--------------------------------------------------------------------
*/
