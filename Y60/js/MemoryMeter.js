//=============================================================================
// Copyright (C) 2005 ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
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
            var myBackgroundColor = window.backgroundColor;
            var myBrightness = (myBackgroundColor[0] + myBackgroundColor[1] + myBackgroundColor[2]) / 3;
            var myTextColor = [1,1,1,1];
            if (myBrightness > 0.5) {
                myTextColor = [0,0,0,1];
            }
            window.setTextColor(myTextColor);
            window.renderText(new Vector2f(10, 10), asMemoryString(getProcessMemoryUsage()), "Screen8");

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
            window.renderText(new Vector2f(10, 20), asMemoryString(_myMaxMemoryUsage), "Screen8");

            var myMem = asMemoryString(getFreeMemory()) + "/" + asMemoryString(getTotalMemory());
            window.setTextColor(myTextColor);
            window.renderText(new Vector2f(10, 30), myMem, "Screen8");
        }
    }
}
