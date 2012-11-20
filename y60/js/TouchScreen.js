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
//   $RCSfile: TouchScreen.js,v $
//   $Author: danielk $
//   $Revision: 1.6 $
//   $Date: 2005/03/17 18:22:41 $
//
//
//=============================================================================

const CONFIG_FILE          = "TouchScreenConfig.xml";
const CROSSHAIR_TEXTURE    = "shadertex/crosshair.png";

function TouchScreen(theEventReceiver, theComPort, thePortraitMode) {
    this.Constructor(this, theEventReceiver, theComPort, thePortraitMode);
}

TouchScreen.prototype.Constructor = function(obj, theEventReceiver, theComPort, thePortraitMode) {

    ///////////////////////////////////////////////////////////////////////////////////////////
    //
    // Constructor code
    //
    ///////////////////////////////////////////////////////////////////////////////////////////

    const BUTTON_UP    = 0;
    const BUTTON_DOWN  = 1;

    const TOP_LEFT     = 0;
    const TOP_RIGHT    = 1;
    const BOTTOM_LEFT  = 2;
    const BOTTOM_RIGHT = 3;

    const MAX_CALIBRATION_ERROR = 0.05;

    var _myPortraitMode        = thePortraitMode;
    if (_myPortraitMode == undefined) {
        _myPortraitMode = false;
    }

    var _myEventReceiver       = theEventReceiver;
    var _myComPort             = theComPort;

    var _myCalibrationModeFlag = false;
    var _myCalibrationStep     = 0;
    var _myCalibration         = [];
    var _myFollowMode          = false;

    var _myCrossHair           = null;
    var _myBackground          = null;

    var _mySerialPort          = null;
    var _myConfig              = null;
    var _myConfigDocument      = null;
    var _myButtonState         = BUTTON_UP;
    var _myLastX               = 0;
    var _myLastY               = 0;
    var _myLastRawX            = 0;
    var _myLastRawY            = 0;

    var _myMessageTop          = "";
    var _myMessageBottom       = "";

    function setup() {
        _myConfigDocument = Node.createDocument();
        if (fileExists(CONFIG_FILE)) {
            _myConfigDocument.parseFile(CONFIG_FILE);
        } else {
            _myConfigDocument.parse('<touchscreen><calibration minx="0" miny="0" maxx="0" maxy="0"/></touchscreen>');
        }
        _myConfig = _myConfigDocument.firstChild.firstChild;

        _mySerialPort = new SerialDevice(_myComPort);
        _mySerialPort.open();
        //_mySerialPort.setNoisy(true);

        sendControlPacket("R",   "Resetting controller");
        sendControlPacket("FT",  "Set to table-format");
        sendControlPacket("MS",  "Set to stream-mode");
        sendControlPacket("OI",  "Identify device");

        //var myWhiteImage = _myEventReceiver.getImageManager().getImageNode("TouchScreenBackgroundImage");
        //myWhiteImage.src = "shadertex/one_white_pixel.png";
        //myWhiteImage.resize = "scale";

        //_myBackground = new ImageOverlay(_myEventReceiver.getScene(), myWhiteImage);
        _myBackground = new ImageOverlay(_myEventReceiver.getScene(), "shadertex/one_white_pixel.png");
        var myWhiteImage = _myBackground.image;
        myWhiteImage.resize = "scale";
        myWhiteImage.name = "TouchScreenBackgroundImage";
        _myBackground.position = new Vector2f(0, 0);
        _myBackground.width  = window.width;
        _myBackground.height = window.height;
        _myBackground.visible = false;

        //var myCrossHairImage = _myEventReceiver.getImageManager().getImageNode("TouchScreenCrossHair");
        //myCrossHairImage.src = CROSSHAIR_TEXTURE;

        //_myCrossHair = new ImageOverlay(_myEventReceiver.getScene(), myCrossHairImage);
        _myCrossHair = new ImageOverlay(_myEventReceiver.getScene(), CROSSHAIR_TEXTURE);
        _myCrossHair.image.name = "TouchScreenCrossHair";
        _myCrossHair.position = new Vector2f(0, 0);
        _myCrossHair.width  = 128;
        _myCrossHair.height = 128;
        _myCrossHair.visible = false;

        setupCalibration();
    }

    function sendControlPacket(thePacket, theComment) {
        _mySerialPort.setPacketFormat(0x01, 0x0d, thePacket.length, SerialDevice.NO_CHECKING);

        _mySerialPort.sendPacket(thePacket);
        msleep(500);

        var myData = _mySerialPort.read();
        print(theComment + " - Return code: " + myData);
    }

    function setupCalibration() {
        _myCalibration[TOP_LEFT]     = { x: 0, y: 0, crossX: 0, crossY: 0};
        _myCalibration[TOP_RIGHT]    = { x: 0, y: 0, crossX: window.width - _myCrossHair.width, crossY: 0};
        _myCalibration[BOTTOM_LEFT]  = { x: 0, y: 0, crossX: 0, crossY: window.height - _myCrossHair.height};
        _myCalibration[BOTTOM_RIGHT] = { x: 0, y: 0, crossX: window.width - _myCrossHair.width, crossY: window.height - _myCrossHair.height};
    }

    obj.setFollowMode = function(theFlag) {
        obj.stopCalibration();

        _myCrossHair.visible  = theFlag;
        _myBackground.visible = theFlag;
        _myFollowMode         = theFlag;

        if (theFlag) {
            _myBackground.moveToTop();
            _myCrossHair.moveToTop();
            _myMessageTop = "FOLLOW MODE: Drag the cross-hair to test touchscreen calibration";
        } else {
            _myMessageTop = "";
        }
    }

    obj.getFollowMode = function() {
        return _myFollowMode;
    }

    obj.startCalibration = function() {
        _myBackground.moveToTop();
        _myCrossHair.moveToTop();

        _myCrossHair.visible   = true;
        _myBackground.visible  = true;
        _myCalibrationModeFlag = true;
        _myCrossHair.position  = new Vector2f(0,0);
        _myCalibrationStep     = 0;
        _myFollowMode          = false;

        _myMessageTop          = "CALIBRATION MODE: Please push the cross hair";
        _myMessageBottom       = "";
    }

    obj.stopCalibration = function() {
        _myCrossHair.visible   = false;
        _myBackground.visible  = false;
        _myCalibrationModeFlag = false;
        _myCrossHair.position  = new Vector2f(0,0);
        _myCalibrationStep     = 0;
        _myMessageTop = "";
    }

    obj.isCalibrating = function() {
        return _myCalibrationModeFlag;
    }

    function parseCoordinates(theCoords) {
        var myBytes =[];
        for (var i = 0; i < theCoords.length; ++i) {
            myBytes[i] = theCoords.charCodeAt(i);
        }

        var myX = myBytes[1] << 7;
        myX = myX | (myBytes[0] & 0x00ff);

        var myY = myBytes[3] << 7;
        myY = myY | (myBytes[2] & 0x00ff);

        _myLastRawX = myX;
        _myLastRawY = myY;

        // Add calibration
        var myWindowWidth  = window.width;
        var myWindowHeight = window.height;
        if (_myPortraitMode) {
            myWindowWidth  = window.height;
            myWindowHeight = window.width;
        }

        myX -= _myConfig.minx;
        myY -= _myConfig.miny;
        myX *= (myWindowWidth  - 128) / (_myConfig.maxx - _myConfig.minx);
        myY *= (myWindowHeight - 128) / (_myConfig.maxy - _myConfig.miny);
        myX += 64;
        myY += 64;

        if (_myPortraitMode) {
            var myTempX = myX;
            myX = myY;
            myY = myWindowWidth - myTempX;
        }

        myX = Math.round(myX);
        myY = Math.round(myY);

        if (myX != _myLastX || myY != _myLastY) {
            if (_myFollowMode) {
                _myCrossHair.position.x = myX - 64;
                _myCrossHair.position.y = myY - 64;
            } else {
                if (!_myCalibrationModeFlag) {
                    _myEventReceiver.onMouseMotion(myX, myY);
                }
            }
        }

        _myLastX = myX;
        _myLastY = myY;
    }

    function setButtonState(theState) {
        if (theState != _myButtonState) {
            _myButtonState = theState;
            var myState = 0;
            if (theState == BUTTON_DOWN) {
                myState = 1;
            }

            if (_myCalibrationModeFlag) {
                if (theState == BUTTON_UP) {
                    calibrationButton(_myLastRawX, _myLastRawY);
                }
            } else {
                if (!_myFollowMode) {
                    _myEventReceiver.onMouseButton(0, myState, _myLastX, _myLastY);
                }
            }
        }
    }

    function getCalibrationErrors() {
        var myWidth  = Math.max(Math.abs(_myCalibration[TOP_LEFT].x - _myCalibration[TOP_RIGHT].x),
                Math.abs(_myCalibration[BOTTOM_LEFT].x - _myCalibration[BOTTOM_RIGHT].x));

        var myHeight = Math.max(Math.abs(_myCalibration[TOP_LEFT].y - _myCalibration[BOTTOM_LEFT].y),
                Math.abs(_myCalibration[TOP_RIGHT].y - _myCalibration[BOTTOM_RIGHT].y));
        var myErrors = [];
        myErrors.push(Math.abs(_myCalibration[TOP_LEFT].x - _myCalibration[BOTTOM_LEFT].x) / myWidth);
        myErrors.push(Math.abs(_myCalibration[TOP_RIGHT].x - _myCalibration[BOTTOM_RIGHT].x) / myHeight);
        myErrors.push(Math.abs(_myCalibration[TOP_LEFT].y - _myCalibration[TOP_RIGHT].y) / myHeight);
        myErrors.push(Math.abs(_myCalibration[BOTTOM_LEFT].y - _myCalibration[BOTTOM_RIGHT].y) / myHeight);

        return myErrors;
    }

    function checkCalibration() {
        var myMessage = null;
        var myErrors = getCalibrationErrors();

        if (myErrors[0] > MAX_CALIBRATION_ERROR) {
            myMessage = "Difference between top-left x and bottom-left x to big: " + (myErrors[0] * 100).toFixed(0) + "%";
        } else if (myErrors[1] > MAX_CALIBRATION_ERROR) {
            myMessage = "Difference between top-right x and bottom-right x  to big: " + (myErrors[1] * 100).toFixed(0) + "%";
        } else if (myErrors[2] > MAX_CALIBRATION_ERROR) {
            myMessage = "Difference between top-left y and top-right y to big: " + (myErrors[2] * 100).toFixed(0) + "%";
        } else if (myErrors[3] > MAX_CALIBRATION_ERROR) {
            myMessage = "Difference between bottom-left y and bottom-right y to big: " + (myErrors[3] * 100).toFixed(0) + "%";
        }

        if (myMessage) {
            _myMessageBottom = "### Error: " + myMessage + ". Please restart";
            return false;
        } else {
            _myMessageBottom = "";
        }

        return true;
    }

    function calibrationButton(theX, theY) {
        _myCalibration[_myCalibrationStep].x = theX;
        _myCalibration[_myCalibrationStep].y = theY;
        _myCalibrationStep++;

        _myMessageBottom = "";

        if (_myCalibrationStep >= _myCalibration.length) {
            _myCalibrationStep = 0;
            if (checkCalibration()) {
                var myErrors = getCalibrationErrors();
                _myMessageBottom = "Calibration done! " + myErrors[0].toFixed(2) +
                    " " + myErrors[1].toFixed(2) + " " + myErrors[2].toFixed(2) + " "
                    + myErrors[3].toFixed(2);

                _myConfig.minx = Math.round(_myCalibration[TOP_LEFT].x + _myCalibration[BOTTOM_LEFT].x) / 2;
                _myConfig.miny = Math.round(_myCalibration[TOP_LEFT].y + _myCalibration[TOP_RIGHT].y) / 2;
                _myConfig.maxx = Math.round(_myCalibration[TOP_RIGHT].x + _myCalibration[BOTTOM_RIGHT].x) / 2;
                _myConfig.maxy = Math.round(_myCalibration[BOTTOM_LEFT].y + _myCalibration[BOTTOM_RIGHT].y) / 2;

                _myConfigDocument.saveFile(CONFIG_FILE);

                obj.setFollowMode(true);
            }
        }

        _myCrossHair.position.x = _myCalibration[_myCalibrationStep].crossX;
        _myCrossHair.position.y = _myCalibration[_myCalibrationStep].crossY;
    }

    obj.onFrame = function(theTime) {
        while (_mySerialPort.peek() > 4) {
            var myStatusByte = _mySerialPort.read(1).charCodeAt(0);
            if (myStatusByte & 0x80) {
                var myCoords = _mySerialPort.read(4);
                if (myCoords.length == 4) {
                     parseCoordinates(myCoords);
                }
                if (myStatusByte & 0x40) {
                    setButtonState(BUTTON_DOWN);
                } else {
                    setButtonState(BUTTON_UP);
                }
            }
        }

        if (_myCalibrationModeFlag || _myFollowMode) {
            var myTextColor = new Vector4f(0.4,0.4,1,1);                                        
            window.renderText([50, 50], _myMessageTop, new Node("<style textColor='" + myTextColor + "'/>"), "SyntaxBold18");
            window.renderText([50, 100], _myMessageBottom, new Node("<style textColor='" + myTextColor + "'/>"), "SyntaxBold18");
            return true;
        } else {
            return false;
        }

    }

    setup();
}



