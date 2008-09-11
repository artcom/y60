//============================================================================
// Copyright (C) 2000-2003, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//============================================================================
//
//   $Id: PLFilterFactory.h,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterFactory.h,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description: Calculate adjencencies between triangles for striping
//
//=============================================================================

/*
supported filters and their parameters

grayscale       -
videoinvert     -
flip            -
fliprgb         -
getalpha        -
mirror          -

lightness       int lightness
rotate          PLFilterRotate enum AngleType {ninety, oneeighty, twoseventy};

colorize        double Hue, double Saturation
contrast        double contrast, PLBYTE offset
resize          int NewXSize, int NewYSize
resizebilinear  int NewXSize, int NewYSize
resizebox       int NewXSize, int NewYSize
quantize        int DitherPaletteType, int DitherType

threshold       int threshold_min,int threshold_max, int channel
resizegaussian  int NewXSize, int NewYSize, double NewRadius
resizehamming   int NewXSize, int NewYSize, double NewRadius
intensity       double intensity, PLBYTE offset, double exponent

unsupported paintlib filters
pixel           const PixelOp & Op
fill            const PixelC& Color
fillrect        PLPoint min, PLPoint max, const PixelC& Color
*/

#ifndef _PAINTLIB_FILTER_FACTORY_INCLUDED_
#define _PAINTLIB_FILTER_FACTORY_INCLUDED_

#include <y60/base/typedefs.h>
#include <asl/base/Ptr.h>
#include <asl/base/Singleton.h>
#include <asl/base/Exception.h>
#include <map>
#include <vector>

class PLFilter;

namespace y60 {

DEFINE_EXCEPTION(InvalidFilterParameter, asl::Exception)

struct IPaintLibFilterFactory {
    virtual ~IPaintLibFilterFactory() {}
    virtual asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) = 0;
};

class PaintLibFilterFactory :
    public asl::Singleton<PaintLibFilterFactory>
{
public:
    asl::Ptr<PLFilter> createFilter(const std::string & theFilterName, const VectorOfFloat & theParameters);
    void addFilterFactory(const std::string & theFilterName, asl::Ptr<IPaintLibFilterFactory> theFactory);
private:
    PaintLibFilterFactory();
    friend class asl::SingletonManager;
    typedef std::map< std::string, asl::Ptr<IPaintLibFilterFactory> > FilterFactoryList;
    FilterFactoryList _myFactories;

    template<class T>   // for convenience in c'tor
    void _add(const std::string & theName, T* theFilterFactoryDumbPointer) {
        addFilterFactory(theName, asl::Ptr<T>(theFilterFactoryDumbPointer));
    }
};

}

#endif
