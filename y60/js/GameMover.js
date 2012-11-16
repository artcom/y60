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
//   $RCSfile: GameMover.js,v $
//   $Author: danielk $
//   $Revision: 1.3 $
//   $Date: 2005/03/17 18:22:41 $
//
//=============================================================================

use("MoverBase.js");

function GameMover() {
    this.Constructor(this);
}

GameMover.prototype.Constructor = function(obj) {
    MoverBase.prototype.Constructor(obj);
    obj.Mover = [];

    var PAN_SPEED  = 1;
    var ZOOM_SPEED = 1;

    //////////////////////////////////////////////////////////////////////

    var _myMousePosX = 0;
    var _myMousePosY = 0;

    //////////////////////////////////////////////////////////////////////
    //
    // public
    //
    //////////////////////////////////////////////////////////////////////

    obj.onMouseMotion = function(theX, theY) {

        var myDeltaX = (_myMousePosX - theX) / window.width;
        var myDeltaY = (_myMousePosY - theY) / window.height;
        var myWorldSize = obj.getWorldSize();

        var myWorldTranslation = null;
        if (obj.getRightButtonFlag()) {
            myWorldTranslation = new Vector3f(0, 0, myDeltaY * myWorldSize / ZOOM_SPEED);
            obj.update(myWorldTranslation, 0);
        } else if (obj.getMiddleButtonFlag()) {
            myWorldTranslation = new Vector3f(0, 0, 0);
            myWorldTranslation.x = - myDeltaX * myWorldSize / PAN_SPEED;
            myWorldTranslation.y = - myDeltaY * myWorldSize / PAN_SPEED;
            obj.update(myWorldTranslation, 0);
        } else if (obj.getLeftButtonFlag()) {
            myWorldTranslation = new Vector3f(0, 0, 0);
            var myWorldHeading = myDeltaX * TWO_PI;
            obj.getMoverObject().orientation.x += myDeltaY * TWO_PI;
            obj.update(myWorldTranslation, myWorldHeading);
        }

        _myMousePosX = theX;
        _myMousePosY = theY;
    };
};
