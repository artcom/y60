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
//   $RCSfile: OverlayPositioner.js,v $
//   $Author: danielk $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 17:16:03 $
//
//=============================================================================

const OVERLAY_POSITIONER_FILE = "OverlayPositions.xml";

function OverlayPositioner(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

OverlayPositioner.prototype.Constructor = function(self, theSceneViewer) {

    var _mySceneViewer   = theSceneViewer;
    var _myOverlays      = theSceneViewer.getViewports().firstChild.firstChild;
    var _myPickedOverlay = null;
    var _myMouseStart    = null;
    var _myShiftFlag     = false;

    self.enabled = false;

    this.enable = function(theFlag) {
        if (!theFlag && _myPickedOverlay) {
            _myPickedOverlay.leftborder   = 0;
            _myPickedOverlay.rightborder  = 0;
            _myPickedOverlay.bottomborder = 0;
            _myPickedOverlay.topborder    = 0;
        }
        self.enabled = theFlag;
    }

    function pickOverlay(theOverlay) {

        if (_myPickedOverlay) {
            var myParent = _myPickedOverlay.parentNode;
            if (myParent.nodeName == "overlay") {
                myParent.leftborder   = 0;
                myParent.rightborder  = 0;
                myParent.bottomborder = 0;
                myParent.topborder    = 0;
            }
            _myPickedOverlay.leftborder   = 0;
            _myPickedOverlay.rightborder  = 0;
            _myPickedOverlay.bottomborder = 0;
            _myPickedOverlay.topborder    = 0;
        }
        if (theOverlay) {
            var myParent = theOverlay.parentNode;
            if (myParent.nodeName == "overlay") {
                myParent.bordercolor = new Vector4f(0.5,0.5,0.5,1);
                myParent.leftborder   = 1;
                myParent.rightborder  = 1;
                myParent.bottomborder = 1;
                myParent.topborder    = 1;
                extendSizeByChildren(myParent);
            }
            theOverlay.bordercolor = new Vector4f(1,0,0,1);
            theOverlay.leftborder   = 1;
            theOverlay.rightborder  = 1;
            theOverlay.bottomborder = 1;
            theOverlay.topborder    = 1;
        }
        _myPickedOverlay = theOverlay;
    }

    function extendSizeByChildren(theNode) {
        for (var i = 0; i < theNode.childNodes.length; ++i) {
            var myChild = theNode.childNode(i);
            extendSizeByChildren(myChild);
            theNode.width  = Math.max(theNode.width, myChild.position.x + myChild.width);
            theNode.height = Math.max(theNode.height, myChild.position.y + myChild.height);
        }
    }

    function mergeConfig(theConfig) {
        for (var i = 0; i < theConfig.childNodes.length; ++i) {
            var myConfigNode = theConfig.childNode(i);
            mergeConfig(myConfigNode);
            var myNewNode = getDescendantByName(_myOverlays, myConfigNode.name, true);
            if (myNewNode) {
                myNewNode.width    = myConfigNode.width;
                myNewNode.height   = myConfigNode.height;
                myNewNode.position = myConfigNode.position;
                extendSizeByChildren(myNewNode);
            }
        }
    }

    function setup() {
        if (fileExists(OVERLAY_POSITIONER_FILE)) {
            var myConfig = new Node();
            myConfig.parseFile(OVERLAY_POSITIONER_FILE);
            mergeConfig(myConfig.firstChild);
        } else {
            _myOverlays.saveFile(OVERLAY_POSITIONER_FILE);
        }
    }

    self.onMouseMotion = function(theX, theY) {
        if (self.enabled && _myMouseStart && _myPickedOverlay) {
            var myMouseX = theX - _myMouseStart[0];
            var myMouseY = theY - _myMouseStart[1];

            if (_myShiftFlag) {
                _myPickedOverlay.width  += myMouseX;
                _myPickedOverlay.height += myMouseY;
            } else {
                _myPickedOverlay.position.x += myMouseX;
                _myPickedOverlay.position.y += myMouseY;
            }

            _myMouseStart = [theX, theY];
        }
    }

    function getPickedOverlay(theParent, theX, theY) {
        for (var i = theParent.childNodes.length - 1; i >= 0; --i) {
            var myOverlay = theParent.childNode(i);
            var myResult = getPickedOverlay(myOverlay, theX - myOverlay.position.x, theY - myOverlay.position.y);
            if (myResult) {
                return myResult;
            } else {
                if (myOverlay.visible && myOverlay.alpha > 0 &&
                    theX > myOverlay.position.x && theX <= myOverlay.position.x + myOverlay.width &&
                    theY > myOverlay.position.y && theY <= myOverlay.position.y + myOverlay.height)
                {
                    return myOverlay;
                }
            }
        }

        return null;
    }

    self.onMouseButton = function(theButton, theState, theX, theY) {
        _myMouseStart = null;
        if (self.enabled && theState) {
            var myOverlay = getPickedOverlay(_myOverlays, theX, theY);
            pickOverlay(myOverlay);
            if (myOverlay) {
                _myMouseStart = [theX, theY];
            }
        }
    }

    self.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theControlFlag, theAltFlag) {
        if (theKeyState && theKey == "h") {
            printHelp();
        }

        if (theKey == "left shift" || theKey == "right shift") {
            _myShiftFlag = theKeyState;
        }

        if (self.enabled && theKeyState && _myPickedOverlay) {
            var myStepSize = 1;
            if (theShiftFlag) {
                myStepSize = 10;
            }

            switch (theKey) {
                case "left":
                    if (theAltFlag) {
                        _myPickedOverlay.width -= myStepSize;
                    } else {
                        _myPickedOverlay.position.x -= myStepSize;
                    }
                    break;
                case "right":
                    if (theAltFlag) {
                        _myPickedOverlay.width += myStepSize;
                    } else {
                        _myPickedOverlay.position.x += myStepSize;
                    }
                    break;
                case "up":
                    if (theAltFlag) {
                        _myPickedOverlay.height -= myStepSize;
                    } else {
                        _myPickedOverlay.position.y -= myStepSize;
                    }
                    break;
                case "down":
                    if (theAltFlag) {
                        _myPickedOverlay.height += myStepSize;
                    } else {
                        _myPickedOverlay.position.y += myStepSize;
                    }
                    break;
                case "s":
                    if (theAltFlag) {
                        _myOverlays.saveFile(OVERLAY_POSITIONER_FILE);
                        _mySceneViewer.setMessage("Calibration saved.");
                    }
                    break;
            }
        }
    }

    setup();

    //////////////////////////////////////////////////////////////////////
    //
    // private
    //
    //////////////////////////////////////////////////////////////////////

    function printHelp() {
        _mySceneViewer.setMessage("F1 toggles calibration mode.");
        _mySceneViewer.setMessage("    Use the mouse, to pick overlays.");
        _mySceneViewer.setMessage("    Use cursor keys, to move the picked overlay around.");
        _mySceneViewer.setMessage("    Use alt + cursor keys, to change the picked overlay size.");
        _mySceneViewer.setMessage("    alt-s to save changes");
    }
}
