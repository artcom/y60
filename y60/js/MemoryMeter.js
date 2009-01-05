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

function MemoryMeter(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

MemoryMeter.prototype.Constructor = function(self, theSceneViewer) {

    self.enabled                = false;

    var _mySceneViewer          = theSceneViewer;
    var _myMaxMemoryUsage       = 0;
    var _myMaxMemoryTime        = 0;
    var _myProcFunctionsPlugged = false;

    self.toggleEnableFlag = function() {
        if (!_myProcFunctionsPlugged) {
            plug("ProcFunctions");
            _myProcFunctionsPlugged = true;
        }
        self.enabled = !self.enabled;
    }


    self.onPostRender = function() {
        if (!_myProcFunctionsPlugged) {
            return;
        }
        var myUsedMemory = getProcessMemoryUsage();
        if (myUsedMemory > _myMaxMemoryUsage) {
            _myMaxMemoryTime  = _mySceneViewer.getCurrentTime();
            _myMaxMemoryUsage = myUsedMemory;
        }

        if (self.enabled) {
            var myBackgroundColor = window.canvas.backgroundcolor;
            var myBrightness = (myBackgroundColor[0] + myBackgroundColor[1] + myBackgroundColor[2]) / 3;
            var myTextColor = [1,1,1,1];
            if (myBrightness > 0.5) {
                myTextColor = [0,0,0,1];
            }
            var myViewport = theSceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
            
            window.setTextColor(myTextColor);
            window.renderText(new Vector2f(10, 10), asMemoryString(getProcessMemoryUsage()), "Screen8", myViewport);

            var myAge = _mySceneViewer.getCurrentTime() - _myMaxMemoryTime;
            var myRed   = 1;
            var myGreen = 1;
            const MAX_MEMORY_GREEN_TIME = 300;
            if (myAge < MAX_MEMORY_GREEN_TIME) {
                myGreen = myAge / MAX_MEMORY_GREEN_TIME;
            }
            if (myAge > MAX_MEMORY_GREEN_TIME) {
                myRed = 1 - (myAge - MAX_MEMORY_GREEN_TIME) / MAX_MEMORY_GREEN_TIME;
            }

            window.setTextColor([myRed,myGreen,0,1]);
            window.renderText(new Vector2f(10, 20), asMemoryString(_myMaxMemoryUsage), "Screen8", myViewport);

            var myMem = asMemoryString(getFreeMemory()) + "/" + asMemoryString(getTotalMemory());
            window.setTextColor(myTextColor);
            window.renderText(new Vector2f(10, 30), myMem, "Screen8", myViewport);
        }
    }
}