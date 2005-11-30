/*
/--------------------------------------------------------------------
|
|      $Id: maindlg.h,v 1.7 2004/09/11 12:41:37 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
--------------------------------------------------------------------
*/

#include <afxmt.h>

#include "plwinbmp.h"
#include "mtdibwnd.h"
#include "plpicdec.h"
#include "pldibsect.h"

#ifdef TEST_DDRAW
#include <ddraw.h>
#endif

class CDibtestDlg : public CDialog
{
// Construction
public:
  CDibtestDlg
    ( char * pszDirName
    );

  ~CDibtestDlg
    ();

  void DoWork
    ();

// Dialog Data
  //{{AFX_DATA(CDibtestDlg)
  enum { IDD = IDD_DIBTEST_DIALOG };
          // NOTE: the ClassWizard will add data members here
  //}}AFX_DATA

  // ClassWizard generated virtual function overrides
  //{{AFX_VIRTUAL(CDibtestDlg)
  protected:
  virtual void DoDataExchange(CDataExchange* pDX);        // DDX/DDV support
  //}}AFX_VIRTUAL

// Implementation
protected:
  HICON m_hIcon;

  // Generated message map functions
  //{{AFX_MSG(CDibtestDlg)
  virtual BOOL OnInitDialog();
  afx_msg void OnPaint();
  afx_msg HCURSOR OnQueryDragIcon();
  afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
  afx_msg void OnDestroy();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
  DECLARE_MESSAGE_MAP()

private:
  void displayDir
    ( PLPicDecoder * pDecoder,
      const char * pszDirName
    );
    // Tries to display all files in a directory and its
    // subdirectories.

  void displayFilesInDir
    ( PLPicDecoder * pDecoder,
      const char * pszDirName,
      const char * pszFName
    );
    // Tries to display all files in a directory matching
    // the specified file name.

  void displayFile
    ( PLPicDecoder * pDecoder,
      char * pszFName
    );
    // Tries to load a file & display it. Does error checking.

  void doBmpTest
    ( int Width,
      int Height,
      const PLPixelFormat& pf,
      int ID
    );
    // Tests the CBmp class for defects by calling its methods
    // with one type of bitmap.

  PLDIBSection * m_pBmp;
  PLWinBmp     * m_pBigBmp;

  CMTDIBWnd   * m_pDIBWnd;

  CString       m_sDirName;

  CRITICAL_SECTION   m_Critical;
  CWinThread       * m_pThread;
  bool               m_bStopDecoding;
  CCriticalSection * m_pThreadCritical;

#ifdef TEST_DDRAW
  IDirectDraw * m_pDDraw;
#endif
};
/*
/--------------------------------------------------------------------
|
|      $Log: maindlg.h,v $
|      Revision 1.7  2004/09/11 12:41:37  uzadow
|      removed plstdpch.h
|
|      Revision 1.6  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.5  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.4  2000/08/13 12:11:44  Administrator
|      Added experimental DirectDraw-Support
|
|
--------------------------------------------------------------------
*/
