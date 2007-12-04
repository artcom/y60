//=============================================================================
// Copyright (C) 1993-2007, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

use("Overlay.js");
use("Exception.js");
use("UnitTest.js");

plug("ProcFunctions");

const ALLOWED_MEMORY_USAGE = 3*1024*1024;

const START_FRAMES = 10;
const END_FRAMES   = 10;
const IMAGE_COUNT  = 1000;

const IMAGE_1 = expandEnvironment("${PRO}/src/Y60/shader/shadertex/ac_logo.png");
const IMAGE_2 = expandEnvironment("${PRO}/src/Y60/shader/shadertex/hatch.jpg");
var myShaderLibrary = "${PRO}/src/Y60/shader/shaderlibrary.xml";
GLResourceManager.prepareShaderLibrary(myShaderLibrary);

var window     = new RenderWindow();
window.fixedFrameTime = 0.04;

function ImageLeakUnitTest() {
    this.Constructor(this, "ImageLeakUnitTest");
};

ImageLeakUnitTest.prototype.Constructor = function(obj, theName) {

    var _myToggle = true;
    var _myOverlay = null;
    var _myFrameCount = 0;
    var _myStartMemory = 0;
    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function() {
        window.onFrame = function(theTime) {
            if (_myFrameCount == START_FRAMES) {
                _myStartMemory = getProcessMemoryUsage();
            } else if (_myFrameCount > START_FRAMES && _myFrameCount < START_FRAMES + IMAGE_COUNT) {
                toggleImage();
            } else if (_myFrameCount == START_FRAMES + IMAGE_COUNT) {
                remove();
            } else if (_myFrameCount >= START_FRAMES + IMAGE_COUNT + END_FRAMES) {
                gc();
                var myUsedMemory = getProcessMemoryUsage();

                // must be attached to obj for ENSURE to work
                obj.myMemoryDifff =  myUsedMemory - _myStartMemory;
                obj.myAllowedMemoryUsage = ALLOWED_MEMORY_USAGE;

                print("Number of textures                      : " + window.scene.textures.childNodesLength());
                print("Number of images                        : " + window.scene.images.childNodesLength());
                print("Memory at first image construction time : " + _myStartMemory);
                print("Memory at app end                       : " + myUsedMemory);
                print("Difference                              : " + obj.myMemoryDifff);
                print("allowed difference                      : " + obj.myAllowedMemoryUsage + " ,(due to some basic memory allocation, i.e. SomImageFactory)");
                ENSURE("obj.myMemoryDifff < obj.myAllowedMemoryUsage");
                ENSURE("window.scene.textures.childNodesLength() == 0");
                ENSURE("window.scene.images.childNodesLength() == 0");
                window.stop();
            }
            _myFrameCount++;
        }
        window.go();
    }
    function remove() {
        if (_myOverlay != null) {
            _myOverlay.removeFromScene();
            _myOverlay = null;
            gc();
        }
    }
    function toggleImage() {
        remove();
        if (_myToggle) {
            _myOverlay = new ImageOverlay(window.scene, IMAGE_1);
        } else {
            _myOverlay = new ImageOverlay(window.scene, IMAGE_2);
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

