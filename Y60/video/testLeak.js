//=============================================================================
// Copyright (C) 1993-2005, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================
//
//   $RCSfile: testText.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/18 11:48:31 $
//
//
//=============================================================================


use("Overlay.js");
use("Exception.js");
use("UnitTest.js");

plug("ProcFunctions");

const START_FRAMES = 199;
const END_FRAMES   = 200;

var window     = new RenderWindow();
window.fixedFrameTime = 0.04;

function MovieLeakUnitTest(theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifference) {
    this.Constructor(this, theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifference);
};
MovieLeakUnitTest.prototype.Constructor = function(obj, theName, theFiles, theDecoderHint, theVideoCount, theAllowedMemoryDifference) {
    var _myToggle = true;
    var _myMovie   = null;
    var _myFrameCount = 0;
    var _myStartMemory = 0;
    var _myLastMemory = 0;
    UnitTest.prototype.Constructor(obj, theName);
    //while(true) {toggleMovie();}
    obj.run = function() {    
        window.onFrame = function(theTime) {
            var myMem = getProcessMemoryUsage();
            if (_myFrameCount == START_FRAMES) {
                _myStartMemory = getProcessMemoryUsage();
            } else if (_myFrameCount > START_FRAMES && _myFrameCount < START_FRAMES + theVideoCount) {
                toggleMovie();
                var myText = "Loop : " + (_myFrameCount - START_FRAMES) + "/" + theVideoCount; 
                window.renderText([500,100], myText, "Screen15");                
                window.renderText([500,150], "Delta memory usage: " + (myMem-_myLastMemory), "Screen15");
                window.renderText([500,200], "Total memory usage: " + myMem, "Screen15");                
            } else if (_myFrameCount == START_FRAMES + theVideoCount){
                toggleMovie();
            } else if (_myFrameCount == START_FRAMES + theVideoCount + 200) {
                toggleMovie();
            } else if (_myFrameCount == START_FRAMES + theVideoCount + 400) {
                remove();
            } else if (_myFrameCount >= START_FRAMES + theVideoCount + END_FRAMES + 400) {
                gc();
                //window.scene.save("empty.xml", false);
                var myUsedMemory = getProcessMemoryUsage();
                obj.myMemoryDifff =  myUsedMemory - _myStartMemory;
                obj.AllowedMemoryUsage = theAllowedMemoryDifference;

                print("-------------------------------------");
                print("Decoder leak test: " + theName);
                print("-------------------------------------");
                print("Memory at first movie construction time : " + _myStartMemory);
                print("Memory at app end                       : " + myUsedMemory);
                print("Difference                              : " + obj.myMemoryDifff);
                print("allowed difference                      : " + obj.AllowedMemoryUsage + " ,(due to some basic memory allocation, i.e. plugin-ctor code, SomImageFactory)");
                ENSURE('obj.myMemoryDifff < obj.AllowedMemoryUsage');                
                window.stop();
            }
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
            _myMovie    = new MovieOverlay(window.scene, theFiles[0], [0,0], null, false, null, theDecoderHint);            
        } else {
            _myMovie    = new MovieOverlay(window.scene, theFiles[1], [0,0], null, false, null, theDecoderHint);
        }
        _myToggle = !_myToggle;                    
    }


}
