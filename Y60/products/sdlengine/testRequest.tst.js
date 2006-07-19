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


