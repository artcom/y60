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
//   $RCSfile: testRequest.tst.js,v $
//   $Author: martin $
//   $Revision: 1.4 $
//   $Date: 2004/08/04 09:37:33 $
//
//
//=============================================================================

print("Running test.js");

use("UnitTest.js");


function RequestUnitTest() {
    this.Constructor(this, "RequestUnitTest");
};

RequestUnitTest.prototype.Constructor = function(obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);
    // create a manager
    // note: a Renderer is also a manager,
    // so use a renderer in a real application

    // obj.myRenderer = new Renderer();
    obj.myManager = new RequestManager();

    obj.run = function() {
        var myPort = Number(expandEnvironment("${AC_TEST_PORT_START}"));
        if (myPort == 0) {
            myPort = 2345;
        }

        obj.myTestServer = new TestServer("localhost",myPort);
        obj.myTestServer.start();

        obj.myRequest = new Request("http://localhost:"+myPort+"/ShortRequest");

       obj.myRequest.onDone = function() {
            print ("onDone called on "+this);
       };


       /*
       obj.myPostRequest = new Request("http://himmel/~martin/upload.php");

       obj.myPostRequest.postFile("/usr/lib/libc.a");
       obj.myPostRequest.addHttpHeader("X-Filename","SomeLogo.gif");
       obj.myPostRequest.onDone = function() {
           print (this.responseString);
       };
       obj.myManager.performRequest(obj.myPostRequest);
       */

        ENSURE("obj.myRequest != null");
        ENSURE("obj.myManager.activeCount==0");
        obj.myManager.performRequest(obj.myRequest);
        ENSURE("obj.myManager.activeCount==1");

        obj.myBadRequest = new Request("http://www.server.invalid");
        obj.myManager.performRequest(obj.myBadRequest);

        obj.myTimeoutRequest = new Request("http://localhost:"+myPort+"/Timeout");
        obj.myTimeout = 5;
        obj.myTimeoutRequest.setTimeoutParams(1, obj.myTimeout);
        obj.myTimeoutRequest.onError = function(theErrorCode) {
            obj.myErrorCode = theErrorCode;
            //print ("onError called with code: " + theErrorCode);
        };

        obj.myManager.performRequest(obj.myTimeoutRequest);


        // loop until no more active requests
        // when using a Renderer, this will happen in the
        // render loop
        var myTime = new Date();
        while (obj.myManager.activeCount) {
            obj.myManager.handleRequests();
        }
        obj.myDiffTime = (new Date() - myTime) / 1000;
        print("diff time: "+obj.myDiffTime, obj.myTimeout);
        // check if tests needs at least the timeout and max 100 seconds
        ENSURE("obj.myDiffTime >= obj.myTimeout");
        ENSURE("obj.myDiffTime < 100");

        ENSURE("obj.myManager.activeCount==0");
        ENSURE("obj.myRequest.responseCode==200");
        print("obj.myTimeoutRequest.responseCode: " + obj.myTimeoutRequest.responseCode);
        ENSURE("obj.myTimeoutRequest.responseCode==0");
        ENSURE("obj.myErrorCode==28");
        print (obj.myBadRequest.errorString);
        print (obj.myRequest.responseString);

        obj.myTestServer.stop();
    };
};

function main() {
    var myTestName = "testRequest.tst.js";
    var mySuite = new UnitTestSuite(myTestName);

    mySuite.addTest(new RequestUnitTest());
    mySuite.run();

    print(">> Finished test suite '"+myTestName+"', return status = " + mySuite.returnStatus() + "");
    return mySuite.returnStatus();
}

if (main() != 0) {
    exit(1);
};


