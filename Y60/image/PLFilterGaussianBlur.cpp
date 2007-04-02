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

#include "PLFilterGaussianBlur.h"

#include <asl/numeric_functions.h>
#include <asl/Logger.h>
#include <asl/Vector234.h>

#include <paintlib/plpixel8.h>
#include <paintlib/plbitmap.h>
#include <paintlib/planybmp.h>
#include <paintlib/Filter/plfiltercrop.h>
#include <paintlib/Filter/plfilterresizebilinear.h>

#include <iostream>

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
    theResult.SetR(minimum(theResult.GetR() + (((theSource->GetR() * theScale)+128) / 256), 255));
    theResult.SetG(minimum(theResult.GetG() + (((theSource->GetG() * theScale)+128) / 256), 255));
    theResult.SetB(minimum(theResult.GetB() + (((theSource->GetB() * theScale)+128) / 256), 255));
    theResult.SetA(minimum(theResult.GetA() + (((theSource->GetA() * theScale)+128) / 256), 255));
}
template<>
void multAndStore(PLPixel24& theResult, PLPixel24 * theSource, int theScale ) { 
    theResult.SetR(minimum(theResult.GetR() + (((theSource->GetR() * theScale)+128) / 256), 255));
    theResult.SetG(minimum(theResult.GetG() + (((theSource->GetG() * theScale)+128) / 256), 255));
    theResult.SetB(minimum(theResult.GetB() + (((theSource->GetB() * theScale)+128) / 256), 255));

}
template<>
void multAndStore(PLPixel16& theResult, PLPixel16 * theSource, int theScale ) { 
    theResult.SetR(minimum(theResult.GetR() + ((theSource->GetR() * theScale) / 256), 255));
    theResult.SetG(minimum(theResult.GetG() + ((theSource->GetG() * theScale) / 256), 255));
    theResult.SetB(minimum(theResult.GetB() + ((theSource->GetB() * theScale) / 256), 255));
}
template<>
void multAndStore(PLPixel8 & theResult, PLPixel8 * theSource, int theScale ) { 
    theResult.Set(minimum(theResult.Get() + ((theSource->Get() * theScale) / 256), 255));
}

template <class PIXELTYPE>
void
gaussianblur(PLBmpBase * theSource, PLBmp * theDestination, const KernelVec & theKernel, double theRadius, 
			 unsigned theRealWidth, unsigned theRealHeight) {    
    int myIntRadius = int(ceil(double(theRadius)));

    unsigned mySrcHeight = theSource->GetHeight();
    unsigned mySrcWidth = theSource->GetWidth();
    int myDestWidth =  mySrcWidth;
    int myDestHeight = mySrcHeight;
    PLBmp * myTempBmp = new PLAnyBmp();
    myTempBmp->Create(myDestWidth, mySrcHeight, theSource->GetPixelFormat());

    theDestination->Create(myDestWidth, myDestHeight, theSource->GetPixelFormat());

    // Convolve in x-direction
    int myOffset = myIntRadius - 1;
    {
        // pass 1: Glur in x direction
        for (int y=0; y < mySrcHeight; ++y) {
            for(int x=0; x<myDestWidth; ++x) {
                PIXELTYPE myColor;
				clearPixel(myColor);      
                for(int w=0; w<myIntRadius; ++w) {
                    int xs = x + w - myOffset;
                    PIXELTYPE myKernelPixel;
                    if (xs>=0 && xs<theRealWidth) {
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
                    if (xs>=0 && xs<theRealWidth) {
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

    for(int x=0; x<myDestWidth; ++x) {
        for (int y=0; y < myDestHeight; ++y) {
            PIXELTYPE myColor;
			clearPixel(myColor);      
            for(int w=0; w<myIntRadius; ++w) {
                int ys = y + w - myOffset;
                PIXELTYPE myKernelPixel;

                if (ys>=0 && ys<theRealHeight) {
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
                if (ys>=0 && ys<theRealHeight) {
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
            gaussianblur<PLPixel32>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight);
            break;
        case 24:
            gaussianblur<PLPixel24>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight);
            break;
        case 16:
            gaussianblur<PLPixel16>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight);
            break;
        case 8:
			gaussianblur<PLPixel8>(theSource, theDestination, _myKernel, _myRadius, _myRealWidth, _myRealHeight);
            break;
        default:
            throw (PLTextException (PL_ERRFORMAT_NOT_SUPPORTED, "Unsupported."));
    };
        
}

void PLFilterGaussianBlur::calcKernel() const
{
    int myIntRadius = int(ceil(double(_myRadius)));    
    _myKernelWidth = myIntRadius*2+1;
    std::vector<double> myFloatKernel;
    myFloatKernel.resize(_myKernelWidth);
	_myKernel.clear();
    _myKernel.resize(_myKernelWidth);
    double mySum = 0;
    for (int i=0; i<= myIntRadius; ++i) {
        myFloatKernel[myIntRadius+i] = exp(-i*double(i/_myRadius)-1)/sqrt(2*PI);
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
