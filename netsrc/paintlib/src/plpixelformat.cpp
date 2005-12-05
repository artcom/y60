/*
/--------------------------------------------------------------------
|
|      $Id: plpixelformat.cpp,v 1.8 2005/07/12 13:10:34 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#include "plpixelformat.h"

#include <vector>
#include <sstream>

using namespace std;

PLPixelFormat::PixelFormatList PLPixelFormat::s_pixelFormatList;

#define DEFINEPIXELFORMAT(X) const PLPixelFormat PLPixelFormat::X(#X)

const PLPixelFormat PLPixelFormat::DONTCARE("");

DEFINEPIXELFORMAT(L1);
DEFINEPIXELFORMAT(I8);
DEFINEPIXELFORMAT(L8);
DEFINEPIXELFORMAT(L16);
DEFINEPIXELFORMAT(L16S);

DEFINEPIXELFORMAT(X1R5G5B5);
DEFINEPIXELFORMAT(A1R5G5B5);
DEFINEPIXELFORMAT(R5G6B5);
DEFINEPIXELFORMAT(R8G8B8);
DEFINEPIXELFORMAT(A8R8G8B8);
DEFINEPIXELFORMAT(X8R8G8B8);
DEFINEPIXELFORMAT(R16G16B16);

DEFINEPIXELFORMAT(B5G5R5X1);
DEFINEPIXELFORMAT(B5G5R5A1);
DEFINEPIXELFORMAT(B5G6R5);
DEFINEPIXELFORMAT(B8G8R8);
DEFINEPIXELFORMAT(B8G8R8A8);
DEFINEPIXELFORMAT(B8G8R8X8);
DEFINEPIXELFORMAT(A8B8G8R8);
DEFINEPIXELFORMAT(X8B8G8R8);

DEFINEPIXELFORMAT(A16B16G16R16); // RGBA half
DEFINEPIXELFORMAT(B16G16R16);    // RGB half
DEFINEPIXELFORMAT(A32B32G32R32); // RGBA float
DEFINEPIXELFORMAT(B32G32R32);    // RGB float

DEFINEPIXELFORMAT(L8Cbr8);	// aka YUV 4:2:2
DEFINEPIXELFORMAT(L8Cb8Cr8);	// aka YUV 4:4:4

static char * ppChannelNames[] =
	{
		"R","G","B","A","I","L","Cbr","Cb","Cr","C","M","Y","K", "X"
	};

/* bitcount:  count 1 bits in x
   See http://users.powernet.co.uk/eton/kandr2/krx209.html
   for an explanation. */
int bitcount(PLPixelFormat::Mask x)
{
    int b;

    for (b = 0; x != 0; x &= (x-1))
        b++;
    return b;
}

const PLPixelFormat & PLPixelFormat:: UseAlpha(bool useAlpha) const {
	string name = GetName();
	if (GetMask(A) && !useAlpha) {
		int i = name.find('A');
		name[i] = 'X';
	} else if (GetMask(X) && useAlpha) {
		int i = name.find('X');
		name[i] = 'A';
	}
	return FromName(name);
}

PLPixelFormat::PLPixelFormat()
{
    *this = PLPixelFormat::DONTCARE;
}

PLPixelFormat ::PLPixelFormat(const PLPixelFormat & that) :
	m_sName(that.m_sName),
	m_BitsPerPixel(that.m_BitsPerPixel)
{
	memcpy(m_Channelmasks, that.m_Channelmasks, sizeof(m_Channelmasks));
}

PLPixelFormat& PLPixelFormat ::operator=(const PLPixelFormat & that)
{
	m_sName = that.m_sName;
	m_BitsPerPixel = that.m_BitsPerPixel;
	memcpy(m_Channelmasks, that.m_Channelmasks, sizeof(m_Channelmasks));
	return *this;
}

bool PLPixelFormat ::operator==(const PLPixelFormat & that) const
{
	return m_sName == that.m_sName;
}

bool PLPixelFormat::operator !=(const PLPixelFormat & that) const
{
    return !(operator==(that));
}

const PLPixelFormat & PLPixelFormat::FromName(const string & name)
{
	PixelFormatList::iterator i;
	for(i = s_pixelFormatList.begin(); i!=s_pixelFormatList.end(); ++i)
    {
		if ((*i)->GetName() == name)
        {
			return **i;
		}
	}
	throw UnsupportedPixelFormat(name);
}

const PLPixelFormat & PLPixelFormat :: FromChannels(const std::string & sChannels, int bpp)
{
  std::string::size_type pos = 0;
  vector<Channel> channels;

  for (unsigned int ch = 0; ch < COUNT; ++ch) {
     pos = sChannels.find(ppChannelNames[ch]);
     if (pos != string::npos) {
        channels.push_back(Channel(ch));
     }
  }

  ostringstream ss;
  for(unsigned i=0; i<channels.size(); ++i)
  {
    ss << string(ppChannelNames[channels[i]]);
    ss << (bpp / channels.size());
  }

  return FromName(ss.str());
}

int PLPixelFormat :: GetNumColors() const
{
	return 1 << (m_BitsPerPixel - bitcount(GetMask(X)) + bitcount(GetMask(A)));
}

PLPixelFormat PLPixelFormat::GetRGBSwapped() const
{
  string s = GetName();
  int RPos = s.find('R');
  int BPos = s.find('B');
  s[RPos] = 'B';
  s[BPos] = 'R';

  // find pixelformat
  PixelFormatList::iterator i;
  for(i = s_pixelFormatList.begin(); i!=s_pixelFormatList.end(); ++i) {
    if ((*i)->GetName() == s) {
      return **i;
    }
  }

  PLPixelFormat * pf = new PLPixelFormat(s);
  return *pf;
}

PLPixelFormat :: PLPixelFormat(const string& sName)
	: m_sName(sName)
{

	static int numChannelNames = sizeof(ppChannelNames) / sizeof(ppChannelNames[0]);

	string sNameCopy(sName);
	string::iterator i;
	string::iterator nameStart = sNameCopy.begin();
	string::iterator numStart = sNameCopy.end();
	int bitOffset = 0;

	// in the first pass, we collect
	// the bitoffsets, bitwidth and the index for each channel.
	typedef vector<  pair<int, int> > ChnnaelInfo;
	//				  offfset  width
	ChnnaelInfo channelInfo(numChannelNames);

	int channelIndex = -1;
	for(i=sNameCopy.begin(); i != sNameCopy.end(); ++i)
	{
		if (*i >= '0' && *i <='9')
		{	// if we have a digit...
			if (nameStart != sNameCopy.end()) {
				string sChannelName(nameStart, i);
				nameStart = sNameCopy.end();

				for(int ch=0; ch<numChannelNames; ++ch)
				{
					if (sChannelName == ppChannelNames[ch])
					{
						channelIndex = ch;
						break;
					}
				}
				PLASSERT(channelIndex!=-1);
			}
			if (numStart == sNameCopy.end()) {
				// we did not start num scanning yet
				numStart = i;
			}

		}
		if (*i < '0' || *i > '9' || i+1 == sNameCopy.end())
		{	// a letter makrs the end of the name string
			// so does end-of-string

			if (nameStart == sNameCopy.end()) {
				// start name scanning
				nameStart = i;
			}
			if (numStart != sNameCopy.end()) {
				string sBitCount(numStart, i+1 == sNameCopy.end() ? sNameCopy.end() : i);
				int nBitCount = atoi(sBitCount.c_str());

				channelInfo[channelIndex] = make_pair<int,int>(bitOffset, nBitCount);
				bitOffset += nBitCount;
				numStart = sNameCopy.end();
			}
		}
	}
	m_BitsPerPixel = bitOffset;

	// Second pass: now we now the total number of bits per pixel,
	// we can calculate the bit masks for each channel;
	for(int i=0; i<numChannelNames; ++i) {
		int nBitOffset = channelInfo[i].first;
		int nBitCount = channelInfo[i].second;
		if (nBitCount) {
			Mask mask = (((unsigned long long)1 << nBitCount) - 1) << (m_BitsPerPixel - nBitOffset - nBitCount);
			m_Channelmasks[i] = mask;
		} else {
			m_Channelmasks[i] = 0;
		}
	}

	s_pixelFormatList.push_back(this);
}
