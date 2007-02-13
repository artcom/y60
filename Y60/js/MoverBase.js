//=============================================================================
// Copyright (C) 2003-2004, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

if ((operatingSystem() == "LINUX" || operatingSystem() == "OSX") && expandEnvironment("${Y60_POWERMATE_SUPPORT}") == 1) {
    plug("PowermateInputExtension");
}


var LEFT_BUTTON   = 1;
var MIDDLE_BUTTON = 3;
var RIGHT_BUTTON  = 2;

var BUTTON_UP     = 0;
var BUTTON_DOWN   = 1;


function MoverBase(theViewport) {
    this.Constructor(this, theViewport);
}

MoverBase.prototype.Constructor = function(obj, theViewport) {
    var _myViewport              = theViewport ? theViewport : window.canvas.childNode("viewport");
    var _myMoverObject           = null;
    var _myWorldSize             = null;

    var _myInitialPosition       = null;
    var _myInitialOrientation    = null;

    var _myLeftButtonFlag        = false;
    var _myMiddleButtonFlag      = false;
    var _myRightButtonFlag       = false;
    var _myDoubleLeftButtonFlag  = false;

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "Mover";

    obj.setMoverObject = function(theMoverObject) {
        if (theMoverObject == null) {
            return;
        }

        // store initial position for reset
        _myInitialPosition    = theMoverObject.position.value;
        _myInitialOrientation = new Quaternionf(theMoverObject.orientation);
        _myMoverObject = theMoverObject;
        obj.reset();
    }

    obj.setup = function() {
    }
    obj.stop = function() {}
    obj.setInitialPose = function() {
        if (_myMoverObject) {
            _myInitialPosition    = _myMoverObject.position.value;
            _myInitialOrientation = new Quaternionf(_myMoverObject.orientation);
        }
    }

    obj.getViewport = function() {
        return _myViewport;
    }

    obj.getViewportCamera = function() {
        return _myViewport.getElementById(_myViewport.camera);
    }

    obj.getWorld = function() {
        var myNodeInWorld = obj.getViewportCamera();
        while (myNodeInWorld.nodeName != "world") {
            myNodeInWorld = myNodeInWorld.parentNode;
        }
        return myNodeInWorld;
    }

    obj.getMoverObject = function() {
        return _myMoverObject;
    }

    obj.setWorldSize = function(theSize) {
        if (theSize) {
            _myWorldSize  = theSize;
        } else {
            _myWorldSize = 1.0;
        }
    }

    obj.getWorldSize = function() {
        return _myWorldSize;
    }

    obj.getLeftButtonFlag = function() {
        return _myLeftButtonFlag;
    }

    obj.getMiddleButtonFlag = function() {
        return _myMiddleButtonFlag;
    }

    obj.getRightButtonFlag = function() {
        return _myRightButtonFlag;
    }

    obj.getDoubleLeftButtonFlag = function() {
        return _myDoubleLeftButtonFlag;
    }

    obj.rotateWithObject = function(theVector) {
        var myWorldTranslation = new Vector4f(0,0,0,0);

        var myGlobalMatrix = _myMoverObject.globalmatrix;
        myWorldTranslation.add(product(myGlobalMatrix.getRow(0), theVector.x));
        myWorldTranslation.add(product(myGlobalMatrix.getRow(1), theVector.y));
        myWorldTranslation.add(product(myGlobalMatrix.getRow(2), theVector.z));
        return myWorldTranslation;
    }

    obj.update = function(theScreenTranslation, theWorldHeading) {
        var myGlobalMatrix = new Matrix4f(_myMoverObject.globalmatrix);
        var myWorldPosition  = myGlobalMatrix.getRow(3);

        // Move the camera
        var myWorldTranslation = obj.rotateWithObject(theScreenTranslation);
        myWorldPosition.add(myWorldTranslation);

        // Rotate about the world up vector
        myGlobalMatrix.rotateY(theWorldHeading);
        myGlobalMatrix.setRow(3, myWorldPosition);

        var myParentMatrix = new Matrix4f(_myMoverObject.parentNode.globalmatrix);
        myParentMatrix.invert();
        myGlobalMatrix.postMultiply(myParentMatrix);

        var myDecomposition = myGlobalMatrix.decompose();
        _myMoverObject.orientation = myDecomposition.orientation;
        _myMoverObject.position    = myGlobalMatrix.getTranslation();
    }

    //////////////////////////////////////////////////////////////////////

    obj.reset = function() {        
        _myMoverObject.position    = _myInitialPosition;
        _myMoverObject.orientation = _myInitialOrientation;
        obj.setup();        
    }

    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theKeyState) {
            if (theShiftFlag) {
                theKey = theKey.toUpperCase();
            }
            switch (theKey) {
                case 'r':
                    obj.reset();
                    break;
                case "h":
                    printHelp();
                    break;
            }
        }
    }

    obj.onFrame = function(theTime) { // 'pure virtual'
    }
    
    var _myLastButtonTime = 0;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
        switch (theButton) {
            case LEFT_BUTTON:
                _myLeftButtonFlag = (theState == BUTTON_DOWN);
                if (_myLeftButtonFlag) {
                    _myDoubleLeftButtonFlag = !_myDoubleLeftButtonFlag && ((millisec() - _myLastButtonTime) < 500);
                    _myLastButtonTime = millisec();
                }
                break;
            case MIDDLE_BUTTON:
                _myMiddleButtonFlag = (theState == BUTTON_DOWN);
                break;
            case RIGHT_BUTTON:
                _myRightButtonFlag = (theState == BUTTON_DOWN);
                break;
            default:
                break;
        }
    }

    obj.onMouseMotion = function(theX, theY) { // 'pure virtual'
    }

    obj.onMouseWheel = function(theDeltaX, theDeltaY) {
    }

    obj.onAxis = function( theDevice, theAxis, theValue) {
    }
    
    obj.getNormalizedScreen = function(theMousePosX, theMousePosY) {
        return new Vector3f((theMousePosX * 2 -_myViewport.width)/_myViewport.width,
            -(theMousePosY * 2 -_myViewport.height)/_myViewport.height, 0);
    }

    function printHelp() {
        print("Mover keys:");
        print("    r    reset mover");
    }
}
