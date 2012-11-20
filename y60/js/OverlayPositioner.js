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
//   $RCSfile: OverlayPositioner.js,v $
//   $Author: danielk $
//   $Revision: 1.4 $
//   $Date: 2005/04/01 17:16:03 $
//
//=============================================================================

function OverlayPositioner(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

OverlayPositioner.prototype.Constructor = function(self, theSceneViewer) {
    const OVERLAY_POSITIONER_FILE = "OverlayPositions.xml";

    var _mySceneViewer   = theSceneViewer;
    var _myOverlays      = theSceneViewer.getActiveViewport().childNode("overlays");
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
            var myNewNode = _myOverlays.find(".//*[@name = '" + myConfigNode.name + "']");
            if (myNewNode) {
                myNewNode.width    = myConfigNode.width;
                myNewNode.height   = myConfigNode.height;
                myNewNode.position = myConfigNode.position;
                extendSizeByChildren(myNewNode);
            }
        }
    }

    function setup() {
        var myUnderlays = theSceneViewer.getActiveViewport().childNode("underlays");
        while (myUnderlays && myUnderlays.childNodesLength() > 0) {
            _myOverlays.appendChild(myUnderlays.firstChild);
        }

        if (fileExists(OVERLAY_POSITIONER_FILE)) {
            var myConfig = new Node();
            myConfig.parseFile(OVERLAY_POSITIONER_FILE);
            mergeConfig(myConfig.firstChild);
        } else {
            //_myOverlays.saveFile(OVERLAY_POSITIONER_FILE);
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
                if (myOverlay.visible && myOverlay.name != "insensible" && myOverlay.alpha > 0 &&
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
                case "space":
                    print("Position: " + _myPickedOverlay.position + ", width: " +
                        _myPickedOverlay.width + ", height: " + _myPickedOverlay.height +
                        " (" + _myPickedOverlay.name + ")");
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
        _mySceneViewer.setMessage("Overlay Positioner Help.");
        _mySceneViewer.setMessage("    Use the mouse, to pick overlays.");
        _mySceneViewer.setMessage("    Use cursor keys, to move the picked overlay around.");
        _mySceneViewer.setMessage("    Use alt + cursor keys, to change the picked overlay size.");
        _mySceneViewer.setMessage("    alt-s to save changes");
    }
}
