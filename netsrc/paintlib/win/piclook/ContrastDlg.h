#if !defined(AFX_CONTRASTDLG_H__7B674093_B2DC_4E9C_B6BA_B66052C2D440__INCLUDED_)
#define AFX_CONTRASTDLG_H__7B674093_B2DC_4E9C_B6BA_B66052C2D440__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ContrastDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CContrastDlg dialog

class CContrastDlg : public CDialog
{
// Construction
public:
	CContrastDlg(CWnd* pParent = NULL);   // standard constructor

	PLBYTE GetOffset();
	double GetContrast();

// Dialog Data
	//{{AFX_DATA(CContrastDlg)
	enum { IDD = IDD_CONTRAST };
	int		m_Offset;
	double	m_Contrast;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContrastDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CContrastDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONTRASTDLG_H__7B674093_B2DC_4E9C_B6BA_B66052C2D440__INCLUDED_)
