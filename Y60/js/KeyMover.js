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
//   $RCSfile: KeyMover.js,v $
//   $Author: christian $
//   $Revision: 1.4 $
//   $Date: 2005/02/16 10:24:43 $
//
//=============================================================================


use("MoverBase.js");

function KeyMover() {
    this.Constructor(this);
}

KeyMover.prototype.Constructor = function(obj) {
    MoverBase.prototype.Constructor(obj);
    obj.Mover = [];

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.name = "KeyMover";

    function printHelp() {
         print("Key mover Keys:");
         print("  CTRL-ALT-x decrease x position");
         print("  CTRL-ALT-X increase x position");
         print("  CTRL-ALT-y decrease y position");
         print("  CTRL-ALT-Y increase y position");
         print("  CTRL-ALT-z decrease z position");
         print("  CTRL-ALT-Z increase Z position");
         print("  CTRL-ALT-j decrease x rotation");
         print("  CTRL-ALT-J increase x rotation");
         print("  CTRL-ALT-k decrease y rotation");
         print("  CTRL-ALT-K increase y rotation");
         print("  CTRL-ALT-l decrease z rotation");
         print("  CTRL-ALT-L increase Z rotation");
    }

    obj.Mover.onMouseButton = obj.onMouseButton;
    obj.onMouseButton = function(theButton, theState, theX, theY) {
    }

    obj.onMouseMotion = function(theX, theY) {
    }

    const _myRotationStep = Math.PI / (180 * 4); // 0.25 Grad
    const _myStepFactor = 1000;
    var _myStepFactorVector = new Vector3f(1/_myStepFactor,1/_myStepFactor,1/_myStepFactor);
    var _myStep = new Vector3f(1,1,1);

    obj.Mover.setWorldSize = obj.setWorldSize;
        obj.setWorldSize = function(theSize) {
        obj.Mover.setWorldSize(theSize);
        //print("theSize" + theSize);
        _myStep  = product(_myStepFactorVector, theSize);
        //print("_myStep" + _myStep);
    }

    obj.Mover.onKey = obj.onKey;
    obj.onKey = function(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag) {
        if (theKeyState) {
            switch (theKey) {
               case 'h':
                  printHelp();
                  break;
            }
        }
        if (theKeyState && theAltFlag) {
            var myMoverObject = obj.getMoverObject();

            if (theShiftFlag) {
                var sign = 1;
            } else {
                var sign = -1;
            }
            theKey = theKey.toLowerCase();
            switch (theKey) {
            case 'x':
                myMoverObject.position.x = myMoverObject.position.x + _myStep.x * sign;
                break;
            case 'y':
                myMoverObject.position.y = myMoverObject.position.y + _myStep.y * sign;
                break;
            case 'z':
                myMoverObject.position.z = myMoverObject.position.z + _myStep.z * sign;
                break;
            case 'j':
                var myDeltaRotation = new Quaternionf(new Vector3f(1,0,0), _myRotationStep * sign*10);
                myMoverObject.orientation.multiply(myDeltaRotation);
                break;
            case 'k':
                var myDeltaRotation = new Quaternionf(new Vector3f(0,1,0), _myRotationStep * sign*10);
                myMoverObject.orientation.multiply(myDeltaRotation);
                break;
            case 'l':
                var myDeltaRotation = new Quaternionf(new Vector3f(0,0,1), _myRotationStep * sign*10);
                myMoverObject.orientation.multiply(myDeltaRotation);
                break;
            }
        } else {
            obj.Mover.onKey(theKey, theKeyState, theX, theY, theShiftFlag, theCtrlFlag, theAltFlag);
        }
    }

}
