/*
/--------------------------------------------------------------------
|
|      $Id: pl2passscale.h,v 1.13 2005/04/21 09:54:43 artcom Exp $
|
|      Copyright (c) 1996-2002 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef _2_PASS_SCALE_H_
#define _2_PASS_SCALE_H_

#include "plcontribdefs.h"
#include "../plpaintlibdefs.h"

#include <cstring>
#include <math.h>
#include <iostream>

#ifdef AC_BUILT_WITH_CMAKE
#   include <half.h>
#else
#   include <OpenEXR/half.h>
#endif

// Not every math.h defines this :-(.
#ifndef  min
#define min(a, b)       ((a) < (b) ? (a) : (b))
#endif

#ifndef  max
#define max(a, b)       ((a) < (b) ? (b) : (a))
#endif

typedef struct
{
   int *Weights;     // Normalized weights of neighboring pixels
   int Left,Right;   // Bounds of source pixels window
} ContributionType;  // Contirbution information for a single pixel

typedef struct
{
   ContributionType *ContribRow; // Row (or column) of contribution weights
   int WindowSize,               // Filter window size (of affecting source pixels)
       LineLength;               // Length of line (no. or rows / cols)
} LineContribType;               // Contribution information for an entire line (row or column)

typedef bool (*ProgressAndAbortCallBack)(PLBYTE bPercentComplete);

class CDataL_UBYTE
{
public:
  typedef unsigned char _DataType[1];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val += (Weight * value[0]);
      };
      void Store (_DataType &value)
      {
        value [0] = (unsigned char) ((val + 128)/256);
      };
      int val;
  };
};

class CDataL_UWORD
{
public:
  typedef PLWORD _DataType[1];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val += (Weight * value[0]);
      };
      void Store (_DataType &value)
      {
        value [0] = (PLWORD) ((val + 128)/256);
      };
      long val;
  };
};

class CDataL_SWORD
{
public:
  typedef PLSWORD _DataType[1];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val += (Weight * value[0]);
      };
      void Store (_DataType &value)
      {
        value [0] = (PLSWORD) ((val + 128)/256);
      };
      long val;
  };
};

class CDataRGBA_UBYTE {
public:
  typedef unsigned char _DataType[4];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val [0] = val [1] = val [2] = val [3] = 0;
      };
      void Accumulate (int dWeight, _DataType &value)
      {
        val [0] += (dWeight * (value [0]));
        val [1] += (dWeight * (value [1]));
        val [2] += (dWeight * (value [2]));
        val [3] += (dWeight * (value [3]));
      };
      void Store (_DataType &value)
      {
        value [0] = (unsigned char) ((val [0] + 128)/256);
        value [1] = (unsigned char) ((val [1] + 128)/256);
        value [2] = (unsigned char) ((val [2] + 128)/256);
        value [3] = (unsigned char) ((val [3] + 128)/256);
      };
      int val [4];
  };
};

class CDataRGB_UBYTE
{
public:
  typedef unsigned char _DataType[3];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val [0] = val [1] = val [2] = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val [0] += (Weight * value [0]);
        val [1] += (Weight * value [1]);
        val [2] += (Weight * value [2]);
      };
      void Store (_DataType &value)
      {
        value [0] = (unsigned char) ((val [0] + 128)/256);
        value [1] = (unsigned char) ((val [1] + 128)/256);
        value [2] = (unsigned char) ((val [2] + 128)/256);
      };
      int val [3];
  };
};

class CDataRGBA_HALF
{
public:
  typedef half _DataType[4];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val[0] = val[1] = val[2] = val[3] = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val[0] += (Weight * float(value[0]));
        val[1] += (Weight * float(value[1]));
        val[2] += (Weight * float(value[2]));
        val[3] += (Weight * float(value[3]));
      };
      void Store (_DataType &value)
      {
        value[0] = half(val[0]);
        value[1] = half(val[1]);
        value[2] = half(val[2]);
        value[3] = half(val[3]);
      };
      float val[4];
  };
};
class CDataRGB_HALF
{
public:
  typedef half _DataType[3];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val[0] = val[1] = val[2] = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val[0] += (Weight * float(value[0]));
        val[1] += (Weight * float(value[1]));
        val[2] += (Weight * float(value[2]));
      };
      void Store (_DataType &value)
      {
        value[0] = half(val[0]);
        value[1] = half(val[1]);
        value[2] = half(val[2]);
      };
      float val[3];
  };
};
class CDataRGBA_FLOAT
{
public:
  typedef float _DataType[4];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val[0] = val[1] = val[2] = val[3] = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val[0] += (Weight * float(value[0]));
        val[1] += (Weight * float(value[1]));
        val[2] += (Weight * float(value[2]));
        val[3] += (Weight * float(value[3]));
      };
      void Store (_DataType &value)
      {
        value[0] = float(val[0]);
        value[1] = float(val[1]);
        value[2] = float(val[2]);
        value[3] = float(val[3]);
      };
      float val[4];
  };
};
class CDataRGB_FLOAT
{
public:
  typedef float _DataType[3];
  typedef _DataType* _RowType;
  class _Accumulator {
  public:
      _Accumulator ()
      {
        val[0] = val[1] = val[2] = 0;
      };
      void Accumulate (int Weight, _DataType &value)
      {
        val[0] += (Weight * float(value[0]));
        val[1] += (Weight * float(value[1]));
        val[2] += (Weight * float(value[2]));
      };
      void Store (_DataType &value)
      {
        value[0] = float(val[0]);
        value[1] = float(val[1]);
        value[2] = float(val[2]);
      };
      float val[3];
  };
};

template <class DataClass>
class C2PassScale
{
public:
    typedef typename DataClass::_DataType _DataType;
    typedef typename DataClass::_RowType _RowType;

    C2PassScale (const PLContribDef& ContribDef,
                 ProgressAndAbortCallBack callback = NULL)
        : m_Callback (callback), m_ContribDef (ContribDef)
    {};

    virtual ~C2PassScale() {};

   _RowType * Scale (
        _RowType   *pOrigImage,
        PLUINT        uOrigWidth,
        PLUINT        uOrigHeight,
        _RowType   *pDstImage,
        PLUINT        uNewWidth,
        PLUINT        uNewHeight);

private:

    ProgressAndAbortCallBack    m_Callback;
    bool                        m_bCanceled;

    LineContribType *AllocContributions (PLUINT uLineLength,
                                         PLUINT uWindowSize);

    void FreeContributions (LineContribType * p);

    LineContribType *CalcContributions (PLUINT    uLineSize,
                                        PLUINT    uSrcSize,
                                        double  dScale);

    void ScaleRow (_RowType           *pSrc,
                   PLUINT                uSrcWidth,
                   _RowType           *pRes,
                   PLUINT                uResWidth,
                   PLUINT                uRow,
                   LineContribType    *Contrib);

    void HorizScale (_RowType           *pSrc,
                     PLUINT                uSrcWidth,
                     PLUINT                uSrcHeight,
                     _RowType           *pDst,
                     PLUINT                uResWidth,
                     PLUINT                uResHeight);

    void VertScale (_RowType           *pSrc,
                    PLUINT                uSrcWidth,
                    PLUINT                uSrcHeight,
                    _RowType           *pDst,
                    PLUINT                uResWidth,
                    PLUINT                uResHeight);

    const PLContribDef& m_ContribDef;
};

template <class DataClass>
LineContribType *
C2PassScale<DataClass>::AllocContributions (PLUINT uLineLength, PLUINT uWindowSize)
{
    LineContribType *res = new LineContribType;
        // Init structure header
    res->WindowSize = uWindowSize;
    res->LineLength = uLineLength;
        // Allocate list of contributions
    res->ContribRow = new ContributionType[uLineLength];
    for (PLUINT u = 0 ; u < uLineLength ; u++)
    {
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new int[uWindowSize];
    }
    return res;
}

template <class DataClass>
void
C2PassScale<DataClass>::FreeContributions (LineContribType * p)
{
    for (int u = 0; u < p->LineLength; u++)
    {
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header
}

template <class DataClass>
LineContribType *
C2PassScale<DataClass>::CalcContributions
    (PLUINT uLineSize, PLUINT uSrcSize, double dScale)
{
    double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = m_ContribDef.GetWidth();

    if (dScale < 1.0)
    {    // Minification
        dWidth = dFilterWidth / dScale;
        dFScale = dScale;
    }
    else
    {    // Magnification
        dWidth= dFilterWidth;
    }

    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1;

    // Allocate a new line contributions strucutre
    LineContribType *res = AllocContributions (uLineSize, iWindowSize);

    for (PLUINT u = 0; u < uLineSize; u++)
    {   // Scan through line of contributions
        double dCenter = (u+0.5)/dScale-0.5;   // Reverse mapping
        // Find the significant edge points that affect the pixel
        int iLeft = max (0, (int)floor (dCenter - dWidth));
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1);

        // Cut edge points to fit in filter window in case of spill-off
        if (iRight - iLeft + 1 > iWindowSize)
        {
            if (iLeft < (int(uSrcSize) - 1 / 2))
            {
                iLeft++;
            }
            else
            {
                iRight--;
            }
        }
  //ets+++ adjusted ileft and iright values not stored in contrib array
        res->ContribRow[u].Left = iLeft;
        res->ContribRow[u].Right = iRight;
  //ets
        int dTotalWeight = 0;  // Zero sum of weights
        for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
        {   // Calculate weights
            int CurWeight = int (dFScale * (m_ContribDef.Filter (dFScale * (dCenter - (double)iSrc)))*256);
            res->ContribRow[u].Weights[iSrc-iLeft] = CurWeight;
            dTotalWeight += CurWeight;
        }
        PLASSERT (dTotalWeight >= 0);   // An error in the filter function can cause this
        int UsedWeight = 0;
        if (dTotalWeight > 0)
        {   // Normalize weight of neighbouring points
            for (int iSrc = iLeft; iSrc < iRight; iSrc++)
            {   // Normalize point
              int CurWeight = (res->ContribRow[u].Weights[iSrc-iLeft]*256)/dTotalWeight;
              res->ContribRow[u].Weights[iSrc-iLeft] = CurWeight;
              UsedWeight += CurWeight;
            }
            // The last point gets everything that's left over so the sum is
            // always correct.
            res->ContribRow[u].Weights[iRight-iLeft] = 256 - UsedWeight;
        }
   }
   return res;
}


template <class DataClass>
void
C2PassScale<DataClass>::ScaleRow (_RowType *pSrc, PLUINT uSrcWidth,
                                  _RowType *pRes, PLUINT uResWidth,
                                  PLUINT uRow, LineContribType *Contrib)
{
    _DataType *pSrcRow = pSrc[uRow];
    _DataType *pDstRow = pRes[uRow];
    for (PLUINT x = 0; x < uResWidth; x++)
    {   // Loop through row
        typename DataClass::_Accumulator a;
        int iLeft = Contrib->ContribRow[x].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[x].Right;  // Retrieve right boundries
        for (int i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
            a .Accumulate (Contrib->ContribRow[x].Weights[i-iLeft], pSrcRow[i]);
        }
        a .Store (pDstRow [x]);
    }
}

template <class DataClass>
void
C2PassScale<DataClass>::HorizScale (_RowType *pSrc, PLUINT uSrcWidth, PLUINT uSrcHeight,
                                    _RowType *pDst, PLUINT uResWidth, PLUINT uResHeight)
{
    if (uResWidth == uSrcWidth)
    { // No scaling required, just copy
      for (PLUINT u = 0; u < uResHeight; u++)
        memcpy (pDst[u], pSrc[u], sizeof (_DataType) * uSrcWidth);

    }
    // Allocate and calculate the contributions
    LineContribType * Contrib;
    Contrib = CalcContributions (uResWidth, uSrcWidth,
                                 double(uResWidth) / double(uSrcWidth));
    for (PLUINT u = 0; u < uResHeight; u++)
    {   // Step through rows
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (PLBYTE(double(u) / double (uResHeight) * 50.0)))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = true;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }

        ScaleRow (  pSrc,
                    uSrcWidth,
                    pDst,
                    uResWidth,
                    u,
                    Contrib);    // Scale each row
    }
    FreeContributions (Contrib);  // Free contributions structure
}


template <class DataClass>
void
C2PassScale<DataClass>::VertScale (_RowType *pSrc, PLUINT uSrcWidth, PLUINT uSrcHeight,
                                   _RowType *pDst, PLUINT uResWidth, PLUINT uResHeight)
{
    PLUINT u;

    if (uSrcHeight == uResHeight)
    {   // No scaling required, just copy
      for (u = 0; u < uResHeight; u++)
        memcpy (pDst[u], pSrc[u], sizeof (_DataType) * uSrcWidth);
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResHeight, uSrcHeight, double(uResHeight) / double(uSrcHeight));
    for (PLUINT y = 0; y < uResHeight; y++)
    {    // Loop through column
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (PLBYTE(double(y) / double (uResHeight) * 50.0) + 50))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = true;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
        for (u = 0; u < uResWidth; u++)
        {   // Step through columns
            typename DataClass::_Accumulator a;
            int iLeft = Contrib->ContribRow[y].Left;    // Retrieve left boundries
            int iRight = Contrib->ContribRow[y].Right;  // Retrieve right boundries
            for (int i = iLeft; i <= iRight; i++)
            {   // Scan between boundries
                // Accumulate weighted effect of each neighboring pixel
                a.Accumulate (Contrib->ContribRow[y].Weights[i-iLeft], pSrc[i][u]);
            }
            a .Store (pDst[y][u]);
        }

    }
    FreeContributions (Contrib);     // Free contributions structure
}


template <class DataClass>
typename C2PassScale<DataClass>::_RowType *
C2PassScale<DataClass>::Scale (_RowType *pOrigImage, PLUINT uOrigWidth, PLUINT uOrigHeight,
                               _RowType *pDstImage, PLUINT uNewWidth, PLUINT uNewHeight)
{
    // Scale source image horizontally into temporary image
    m_bCanceled = false;
    PLUINT u;

    // Allocate temp image
    _RowType *pTemp = new _RowType[uOrigHeight];
    for (u = 0; u < uOrigHeight; u++)
      pTemp[u] = new _DataType[uNewWidth];

    HorizScale (pOrigImage,
                uOrigWidth,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uOrigHeight);
    if (m_bCanceled)
    {
      for (u = 0; u < uOrigHeight; u++)
        delete [] pTemp[u];
      delete [] pTemp;
      return NULL;
    }

    // Scale temporary image vertically into result image
    VertScale ( pTemp,
                uNewWidth,
                uOrigHeight,
                pDstImage,
                uNewWidth,
                uNewHeight);
    for (u = 0; u < uOrigHeight; u++)
      delete [] pTemp[u];
    delete [] pTemp;
    if (m_bCanceled)
    {
        return NULL;
    }
    return pDstImage;
}


#endif //   _2_PASS_SCALE_H_

/*
/--------------------------------------------------------------------
|
|      $Log: pl2passscale.h,v $
|      Revision 1.13  2005/04/21 09:54:43  artcom
|      corrected MSVC warning
|
|      Revision 1.12  2005/04/21 09:17:45  artcom
|      added 16-bit signed greyscale pixel type (nice for dicom)
|      bilinear resize now supported 16-bit signed & unsigned greyscale formats
|      png encoder now supports 16-bit greyscale format
|
|      Revision 1.11  2005/03/08 14:22:36  artcom
|      Added resizing of Luminace Bitmaps
|
|      Revision 1.10  2004/10/05 13:54:28  artcom
|      *** empty log message ***
|
|      Revision 1.9  2004/07/28 13:55:48  artcom
|      Added 24 bpp support to plfilterresizebilinear.
|
|      Revision 1.8  2002/11/04 22:40:13  uzadow
|      Updated for gcc 3.1
|
|      Revision 1.7  2002/02/24 13:00:45  uzadow
|      Documentation update; removed buggy PLFilterRotate.
|
|      Revision 1.6  2001/10/06 22:03:26  uzadow
|      Added PL prefix to basic data types.
|
|      Revision 1.5  2001/10/03 14:00:29  uzadow
|      Much improved quality in FilterResizeBilinear.
|
|      Revision 1.4  2001/09/30 17:19:16  uzadow
|      Removed lots of floating-point calculations.
|
|      Revision 1.3  2001/09/30 16:57:25  uzadow
|      Improved speed of 2passfilter.h, code readability changes.
|
|      Revision 1.2  2001/09/16 20:57:17  uzadow
|      Linux version name prefix changes
|
|      Revision 1.1  2001/09/16 19:03:23  uzadow
|      Added global name prefix PL, changed most filenames.
|
|      Revision 1.8  2000/12/04 23:56:11  uzadow
|      no message
|
|      Revision 1.7  2000/08/13 12:11:43  Administrator
|      Added experimental DirectDraw-Support
|
|      Revision 1.6  2000/05/27 16:34:05  Ulrich von Zadow
|      Linux compatibility changes
|
|      Revision 1.5  2000/01/16 20:43:15  anonymous
|      Removed MFC dependencies
|
|      Revision 1.4  1999/12/31 17:55:08  Ulrich von Zadow
|      Corrected rounding error.
|
|      Revision 1.3  1999/12/08 16:31:40  Ulrich von Zadow
|      Unix compatibility
|
|      Revision 1.2  1999/10/22 21:21:34  Ulrich von Zadow
|      no message
|
|      Revision 1.1  1999/10/21 16:05:17  Ulrich von Zadow
|      Moved filters to separate directory. Added Crop, Grayscale and
|      GetAlpha filters.
|
|      Revision 1.1  1999/10/19 21:29:55  Ulrich von Zadow
|      Added filters.
|
|
\--------------------------------------------------------------------
*/
