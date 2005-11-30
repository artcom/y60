#if !defined(AFX_CROPFILTERDLG_H__41840B01_8737_11D3_9BDF_0020A9004404__INCLUDED_)
#define AFX_CROPFILTERDLG_H__41840B01_8737_11D3_9BDF_0020A9004404__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// CropFilterDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CCropFilterDlg dialog

class CCropFilterDlg : public CDialog
{
    // Construction
public:
    CCropFilterDlg(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
    //{{AFX_DATA(CCropFilterDlg)
    enum { IDD = IDD_CROPFILTER };
    int    m_XMax;
    int    m_XMin;
    int    m_YMax;
    int    m_YMin;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCropFilterDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CCropFilterDlg)
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CROPFILTERDLG_H__41840B01_8737_11D3_9BDF_0020A9004404__INCLUDED_)
