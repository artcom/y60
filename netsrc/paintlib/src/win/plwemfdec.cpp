/*
/--------------------------------------------------------------------
|
|      $Id: plwemfdec.cpp,v 1.9 2004/11/09 15:55:06 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#if _MSC_VER > 1
#pragma hdrstop
#endif

// This only makes sense for Windows
#ifdef _WIN32

#include "plfilesrc.h"
#include "plexcept.h"
#include "plwemfdec.h"
#include "plwinbmp.h"


// Some defines and types for Aldus Placeable Metafiles (APM)
// I got these from the Windows 3.1 SDK and modified them to
// match the 32-Bit environment

#define  ALDUSKEY   0x9AC6CDD7

#pragma pack(2)
struct SAPMFILEHEADER
{
  DWORD   key;
  WORD  hmf;
  short  Left;    // left, top, right, bottom in twips
  short  Top;
  short  Right;
  short  Bottom;
  WORD    inch;    // Number of twips per inch
  DWORD   reserved;
  WORD    checksum;
};
#pragma pack()


//###########################################################################
// PLWEMFDecoder
//###########################################################################


/////////////////////////////////////////////////////////////////////////////
// Ctor
PLWEMFDecoder::PLWEMFDecoder()
    : PLPicDecoder(),
      m_hemf(0),
	    m_memdc(0),
	    m_bm(0),
      m_dc(0),
      m_holdpal(0),
      m_phdr(0)
{}


/////////////////////////////////////////////////////////////////////////////
// Dtor

PLWEMFDecoder::~PLWEMFDecoder()
{}


/////////////////////////////////////////////////////////////////////////////
// Debugging support

#ifdef _DEBUG
void PLWEMFDecoder::AssertValid() const
{
}
#endif

void PLWEMFDecoder::Open (PLDataSource * pDataSrc)
{
	PLASSERT_VALID(this);
  PLASSERT(m_bm == 0);
  PLASSERT(m_memdc == 0);
  PLASSERT(m_hemf == 0);

	SAPMFILEHEADER* pplaceablehdr = NULL;
	bool isadobe = false;
      bool isemf;

	// Get the type of the file (WMF or EMF) from the file name
      if (pDataSrc->NameIsWide()){
        wchar_t* strname = wcsdup(pDataSrc->GetNameW());
        PLASSERT(strname);
        if (strname == NULL) {
                // This should never happen under 32-Bit, but who knows?
                PLASSERT(false);
                raiseError (PL_ERRNO_MEMORY,"Out of memory during strdup.");
        }
        wcsupr(strname);
        isemf = wcsstr(strname,L".EMF") != NULL;
        free(strname);
      }
      else{
	char* strname = strdup(pDataSrc->GetName());
	PLASSERT(strname);
	if (strname == NULL) {
		// This should never happen under 32-Bit, but who knows?
		PLASSERT(false);
		raiseError (PL_ERRNO_MEMORY,"Out of memory during strdup.");
	}
	strupr(strname);
	bool isemf = strstr(strname,".EMF") != NULL;
	free(strname);
      }

	// Get a DC for the display
	m_dc = ::GetDC(NULL);
	PLASSERT(m_dc);
	if (m_dc == NULL) {
		PLASSERT(false);
		raiseError (PL_ERRNO_MEMORY,"Cannot allocate device context.");
	}

	if (isemf) {
		// We have an enhanced meta file which makes it alot easier
		m_hemf = SetEnhMetaFileBits(pDataSrc->GetFileSize(),pDataSrc->ReadEverything());
	}
	else {
		// Buh, old 16-Bit WMF, Convert it to an enhanced metafile before proceeding.
		// Also, check if this is a placeable metafile with an Adobe Placeable header
		pplaceablehdr = (SAPMFILEHEADER*)pDataSrc->ReadEverything();
		PLBYTE* p = NULL;
		UINT size;
		// If we have an adobe header, skip it to use only the real windows-conform data
		if (pplaceablehdr->key == ALDUSKEY) {
			isadobe = true;
			p = pDataSrc->ReadEverything()+sizeof(SAPMFILEHEADER);
			size = pDataSrc->GetFileSize() - sizeof(SAPMFILEHEADER);
		}
		else {
			// Else use the whole file contents as the metafile and assume
			// a native 16-Bit Windows-conform WMF
			p = pDataSrc->ReadEverything();
			size = pDataSrc->GetFileSize();
		}
		#ifdef _MFC_VER
		PLASSERT(AfxIsValidAddress(p,size,false));
		#endif
		m_hemf = SetWinMetaFileBits(size,p,m_dc,NULL);
	}

	// If m_hemf is NULL, windows refused to load the metafile. If this is
	// the case, we're done. Notify the caller
	if (m_hemf == NULL) {
		raiseError (PL_ERRFORMAT_NOT_SUPPORTED,"Windows Metafile functions failed to load this image.");
	}

	// Get the header from the enhanced metafile, It contains some
	// useful information which will aid us during constuction of
	// the bitmap.
	// The header is of variable length. First get the amount of
	//  memory required for the header
	UINT sizeneeded = GetEnhMetaFileHeader(m_hemf,0,NULL);
	if (sizeneeded == 0) {
		raiseError (PL_ERRFORMAT_UNKNOWN,"No header information in metafile");
	}

	// Allocate storage for the header and read it in
	m_phdr = (LPENHMETAHEADER) new PLBYTE[sizeneeded];
	if (m_phdr == NULL) {
		PLASSERT(false);
		raiseError (PL_ERRNO_MEMORY,"Out of memory during allocation of header.");
	}
	m_phdr->iType = EMR_HEADER;
	m_phdr->nSize = sizeneeded;
	#ifdef _MFC_VER
	PLASSERT(AfxIsValidAddress(m_phdr,sizeneeded,true));
	#endif
	GetEnhMetaFileHeader(m_hemf,sizeneeded,m_phdr);

	int bpp = GetDeviceCaps(m_dc,BITSPIXEL);

	// Calculate the dimensions of the final bitmap. If we have
	// a placeable header in the WMF, we use the dimensions of
	// that image, else we use the calculated dimensions in the
	// EMF
	int width,height;
	if (isadobe) {
		PLASSERT(pplaceablehdr);
		int lpx = GetDeviceCaps(m_dc,LOGPIXELSX);
		int lpy = GetDeviceCaps(m_dc,LOGPIXELSY);
		// Calculate the absolute with and height and transform from twips to pixel
		width  = (int) (pplaceablehdr->Right-pplaceablehdr->Left) * lpx / pplaceablehdr->inch;
		height = (int) (pplaceablehdr->Bottom-pplaceablehdr->Top) * lpy / pplaceablehdr->inch;
	}
	else {
		// Use the rclFrame of the header because it is the true device independent
		// information and also some applications (e.g. Corel) don't fill the
		// rclBounds correctly
		// Using:
		//     MetaPixelsX = MetaWidthMM * MetaPixels / (MetaMM * 100);
		// where:
		//     MetaWidthMM = metafile width in 0.01mm units
		//     MetaPixels  = width in pixels of the reference device
		//     MetaMM      = width in millimeters of the reference device
		// Same applies to the Y axis
		width  = ((m_phdr->rclFrame.right  - m_phdr->rclFrame.left) * m_phdr->szlDevice.cx) / (m_phdr->szlMillimeters.cx*100);
		height = ((m_phdr->rclFrame.bottom  - m_phdr->rclFrame.top) * m_phdr->szlDevice.cy) / (m_phdr->szlMillimeters.cy*100);
	}

	// If this is a very old WMF without a PLACEABLE info,
	// we use somewhat meaningful defaults. Also, if the header was
	// not written correctly, we use this as a fallback
	if (width <= 0) {
		width = 320;
	}
	if (height <= 0) {
		height = 200;
	}

	// Create a device content for the screen, and a memory device
	// content to play the metafile to

	m_memdc = CreateCompatibleDC(m_dc);
	PLASSERT(m_memdc);
	if (m_memdc == NULL) {
		PLASSERT(false);
		raiseError (PL_ERRNO_MEMORY,"Cannot allocate device context.");
	}

	m_bm = CreateCompatibleBitmap(m_dc,width,height);
	if (m_bm == NULL) {
		PLASSERT(false);
		raiseError (PL_ERRNO_MEMORY,"Cannot allocate memory bitmap.");
	}

	m_holdbm = SelectObject(m_memdc,m_bm);

	// If the metafile has a palette, read it in
	UINT pe = GetEnhMetaFilePaletteEntries(m_hemf, 0, NULL);

	// pe is the real number of palette entries. To make the resulting
	// bitmap more useful, we always setup a 256 color palette if the
	// metafile has a palette
  if (pe>0 && pe<256)
  {
    SetBmpInfo (PLPoint (width, height), PLPoint(0,0), 
             PLPixelFormat::L8);
	}
	else 
  {
    SetBmpInfo (PLPoint (width, height), PLPoint(0,0), 
            PLPixelFormat::B8G8R8A8);
  }
}

void PLWEMFDecoder::GetImage (PLBmpBase & Bmp)
{
	HPALETTE hpal = NULL;
  LPLOGPALETTE plogpal (0);

  if (GetBitsPerPixel() == 8) {
		plogpal = (LPLOGPALETTE)new PLBYTE[sizeof(LOGPALETTE) + (sizeof(PALETTEENTRY) * 256)];
		memset(plogpal,0x0,sizeof(LOGPALETTE) + sizeof(PALETTEENTRY)*256);
		plogpal->palVersion = 0x300;
		plogpal->palNumEntries = 256;
		if (plogpal == NULL) {
			PLASSERT(false);
			raiseError (PL_ERRNO_MEMORY,"Cannot allocate palette.");
		}
    UINT pe = GetEnhMetaFilePaletteEntries(m_hemf, 0, NULL);
		GetEnhMetaFilePaletteEntries(m_hemf, pe, plogpal->palPalEntry);
	}

	// Setup a logical palette for our memory dc and also a
	// paintlib compatible palette for the paintlib bitmap
  PLPixel32 pPal[256];
	if (plogpal) {
		for (UINT i = 0; i < 256; i++) {
			pPal[i] = *(PLPixel32*)&plogpal->palPalEntry[i];
		}

		if ((hpal = CreatePalette((LPLOGPALETTE)plogpal))) {
			m_holdpal = SelectPalette(m_memdc, hpal, false);
			RealizePalette(m_memdc);
		}
		Bmp.SetPalette(pPal);
		delete [] plogpal;
	}

	// Play the metafile into the device context
	// First, setup a bounding rectangle and fill
	// the memory dc with white (some metafiles only
	// use a black pen to draw and have no actual fill
	// color set, This would cause a black on black
	// painting which is rather useless
	RECT rc;
	rc.left = rc.top = 0;
	rc.bottom = GetHeight();
	rc.right = GetWidth();

	FillRect(m_memdc,&rc,(HBRUSH)GetStockObject(WHITE_BRUSH));

	// Heeere we go....
	BOOL bres = PlayEnhMetaFile(m_memdc,m_hemf,&rc);

	// Finally, convert the Windows bitmap into a paintlib bitmap
	PLWinBmp& winbmp = dynamic_cast<PLWinBmp&>(Bmp);
	BITMAPINFO* pBMI = (BITMAPINFO*)winbmp.GetBMI();
	PLBYTE* pBits = (PLBYTE*)winbmp.GetBits();
	if (GetBitsPerPixel() == 8) {
		GetDIBits(m_dc, m_bm, 0, GetHeight(), winbmp.GetBits(), pBMI, DIB_RGB_COLORS);
	}
	else {
		GetDIBits(m_dc, m_bm, 0, GetHeight(), winbmp.GetBits(), pBMI, DIB_PAL_COLORS);
	}
}

void PLWEMFDecoder::Close()
{
	// Clean-up the remaining stuff
	ReleaseDC(0,m_dc);
	m_dc = 0;
	DeleteObject(m_bm);
	m_bm = NULL;

  DeleteEnhMetaFile(m_hemf);
	m_hemf = NULL;

	// Free all windows DC stuff
	SelectObject(m_memdc,m_holdpal);
	SelectObject(m_memdc,m_holdbm);

	// Free resources (set to null for exception safety)
	delete [] m_phdr;
	m_phdr = NULL;
	DeleteDC(m_memdc);
	m_memdc = NULL;

  PLPicDecoder::Close();
}


#endif // _WIN32
/*
/--------------------------------------------------------------------
|
|      $Log: plwemfdec.cpp,v $
|      Revision 1.9  2004/11/09 15:55:06  artcom
|      changed #ifdef _WINDOWS to #ifdef _WIN32 (since this is a macro that actually is predefined on  win32 platforms)
|
|      Revision 1.8  2004/09/11 12:41:37  uzadow
|      removed plstdpch.h
|
|      Revision 1.7  2004/09/09 16:52:50  artcom
|      refactored PixelFormat
|
|      Revision 1.6  2004/06/19 16:49:11  uzadow
|      Changed GetImage so it works with PLBmpBase
|
|      Revision 1.5  2003/08/03 12:03:22  uzadow
|      Added unicode support; fixed some header includes.
|
|      Revision 1.4  2002/08/04 20:08:01  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|
|      Revision 1.3  2002/03/31 13:36:42  uzadow
|      Updated copyright.
|
|      Revision 1.2  2001/10/21 17:12:40  uzadow
|      Added PSD decoder beta, removed BPPWanted from all decoders, added PLFilterPixel.
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.6  2000/12/18 22:42:53  uzadow
|      Replaced RGBAPIXEL with PLPixel32.
|
|      Revision 1.5  2000/01/16 20:43:18  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  2000/01/10 23:53:01  Ulrich von Zadow
|      Changed formatting & removed tabs.
|
|      Revision 1.3  1999/10/21 18:48:18  Ulrich von Zadow
|      no message
|
|
\--------------------------------------------------------------------
*/
