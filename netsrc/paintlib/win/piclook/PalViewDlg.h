#if !defined(AFX_PALVIEWDLG_H__B222EB8D_89CE_464B_A3F7_39A5B9C22891__INCLUDED_)
#define AFX_PALVIEWDLG_H__B222EB8D_89CE_464B_A3F7_39A5B9C22891__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PalViewDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPalViewDlg dialog

class CPalViewDlg : public CDialog
{
// Construction
public:
	CPalViewDlg(CWnd* pParent, PLPixel32 * pPal);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPalViewDlg)
	enum { IDD = IDD_PALETTE };
	CStatic	m_PalFrameCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPalViewDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPalViewDlg)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
  PLPixel32 * m_pPal;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PALVIEWDLG_H__B222EB8D_89CE_464B_A3F7_39A5B9C22891__INCLUDED_)
