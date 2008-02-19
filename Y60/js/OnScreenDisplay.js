//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: OnScreenDisplay.js,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/03/30 17:45:57 $
//
//
//=============================================================================

function OnScreenDisplay(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

OnScreenDisplay.prototype.Constructor = function(self, theSceneViewer) {
    const DISPLAY_DURATION  = 3;
    const FADE_DURATION     = 1;
    const LINE_COUNT        = 5;
    const BOX_WIDTH         = 600;
    const BOX_HEIGHT        = (LINE_COUNT + 1) * 24;

    var _myDisplayStartTime = 0;
    var _myOverlay          = null;
    var _myMessage          = [];
    var _myNextMessageLine  = 0;
    var _myFontname         = "Screen15";
    var _myViewport         = null;
    self.setFontname = function(theFontname) {
        _myFontname = theFontname;
    }

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
    }

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
    }

    self.hide = function() {
        _myOverlay.visible     = false;
        _myDisplayStartTime    = 0;
        _myMessage             = [];
        _myNextMessageLine     = 0;
    }
    self.onPostRender = function() {
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
                var myGreyValue = 1 - (_myMessage.length - myLine) * (0.3 / LINE_COUNT);
                window.setTextColor([myGreyValue,myGreyValue,myGreyValue,_myOverlay.alpha]);
                window.renderText(new Vector2f(myXPos, (myYPos + (myLine * 24))), _myMessage[i], _myFontname, _myViewport);
                myLine++;
            }
            for (i = 0; i < _myNextMessageLine; ++i) {
                var myGreyValue = 1 - (_myMessage.length - myLine) * (0.3 / LINE_COUNT);
                window.setTextColor([myGreyValue,myGreyValue,myGreyValue,_myOverlay.alpha]);
                window.renderText(new Vector2f(myXPos, (myYPos + (myLine * 24))), _myMessage[i], _myFontname, _myViewport);
                myLine++;
            }
            window.setTextColor([1,1,1,1]);
        }
    }

    function createTextOverlay() {
        var myImage = window.scene.images.find("//image[@name = 'OSD_Overlay']");
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
}
