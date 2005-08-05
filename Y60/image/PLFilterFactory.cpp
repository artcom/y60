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
//   $Id: PLFilterFactory.cpp,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterFactory.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description: Calculate adjencencies between triangles for striping
//
//=============================================================================

#include "PLFilterFactory.h"

#include <paintlib/Filter/plfiltergrayscale.h>
#include <paintlib/Filter/plfiltervideoinvert.h>
#include <paintlib/Filter/plfilterflip.h>
#include <paintlib/Filter/plfilterfliprgb.h>
#include <paintlib/Filter/plfiltergetalpha.h>
#include <paintlib/Filter/plfiltermirror.h>
#include <paintlib/Filter/plfilterlightness.h>
#include <paintlib/Filter/plfilterrotate.h>
#include <paintlib/Filter/plfiltercolorize.h>
#include <paintlib/Filter/plfiltercontrast.h>
#include <paintlib/Filter/plfilterresize.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#include <paintlib/Filter/plfilterresizebox.h>
#include <paintlib/Filter/plfilterquantize.h>
#include <paintlib/Filter/plfilterthreshold.h>
#include <paintlib/Filter/plfilterresizegaussian.h>
#include <paintlib/Filter/plfilterresizehamming.h>
#include <paintlib/Filter/plfilterintensity.h>
#include <paintlib/Filter/plfiltercrop.h>

#include <iostream>

using namespace std;
using namespace asl;
using namespace y60;

template<class T>
class FilterFactory0 :
    public IPaintLibFilterFactory
{
public:
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        return asl::Ptr<PLFilter>(new T);
    }
};

template<class T, class P1>
class FilterFactory1 :
    public IPaintLibFilterFactory
{
public:
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        return asl::Ptr<PLFilter>(
            new T(
                (P1)theParameters[0]
            )
        );
    }
};

template<class T, class P1, class P2>
class FilterFactory2 :
    public IPaintLibFilterFactory
{
public:
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        return asl::Ptr<PLFilter>(
            new T(
                (P1)theParameters[0],
                (P2)theParameters[1]
            )
        );
    }
};

template<class T, class P1, class P2, class P3>
class FilterFactory3 :
    public IPaintLibFilterFactory
{
public:
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        return asl::Ptr<PLFilter>(
            new T(
                (P1)theParameters[0],
                (P2)theParameters[1],
                (P3)theParameters[2]
            )
        );
    }
};

template<class T, class P1, class P2, class P3, class P4>
class FilterFactory4 :
    public IPaintLibFilterFactory
{
public:
    asl::Ptr<PLFilter> createFilter(const VectorOfFloat & theParameters) {
        return asl::Ptr<PLFilter>(
            new T(
                (P1)theParameters[0],
                (P2)theParameters[1],
                (P3)theParameters[2],
                (P4)theParameters[3]
            )
        );
    }
};

class MyAngleType {
public:
    MyAngleType(float theFloat) {
        if (theFloat == 90.0f) {
            _myAngleType =  PLFilterRotate::ninety;
        } else if (theFloat == 180.0f) {
            _myAngleType = PLFilterRotate::oneeighty;
        } else if (theFloat == 270.0f) {
            _myAngleType = PLFilterRotate::twoseventy;
        } else {
            throw InvalidFilterParameter("Unsupported value fot angle: must be 90, 180 or 270", PLUS_FILE_LINE);
        }
    }
    operator PLFilterRotate::AngleType() {
        return _myAngleType;
    }
private:
    PLFilterRotate::AngleType _myAngleType;
};

Ptr<PLFilter> PaintLibFilterFactory :: createFilter(
    const string & theFilterName,
    const VectorOfFloat & theParameters
) {
    FilterFactoryList::iterator iter = _myFactories.find(theFilterName);
    if (iter != _myFactories.end()) {
        return (iter->second)->createFilter(theParameters);
    }
    return Ptr<PLFilter>(0);
}

void PaintLibFilterFactory :: addFilterFactory(
    const string & theFilterName,
    asl::Ptr<IPaintLibFilterFactory> theFactory
) {
    _myFactories[theFilterName] = theFactory;
}

PaintLibFilterFactory::PaintLibFilterFactory() {
    _add("grayscale",           new FilterFactory0<PLFilterGrayscale>);
    _add("videoinvert",         new FilterFactory0<PLFilterVideoInvert>);
    _add("flip",                new FilterFactory0<PLFilterFlip>);
    _add("fliprgb",              new FilterFactory0<PLFilterFlipRGB>);
    _add("getalpha",            new FilterFactory0<PLFilterGetAlpha>);
    _add("mirror",              new FilterFactory0<PLFilterMirror>);;

    _add("lightness",           new FilterFactory1<PLFilterLightness, int>);
    _add("rotate",              new FilterFactory1<PLFilterRotate, MyAngleType>);

    _add("colorize",            new FilterFactory2<PLFilterColorize,double, double>);
    _add("contrast",            new FilterFactory2<PLFilterContrast,double, PLBYTE>);
    _add("resize",              new FilterFactory2<PLFilterResize,int, int>);
    _add("resizebilinear",      new FilterFactory2<PLFilterResizeBilinear,int,int>);
    _add("resizebox",           new FilterFactory2<PLFilterResizeBox,int,int>);
    _add("quantize",            new FilterFactory2<PLFilterQuantize,int,int>);

    _add("threshold",           new FilterFactory3<PLFilterThreshold,int,int,int>);
    _add("resizegaussian",      new FilterFactory3<PLFilterResizeGaussian,int,int,double>);
    _add("resizehamming",       new FilterFactory3<PLFilterResizeHamming,int,int,double>);
    _add("intensity",           new FilterFactory3<PLFilterIntensity,double,PLBYTE,double>);

    _add("crop",                new FilterFactory4<PLFilterCrop, int, int, int, int>);
}

