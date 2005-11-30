/*
/--------------------------------------------------------------------
|
|      $Id: StdAfx.h,v 1.9 2002/03/31 13:36:42 uzadow Exp $
|
--------------------------------------------------------------------
*/

#if !defined(AFX_STDAFX_H__6907569C_335B_11D2_928D_004033A161F7__INCLUDED_)
#define AFX_STDAFX_H__6907569C_335B_11D2_928D_004033A161F7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>	    // MFC core and standard components
#include <afxext.h>	    // MFC extensions
#ifndef _AFX_NO_AFXCMN_PL_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_PL_SUPPORT

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

// #include <MinSize.h>

#include <stdlib.h>
#include <string.h>

#ifndef _WITHOUT_VFW_
#include <vfw.h>			// DrawDib support
#endif

// forward declare our main app-specific classes
class CPaintLibViewerApp;
class CPLViewerDoc;
class CPLView;

#define thisApp (*((CPaintLibViewerApp*)AfxGetApp()))

#include <math.h>

class PLFilter;

#endif // !defined(AFX_STDAFX_H__6907569C_335B_11D2_928D_004033A161F7__INCLUDED_)
/*
/--------------------------------------------------------------------
|
|      $Log: StdAfx.h,v $
|      Revision 1.9  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.8  2000/03/31 11:53:32  Ulrich von Zadow
|      Added quantization support.
|
|      Revision 1.7  2000/03/30 21:47:41  Ulrich von Zadow
|      Added zoom-in mode, PLWinBmpEx, conditional use of DrawDIB
|      and some other nice stuff by Bernard Delmée.
|
|      Revision 1.6  2000/03/28 21:05:05  Ulrich von Zadow
|      Added zoom capability.
|
|      Revision 1.5  2000/01/17 23:38:07  Ulrich von Zadow
|      MFC removal aftermath.
|
|      Revision 1.4  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.3  1999/12/02 17:07:36  Ulrich von Zadow
|      Changes by bdelmee.
|
|
--------------------------------------------------------------------
*/
