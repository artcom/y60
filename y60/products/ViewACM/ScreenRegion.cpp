//=============================================================================
//
// Copyright (C) 2000-2001, ART+COM AG Berlin
//
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or dupli:mcated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: ScreenRegion.cpp,v $
//
//     $Author: valentin $
//
//   $Revision: 1.2 $
//
//
// Description:
//
// (CVS log at the bottom of this file)
//
//
//=============================================================================


#include "ScreenRegion.h"
#include <iostream>


using namespace std;  // automatically added!



namespace video {

    ScreenRegion::ScreenRegion() {
        reset();
    }
    
    ScreenRegion::~ScreenRegion() {
    }

    void 
    ScreenRegion::reset() {
        _myTopLeft[0]     = 0.0;
        _myTopLeft[1]     = 0.0;
        _myTopRight[0]    = 1.0;
        _myTopRight[1]    = 0.0;
        
        _myBottomRight[0] = 1.0;
        _myBottomRight[1] = 1.0;
        _myBottomLeft[0]  = 0.0;
        _myBottomLeft[1]  = 1.0;
    }
    
    void
    ScreenRegion::set(const EDGE & theEdge, const double & s, const double & t) {
        switch (theEdge) {
            case TOP_LEFT :
                _myTopLeft[0] = s;
                _myTopLeft[1] = t;
                break;
            case TOP_RIGHT:
                _myTopRight[0] = s;
                _myTopRight[1] = t;
                break;
            case BOTTOM_RIGHT:
                _myBottomRight[0] = s;
                _myBottomRight[1] = t;
                break;
            case BOTTOM_LEFT:
                _myBottomLeft[0] = s;
                _myBottomLeft[1] = t;
                break;
        }
        
    }
    
    void
    ScreenRegion::dump() {
        cout <<"_myTopLeft : "<<_myTopLeft[0]<<", "<<_myTopLeft[1]<<endl;
        cout <<"_myTopRight : "<<_myTopRight[0]<<", "<<_myTopRight[1]<<endl;
        cout <<"_myBottomRight : "<<_myBottomRight[0]<<", "<<_myBottomRight[1]<<endl;
        cout <<"_myBottomLeft : "<<_myBottomLeft[0]<<", "<<_myBottomLeft[1]<<endl;
    }
    
    void
    ScreenRegion::rotate(const double & angle) {
        if (angle > 0.0 && angle <= 90.0) {
            _myTopLeft[0]     = 0.0;
            _myTopLeft[1]     = 0.0;
            _myTopRight[0]    = 0.0;
            _myTopRight[1]    = 1.0;

            _myBottomRight[0] = 1.0;
            _myBottomRight[1] = 1.0;
            _myBottomLeft[0]  = 1.0;
            _myBottomLeft[1]  = 0.0;
        }            
    }
    
    void
    ScreenRegion::stretch(const double & myXStretch, const double & myYStretch) {
            _myTopLeft[0]     *= myXStretch;
            _myTopRight[0]    *= myXStretch;
            _myBottomRight[0] *= myXStretch;
            _myBottomLeft[0]  *= myXStretch;
            
            _myTopLeft[1]     *= myYStretch;
            _myTopRight[1]    *= myYStretch;
            _myBottomRight[1] *= myYStretch;
            _myBottomLeft[1]  *= myYStretch;
     }
    
    void 
    ScreenRegion::get(const EDGE& theEdge, double & s, double & t) const{
        switch (theEdge) {
            case TOP_LEFT :
                s = _myTopLeft[0];
                t = _myTopLeft[1];
                break;
            case TOP_RIGHT:
                s = _myTopRight[0];
                t = _myTopRight[1];
                break;
            case BOTTOM_RIGHT:
                s = _myBottomRight[0];
                t = _myBottomRight[1];
                break;
            case BOTTOM_LEFT:
                s = _myBottomLeft[0];
                t = _myBottomLeft[1];
                break;
        }
    }
    
    vector<float>  
    ScreenRegion::get(const EDGE& theEdge) const{
        vector<float> theAnswer;
        theAnswer.resize(2);
        switch (theEdge) {
            case TOP_LEFT :
                theAnswer[0] = _myTopLeft[0];
                theAnswer[1] = _myTopLeft[1];
                break;
            case TOP_RIGHT:
                theAnswer[0] = _myTopRight[0];
                theAnswer[1] = _myTopRight[1];
                break;
            case BOTTOM_RIGHT:
                theAnswer[0] = _myBottomRight[0];
                theAnswer[1] = _myBottomRight[1];
                break;
            case BOTTOM_LEFT:
                theAnswer[0] = _myBottomLeft[0];
                theAnswer[1] = _myBottomLeft[1];
                break;
        }
        return theAnswer;
    }
}


