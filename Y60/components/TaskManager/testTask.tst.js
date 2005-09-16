/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSfile: testTask.tst.js,v $
//
//   $Revision: 1.7 $
//
// Description: Utility Classes and Macros for easy unit testing
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

const TEST_SPEED = 1000;

use("UnitTest.js");
plug("TaskManager");

function TaskManagerUnitTest() {
    this.Constructor(this, "TestUnitTest");
};

TaskManagerUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testApplication(theApplication, theWindowName, theWindowCount) {
        print(">>>  Test Application: '" + theApplication);
        print("Open tasks");
        obj.myTask  = new Task(theApplication);
        obj.myTask1 = new Task("C:/WINDOWS/notepad.exe", "C:/WINDOWS", false);
        obj.myTask2 = new Task("C:/WINDOWS/notepad.exe", "C:/WINDOWS", false);
        msleep(TEST_SPEED);

        ENSURE('obj.myTask.windows.length == ' + theWindowCount);
        ENSURE('obj.myTask1.windows.length == 1');
        ENSURE('obj.myTask2.windows.length == 1');
        obj.myWindow = obj.myTask.windows[0];
        ENSURE('obj.myWindow.windowname == "' + theWindowName + '"');

        // Attach first window by name
        obj.myTaskWindow  = new TaskWindow(theWindowName);
        obj.myTaskWindow1 = obj.myTask1.windows[0];
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
        obj.myTaskWindow.capture("pruefimage.png");
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
        var myTask3 = new Task("C:/WINDOWS/notepad.exe");
        msleep(TEST_SPEED);
        obj.myTaskWindow3 = myTask3.windows[0];

        print("Move external window");
        obj.myTaskWindow3.position = [0, 700];
        obj.myTaskWindow3.resize(200, 100);
        ENSURE("almostEqual(obj.myTaskWindow3.position, [0, 700])");
        msleep(TEST_SPEED);

        print("Add external window to main task");
        obj.myTask.addExternalWindow(obj.myTaskWindow3.windowname);
        ENSURE('obj.myTask.windows.length == ' + (theWindowCount + 1));
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
            testApplication("C:/WINDOWS/System32/calc.exe", "Rechner", 2);
            //testApplication("C:/WINDOWS/notepad.exe");
            //testApplication("C:/Programme/Mozilla Firefox/firefox.exe -k http://himmel.artcom.de");
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
