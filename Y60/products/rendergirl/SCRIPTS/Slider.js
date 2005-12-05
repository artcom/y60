//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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