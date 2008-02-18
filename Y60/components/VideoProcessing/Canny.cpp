#include "Canny.h"

using namespace asl;
using namespace dom;
using namespace std;
using namespace y60;


Canny::Canny( const string & theName ) : Algorithm( theName ) {
    _myGradientThreshold = 0.015;
}

void Canny::onFrame( double t ) {

    const GRAYRaster * mySourceFrame = 
        dom::dynamic_cast_Value<GRAYRaster>(&*_mySourceImage->getRasterValue());
    dom::Node::WritableValue<GRAYRaster> myTargetFrameLock(_myTargetImage->getRasterValueNode());
    GRAYRaster & myTargetFrame = myTargetFrameLock.get();

    float mySobelX[9] = { -1.0, 0.0, 1.0, -2.0, 0.0, 2.0, -1.0, 0.0, 1.0 };
    float mySobelY[9] = { 1.0, 2.0, 1.0, 0.0, 0.0, 0.0, -1.0, -2.0, -1.0 };

    _myMaxGradient = 0;

    for (unsigned y = 0; y < _myHeight; y++) {
        for (unsigned x = 0; x < _myWidth; x++) {
            GRAYRaster::const_sub_iterator itSrc = mySourceFrame->begin(x,y,3,3);
            unsigned i = 0;
            float gx = 0;
            float gy = 0;
            while (itSrc != mySourceFrame->end(x,y,3,3)) {
                float mySrcFloat = static_cast<float>((*itSrc++).get())/255;
                gx += mySobelX[i++] * mySrcFloat;
                gy += mySobelY[i++] * mySrcFloat;
            }
            float myGradient = sqrt(gx*gx + gy*gy);            
            _myGradientImage[y * _myWidth + x] = myGradient;
            if (_myMaxGradient < myGradient) {
                _myMaxGradient = myGradient;
            }
            
            float myDirection = 0.0f;
            if (gx == 0 && gy != 0) {
                myDirection = 0.5 * PI;
            } else {
                myDirection = atan(gy/gx);            
            }
            // round to 0, 1/4pi, 1/2pi, 3/4pi
            if (myDirection > 0.125 * PI && myDirection < 0.375 * PI) {
                myDirection = 0.25 * PI;
            } else if (myDirection > 0.375 * PI && myDirection < 0.625 * PI) {
                myDirection = 0.5 * PI;
            } else if (myDirection > 0.625 * PI && myDirection < 0.875 * PI) {
                myDirection = 0.75 * PI;
            } else {
                myDirection = 0;
            }

            _myDirectionImage[y * _myWidth + x] = myDirection;
        }
    }

    // nonmaximum suppression
    GRAYRaster::iterator itTrgt = myTargetFrame.begin();
    float myResult = 0;
    for (unsigned y = 0; y < _myHeight; y++) {
        for (unsigned x = 0; x < _myWidth; x++) {
            unsigned myIndex = y * _myWidth + x;
            if (y == 0 || y == _myHeight -1 || x == 0 || x == _myWidth-1) {
                _myResultImage[myIndex] = 0;
                continue;
            }
            float myGradient = _myGradientImage[myIndex];
            if (_myGradientImage[myIndex] <= _myGradientThreshold * _myMaxGradient) {
                _myGradientImage[myIndex] = 0;
            }
            _myResultImage[myIndex] = _myGradientImage[myIndex] == 0 ? 0 : 1;
            // check surrounding pixels"
            for (int i = -1; i <= 1; i=i+2) {
                for (int j = -1; j <= 1; j=j+2) {
                    if (!isOnEdge(myIndex, i, j)) {
                        if (_myGradientImage[(y+j) * _myWidth + (x+i)] 
                            > _myGradientImage[myIndex]) 
                        {
                            _myResultImage[myIndex] = 0;
                        }
                    }
                }
            }
        }
    }

    // hysteresis
//    for (unsigned y = 0; y < _myHeight; y++) {
//        for (unsigned x = 0; x < _myWidth; x++) {
//            unsigned myIndex = y * _myWidth + x;
//            _myResult[myIndex] = 0;
//            if (_myResultImage[myIndex] == 1) {
//                if (_myGradientImage[myIndex] > _myHighThreshold * _myMaxGradient) {
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
    for (unsigned y = 0; y < _myHeight; y++) {
        for (unsigned x = 0; x < _myWidth; x++) {
            unsigned myIndex = y * _myWidth + x;
            switch(_myOutput) {
                case CANNY_OUTPUT:
                    (*itTrgt++) = static_cast<unsigned char>((1-_myResultImage[myIndex])*255);
                    break;
                case GRADIENT_OUTPUT:
                    (*itTrgt++) = 
                        static_cast<unsigned char>((1-_myGradientImage[myIndex]) * 255);
                    break;
                case DIRECTION_OUTPUT:
                    (*itTrgt++) = 
                        static_cast<unsigned char>((1-_myDirectionImage[myIndex]) * 255);
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
                if(_myGradientImage[myIndex] >= _myLowThreshold * _myMaxGradient
                   && _myGradientImage[myIndex] <= _myHighThreshold * _myMaxGradient) {
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
    if (_myDirectionImage[theIndex] == 0) {
        return theYOffset == 0;
    } else if (_myDirectionImage[theIndex] == 0.25*PI) {
        return theXOffset * theYOffset == -1; 
    } else if (_myDirectionImage[theIndex] == 0.5*PI) {
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
                _myGradientImage.reserve(_myWidth * _myHeight);
                _myDirectionImage.reserve(_myWidth * _myHeight);
                _myResultImage.reserve(_myWidth * _myHeight);
                _myResult.reserve(_myWidth * _myHeight);
            } 
        }
        if (myName == "lowthreshold") {
            _myLowThreshold = as<float>(myValue);
        } else if (myName == "highthreshold") {
            _myHighThreshold = as<float>(myValue);
        } else if (myName == "output") {
            if (myValue == "gradient") {
                _myOutput = GRADIENT_OUTPUT;
            } else if (myValue == "direction") {
                _myOutput = DIRECTION_OUTPUT;
            } else {
                _myOutput = CANNY_OUTPUT;
            }
        }
    }
}
