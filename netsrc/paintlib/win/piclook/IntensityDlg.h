#if !defined(AFX_INTENSITYDLG_H__E39430BC_2863_4B3E_8DDF_BBBFAF3E15A6__INCLUDED_)
#define AFX_INTENSITYDLG_H__E39430BC_2863_4B3E_8DDF_BBBFAF3E15A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IntensityDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIntensityDlg dialog

class CIntensityDlg : public CDialog
{
// Construction
public:
	CIntensityDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIntensityDlg)
	enum { IDD = IDD_INTENSITY };
	double	m_Exponent;
	double	m_Intensity;
	int		m_Offset;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIntensityDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIntensityDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTENSITYDLG_H__E39430BC_2863_4B3E_8DDF_BBBFAF3E15A6__INCLUDED_)
