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
//   $RCSfile: testText.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/18 11:48:31 $
//
//
//=============================================================================

// XXX
// IMHO this thing is broken by design. It ignores page-size effects, 
// difference in ptr size and all kind of other issues. It is the number
// one cause of failed tests accross all platforms. I'm going to disable
// at least the FFMpegDecoder2 leak test until someone rewrites this piece
// of ... code
// [DS]
use("Overlay.js");
use("Exception.js");
use("UnitTest.js");

plug("ProcessFunctions");

const START_FRAMES = 200;
const END_FRAMES   = 4;

var window     = new RenderWindow();
window.fixedFrameTime = 0.04;

function MovieLeakUnitTest(theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifferenceInBytes) {
    this.Constructor(this, theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifferenceInBytes);
};
MovieLeakUnitTest.prototype.Constructor = function(obj, theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifferenceInBytes) {
    var _myToggle = true;
    var _myMovie   = null;
    var _myFrameCount = 0;
    var _myStartMemory = 0;
    var _myLastMemory = 0;
    var _myTrendCounter = 0;
    var _myTrend = 0;
    var _myMaxMemory = 0;
    var _myMaxFrame = 0;
    var _myMinMemory = 0;
    var _myMinFrame = 0;

    UnitTest.prototype.Constructor(obj, theName);
    obj.run = function() {
        window.onFrame = function(theTime) {
            gc();
            var myMem = getProcessMemoryUsage();
            if (_myFrameCount == START_FRAMES) {
                _myStartMemory = myMem;
                //print("**** startmemory =" + _myStartMemory);
            } else if (_myFrameCount < START_FRAMES + theVideoCount) {
                //print("---- memory = "+getProcessMemoryUsage());
                toggleMovie();
                var myText = "Loop : " + (_myFrameCount - START_FRAMES) + "/" + theVideoCount;
                
                window.renderText([500,100], myText);
            } else if (_myFrameCount >= START_FRAMES + theVideoCount + END_FRAMES + 1) {
                //window.scene.save("empty.xml", false);
                var myUsedMemory = myMem;
                obj.myMemoryDiff =  myUsedMemory - _myStartMemory;
                obj.AllowedMemoryUsage = theAllowedMemoryDifferenceInBytes/1024;
                obj.myTrend = _myTrend;
                obj.myMaxFrame = _myMaxFrame;
                obj.myFrameCount = _myFrameCount;

                print("-------------------------------------");
                print("Decoder leak test: " + theName);
                print("-------------------------------------");
                print("Memory at frame " + START_FRAMES + " construction time : " + asMemoryString(_myStartMemory));
                print("Memory at app end                       : " + asMemoryString(myUsedMemory));
                print("Difference                              : " + asMemoryString(obj.myMemoryDiff));
                print("Max Memory Usage                        : " + asMemoryString(_myMaxMemory));
                print("Max Memory Usage in frame               : " + _myMaxFrame);
                print("No new maximum for last n frames, n =   : " + (obj.myFrameCount - obj.myMaxFrame));
                print("Avrg. Difference per Movie              : " + asMemoryString(obj.myTrend));
                print("allowed difference per Movie            : " + asMemoryString(obj.AllowedMemoryUsage));
                ENSURE('obj.myTrend < obj.AllowedMemoryUsage || obj.myFrameCount - obj.myMaxFrame > 50');

                //window.scene.save("leaktest.x60");
                window.stop();
            }
            if (_myStartMemory) {
                ++_myTrendCounter;
                _myMinMemory = _myMaxMemory;
                _myTrend = (myMem-_myStartMemory)/_myTrendCounter;
            } else {
                _myTrend = myMem-_myLastMemory;
            }
            if (myMem > _myMaxMemory) {
                _myMaxMemory = myMem;
                _myMaxFrame = _myFrameCount;
            }
            if (myMem < _myMinMemory) {
                _myMinMemory = myMem;
                _myMinFrame = _myFrameCount;
            }

            window.renderText( [500,150], "Delta memory usage: " + asMemoryString(myMem-_myLastMemory));
            window.renderText( [500,175], "Total memory usage: " + asMemoryString(myMem));
            window.renderText( [500,200], "Trend:              " + _myTrend.toFixed(0));
            window.renderText( [500,225], "Max. memory usage:  " + asMemoryString(_myMaxMemory));
            window.renderText( [500,250], "Max. in Frame       " + _myMaxFrame);
            _myFrameCount++;
            _myLastMemory = myMem;
        }
        window.go();
    }
    function remove() {
        if (_myMovie != null) {
            //print("removed :" + _myMovie.src + " from scene");
            _myMovie.removeFromScene();
            _myMovie = null;
            gc();
        }
    }
    function toggleMovie() {
        remove();
        if (_myToggle) {
            _myMovie    = new MovieOverlay(window.scene, theFiles[0], [0,0], null, false, null, theDecoderHint, "play");
        } else {
            _myMovie    = new MovieOverlay(window.scene, theFiles[1], [0,0], null, false, null, theDecoderHint, "play");
        }
        _myToggle = !_myToggle;
    }


}
