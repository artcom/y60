// VCPaintXDemoDlg.h : header file
//
//{{AFX_INCLUDES()
#include "image.h"
//}}AFX_INCLUDES

#ifdef _DEBUG
  #import "..\..\bin\debug\PaintX.dll"
#else
  #import "..\..\bin\release\PaintX.dll"
#endif

#if !defined(AFX_VCPAINTXDEMODLG_H__9B270259_CDCA_11D3_8258_0020AF58A97B__INCLUDED_)
#define AFX_VCPAINTXDEMODLG_H__9B270259_CDCA_11D3_8258_0020AF58A97B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CVCPaintXDemoDlg dialog

class CVCPaintXDemoDlg : public CDialog
{
// Construction
public:
	CVCPaintXDemoDlg(CWnd* pParent = NULL);	// standard constructor
	void LoadPictureTest(CString strFilename);
	void LoadResPictureTest(int iResourceID);

// Dialog Data
	//{{AFX_DATA(CVCPaintXDemoDlg)
	enum { IDD = IDD_VCPAINTXDEMO_DIALOG };
	CImage	m_Image;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCPaintXDemoDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
    PaintX::IPictureDecoderPtr MyPictureDecoder;
    void HandleErrors(_com_error comerr);

	// Generated message map functions
	//{{AFX_MSG(CVCPaintXDemoDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnClose();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCPAINTXDEMODLG_H__9B270259_CDCA_11D3_8258_0020AF58A97B__INCLUDED_)
