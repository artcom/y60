#if !defined(AFX_LIGHTNESSDLG_H__58D0F146_036F_4BB5_AD4E_108529B5D7BF__INCLUDED_)
#define AFX_LIGHTNESSDLG_H__58D0F146_036F_4BB5_AD4E_108529B5D7BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LightnessDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CLightnessDlg dialog

class CLightnessDlg : public CDialog
{
// Construction
public:
	CLightnessDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLightnessDlg)
	enum { IDD = IDD_LIGHTNESS };
	int		m_Lightness;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLightnessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLightnessDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LIGHTNESSDLG_H__58D0F146_036F_4BB5_AD4E_108529B5D7BF__INCLUDED_)
