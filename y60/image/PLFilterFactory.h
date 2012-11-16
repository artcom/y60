/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2008, ART+COM AG Berlin, Germany <www.artcom.de>
//
// These coded instructions, statements, and computer programs contain
// proprietary information of ART+COM AG Berlin, and are copy protected
// by law. They may be used, modified and redistributed under the terms
// of GNU General Public License referenced below.
//
// Alternative licensing without the obligations of the GPL is
// available upon request.
//
// GPL v3 Licensing:
//
// This file is part of the ART+COM Y60 Platform.
//
// ART+COM Y60 is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// ART+COM Y60 is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with ART+COM Y60.  If not, see <http://www.gnu.org/licenses/>.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
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

#include "y60_image_settings.h"

#include <map>
#include <vector>

#include <asl/base/Ptr.h>
#include <asl/base/Singleton.h>
#include <asl/base/Exception.h>

#include <y60/base/typedefs.h>

class PLFilter;

namespace y60 {

DEFINE_EXCEPTION(InvalidFilterParameter, asl::Exception)

struct IPaintLibFilterFactory {
    virtual ~IPaintLibFilterFactory() {}
    virtual asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) = 0;
};

class Y60_IMAGE_DECL PaintLibFilterFactory :
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
