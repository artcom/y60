/*
/--------------------------------------------------------------------
|
|      $Id: plpixelformat.h,v 1.8 2005/07/12 13:10:34 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef INCL_PLPIXELFORMAT
#define INCL_PLPIXELFORMAT

#include "pldebug.h"

#include <string>
#include <list>

class PLPixelFormat {
public:
	static const PLPixelFormat DONTCARE;

    // Pixel format descriptions are MSB first on all machines.
	static const PLPixelFormat L1;
	static const PLPixelFormat I8;
	static const PLPixelFormat L8;
	static const PLPixelFormat L16;
	static const PLPixelFormat L16S; // 16-bit signed luminence

	static const PLPixelFormat X1R5G5B5;
	static const PLPixelFormat A1R5G5B5;
	static const PLPixelFormat R5G6B5;
	static const PLPixelFormat R8G8B8;
	static const PLPixelFormat A8R8G8B8;
	static const PLPixelFormat X8R8G8B8;
	static const PLPixelFormat R16G16B16;

	static const PLPixelFormat B5G5R5X1;
	static const PLPixelFormat B5G5R5A1;
	static const PLPixelFormat B5G6R5;
	static const PLPixelFormat B8G8R8;
	static const PLPixelFormat B8G8R8A8;
	static const PLPixelFormat B8G8R8X8;
	static const PLPixelFormat A8B8G8R8;
	static const PLPixelFormat X8B8G8R8;
	static const PLPixelFormat B16G16R16;
	static const PLPixelFormat A16B16G16R16;

	static const PLPixelFormat L8Cbr8;	// aka YUV 4:2:2
	static const PLPixelFormat L8Cb8Cr8;	// aka YUV 4:4:4

    PLPixelFormat();
	PLPixelFormat(const PLPixelFormat & that);
	PLPixelFormat& operator=(const PLPixelFormat & that);
	bool operator==(const PLPixelFormat & that) const;
	bool operator!=(const PLPixelFormat & that) const;

	enum Channel {R=0,G,B,A,I,L,Cbr,Cb,Cr,C,M,Y,K,X, COUNT};

	typedef unsigned long long Mask;

	Mask GetMask(Channel ch) const;
	unsigned GetBitsPerPixel() const;
	bool HasAlpha() const;
	const PLPixelFormat & UseAlpha(bool useAlpha) const;
	bool IsGrayscale() const;
	const std::string& GetName() const;
	static const PLPixelFormat & FromName(const std::string & name);
    // find a PixelFormat matching the given channel order where each channel has a width of
    // bpp / number of channels.
    static const PLPixelFormat & FromChannels(const std::string & channels, int bpp);
	int GetNumColors() const;
    PLPixelFormat GetRGBSwapped() const;

	struct UnsupportedPixelFormat {
		UnsupportedPixelFormat(const std::string& what) : m_what(what) {};
		const std::string m_what;
	};

private:
	PLPixelFormat(const std::string& sName);
    static Channel parseChannel(const std::string& s, std::string::size_type & pos);

	std::string m_sName;
	int m_BitsPerPixel;
	Mask m_Channelmasks[COUNT];

	typedef std::list<PLPixelFormat *> PixelFormatList;
	static PixelFormatList s_pixelFormatList;
};

inline bool PLPixelFormat :: HasAlpha() const {
	return GetMask(A) != 0;
}

inline bool PLPixelFormat :: IsGrayscale() const {
	return GetMask(L) == (((Mask)1 << m_BitsPerPixel) - 1);
}

inline unsigned PLPixelFormat :: GetBitsPerPixel() const {
	return m_BitsPerPixel;
}

inline const std::string& PLPixelFormat :: GetName() const {
	return m_sName;
}

inline PLPixelFormat :: Mask PLPixelFormat :: GetMask(PLPixelFormat :: Channel ch) const {
	PLASSERT(ch!=COUNT);
	return m_Channelmasks[ch];
}

#endif
