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

GameMover.prototype.Constructor = function(obj)
{
    MoverBase.prototype.Constructor(obj);
    obj.Mover = [];

    const PAN_SPEED  = 1;
    const ZOOM_SPEED = 1;

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

        if (obj.getRightButtonFlag()) {
            var myWorldTranslation = new Vector3f(0, 0, myDeltaY * myWorldSize / ZOOM_SPEED);
            obj.update(myWorldTranslation, 0);
        } else if (obj.getMiddleButtonFlag()) {
            var myWorldTranslation = new Vector3f(0, 0, 0);
            myWorldTranslation.x = - myDeltaX * myWorldSize / PAN_SPEED;
            myWorldTranslation.y = - myDeltaY * myWorldSize / PAN_SPEED;
            obj.update(myWorldTranslation, 0);
        } else if (obj.getLeftButtonFlag()) {
            var myWorldTranslation = new Vector3f(0, 0, 0);
            var myWorldHeading = myDeltaX * TWO_PI;
            obj.getMoverObject().orientation.x += myDeltaY * TWO_PI;
            obj.update(myWorldTranslation, myWorldHeading);
        }

        _myMousePosX = theX;
        _myMousePosY = theY;
    }
}
