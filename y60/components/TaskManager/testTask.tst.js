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

const TEST_SPEED = 1000;

use("UnitTest.js");
plug("y60TaskManager");

function TaskManagerUnitTest() {
    this.Constructor(this, "TestUnitTest");
};

TaskManagerUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function getTaskWindowsWithTimeout(theTask, theTimeoutInMs) {
        var myStartTime = millisec();
        var myWindows = theTask.windows;
        while(myWindows.length == 0 && (millisec() - myStartTime < theTimeoutInMs)) {
            msleep(100);
            myWindows = theTask.windows;
        }
        return myWindows;
    }

    function testApplication(theApplication, theWindowName) {
        print(">>>  Test Application: '" + theApplication);

        var myWindowsPath = getFolderPath("WINDOWS");
        print("Windows path=" + myWindowsPath);

        print("Open tasks");
        obj.myTask  = new Task(theApplication);
        obj.myTask1 = new Task(myWindowsPath + "/notepad.exe", myWindowsPath, false);
        obj.myTask2 = new Task(myWindowsPath + "/notepad.exe", myWindowsPath, false);
        msleep(TEST_SPEED);

        ENSURE('obj.myTask.windows.length > 0');
        ENSURE('obj.myTask1.windows.length > 0');
        ENSURE('obj.myTask2.windows.length > 0');
        obj.myWindows = getTaskWindowsWithTimeout(obj.myTask, 2000);
        ENSURE('obj.myWindows.length > 0');
        obj.myWindow = obj.myTask.windows[0];
        ENSURE('obj.myWindow.windowname == "' + theWindowName + '"');

        // Attach first window by name
        obj.myTaskWindow  = new TaskWindow(theWindowName);
        obj.myWindows = getTaskWindowsWithTimeout(obj.myTask1, 2000);
        ENSURE('obj.myWindows.length > 0');
        obj.myTaskWindow1 = obj.myTask1.windows[0];
        obj.myWindows = getTaskWindowsWithTimeout(obj.myTask2, 2000);
        ENSURE('obj.myWindows.length > 0');
        obj.myTaskWindow2 = obj.myTask2.windows[0];

        print("Restore minimized tasks");                    
        obj.myTaskWindow1.restore();
        obj.myTaskWindow2.restore();              
        msleep(TEST_SPEED);

        print("Move and resize task");
        obj.myTaskWindow.position = [0, 0];
        obj.myTaskWindow.resize(200, 400);
        ENSURE("almostEqual(obj.myTaskWindow.position, [0, 0])");

        obj.myTaskWindow1.position = [0, 400];
        obj.myTaskWindow1.resize(200, 200);
        ENSURE("almostEqual(obj.myTaskWindow1.position, [0, 400])");

        obj.myTaskWindow2.position = [0, 600];
        obj.myTaskWindow2.resize(200, 100);
        ENSURE("almostEqual(obj.myTaskWindow2.position, [0, 600])");
        msleep(TEST_SPEED);

        print("Toogle task visibility");
        ENSURE("obj.myWindow.visible");
        obj.myWindow.visible = false;
        ENSURE("!obj.myWindow.visible");
        msleep(TEST_SPEED / 2);
        obj.myWindow.visible = true;
        ENSURE("obj.myWindow.visible");
        msleep(TEST_SPEED / 2);


        print("Activate second task");
        obj.myTaskWindow1.activate();
        msleep(TEST_SPEED);

        print("Capture task");
        obj.myTaskWindow.capture("test.png");
        msleep(TEST_SPEED);

        print("Minimize task");
        obj.myTaskWindow.minimize();
        msleep(TEST_SPEED);

        print("Maximize task");
        obj.myTaskWindow.maximize();
        msleep(TEST_SPEED);

        print("Restore task");
        obj.myTaskWindow.restore();
        msleep(TEST_SPEED);

        print("Hide second task decoration");
        obj.myTaskWindow1.hideDecoration();
        msleep(TEST_SPEED);

        print("Set task alpha");
        obj.myTaskWindow.setAlpha(0.5);
        msleep(TEST_SPEED);

        print("Create external window");
        var myTask3 = new Task(myWindowsPath + "/notepad.exe");
        msleep(TEST_SPEED);
        obj.myWindows = getTaskWindowsWithTimeout(myTask3, 2000);
        ENSURE('obj.myWindows.length > 0');        
        obj.myTaskWindow3 = myTask3.windows[0];

        print("Move external window");
        obj.myTaskWindow3.position = [0, 700];
        obj.myTaskWindow3.resize(200, 100);
        ENSURE("almostEqual(obj.myTaskWindow3.position, [0, 700])");
        msleep(TEST_SPEED);

        print("Add external window to main task");
        var myWindowCount = obj.myTask.windows.length;
        obj.myTask.addExternalWindow(obj.myTaskWindow3.windowname);
        ENSURE('obj.myTask.windows.length == ' + (myWindowCount + 1));
        msleep(TEST_SPEED);

        print("Close all windows belonging to first task");
        var myTaskWindows = obj.myTask.windows;
        for (var i = 0; i < myTaskWindows.length; ++i) {
            myTaskWindows[i].close();
        }
        msleep(TEST_SPEED);

        print("Destroy task");
        obj.myTaskWindow1.destroy();
        msleep(TEST_SPEED);

        print("Terminate task");
        obj.myTask.terminate();
        obj.myTask1.terminate();
        obj.myTask2.terminate();
        msleep(TEST_SPEED);
    }

    obj.run = function() {
        try {
            testApplication(getFolderPath("SYSTEM") + "/calc.exe", "Rechner");
            //testApplication(getFolderPath("PROGRAM_FILES") + "/Mozilla Firefox/firefox.exe -k http://himmel.artcom.de");
            //testApplication("C:/Programme/Internet Explorer/iexplore.exe http://himmel.artcom.de");
        } catch (ex) {
            FAILURE("Exception caught");
            reportException(ex);
            return;
        }

        SUCCESS("No exception caught");
    }
};

function main() {
    var myTestName = "testTaskManager.tst.js";
    var mySuite = new UnitTestSuite(myTestName);
    mySuite.addTest(new TaskManagerUnitTest());

    mySuite.run();

    print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(5);
} else {
    exit(0);
}
