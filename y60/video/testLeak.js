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
//   $RCSfile: testText.js,v $
//   $Author: martin $
//   $Revision: 1.5 $
//   $Date: 2005/03/18 11:48:31 $
//
//
//=============================================================================

includePath("../..");  //TODO: remove this after deprecating  ant-build
use("Overlay.js");
use("Exception.js");
use("UnitTest.js");

plug("y60ProcFunctions");

//const START_FRAMES = 199;
//const END_FRAMES   = 200;
const START_FRAMES = 3;
const END_FRAMES   = 4;

var myShaderLibrary = "../../shader/shaderlibrary.xml";
GLResourceManager.prepareShaderLibrary(myShaderLibrary);

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
/*        
            } else if (_myFrameCount == START_FRAMES + theVideoCount){
                toggleMovie();
            } else if (_myFrameCount == START_FRAMES + theVideoCount + 200) {
                toggleMovie();
*/
            } else if (_myFrameCount == START_FRAMES + theVideoCount + END_FRAMES) {
//                remove();
            } else if (_myFrameCount >= START_FRAMES + theVideoCount + END_FRAMES + 1) {
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
                window.scene.save("leaktest.x60");
                window.stop();
            }
            window.renderText([500,150], "Delta memory usage: " + (myMem-_myLastMemory), "Screen15");
            window.renderText([500,200], "Total memory usage: " + myMem, "Screen15");                
            _myFrameCount++; 
            _myLastMemory = myMem;     
            //if (_myFrameCount % 10 == 0) window.printStatistics();      
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
