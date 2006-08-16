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

const START_FRAMES = 10;
const END_FRAMES   = 10;
const IMAGE_COUNT  = 1000;

const IMAGE_1 = "../../../../shader/shadertex/ac_logo.png";
const IMAGE_2 = "../../../../shader/shadertex/hatch.jpg";

var window     = new RenderWindow();
window.fixedFrameTime = 0.04;

function ImageLeakUnitTest() {
    this.Constructor(this, "ImageLeakUnitTest");
};
ImageLeakUnitTest.prototype.Constructor = function(obj, theName) {
    var _myToggle = true;
    var _myImage   = null;
    var _myFrameCount = 0;
    var _myStartMemory = 0;
    UnitTest.prototype.Constructor(obj, theName);
    
    obj.run = function() {    
        window.onFrame = function(theTime) {
            if (_myFrameCount == START_FRAMES) {
                _myStartMemory = getProcessMemoryUsage();
            } else if (_myFrameCount > START_FRAMES && _myFrameCount < START_FRAMES + IMAGE_COUNT) {
                toggleImage();
            } else if (_myFrameCount == START_FRAMES + IMAGE_COUNT){
                remove();
            } else if (_myFrameCount >= START_FRAMES + IMAGE_COUNT + END_FRAMES) {
                gc();
                //window.scene.save("empty.xml", false);
                var myUsedMemory = getProcessMemoryUsage();
                obj.myMemoryDifff =  myUsedMemory - _myStartMemory;
                obj.AllowedMemoryUsage = 2*1024*1024;//4000000;

                print("Memory at first image construction time : " + _myStartMemory);
                print("Memory at app end                       : " + myUsedMemory);
                print("Difference                              : " + obj.myMemoryDifff);
                print("allowed difference                      : " + obj.AllowedMemoryUsage + " ,(due to some basic memory allocation, i.e. SomImageFactory)");
                ENSURE('obj.myMemoryDifff < obj.AllowedMemoryUsage');                
                window.stop();
            }
            _myFrameCount++;            
        }
        window.go();
    }
    function remove() {
        if (_myImage != null) {
            _myImage.removeFromScene();
            _myImage = null;
            gc();
        }
    }
    function toggleImage() {
        remove();
        if (_myToggle) {
            _myImage    = new ImageOverlay(window.scene, IMAGE_1);            
        } else {
            _myImage    = new ImageOverlay(window.scene, IMAGE_2);
        }
        _myToggle = !_myToggle;                    
    }


}

var myTestName = "ImageLeakUnitTest.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new ImageLeakUnitTest());
mySuite.run();

print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());

