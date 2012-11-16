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

use("ASSManager.js");

use("Button.js")
use("Overlay.js")

var QUIT_OSD = true;
var OSD_FILE = undefined;
const ENABLE_QUIT_OSD_TIME = 4.0;
const ENABLE_QUIT_OSD_DISTANCE = 50;//65.0;

function ASSOSD(theViewer, theOSDFlag) {
    this.Constructor(this, {}, theViewer, theOSDFlag);
}

ASSOSD.prototype.Constructor = function(Public, Protected, theViewer) {

    ////////////////////////////////////////
    // Inheritance
    ////////////////////////////////////////

    ASSManager.prototype.Constructor(Public, Protected, theViewer, true);

    ////////////////////////////////////////
    // Member
    ////////////////////////////////////////

    var Base = {};

    var _myQuitCancelButton  = null;
    var _myQuitConfirmButton = null;
    var _myQuitOSD = null;
    var _myQuitCursorEvent = null;

    ////////////////////////////////////////
    // Public
    ////////////////////////////////////////

    Public.setOSDInvisible = function() {
        _myQuitOSD.visible = false;
        _myQuitCursorEvent = null;
    }

    function tryPressQuitCancelButton( theEventNode ) {
        if (_myQuitOSD.visible) {
            var myPosition = theEventNode.position3D;
            _myQuitCancelButton.onMouseButton(MOUSE_DOWN, myPosition.x,
                                              myPosition.y, 20);
            _myQuitCancelButton.onMouseButton(MOUSE_UP, myPosition.x,
                                              myPosition.y, 20);
            _myQuitConfirmButton.onMouseButton(MOUSE_DOWN, myPosition.x,
                                               myPosition.y, 20);
            _myQuitConfirmButton.onMouseButton(MOUSE_UP, myPosition.x,
                                               myPosition.y, 20);
        }
    }

    Base.onASSEvent = Public.onASSEvent;
    Public.onASSEvent = function(theEventNode) {
        switch(theEventNode.type) {
        case "add":
            if (_myQuitOSD.visible) {
                print( "got add event and trying to press button!" );
                tryPressQuitCancelButton( theEventNode );
            } else {
                if (!_myQuitCursorEvent) {
                    _myQuitCursorEvent = theEventNode;
                }
            }
            break;
        case "move":
            if (_myQuitCursorEvent && _myQuitCursorEvent.id == theEventNode.id) {
                var myTouchDuration = theEventNode.simulation_time
                                      - _myQuitCursorEvent.simulation_time;
                var myHandTraveled = distance(theEventNode.position3D,
                                              _myQuitCursorEvent.position3D);
                if (myHandTraveled > ENABLE_QUIT_OSD_DISTANCE) {
                    _myQuitCursorEvent = null;
                } else if (myTouchDuration > ENABLE_QUIT_OSD_TIME) {
                    if(_myQuitOSD.visible == false) {
                        _myQuitOSD.moveToTop();
                        _myQuitOSD.visible = true;
                        window.setTimeout("setOSDInvisible", 5.0*1000, Public);
                    }
                }
            } else {
                tryPressQuitCancelButton( theEventNode );
            }
            break;
        case "touch":
            break;
        case "remove":
            if (_myQuitCursorEvent && _myQuitCursorEvent.id == theEventNode.id) {
                _myQuitCursorEvent = null;
            }
            break;
        }

        Base.onASSEvent(theEventNode);
    }

    ////////////////////////////////////////
    // Protected
    ////////////////////////////////////////

    Base.setup = Protected.setup;
    Protected.setup = function() {
        Base.setup();

        buildQuitOSD();
    }

    ////////////////////////////////////////
    // Private
    ////////////////////////////////////////

    function buildQuitOSD() {
        var myFontPath = "FONTS/acswissmed.ttf";
        if (!fileExists(myFontPath)) {
            if (operatingSystem() == "WIN32") {
                myFontPath = "${SYSTEMROOT}/fonts/arial.ttf";
            } else {
                Logger.error("Could not find a font for rendering");
            }
        }

        const myStyle = {
            color:             asColor("FFFFFF"),
            selectedColor:     asColor("FFFFFF"),
            textColor:         asColor("00FFFF"),
            font:              myFontPath,
            HTextAlign:        Renderer.LEFT_ALIGNMENT,
            fontsize:          18
        }

        var myOSDSize = new Vector2i(300, 80);
        var myImageSrc = null;
        if (OSD_FILE != undefined) {
            myImageSrc = OSD_FILE;
        } else {
            myImageSrc = "shadertex/on_screen_display.rgb";
        }

        _myQuitOSD = new ImageOverlay(theViewer.getScene(), myImageSrc);
        var myImageSize = getImageSize(_myQuitOSD.image);
        _myQuitOSD.visible = false;

        var myColor = 0.3
        var myQuitText="";
        var myContinueText="";
        if (OSD_FILE == undefined) {
            _myQuitOSD.color = new Vector4f(myColor,myColor,myColor,0.75);
            myQuitText = "Quit";
            myContinueText="Continue";
            _myQuitOSD.width  = myOSDSize.x;
            _myQuitOSD.height = myOSDSize.y;
        } else {
            _myQuitOSD.width  = myImageSize.x;
            _myQuitOSD.height = myImageSize.y;
        }
        _myQuitOSD.position = new Vector2f((window.width - _myQuitOSD.width) / 2,
                                           (window.height - _myQuitOSD.height) / 2);

        var myButtonSize = new Vector2i((_myQuitOSD.width/2)-10,(_myQuitOSD.height/2)-10);
        var myButtonPos = new Vector2f(10,10);
        _myQuitConfirmButton = new TextButton(window.scene, "Confirm_Quit", myQuitText, myButtonSize, myButtonPos, myStyle, _myQuitOSD);
        _myQuitConfirmButton.onClick = function() {
            exit();
        }
        myStyle.HTextAlign = Renderer.RIGHT_ALIGNMENT,
        myButtonPos.x += myButtonSize.x;
        _myQuitCancelButton = new TextButton(window.scene, "Cancel_Quit", myContinueText, myButtonSize, myButtonPos, myStyle, _myQuitOSD);
        _myQuitCancelButton.onClick = function() {
            _myQuitOSD.visible = false;
        }
    }

    ////////////////////////////////////////
    // Setup
    ////////////////////////////////////////

    Protected.setup();
}
