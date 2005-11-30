#if !defined(AFX_THRESHOLDDLG_H__EB7B35A0_8779_11D4_B5A2_00D00915E14A__INCLUDED_)
#define AFX_THRESHOLDDLG_H__EB7B35A0_8779_11D4_B5A2_00D00915E14A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ThresholdDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CThresholdDlg dialog

class CThresholdDlg : public CDialog
{
// Construction
public:
	int GetMaxThreshold();
	int GetChannel();
	int GetMinThreshold();
	int m_channel;
	CThresholdDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CThresholdDlg)
	enum { IDD = IDD_THRESHOLDFILTER };
	int		m_threshold_max;
	int		m_threshold_min;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CThresholdDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CThresholdDlg)
	afx_msg void OnBlueRadio();
	afx_msg void OnGreenRadio();
	afx_msg void OnRedRadio();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_THRESHOLDDLG_H__EB7B35A0_8779_11D4_B5A2_00D00915E14A__INCLUDED_)
