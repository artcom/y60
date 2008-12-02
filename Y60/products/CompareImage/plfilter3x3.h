/*
/--------------------------------------------------------------------
|
|      $Id: plfilter3x3.h,v 1.2 2004/08/03 14:44:36 martin Exp $
|
|      Copyright (c) 1996-1998 Ulrich von Zadow
|
\--------------------------------------------------------------------
*/

#ifndef FILTER3x3_H
#define FILTER3x3_H

#include <asl/math/Matrix4.h>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plbitmap.h>
#include <paintlib/Filter/plfilter.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

namespace asl {
    // TODO: Move this to somewhere in asl/Math
    typedef asl::Vector3<asl::Vector3<float> > Matrix3f;
}

//! Applys a 3x3 convolution filter
class PLFilter3x3 : public PLFilter  
{
public:
  //! 
  PLFilter3x3(const asl::Matrix3f & theConvolution);
  //! 
  virtual ~PLFilter3x3();
  //! 
  virtual void Apply(PLBmpBase * pBmpSource, PLBmp * pBmpDest) 
    const;
private:
    asl::Matrix3f _myConvolution;
};

#endif 

/*
/--------------------------------------------------------------------
|
|      $Log: plfilter3x3.h,v $
|      Revision 1.2  2004/08/03 14:44:36  martin
|      PL include bug workaround
|
|      Revision 1.1  2004/08/03 14:30:09  martin
|      better image compare algorithem
|
|
\--------------------------------------------------------------------
*/
