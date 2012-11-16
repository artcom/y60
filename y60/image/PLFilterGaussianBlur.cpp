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
//   $Id: PLFilterGaussianBlur.cpp,v 1.1 2005/03/24 23:36:00 christian Exp $
//   $RCSfile: PLFilterGaussianBlur.cpp,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/03/24 23:36:00 $
//
//
//  Description:
//
//
//=============================================================================

// own header
#include "PLFilterGaussianBlur.h"

#include <iostream>

#if defined(_MSC_VER)
#   pragma warning (push,1)
#endif //defined(_MSC_VER)
#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterresizebilinear.h>
#if defined(_MSC_VER)
#   pragma warning (pop)
#endif //defined(_MSC_VER)

#include <asl/math/numeric_functions.h>
#include <asl/base/Logger.h>
#include <asl/math/Vector234.h>

#define DB(x) // x

using namespace std;
using namespace asl;



template<class PIXELTYPE>
void getPix(PLBmpBase * pBmpSource, int theX, int theY, PIXELTYPE& theResult);
template<>
void getPix(PLBmpBase * pBmpSource, int theX, int theY, PLPixel32& theResult) { theResult = pBmpSource->GetPixel32(theX,theY);}
template<>
void getPix(PLBmpBase * pBmpSource, int theX, int theY, PLPixel24& theResult) { theResult = pBmpSource->GetPixel24(theX,theY);}
template<>
void getPix(PLBmpBase * pBmpSource, int theX, int theY, PLPixel16& theResult) { theResult = pBmpSource->GetPixel16(theX,theY);}
template<>
void getPix(PLBmpBase * pBmpSource, int theX, int theY, PLPixel8& theResult) { theResult = *(pBmpSource->GetLineArray()[theY]+theX);}

template<class PIXELTYPE>
void clearPixel(PIXELTYPE& theResult);
template<>
void clearPixel(PLPixel32& theResult ) { theResult.Set(0,0,0,0); }
template<>
void clearPixel(PLPixel24& theResult ) { theResult.Set(0,0,0); }
template<>
void clearPixel(PLPixel16& theResult ) { theResult.Set(0,0,0); }
template<>
void clearPixel(PLPixel8& theResult ) { theResult.Set(0); }

template<class PIXELTYPE>
void multAndStore(PIXELTYPE& theResult, PIXELTYPE * theSource, int theScale );
template<>
void multAndStore(PLPixel32 & theResult, PLPixel32 * theSource, int theScale ) {
    theResult.SetR( static_cast<PLBYTE>(minimum(theResult.GetR() + (((theSource->GetR() * theScale)+128) / 256), 255)) );
    theResult.SetG( static_cast<PLBYTE>(minimum(theResult.GetG() + (((theSource->GetG() * theScale)+128) / 256), 255)) );
    theResult.SetB( static_cast<PLBYTE>(minimum(theResult.GetB() + (((theSource->GetB() * theScale)+128) / 256), 255)) );
    theResult.SetA( static_cast<PLBYTE>(minimum(theResult.GetA() + (((theSource->GetA() * theScale)+128) / 256), 255)) );
}
template<>
void multAndStore(PLPixel24& theResult, PLPixel24 * theSource, int theScale ) {
    theResult.SetR( static_cast<PLBYTE>(minimum(theResult.GetR() + (((theSource->GetR() * theScale)+128) / 256), 255)) );
    theResult.SetG( static_cast<PLBYTE>(minimum(theResult.GetG() + (((theSource->GetG() * theScale)+128) / 256), 255)) );
    theResult.SetB( static_cast<PLBYTE>(minimum(theResult.GetB() + (((theSource->GetB() * theScale)+128) / 256), 255)) );

}
template<>
void multAndStore(PLPixel16& theResult, PLPixel16 * theSource, int theScale ) {
    theResult.SetR( static_cast<PLBYTE>(minimum(theResult.GetR() + ((theSource->GetR() * theScale) / 256), 255)) );
    theResult.SetG( static_cast<PLBYTE>(minimum(theResult.GetG() + ((theSource->GetG() * theScale) / 256), 255)) );
    theResult.SetB( static_cast<PLBYTE>(minimum(theResult.GetB() + ((theSource->GetB() * theScale) / 256), 255)) );
}
template<>
void multAndStore(PLPixel8 & theResult, PLPixel8 * theSource, int theScale ) {
    theResult.Set( static_cast<PLBYTE>(minimum(theResult.Get() + ((theSource->Get() * theScale) / 256), 255)) );
}

template <class PIXELTYPE>
void
gaussianblur(PLBmpBase * theSource, PLBmp * theDestination, const KernelVec & theKernel, double theRadius,
             unsigned theRealWidth, unsigned theRealHeight, double theSigma) {
    int myIntRadius = int(ceil(double(theRadius)));

    unsigned mySrcHeight = theSource->GetHeight();
    unsigned mySrcWidth = theSource->GetWidth();
    unsigned myDestWidth =  mySrcWidth;
    unsigned myDestHeight = mySrcHeight;
    PLBmp * myTempBmp = new PLAnyBmp();
    myTempBmp->Create(myDestWidth, mySrcHeight, theSource->GetPixelFormat());

    theDestination->Create(myDestWidth, myDestHeight, theSource->GetPixelFormat());

    // Convolve in x-direction
    int myOffset = myIntRadius - 1;
    {
        // pass 1: Glur in x direction
        for (unsigned y=0; y < mySrcHeight; ++y) {
            for(unsigned x=0; x<myDestWidth; ++x) {
                PIXELTYPE myColor;
                clearPixel(myColor);
                for(int w=0; w<myIntRadius; ++w) {
                    int xs = x + w - myOffset;
                    PIXELTYPE myKernelPixel;
                    if (xs>=0 && static_cast<unsigned>(xs)<theRealWidth) {
                        getPix<PIXELTYPE>(theSource, xs,y, myKernelPixel);
                    } else {
                        if (xs < 0) {
                            getPix<PIXELTYPE>(theSource, 0,y, myKernelPixel);
                        } else {
                            getPix<PIXELTYPE>(theSource, theRealWidth-1,y, myKernelPixel);
                        }
                    }
                    multAndStore(myColor, &myKernelPixel, theKernel[w]);
                    xs = x + w;
                    if (xs>=0 && static_cast<unsigned>(xs)<theRealWidth) {
                        getPix<PIXELTYPE>(theSource, xs,y, myKernelPixel);
                    } else {
                        if (xs < 0) {
                            getPix<PIXELTYPE>(theSource, 0,y, myKernelPixel);
                        } else {
                            getPix<PIXELTYPE>(theSource, theRealWidth-1,y, myKernelPixel);
                        }
                    }
                    multAndStore(myColor, &myKernelPixel, theKernel[w+myIntRadius]);
                }
                myTempBmp->SetPixel(x,y,myColor);
            }
        }
    }

    for(unsigned x=0; x<myDestWidth; ++x) {
        for (unsigned y=0; y < myDestHeight; ++y) {
            PIXELTYPE myColor;
            clearPixel(myColor);
            for(int w=0; w<myIntRadius; ++w) {
                int ys = y + w - myOffset;
                PIXELTYPE myKernelPixel;

                if (ys>=0 && static_cast<unsigned>(ys)<theRealHeight) {
                    getPix<PIXELTYPE>(myTempBmp, x,ys, myKernelPixel);
                } else {
                    if (ys < 0) {
                        getPix<PIXELTYPE>(myTempBmp, x,0, myKernelPixel);
                    } else {
                        getPix<PIXELTYPE>(myTempBmp, x,theRealHeight-1, myKernelPixel);
                    }
                }
                multAndStore(myColor, &myKernelPixel, theKernel[w]);
                ys = y + w;
                if (ys>=0 && static_cast<unsigned>(ys)<theRealHeight) {
                    getPix<PIXELTYPE>(myTempBmp, x,ys, myKernelPixel);
                } else {
                    if (ys < 0) {
                        getPix<PIXELTYPE>(myTempBmp, x,0, myKernelPixel);
                    } else {
                        getPix<PIXELTYPE>(myTempBmp, x,theRealHeight-1, myKernelPixel);
                    }
                }
                multAndStore(myColor, &myKernelPixel, theKernel[w+myIntRadius]);
            }
            theDestination->SetPixel(x,y,myColor);
        }
    }
    delete myTempBmp;
}

void
PLFilterGaussianBlur::Apply(PLBmpBase * theSource, PLBmp * theDestination) const {
    int myIntRadius = int(ceil(double(_myRadius)));
    bool myRecalcKernelFlag = false;
    if (theSource->GetWidth() <= myIntRadius*2) {
        _myRadius = (theSource->GetWidth() / 2) -1;
        myIntRadius = int(ceil(double(_myRadius)));
        myRecalcKernelFlag = true;
    }
    if (theSource->GetHeight() <= myIntRadius*2) {
        _myRadius = (theSource->GetHeight() / 2) -1;
        myRecalcKernelFlag = true;
    }
    if (myRecalcKernelFlag) {
        AC_WARNING << "Radius to high -> set to " << _myRadius << " and recalculate kernel";
        calcKernel();
    }
    switch (theSource->GetBitsPerPixel()) {
        case 32:
            gaussianblur<PLPixel32>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight, _mySigma);
            break;
        case 24:
            gaussianblur<PLPixel24>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight, _mySigma);
            break;
        case 16:
            gaussianblur<PLPixel16>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight, _mySigma);
            break;
        case 8:
            gaussianblur<PLPixel8>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight, _mySigma);
            break;
        default:
            throw (PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Unsupported."));
    };

}


void PLFilterGaussianBlur::calcKernel() const
{
    int myIntRadius = int(ceil(double(_myRadius)));
    double mySigma = _mySigma;
    double mySigma22 = 2*mySigma*mySigma;
    double mySigmaPi2 = 2*PI*mySigma;
    double mySqrtSigmaPi = sqrt(mySigmaPi2);
    _myKernelWidth = myIntRadius*2+1;
    std::vector<double> myFloatKernel;
    myFloatKernel.resize(_myKernelWidth);
    _myKernel.clear();
    _myKernel.resize(_myKernelWidth);
    double mySum = 0;
    for (int i=0; i<= myIntRadius; ++i) {

        // triangle kernel -- myFloatKernel[myIntRadius+i] = (0.5+0.5*i/myIntRadius)/(0.75*myIntRadius); // exp(-i*double(i/_myRadius)-1)/sqrt(2*PI);
        // original gauus without sigma --  myFloatKernel[myIntRadius+i] = exp(-i*double(i/_myRadius)-1)/sqrt(2*PI);
        myFloatKernel[myIntRadius+i] = exp((-i*double(i/_myRadius)-1)/mySigma22)/mySqrtSigmaPi;
        myFloatKernel[myIntRadius-i] = myFloatKernel[myIntRadius+i];
        mySum += myFloatKernel[myIntRadius+i];
        if (i != 0) {
            mySum += myFloatKernel[myIntRadius-i];
        }
    }
    unsigned myKernelSum = 0;
    for (int i=0; i<_myKernelWidth; ++i) {
        _myKernel[i] = int(myFloatKernel[i]*256/mySum+0.5);
        myKernelSum += _myKernel[i];
    }
    int myDiff = 256 - myKernelSum;
    _myKernel[myIntRadius] += myDiff;

}
