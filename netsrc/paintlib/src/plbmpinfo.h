/*
/--------------------------------------------------------------------
|
|      $Id: plbmpinfo.h,v 1.4 2004/10/02 22:23:12 uzadow Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLBMPINFO
#define INCL_PLBMPINFO

#include "plobject.h"
#include "plpoint.h"
#include "plpixelformat.h"

#include <string>

//! This is a simple base class that holds bitmap metainformation. It is
//! used as a base class by PLPicDecoder and PLBmp and can be used standalone
//! as well.
class PLBmpInfo : public PLObject
{

public:
  //! Creates an info object.
  PLBmpInfo (const PLPoint& Size,  const PLPoint& Resolution, 
             const PLPixelFormat & pf);

  //! Creates an uninitialized info object.
  PLBmpInfo () : m_pf(PLPixelFormat::DONTCARE) {}

  //! Creates an info object from an ascii representation. This ascii representation
  //! can be created using AsString();
  PLBmpInfo (const char * pszInfo);

  // This class doesn't have any virtual functions and there's nothing to 
  // destroy, so we don't define a destructor  - and the default assignment
  // and equality operators work well too.

  //!
  const PLPoint& GetSize () const
  { return m_Size; }

  //!
  int GetWidth () const
  { return m_Size.x; }

  //!
  int GetHeight () const
  { return m_Size.y; }

  //!
  unsigned GetBitsPerPixel () const
  { return m_pf.GetBitsPerPixel(); }

  //!
  bool HasAlpha () const
  { return m_pf.HasAlpha(); }

  //!
  bool IsGreyscale () const
  { return m_pf.IsGrayscale(); }

  //!
  const PLPixelFormat & GetPixelFormat() const
  { return m_pf; }
  
  //! Changes the pixel format so that the interpretations of R and B are swapped. 
  void SwapPixelFormatRGB()
  {
    m_pf = m_pf.GetRGBSwapped();
  }

  //! Gets the bitmap resolution in pixels per inch. Returns 0 if the
  //! resolution is unknown.
  const PLPoint& GetResolution () const
  { return m_Resolution; }

  //!
  void AsString (char * psz, int len) const;
  
  //!
  std::string AsString () const;
  
  //!
  bool operator == (const PLBmpInfo & Other) const;

protected:
  void SetBmpInfo (const PLPoint& Size, const PLPoint& Resolution, 
                   const PLPixelFormat & pf);
  void SetBmpInfo (const PLBmpInfo& SrcInfo);

  PLPoint m_Size;
  
  PLPoint m_Resolution;

  PLPixelFormat m_pf;  
};


#endif
/*
/--------------------------------------------------------------------
|
|      $Log: plbmpinfo.h,v $
|      Revision 1.4  2004/10/02 22:23:12  uzadow
|      - configure and Makefile cleanups\n- Pixelformat enhancements for several filters\n- Added PLBmpBase:Dump\n- Added PLBmpBase::GetPixelNn()-methods\n- Changed default unix byte order to BGR
|
|      Revision 1.3  2004/09/09 16:52:49  artcom
|      refactored PixelFormat
|
|      Revision 1.2  2004/06/06 12:56:38  uzadow
|      Doxygenified documentation.
|
|      Revision 1.1  2002/08/04 20:17:57  uzadow
|      Added PLBmpInfo class, ability to extract metainformation from images without loading the whole image and proper greyscale support.
|      Major improvements in tests.
|      Update to VS .NET
|
|
\--------------------------------------------------------------------
*/
