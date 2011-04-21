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


function Ruler(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

Ruler.prototype.Constructor = function (Public, theSceneViewer) {
    
    const RULER_COLOR  = new Vector4f(0.62,0.75,0.15,1);
    const CROSS_COLOR  = new Vector4f(0.9,0.01,0.52,1);
    const CROSS_RADIUS = 10;
    const TEXT_DISTANCE = 20;
    
    var _mySceneViewer = theSceneViewer;
    var _myEnabledFlag = false;
    var _myStartPoint  = null;
    var _myEndPoint    = null;
    var _myZ           = null;
    var _myShiftFlag   = false;
    var _myMouseX      = null;
    var _myMouseY      = null;

    //////////////////////////////////////////////////////////////////////
    // Public
    //////////////////////////////////////////////////////////////////////
    
    Public.enabled getter = function() {
        return _myEnabledFlag;
    }
    
    Public.enabled setter = function(theValue) {
        _myEnabledFlag = theValue;
    }
    
    Public.onPostRender = function() {
        
            _myZ = window.camera.position.z-1;
        
            var myMatrix = new Matrix4f();
            var myViewport = _mySceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
            window.setTextColor(CROSS_COLOR);
                var myText = "";
            
            if (!_myStartPoint && _myMouseX && _myMouseY) {
                window.getRenderer().draw(new Sphere(new Point3f(_myMouseX, window.height-_myMouseY, _myZ), CROSS_RADIUS), CROSS_COLOR, myMatrix, 1);
                myText = "x:" + _myMouseX;
                window.renderText(new Vector2f(_myMouseX + TEXT_DISTANCE, _myMouseY + TEXT_DISTANCE), myText, "Screen15", myViewport); 
                myText = "y:" + _myMouseY + "|" + (window.height-_myMouseY);
                window.renderText(new Vector2f(_myMouseX + TEXT_DISTANCE, _myMouseY + 2*TEXT_DISTANCE), myText, "Screen15", myViewport); 
                
            } else if (_myStartPoint) {
                
                _myEndPoint = new Point3f(_myMouseX, window.height-_myMouseY, _myZ);
                
                if (_myShiftFlag) {
                    var myWidth = Math.abs(_myStartPoint.x-_myEndPoint.x);
                    var myHeight = Math.abs(_myStartPoint.y-_myEndPoint.y);
                    if (myWidth > myHeight) {
                        _myEndPoint.y = _myStartPoint.y;
                    } else {
                        _myEndPoint.x = _myStartPoint.x;
                    }
                }
                window.getRenderer().draw(new LineSegment(_myStartPoint,_myEndPoint), RULER_COLOR, myMatrix, 1);
                var myFirstEdge = new Point3f(_myStartPoint.x, _myEndPoint.y, _myZ);
                var mySecondEdge = new Point3f(_myEndPoint.x, _myStartPoint.y, _myZ);
                window.getRenderer().draw(new LineSegment(_myStartPoint, myFirstEdge), RULER_COLOR, myMatrix, 1);
                window.getRenderer().draw(new LineSegment(_myStartPoint, mySecondEdge), RULER_COLOR, myMatrix, 1);
                window.getRenderer().draw(new LineSegment(myFirstEdge, _myEndPoint), RULER_COLOR, myMatrix, 1);
                window.getRenderer().draw(new LineSegment(mySecondEdge, _myEndPoint), RULER_COLOR, myMatrix, 1);
                
                window.getRenderer().draw(new Sphere(new Point3f(_myEndPoint), CROSS_RADIUS), CROSS_COLOR, myMatrix, 1);
                myText = "x:" + _myEndPoint.x;
                window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + TEXT_DISTANCE), myText, "Screen15", myViewport); 
                myText = "y:" + _myEndPoint.y + "|" + (window.height-_myEndPoint.y);
                window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + 2*TEXT_DISTANCE), myText, "Screen15", myViewport); 

                var myLength = magnitude(difference(_myStartPoint, _myEndPoint)).toFixed(1);
                
                if (_myShiftFlag) {
                    myText = "length:" +  myLength;
                    window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + 4*TEXT_DISTANCE), myText, "Screen15", myViewport);   
                } else {
                    var myAdjacentLeg = magnitude(difference(mySecondEdge, _myStartPoint));
                    var myHypotenuse = magnitude(difference(_myEndPoint, _myStartPoint));
                    myAdjacentLeg = myAdjacentLeg/myHypotenuse;
                    var myAlpha = degFromRad(Math.acos(myAdjacentLeg)).toFixed(1);;
                    var myWidth = Math.abs(_myStartPoint.x-_myEndPoint.x);
                    var myHeight = Math.abs(_myStartPoint.y-_myEndPoint.y);
                    myText = "size:" + myWidth + "x" + myHeight; 
                    window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + 4*TEXT_DISTANCE), myText, "Screen15", myViewport);   
                    myText = "length:" +  myLength;
                    window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + 5*TEXT_DISTANCE), myText, "Screen15", myViewport);   
                    myText = "angle:" + myAlpha;
                    window.renderText(new Vector2f(_myEndPoint.x + TEXT_DISTANCE, window.height-_myEndPoint.y + 6*TEXT_DISTANCE), myText, "Screen15", myViewport);   
                }
        }
    }
    
    Public.onKey = function (theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        switch(theKey) {
            case "right shift":
            case "left shift":
                if (theKeyState) {
                    _myShiftFlag = true;
                } else {
                    _myShiftFlag = false;
                }
                break;
            default:
                break;
        }
    }
    
    Public.onMouseButton = function (theButton, theState, theX, theY) {
       
        if (theState) {
            _myStartPoint = new Point3f(theX, window.height-theY, _myZ);
        } else {
            _myStartPoint = null;
            _myEndPoint = null;
            _myMouseX = null;
            _myMouseY = null;
            _myShiftFlag = false;
        }
    }
    
    Public.onMouseMotion = function (theX, theY) {
        _myMouseX = theX;
        _myMouseY = theY;
    }
    
    //////////////////////////////////////////////////////////////////////
    // Private
    //////////////////////////////////////////////////////////////////////

    function setup() {
    }

    
}
