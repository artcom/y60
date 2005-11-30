/*
/--------------------------------------------------------------------
|
|      $Id: Doc.h,v 1.9 2001/09/16 19:03:23 uzadow Exp $
|      Document class declaration
|
|      Copyright (c) 1998 Bernard Delmée
|
\--------------------------------------------------------------------
*/

#ifndef INCL_DOCUMENT
#define INCL_DOCUMENT

#include "WinBmpEx.h"

// Jo Hagelberg 15.4.99: inherit from PLIProgressNotification
#include "plprognot.h"


// Jo Hagelberg 15.4.99: inherit from PLIProgressNotification
class CPLViewerDoc : public CDocument, PLIProgressNotification
{
protected: // create from serialization only
    CPLViewerDoc();
    DECLARE_DYNCREATE(CPLViewerDoc)

    // Attributes
public:
    CSize GetDocSize()
    {
        return m_pDib->GetSize();
    }
    HPALETTE GetDocPalette()
    {
        return m_pDib->GetLogPalette();
    }

    // Operations
public:
    void ConvertTo8BPP(UINT iDitherPaletteType, UINT iDitherType);
    virtual void DeleteContents();
    PLWinBmpEx *GetDib()
    {
      return m_pDib;
    }
    // Jo Hagelberg 15.4.99: copied OnProgress from DibStatic
    void OnProgress( double Part);

    // Implementation
protected:
    virtual ~CPLViewerDoc();
    virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);

protected:
    PLWinBmpEx*	m_pDib;

#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // virtual bool OnSaveDocument( LPCTSTR lpszPathName );

    // Generated message map functions
protected:
    //{{AFX_MSG(CPLViewerDoc)
    afx_msg void OnFileSave();
    afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif  // INCL_DOCUMENT
/*
/--------------------------------------------------------------------
|
|      $Log: Doc.h,v $
|      Revision 1.9  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2000/03/31 11:53:31  Ulrich von Zadow
|      Added quantization support.
|
|      Revision 1.7  2000/03/28 21:05:03  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.6  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.5  1999/12/02 17:07:35  Ulrich von Zadow
|      Changes by bdelmee.
|
|      Revision 1.4  1999/11/08 22:15:05  Ulrich von Zadow
|      Added File/SaveAs
|
|
\--------------------------------------------------------------------
*/
