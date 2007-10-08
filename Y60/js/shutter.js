//=============================================================================
// Copyright (C) 2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//    $RCSfile: shutter.js,v $
//     $Author: ulrich $
//   $Revision: 1.15 $
//       $Date: 2005/04/07 14:47:52 $
//
//=============================================================================

use("AnimationManager.js");

function Shutter(theSceneViewer, theSize, theCustomShutterFile) {
    this.Constructor(this, theSceneViewer, theSize, theCustomShutterFile);
}

Shutter.prototype.Constructor = function(self, theSceneViewer, theSize, theCustomShutterFile) {
    const SHUTTER_OVERLAY = "shadertex/black.rgb";
    const CUSTOM_SHUTTER_OVERLAY = "shadertex/custom_shutter.png";

    const LEFT_SHUTTER    = 0;
    const RIGHT_SHUTTER   = 1;
    const TOP_SHUTTER     = 2;
    const BOTTOM_SHUTTER  = 3;
    const DEFAULT_SHUTTER_SIZE   = 0;

    var _myShutterXmlFileName = "";
    var _mySceneViewer           = theSceneViewer;
    var _myLeftOverlay           = null;
    var _myRightOverlay          = null;
    var _myTopOverlay            = null;
    var _myBottomOverlay         = null;
    var _myPixelImage            = null;
    var _myInteractiveOverlay    = null;
    var _myInteractivePixelImage = null;
    var _myInteractiveFlag       = false;
    var _myDrawFlag              = false;
    var _myEnabledFlag           = true;

    // cursor
    const CURSOR_IMAGE  = "shadertex/crosshair.png";
    const CURSOR_SIZE   = 10;
    var _myCursor       = null;

    var _myPosX         = 0;
    var _myPosY         = 0;
    var _myDeltaX       = 0;
    var _myDeltaY       = 0;

    var _myBrushSize    = 10;
    var _myDrawingAlpha = 255;
    var _myDrawingColor = new Vector4i(0,0,0,_myDrawingAlpha);

    if (theSize == undefined) {
        theSize = new Vector4f(DEFAULT_SHUTTER_SIZE, DEFAULT_SHUTTER_SIZE, DEFAULT_SHUTTER_SIZE,DEFAULT_SHUTTER_SIZE);
    }
    var _myShutterSize        = theSize;
    var _myDefaultShutterSize = theSize;

    // public functions
    self.setup = function(theShutterOverlayFile, theCustomShutterFile) {
        _myPixelImage = theSceneViewer.getImageManager().getImageNode("Shutter");
        _myPixelImage.src = theShutterOverlayFile;
        _myShutterXmlFileName = "shutter."+hostname()+".xml";
        if (fileExists(_myShutterXmlFileName)) {
            var myInteractivePixelImageDom = new Node();
            myInteractivePixelImageDom.parseFile(_myShutterXmlFileName);
            _myInteractivePixelImage = myInteractivePixelImageDom.firstChild;
            _myInteractivePixelImage.id = createUniqueId();
            theSceneViewer.getImages().appendChild(_myInteractivePixelImage);
            print("Found a "+_myShutterXmlFileName+" image, delete it to start over!");
        } else {
            _myInteractivePixelImage = theSceneViewer.getImageManager().getImageNode("InteractiveShutter");
            if (theCustomShutterFile != undefined) {
                _myInteractivePixelImage.src = theCustomShutterFile;
            } else {
                _myInteractivePixelImage.src = CUSTOM_SHUTTER_OVERLAY;
            }
        }

        _myLeftOverlay   = new ImageOverlay(theSceneViewer.getScene(), _myPixelImage);
        _myRightOverlay  = new ImageOverlay(theSceneViewer.getScene(), _myPixelImage);
        _myTopOverlay    = new ImageOverlay(theSceneViewer.getScene(), _myPixelImage);
        _myBottomOverlay = new ImageOverlay(theSceneViewer.getScene(), _myPixelImage);

        _myInteractiveOverlay   = new ImageOverlay(theSceneViewer.getScene(), _myInteractivePixelImage);

        // cursor
        var myCursorImage = theSceneViewer.getImageManager().getImageNode("ShutterCursor");
        myCursorImage.src = CURSOR_IMAGE;

        _myCursor = new ImageOverlay(theSceneViewer.getScene(), myCursorImage);
        _myCursor.width = CURSOR_SIZE;
        _myCursor.height = CURSOR_SIZE;
        _myCursor.visible = true;

        _myPosX = window.width / 2;
        _myPosY = window.height / 2;
        drawPixel(_myPosX, _myPosY);

        self.setBrush(_myBrushSize);

        self.enable(true);
        refresh();
    }

    self.onUpdateSettings = function(theNode) {
        if (theNode && theNode.childNode("shutter_size")) {
            var myShutterSize = stringToArray(theNode.childNode("shutter_size").firstChild.nodeValue);
            self.setScale(myShutterSize);
        }
    }

    self.setScale = function(theValue) {
        _myShutterSize = product(theValue, 0.01);
        refresh();
    }

    self.setBrush = function(theSize) {
        _myBrushSize = theSize;

        if (_myCursor) {
            _myCursor.width = theSize;
            _myCursor.height = theSize;
            _myCursor.position = new Vector2i(_myPosX - theSize / 2,
                                              _myPosY - theSize / 2);
        }
        drawPixel(_myPosX, _myPosY);
    }

    self.enable = function(theFlag) {
        _myEnabledFlag = theFlag;
        print("Shutter " + (_myEnabledFlag ? "on" : "off"));
        if (_myEnabledFlag) {
            _myLeftOverlay.visible   = true;
            _myRightOverlay.visible  = true;
            _myTopOverlay.visible    = true;
            _myBottomOverlay.visible = true;
            _myInteractiveOverlay.visible = true;
        } else {
            _myLeftOverlay.visible   = false;
            _myRightOverlay.visible  = false;
            _myTopOverlay.visible    = false;
            _myBottomOverlay.visible = false;
            _myInteractiveOverlay.visible = false;
        }
    }

    self.enabled = function() {
        return _myEnabledFlag;
    }

    self.reset = function() {
        _myShutterSize = _myDefaultShutterSize;
        refresh();
    }

    self.onResize = function() {
        refresh();
    }

    self.onFrame = function(theTime) {

        if (_myEnabledFlag && _myInteractiveFlag) {

            _myPosX += _myDeltaX;
            _myPosY += _myDeltaY;
            drawPixel(_myPosX, _myPosY);
        }
    }

    self.onMouseMotion = function(theX, theY) {

        /*if (_myEnabledFlag && _myInteractiveFlag) {
            _myPosX = theX;
            _myPosY = theY;
            drawPixel(theX, theY);
        }*/

        return _myInteractiveFlag;
    }

    self.onMouseButton = function(theButton, theState, theX, theY) {

        if (_myEnabledFlag == false || _myInteractiveFlag == false) {
            return false;
        }

        if (theState == BUTTON_DOWN) {
            if (theButton == LEFT_BUTTON) {
                _myDrawingColor = new Vector4i(0 ,0, 0, _myDrawingAlpha);
            } else if (theButton == RIGHT_BUTTON) {
                _myDrawingColor = new Vector4i(0 ,0, 0, 0);
            }
            _myDrawFlag = true;
            drawPixel(_myPosX, _myPosY);
        } else {
            _myDrawFlag = false;
        }

        return true;
    }

    self.onKey = function(theKey, theKeyState, theShiftFlag, theCtrlFlag, theAltFlag) {

        if (theAltFlag == false) {
            return false;
        }

        var myDelta = 1;
        if (theShiftFlag) {
            theKey = theKey.toUpperCase();
        }
        switch (theKey) {
            case 'LEFT':
                _myDrawingColor = new Vector4i(0 ,0, 0, 0);
                _myDeltaX = (theKeyState ? -myDelta : 0);
                break;
            case 'left':
                _myDrawingColor = new Vector4i(0 ,0, 0, _myDrawingAlpha);
                _myDeltaX = (theKeyState ? -myDelta : 0);
                break;
            case 'RIGHT':
                _myDrawingColor = new Vector4i(0 ,0, 0, 0);
                _myDeltaX = (theKeyState ? myDelta : 0);
                break;
            case 'right':
                _myDrawingColor = new Vector4i(0 ,0, 0, _myDrawingAlpha);
                _myDeltaX = (theKeyState ? myDelta : 0);
                break;
            case 'UP':
                _myDrawingColor = new Vector4i(0 ,0, 0, 0);
                _myDeltaY = (theKeyState ? -myDelta : 0);
                break;
            case 'up':
                _myDrawingColor = new Vector4i(0 ,0, 0, _myDrawingAlpha);
                _myDeltaY = (theKeyState ? -myDelta : 0);
                break;
            case 'DOWN':
                _myDrawingColor = new Vector4i(0 ,0, 0, 0);
                _myDeltaY = (theKeyState ? myDelta : 0);
                break;
            case 'down':
                _myDrawingColor = new Vector4i(0 ,0, 0, _myDrawingAlpha);
                _myDeltaY = (theKeyState ? myDelta : 0);
                break;
            case '-':
                if (theKeyState) {
                    self.setBrush(Math.max(_myBrushSize-0.25, 1));
                }
                break;
            case '+':
                if (theKeyState) {
                    self.setBrush(Math.min(_myBrushSize+0.25, 20));
                }
                break;
            case 's':
                // toggle shutter
                if (theKeyState) {
                    self.enable(!_myEnabledFlag);
                    refresh();
                }
                break;
            case 'd':
                // toggle interactive/free-form shutter
                if (theKeyState && _myEnabledFlag) {
                    _myInteractiveFlag = !_myInteractiveFlag;
                    print("Interactive Shutter " + (_myInteractiveFlag ? "enabled" : "disabled"));
                    drawPixel(_myPosX, _myPosY);
                    refresh();
                }
                break;
            case 'D':
                // toggle interactive drawing
                if (theKeyState && _myInteractiveFlag) {
                    _myDrawFlag = !_myDrawFlag;
                    print("Interactive drawing is: " + (_myDrawFlag ? "on" : "off"));
                    drawPixel(_myPosX, _myPosY);
                }
                break;
            case 'w':
                if (theKeyState) {
                    print("Saving shutter image");
                    _myInteractivePixelImage.saveFile(_myShutterXmlFileName);
                }
                break;
            case 'h':
                printHelp();
                break;
        }

        return true;
    }

    // private functions
    function printHelp() {
         print("Shutter Keys:");
         print("  ALT-s       toggle shutter");
         print("  ALT-d       toggle interactive shutter");
         print("  ALT-SHIFT-d toggles interactive shutter drawing");
         print("  ALT-w       save interactive shutter");
    }

    function drawPixel(theX, theY) {

        // XXX this should be two separate functions (position cursor, draw)
        var myHalfBrushSize = _myBrushSize / 2;
        _myCursor.position = new Vector2i(theX-myHalfBrushSize, theY-myHalfBrushSize);
        _myCursor.visible = _myInteractiveFlag;

        if (_myEnabledFlag == false || _myDrawFlag == false) {
            return;
        }
        print("drawPixel " + theX + "," + theY);

        var myRasterData = _myInteractivePixelImage.childNode(0).childNode(0).nodeValue;
        var myImageX = (theX / window.width)  * myRasterData.width;
        var myImageY = (theY / window.height) * myRasterData.height;

        var myMinX = Math.max(0, myImageX - myHalfBrushSize);
        var myMinY = Math.max(0, myImageY - myHalfBrushSize);
        var myMaxX = Math.min(myRasterData.width-1, myImageX + myHalfBrushSize);
        var myMaxY = Math.min(myRasterData.height-1, myImageY + myHalfBrushSize);

        myRasterData.fillRect(myMinX, myMinY,
                              myMaxX, myMaxY,
                              _myDrawingColor[0], _myDrawingColor[1], _myDrawingColor[2], _myDrawingColor[3]);

        /*
        print("x: " + myImageX)
        print("y: " + myImageY)
        for (var x = 0; x<_myBrushSize;x++) {
            for (var y = 0; y<_myBrushSize;y++) {
                myRasterData.setPixel((myImageX-_myBrushSize/2)+x,
                                      (myImageY-_myBrushSize/2)+y, 255 ,255,255,255);
                print(myRasterData.getPixel((theX-_myBrushSize/2)+x,
                                            (theY-_myBrushSize/2)+y));
            }
        }
        */
    }

    function refresh() {
        // XXX UH: what's all this -10 business? it effectively moves the shutter up-and-left... ?!?
        _myLeftOverlay.width  = window.width * _myShutterSize[LEFT_SHUTTER];
        _myLeftOverlay.height = window.height;
        //_myLeftOverlay.position = new Vector2i(-10,-10);
        _myLeftOverlay.position = new Vector2i(0,0);

        _myRightOverlay.width  = window.width * _myShutterSize[RIGHT_SHUTTER];
        _myRightOverlay.height = window.height;
        //_myRightOverlay.position = new Vector2i(window.width - _myRightOverlay.width, -10);
        _myRightOverlay.position = new Vector2i(window.width - _myRightOverlay.width, 0);

        _myTopOverlay.width  = window.width;
        _myTopOverlay.height = window.height * _myShutterSize[TOP_SHUTTER];
        //_myTopOverlay.position = new Vector2i(-10,-10);
        _myTopOverlay.position = new Vector2i(0,0);

        _myBottomOverlay.width  = window.width;
        _myBottomOverlay.height = window.height * _myShutterSize[BOTTOM_SHUTTER];
        //_myBottomOverlay.position = new Vector2i(-10, window.height - _myBottomOverlay.height);
        _myBottomOverlay.position = new Vector2i(0, window.height - _myBottomOverlay.height);

        _myInteractiveOverlay.width  = window.width;
        _myInteractiveOverlay.height  = window.height;
        //_myInteractiveOverlay.position = new Vector2i(-10,-10);
        _myInteractiveOverlay.position = new Vector2i(0,0);
    }

    self.setup(SHUTTER_OVERLAY, theCustomShutterFile);
}
