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
//   $RCSfile: SceneViewer.js,v $
//   $Author: valentin $
//   $Revision: 1.231 $
//   $Date: 2005/04/25 14:59:10 $
//
//=============================================================================

/*jslint nomen:false*/
/*globals plug, window, Vector2f, gl, getProcessMemoryUsage, asMemoryString,
          getFreeMemory, getTotalMemory*/

function MemoryMeter(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

MemoryMeter.prototype.Constructor = function (self, theSceneViewer) {

    self.enabled                = false;

    /////////////////////
    // Private Members //
    /////////////////////

    var _mySceneViewer          = theSceneViewer;
    var _myMaxMemoryUsage       = 0;
    var _myMaxMemoryTime        = 0;
    var _myProcFunctionsPlugged = false;
    var _myGLPlugged            = false;
    var _myGLMemExtensionAvail  = false;

    ////////////////////
    // Public Methods //
    ////////////////////

    self.toggleEnableFlag = function () {
        if (!_myProcFunctionsPlugged) {
            plug("ProcessFunctions");
            _myProcFunctionsPlugged = true;
        }
        if (!_myGLPlugged) {
            plug("GLBinding");
            _myGLPlugged = true;
            try {
                gl.GetFreeMem();
                _myGLMemExtensionAvail = true;
            } catch (theEx) {
                _myGLMemExtensionAvail = false;
            }
        }
        self.enabled = !self.enabled;
    };

    self.onPostRender = function () {
        if (!_myProcFunctionsPlugged) {
            return;
        }
        var myUsedMemory = getProcessMemoryUsage();
        if (myUsedMemory > _myMaxMemoryUsage) {
            _myMaxMemoryTime  = _mySceneViewer.getCurrentTime();
            _myMaxMemoryUsage = myUsedMemory;
        }

        if (self.enabled) {
            var myViewport = theSceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
            var myAge = _mySceneViewer.getCurrentTime() - _myMaxMemoryTime;
            var myRed   = 1;
            var myGreen = 1;
            var MAX_MEMORY_GREEN_TIME = 300;
            if (myAge < MAX_MEMORY_GREEN_TIME) {
                myGreen = myAge / MAX_MEMORY_GREEN_TIME;
            }
            if (myAge > MAX_MEMORY_GREEN_TIME) {
                myRed = 1 - (myAge - MAX_MEMORY_GREEN_TIME) / MAX_MEMORY_GREEN_TIME;
            }

            var myTextColor = new Vector4f(myRed, myGreen, 0, 1);
            var myStyle = new Node("<style textColor='" + myTextColor + "'/>");
            window.renderText(new Vector2f(10, 10), asMemoryString(getProcessMemoryUsage()), myStyle, "Screen8", myViewport);

            window.renderText(new Vector2f(10, 20), asMemoryString(_myMaxMemoryUsage), myStyle, "Screen8", myViewport);

            var myMem = asMemoryString(getFreeMemory()) + "/" + asMemoryString(getTotalMemory());
            window.renderText(new Vector2f(10, 30), myMem, myStyle, "Screen8", myViewport);
            if (_myGLMemExtensionAvail) {
                var myTotalGLMem = gl.GetTotalMem();
                var myGLMem = "GL: " + asMemoryString(myTotalGLMem - gl.GetFreeMem()) + "/" + asMemoryString(myTotalGLMem);
                window.renderText(new Vector2f(10, 40), myGLMem, myStyle, "Screen8", myViewport);
            }
        }
    };
};
