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
//
// Description: TODO  
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations: 
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "Canny.h"

using namespace asl;
using namespace dom;
using namespace std;
using namespace y60;


Canny::Canny( const string & theName ) : Algorithm( theName ) {
//    _myGradientThreshold = 0.03;
    _myGradientThreshold = 100;
}

void Canny::onFrame( double t ) {

    const GRAYRaster * mySourceFrame = 
        dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceImage->getRasterValue());
    dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
    GRAYRaster & myTargetFrame = myTargetFrameLock.get();

//    float mySobelX[9] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
//    float mySobelY[9] = { -1.0, -2.0, -1.0, 0.0, 0.0, 0.0, 1.0, 2.0, 1.0 };
//
//    _myMaxGradient = 0;
//
//    for (unsigned y = 0; y < _myHeight; y++) {
//        for (unsigned x = 0; x < _myWidth; x++) {
//            GRAYRaster::const_sub_iterator itSrc = mySourceFrame->begin(x,y,3,3);
//            unsigned i = 0;
//            float gx = 0;
//            float gy = 0;
//            while (itSrc != mySourceFrame->end(x,y,3,3)) {
//                float mySrcFloat = static_cast<float>((*itSrc++).get())/255;
//                gx += mySobelX[i++] * mySrcFloat;
//                gy += mySobelY[i++] * mySrcFloat;
//            }
//            float myGradient = sqrt(gx*gx + gy*gy);            
//            _myGradients[y * _myWidth + x] = myGradient; 
//            if (_myMaxGradient < myGradient) {
//                _myMaxGradient = myGradient;
//            }
//            
//            float myDirection = 0.0f;
//            if (gx == 0 && gy != 0) {
//                myDirection = 0.5 * PI;
//            } else {
//                myDirection = atan(gy/gx);            
//            }
//            // round to 0, 1/4pi, 1/2pi, 3/4pi
//            if (myDirection > 0.125 * PI && myDirection < 0.375 * PI) {
//                myDirection = 0.25 * PI;
//            } else if (myDirection > 0.375 * PI && myDirection < 0.625 * PI) {
//                myDirection = 0.5 * PI;
//            } else if (myDirection > 0.625 * PI && myDirection < 0.875 * PI) {
//                myDirection = 0.75 * PI;
//            } else {
//                myDirection = 0;
//            }
//
//            _myDirections[y * _myWidth + x] = myDirection;
//        }
//    }
//
//  // nonmaximum suppression
//    float myResult = 0;
//    for (int y = 0; y < _myHeight; y++) {
//        for (unsigned x = 0; x < _myWidth; x++) {
//            unsigned myIndex = y * _myWidth + x;
//            if (y == 0 || y == _myHeight -1 || x == 0 || x == _myWidth-1) {
//                _myResultImage[myIndex] = 0;
//                continue;
//            }
//            float myGradient = _myGradients[myIndex];
//            if (_myGradients[myIndex] <= _myGradientThreshold * _myMaxGradient) {
//                _myGradients[myIndex] = 0;
//            }
//            _myResultImage[myIndex] = _myGradients[myIndex] == 0 ? 0 : 1;
//            // check surrounding pixels"
//            for (int i = -1; i <= 1; i=i+2) {
//                for (int j = -1; j <= 1; j=j+2) {
//                    if (!isOnEdge(myIndex, i, j)) {
//                        if (!(i==0 && j==0)) {
//                            if (_myGradients[(y+j) * _myWidth + (x+i)] 
//                                > _myGradients[myIndex]) 
//                            {
//                                _myResultImage[myIndex] = 0;
//                            }
//                        }
//                    }
//                }
//            }
//        }
//    }

//
//    for (int y = 1; y < _myHeight-1; y++) {
//        for (unsigned x = 1; x < _myWidth-1; x++) {
//            unsigned myIndex = y * _myWidth + x;
//            _myResult[myIndex] = 0;
//            if (_myResultImage[myIndex] == 1) {
//                for (int i = -1; i <= 1; i++) {
//                    for (int j = -1; j <= 1; j++) {
//                        _myResult[(y+j) * _myWidth + (x+i)] = 1;
//                    }
//                }
//            }
//        }
//    }

  // hysteresis
//    for (unsigned y = 0; y < _myHeight; y++) {
//        for (unsigned x = 0; x < _myWidth; x++) {
//            unsigned myIndex = y * _myWidth + x;
//            _myResult[myIndex] = 0;
//            if (_myResultImage[myIndex] == 1) {
//                if (_myGradients[myIndex] > _myHighThreshold * _myMaxGradient) {
//                    _myResult[myIndex] = 1;
//                }
//            }
//        }
//    }
//
//    bool myDone = false;
//    for (unsigned i = 0; myDone = false || i < 500; i++) { 
//        myDone = doHysteresisStep();
//    }
//
//    dom::Node::WritableValue<GRAYRaster>    
//        myDirectionFrameLock(_myDirectionImage->getRasterValueNode());
//    GRAYRaster & myDirectionFrame = myDirectionFrameLock.get();
//    GRAYRaster::iterator itDir = myDirectionFrame.begin();
    GRAYRaster::iterator itTrgt = myTargetFrame.begin();
    GRAYRaster::const_iterator itSrc = mySourceFrame->begin();
    for (int y = _myHeight-1; y >= 0; y--) {
        for (unsigned x = 0; x < _myWidth; x++) {
            unsigned myIndex = y * _myWidth + x;
            unsigned char mySrcValue = (*itSrc++).get();
            switch(_myOutput) {
                case CANNY_OUTPUT:
                    //(*itTrgt++) = static_cast<unsigned char>((_myResultImage[myIndex])*255);
                    (*itTrgt++) = mySrcValue > _myGradientThreshold ? 255 : 0;
                    break;
                case GRADIENT_OUTPUT:
                    (*itTrgt++) = 
                        static_cast<unsigned char>((_myGradients[myIndex]) * 255);
                    break;
                case DIRECTION_OUTPUT:
                    break;
            }
        }
    }
}

bool Canny::doHysteresisStep() {
    bool myDone = true;
    for (unsigned y = 0; y < _myHeight; y++) {
        for (unsigned x = 0; x < _myWidth; x++) {
            unsigned myIndex = y*_myWidth+x;
            bool hasMarkedNeighbour = false;
            if (_myResultImage[myIndex] == 1) {
                if(_myGradients[myIndex] >= _myLowThreshold * _myMaxGradient
                   && _myGradients[myIndex] <= _myHighThreshold * _myMaxGradient) {
                   for (int r = -1; r <= 1; r++) {
                       for (int s = -1; s <= 1; s++) {
                           hasMarkedNeighbour |= _myResult[(y+r) * _myWidth + x + s] == 1;
                       }
                   }
                   if (hasMarkedNeighbour) {
                       _myResult[myIndex] = 1;
                       myDone = false; 
                   }
                }
            }
        }
    }
    return myDone;
}


bool Canny::isOnEdge( unsigned theIndex, int theXOffset, int theYOffset) {
    if (_myDirections[theIndex] == 0) {
        return theYOffset == 0;
    } else if (_myDirections[theIndex] == 0.25*PI) {
        return theXOffset * theYOffset == -1; 
    } else if (_myDirections[theIndex] == 0.5*PI) {
        return theXOffset == 0;
    } else {
        return theXOffset * theYOffset == 1;
    }
}


void Canny::configure( const dom::Node & theNode ) {

    for (unsigned i = 0; i < theNode.childNodesLength(); i++) {
        const std::string myName = 
            theNode.childNode("property",i)->getAttribute("name")->nodeValue();
        const std::string myValue = 
            theNode.childNode("property",i)->getAttribute("value")->nodeValue();

        dom::NodePtr myImage = _myScene->getSceneDom()->getElementById(myValue);
        if (myImage) {
            if( myName == "sourceimage") {
                _mySourceImage = myImage->getFacade<y60::Image>();
            } else if( myName == "targetimage") {
                _myTargetImage = myImage->getFacade<y60::Image>();
                _myWidth  = _mySourceImage->getRasterPtr()->width();
                _myHeight = _mySourceImage->getRasterPtr()->height();
                _myGradients.reserve(_myWidth * _myHeight);
                _myDirections.reserve(_myWidth * _myHeight);
                _myResultImage.reserve(_myWidth * _myHeight);
                _myResult.reserve(_myWidth * _myHeight);
            } else if (myName == "directionimage") {
                _myDirectionImage = myImage->getFacade<y60::Image>();
            }
        }
        if (myName == "output") {
            if (myValue == "gradient") {
                _myOutput = GRADIENT_OUTPUT;
            } else if (myValue == "direction") {
                _myOutput = DIRECTION_OUTPUT;
            } else {
                _myOutput = CANNY_OUTPUT;
            }
        }
        else if (myName == "gradientthreshold") {
            float myThreshold = as<float>(myValue);
            _myGradientThreshold = static_cast<unsigned char>(myThreshold);
            AC_INFO << "myValue: " << myValue;
            AC_INFO << "myGradientThreshold: " << _myGradientThreshold;
        }
    }
}
