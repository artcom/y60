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
//    readability            : poor
//    understandabilty       : poor
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

/*jslint nomen:false plusplus:false*/
/*globals window, Modelling, Vector2f, sum, Vector4f, ImageOverlay, Logger,
          print*/

function PerfMeter(theSceneViewer) {
    this.Constructor(this, theSceneViewer);
}

PerfMeter.prototype.Constructor = function (self, theSceneViewer) {
    /////////////////////
    // Private Members //
    /////////////////////

    //var LINE_COUNT        = 5;
    var BOX_WIDTH  = 600;
    var BOX_HEIGHT = 200;
    var SLOT_WIDTH = 2;
    var H_VALUES   = (BOX_WIDTH - 2 * SLOT_WIDTH) / SLOT_WIDTH;
    var T_MAX      = 0.1; // 500 ms

    var FILTER_WINDOW = 20;
    var FILTER        = 1 / FILTER_WINDOW;

    var _mySceneViewer          = theSceneViewer;
    //var _myMaxMemoryUsage       = 0;
    //var _myMaxMemoryTime        = 0;
    var _myBaseLine             = 0;
    //var _myProcFunctionsPlugged = false;
    var _myOverlay = null;
    var _myImage   = null;
/*
    var _myMessage          = [];
    var _myNextMessageLine  = 0;
    var _myFontname         = "Screen15";
*/
    var _myViewport = null;

    var _myFrameCount = 0;
    var _myAvrgT      = 0;
    var _lastTime     = 0;
    
    var _myTimeColor   = new Vector4f(1, 0, 0, 1);
    var _myMaxColor    = _myTimeColor;
    var _myTranspColor = new Vector4f(0, 0, 0, 0);
    var _myLineColor   = new Vector4f(0.5, 0.5, 0.5, 1);
    //var _myBGColor           = new Vector4f(1,0,0,0.3);
    var _myBGColor     = _myTranspColor;
    var _myTimings    = [{"name" : "gc",                 "color" : new Vector4f(0.3, 0.1, 0.1, 1), "time" : 0, "lastelapsed" : 0 }, 
                         {"name" : "onFrame_JSCallback", "color" : new Vector4f(0, 1, 0, 1), "time" : 0, "lastelapsed" : 0 },
                         {"name" : "SDL_GL_SwapBuffers", "color" : new Vector4f(1, 1, 0, 1), "time" : 0, "lastelapsed" : 0 },
                         {"name" : "render",             "color" : new Vector4f(0, 0, 1, 1), "time" : 0, "lastelapsed" : 0 }
                        ];

    var _myTopMaxTimers = [];

    /*self.setFontname = function(theFontname) {
        _myFontname = theFontname;
    };*/
    //var _myTimes = [];

    /////////////////////
    // Private Methods //
    /////////////////////

    function createPerfOverlay() {
        var myImage = window.scene.images.find("image[@name = 'PERF_Overlay']");
        if (!myImage) {
            myImage = Modelling.createImage(window.scene, BOX_WIDTH, BOX_HEIGHT, "RGBA");
            myImage.name = "OSD_Overlay";
        }
        myImage.raster.fillRect(0, 0, BOX_WIDTH, BOX_HEIGHT, _myTranspColor);

        _myViewport = theSceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel
        var myBoxOverlay = new ImageOverlay(window.scene, myImage, [0, 0], _myViewport.childNode("overlays"));
        myBoxOverlay.width  = BOX_WIDTH;
        myBoxOverlay.height = BOX_HEIGHT;
        myBoxOverlay.position = new Vector2f(((_myViewport.size[0] * window.width) - myBoxOverlay.width) / 2,
                                             ((_myViewport.size[1] * window.height) - myBoxOverlay.height) * 1);
        //print(myBoxOverlay.position, _myViewport.size);
        myBoxOverlay.visible = false;

        //var myColor = 0.3;
        //myBoxOverlay.color = new Vector4f(myColor,myColor,myColor,0.75);
        return { "overlay": myBoxOverlay, "image": myImage };
    }
    
    function ensureOverlay() {
        if (!_myOverlay) {
            var myResult = createPerfOverlay();
            _myOverlay =  myResult.overlay;
            _myImage = myResult.image;
            // print(myResult.image);
        }
    }

    function yToMap(theValue) {
        if (theValue < 0) {
            Logger.warning("PerfMeter.drawValue:  value out of range" + theValue);
            return 0;
        }
        if (theValue > 1)  {
            return 1;
        }
        return BOX_HEIGHT * (1 - theValue);
    }
    function xToMap(theFrame) {
        var mySlot = theFrame % H_VALUES;
        return mySlot * SLOT_WIDTH;
    }


    // value between 0 and 1
    function drawValue(theFrame, theBaseValue, theValue, theColor) {
        //print("from "+theBaseValue+" to" + theValue);
        var mySlotPos = xToMap(theFrame);
        // print("coord:"+mySlotPos+","+(BOX_HEIGHT*(1-theValue))+","+(mySlotPos+SLOT_WIDTH)+","+((BOX_HEIGHT-1)*(1-theBaseValue))+","+ theColor);
        _myImage.raster.fillRect(mySlotPos, yToMap(theValue), mySlotPos + SLOT_WIDTH, yToMap(theBaseValue), theColor);
    }

    function drawLine(theFrame, theValue, theColor) {
        _myImage.raster.setPixel(xToMap(theFrame), yToMap(theValue), theColor);
    }

    function updateTopMaxTimers(T_min, deltaT) {
        var newTopMaxTimers = Logger.getNewMaxTimers(T_min, 1, 8);
        if (!newTopMaxTimers) {
            return;
        }
        for (var t = 0; t < newTopMaxTimers.length; t++) {
            var myTimer = newTopMaxTimers[t];
            // print("MAX "+ t + ":" + myTimer.name+ ", max = " + myTimer.max);
            if (!(myTimer.name in _myTopMaxTimers) || _myTopMaxTimers[myTimer.name].lastelapsed  > myTimer.lastelapsed) {
                _myTopMaxTimers[myTimer.name] = myTimer;
                _myTopMaxTimers[myTimer.name].frame = _myFrameCount;
                _myTopMaxTimers[myTimer.name].lastelapsed = myTimer.lastelapsed;
            }
        }
    }
    
    ////////////////////
    // Public Members //
    ////////////////////
    
    self.enabled = false;

    ////////////////////
    // Public Methods //
    ////////////////////
    
    self.setTimings = function (theTiming) {
        _myTimings = theTiming;
    };
    self.setMaxTime = function (theMax) {
        T_MAX = theMax;
    };
    self.addTiming = function (theName, theColor) {
        _myTimings.push({"name": theName, "color": theColor, "time": 0});
    };
    
    self.toggleEnableFlag = function () {
        ensureOverlay();
        //if (!_myProcFunctionsPlugged) {
        //    plug("ProcessFunctions");
        //    _myProcFunctionsPlugged = true;
        //}
        _myOverlay.visible = !_myOverlay.visible;
    };
    
    self.setVisibleFlag = function (theFlag) {
        ensureOverlay();
        _myOverlay.visible     = theFlag;
        /*
        _myDisplayStartTime    = 0;
        _myMessage             = [];
        _myNextMessageLine     = 0;
        */
    };

    self.onFrame = function (theTime) {
        if (!_myOverlay.visible) {
            return;
        }
        var deltaT = theTime - _lastTime;
        if (deltaT > T_MAX) {
            deltaT = T_MAX;
        }

        _myAvrgT = (_myAvrgT * (1 - FILTER) + deltaT * FILTER);
        drawValue(_myFrameCount,  0, deltaT / T_MAX, _myTimeColor);
        drawValue(_myFrameCount + 1, 0, 1, _myBGColor);

        var myTotal = 0;
        for (var i = 0; i < _myTimings.length; i++) {
            var myTiming = _myTimings[i];
            myTiming.time = Logger.getLastElapsed(myTiming.name);
            //myTiming.time = 1/100;
            //print (myTiming.name + myTiming.time);
            drawValue(_myFrameCount,  myTotal / T_MAX, (myTotal + myTiming.time) / T_MAX, myTiming.color);
            myTotal += myTiming.time;
        }
        drawLine(_myFrameCount, (1 / 30) / T_MAX, _myLineColor);
        drawLine(_myFrameCount, (1 / 60) / T_MAX, _myLineColor);
        //drawLine(_myFrameCount, _myBaseLine, _myMaxColor);
        drawLine(_myFrameCount, _myAvrgT / T_MAX, _myMaxColor);

        if (deltaT > _myAvrgT + _myAvrgT * 0.5) {
            //print("updating");
            updateTopMaxTimers(_myAvrgT / 3, deltaT);
        }

        _myBaseLine = _myAvrgT / T_MAX * 2;
        /*
        if (xToMap(_myFrameCount) < SLOT_WIDTH) {
            _myBaseLine = deltaT/T_MAX;
        } else {
            var newMax = deltaT/T_MAX;
            if (newMax > _myBaseLine) {
                _myBaseLine = newMax;
            }
        }
        */
        ++_myFrameCount;
        _lastTime = theTime;
    };

    self.onPostRender = function () {
        if (!_myOverlay.visible) {
            return;
        }
        var i, myTiming, myPosition;
        var myViewport = _mySceneViewer.getViewportAtWindowCoordinates(0, 0); // get viewport containing upper left pixel

        for (i = 0; i < _myTimings.length; i++) {
            myTiming = _myTimings[i];
            myPosition = sum(_myOverlay.position, new Vector2f(-280, BOX_HEIGHT + -(_myTimings.length - i) * 14));
            window.setTextColor(myTiming.color);
            window.renderText(myPosition, myTiming.name + "=" + (myTiming.time * 1000).toFixed(1) + "ms", "Screen13", myViewport);
        }
        
        for (i = 0; i < _myTopMaxTimers.length; i++) {
            myTiming = _myTopMaxTimers[i];
            //print(":"+i+":"+t+"="+myTiming.name + ", max = " +  myTiming.max+ ", frame = "+myTiming.frame);
            if (myTiming.frame + BOX_WIDTH / SLOT_WIDTH < _myFrameCount) {
                //print("erasing :"+i+":"+t+"="+myTiming.name + ", max = " +  myTiming.max+ ", frame = "+myTiming.frame);
                //_myTopMaxTimers[t] = undefined;
                delete _myTopMaxTimers[i];
            } else {
                myPosition = sum(_myOverlay.position, new Vector2f(xToMap(myTiming.frame), yToMap(_myBaseLine) - i * 14));
                // print("pos = "+myPosition);
                window.setTextColor(new Vector4f(1, 0.5, 0, 1));
                window.renderText(myPosition, myTiming.name + "=" + (myTiming.lastelapsed * 1000).toFixed(1) + "ms", "Screen13", myViewport);
            }
            ++i;
        }

        /*
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
        */
    };
};
