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
//
// Description: TODO
//
// Last Review: NEVER, NOONE
//
//  review status report: (perfect, ok, fair, poor, disaster, notapplicable, unknown)
//    usefullness            : unknown
//    formatting             : unknown
//    documentation          : unknown
//    test coverage          : unknown
//    names                  : unknown
//    style guide conformance: unknown
//    technical soundness    : unknown
//    dead code              : unknown
//    readability            : unknown
//    understandabilty       : unknown
//    interfaces             : unknown
//    confidence             : unknown
//    integration            : unknown
//    dependencies           : unknown
//    cheesyness             : unknown
//
//    overall review status  : unknown
//
//    recommendations:
//       - unknown
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/
//
//   $RCSfile: HeartbeatThrober.js,v $
//   $Author: christian $
//   $Revision: 1.3 $
//   $Date: 2004/11/10 18:37:33 $
//
//=============================================================================

/*jslint nomen:false*/
/*globals Node*/

// XXX A **throbber** is a graphic found in a graphical user interface of a
// computer program (especially a web browser) that animates to show the
// user that the program is performing an action (such as downloading a web page).
// spelling: throber -> throbber

function HeartbeatThrober(theEnableFlag, theFrequency, theFilename) {
    var _myFrequency      = theFrequency; // in seconds
    var _myLastThrobTime  = -1;
    var _IsEnabled        = theEnableFlag;
    var _myFilename       = theFilename;
    var _myFirstThrob     = true;
    this.throb = function (theTime) {
        if (_myLastThrobTime === -1) {
            _myLastThrobTime = theTime;
            _myFirstThrob = true;
        }
        if (_IsEnabled) {
            var myDeltaTime = theTime - _myLastThrobTime;
            if (myDeltaTime > _myFrequency || _myFirstThrob) {
                _myFirstThrob = false;
                // we want seconds but Date returns milliseconds
                var mySecondsSince1970 = Date.parse(Date()) / 1000;
                var myHeartbeatString =
                    '<heartbeat secondsSince1970="' + mySecondsSince1970 + '" >\n' +
                    '</heartbeat>';
                var myHeartbeatDoc = new Node(myHeartbeatString);
                try {
                    myHeartbeatDoc.saveFile(_myFilename);
                } catch (ex) {
                    Logger.error("caught exception during saveFile: " + ex);
                }
                _myLastThrobTime = theTime;
            }
        }
    };
    
    this.use = function (theFlag, theFrequency, theHeartbeatfile) {
        _IsEnabled       = theFlag;
        _myFrequency     = theFrequency;
        _myFilename      = theHeartbeatfile;
        _myLastThrobTime = -1;
    };

    this.enable = function (theFlag) {
        _IsEnabled = theFlag;
        _myFirstThrob = true;
    };

}
