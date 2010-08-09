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
//
//   $RCSfile: OnScreenDisplay.js,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/30 17:45:57 $
//
//
//=============================================================================

/*jslint white:false nomen:false plusplus:false*/
/*globals Vector2f, Vector4f, window, Modelling, ImageOverlay*/

function OnScreenDisplay(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

OnScreenDisplay.prototype.Constructor = function(self, theSceneViewer) {
    
    /////////////////////
    // Private Members //
    /////////////////////
    
    var DISPLAY_DURATION  = 3;
    var FADE_DURATION     = 1;
    var LINE_COUNT        = 5;
    var BOX_WIDTH         = 600;
    var BOX_HEIGHT        = (LINE_COUNT + 1) * 24;

    var _myDisplayStartTime = 0;
    var _myOverlay          = null;
    var _myMessage          = [];
    var _myNextMessageLine  = 0;
    var _myFontname         = "Screen15";
    var _myViewport         = null;
    
    /////////////////////
    // Private Methods //
    /////////////////////
    
    function createTextOverlay() {
        var myImage = window.scene.images.find("image[@name = 'OSD_Overlay']");
        if(!myImage) {
            myImage = Modelling.createImage(window.scene, 1,1,"RGB");
            myImage.name = "OSD_Overlay";
        }
        myImage.src = "shadertex/on_screen_display.rgb";
        _myViewport = theSceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
        var myBoxOverlay = new ImageOverlay(window.scene, myImage, [0,0], _myViewport.childNode("overlays"));
        myBoxOverlay.width  = BOX_WIDTH;
        myBoxOverlay.height = BOX_HEIGHT;
        myBoxOverlay.position = new Vector2f(((_myViewport.size[0] * window.width) - myBoxOverlay.width) / 2,
                                             ((_myViewport.size[1] * window.height) - myBoxOverlay.height) / 2);
        //print(myBoxOverlay.position, _myViewport.size);
        myBoxOverlay.visible = true;

        var myColor = 0.3;
        myBoxOverlay.color = new Vector4f(myColor,myColor,myColor,0.75);
        return myBoxOverlay;
    }
    
    ////////////////////
    // Public Methods //
    ////////////////////
    
    self.setFontname = function(theFontname) {
        _myFontname = theFontname;
    };

    self.setMessage = function(theMessage, theLine) {
        if (!_myOverlay) {
            _myOverlay = createTextOverlay();
        }

        if (theLine != undefined) {
            _myMessage[theLine] = theMessage;
        } else {
            var mySplitMessage = String(theMessage).split("\n");
            for (var i = 0; i < mySplitMessage.length; ++i) {
                _myMessage[_myNextMessageLine++] = mySplitMessage[i];
                if (_myNextMessageLine >= LINE_COUNT) {
                    _myNextMessageLine = 0;
                }
            }
        }
        _myDisplayStartTime = theSceneViewer.getCurrentTime();
    };

    self.onFrame = function(theTime) {
        if (_myOverlay && _myDisplayStartTime) {
            var myDisplayDuration = theTime - _myDisplayStartTime;
            if (myDisplayDuration <= DISPLAY_DURATION) {
                _myOverlay.visible = true;
                _myOverlay.alpha   = 1;
            } else if (myDisplayDuration <= DISPLAY_DURATION + FADE_DURATION) {
                _myOverlay.alpha = 1 - (myDisplayDuration - DISPLAY_DURATION) / FADE_DURATION;
            } else {
                self.hide();
            }

            _myOverlay.position = new Vector2f(((_myViewport.size[0] * window.width) - _myOverlay.width) / 2,
                                               ((_myViewport.size[1] * window.height) - _myOverlay.height) / 2);
        }
    };

    self.hide = function() {
        _myOverlay.visible     = false;
        _myDisplayStartTime    = 0;
        _myMessage             = [];
        _myNextMessageLine     = 0;
    };
    
    self.onPostRender = function() {
        var myGreyValue;
        if (_myOverlay && _myMessage.length) {
            var myXPos = ((_myViewport.size[0] * window.width)  - BOX_WIDTH + 140) / 2;
            var myYPos = ((_myViewport.size[1] * window.height) - BOX_HEIGHT + 50) / 2;
            if (myXPos < 10) {
                myXPos = 10;
            }
            if (myYPos < 10) {
                myYPos = 10;
            }

            var myLine = 0;
            for (var i = _myNextMessageLine; i < _myMessage.length; ++i) {
                myGreyValue = 1 - (_myMessage.length - myLine) * (0.3 / LINE_COUNT);
                window.setTextColor([myGreyValue,myGreyValue,myGreyValue,_myOverlay.alpha]);
                window.renderText(new Vector2f(myXPos, (myYPos + (myLine * 24))), _myMessage[i], _myFontname, _myViewport);
                myLine++;
            }
            for (i = 0; i < _myNextMessageLine; ++i) {
                myGreyValue = 1 - (_myMessage.length - myLine) * (0.3 / LINE_COUNT);
                window.setTextColor([myGreyValue,myGreyValue,myGreyValue,_myOverlay.alpha]);
                window.renderText(new Vector2f(myXPos, (myYPos + (myLine * 24))), _myMessage[i], _myFontname, _myViewport);
                myLine++;
            }
            window.setTextColor([1,1,1,1]);
        }
    };
};
