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
//   $RCSfile: Slider.js,v $
//   $Author: christian $
//   $Revision: 1.1 $
//   $Date: 2005/04/22 16:30:06 $
//
//
//=============================================================================

function Slider(theGtkSlider) {
    this.Constructor(this, theGtkSlider);
}

Slider.prototype.Constructor = function(self, theGtkSlider) {
    var _myGtkSlider   = theGtkSlider;
    var _myBegin       = 0;
    var _myEnd         = 1;
    var _mySliderValue = _myGtkSlider.value;

    self.length = (_myEnd - _myBegin);

    self.setBegin = function(theBegin) {
        _myBegin = theBegin;
        self.length = (_myEnd - _myBegin);
    }

    self.setEnd = function(theEnd) {
        _myEnd = theEnd;
        self.length = (_myEnd - _myBegin);
    }

    self.setValue = function(theValue) {
        _mySliderValue = (theValue - _myBegin) / self.length;
        _mySliderValue = clamp(_mySliderValue, 0, 1);
        _myGtkSlider.value = _mySliderValue;
    }

    self.wasMoved = function() {
        return (_mySliderValue != _myGtkSlider.value);
    }

    self.getValue = function() {
        return _myBegin + _myGtkSlider.value * self.length;
    }
    self.endPosition = function() {
        return (_myGtkSlider.value == 1);
    }
}

function AnimationSlider(theGtkSlider) {
    this.Constructor(this, theGtkSlider);
}

AnimationSlider.prototype.Constructor = function(self, theGtkSlider) {
    Slider.prototype.Constructor(self, theGtkSlider);

    var _myLoopFlag    = false;

    self.isDone = function() {
        return (self.endPosition() && !_myLoopFlag);
    }

    self.setLimits = function(theAnimationNode) {
        var myStart  = Number.MAX_VALUE;
        var myEnd    = Number.MIN_VALUE;
        _myLoopFlag  = false;
        for (var i = 0; i < theAnimationNode.childNodes.length; ++i) {
            var myAnimation = theAnimationNode.childNode(i);
            if (!myAnimation) {
                continue;
            }
            if (myAnimation.begin < myStart) {
                myStart = myAnimation.begin;
            }
            if ((myAnimation.begin + myAnimation.duration) > myEnd) {
                myEnd = myAnimation.begin + myAnimation.duration;
            }
            if (myAnimation.count == 0) {
                _myLoopFlag = true;
            }
        }

        self.setBegin(myStart);
        self.setEnd(myEnd);
    }
}
