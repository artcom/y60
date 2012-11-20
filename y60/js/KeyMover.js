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
