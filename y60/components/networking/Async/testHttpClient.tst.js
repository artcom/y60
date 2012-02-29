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

/*jslint white : false*/
/*globals use plug UnitTest millisec Socket HttpClient ENSURE UnitTestSuite
          print exit RequestManager Request*/

use("UnitTest.js");

plug("Network");
print("=========================");
plug("NetAsync");
print("=========================");

var TIMEOUT = 1000;

function HttpClientUnitTest() {
    this.Constructor(this, "HttpClientUnitTest");
}

HttpClientUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function testFireAndForget() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://192.168.56.10:3080/foo",
            success: function() {
                SUCCESS("FireAndForget");
                done = true;
            },
            verbose: true
        });
        var myClient = null;
        gc();

        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }
    };
    function testError() {
        var done = false;
        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:88/foobar",
            progress: function(theBlock) {
                Logger.info(theBlock.size);
                theBlock.resize(0);
            },
            success: function() {
                FAILURE("testError");
            },
            error: function() {
                SUCCESS("testError");
                print(this);
                Logger.warning("error callback called!");
                done = true;
            },
            verbose: true
        });


        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }
    };
    function testBigRequest() {
        var done = false;

        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://files.t-gallery.act/data/repository/original/vol0/24/vater_der_braut_de_hd_eff5390ebdbdf0bd62f86b716f8f8adf3d9512d6.mp4",
            progress: function(theBlock) {
                // Logger.info(theBlock.size);
                theBlock.resize(0);
            },
            success: function() {
                Logger.warning("onDone called!");
                done = true;
            },
            verbose: true
        });


        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }
    };

    function testSmallRequests() {
        // test many small requests
        var i = 10;

        var iterate = function() {
            Logger.warning("iterate called");
            i--;
            if (i>0) {
                obj.myClient = new Async.HttpClient({ url: "http://127.0.0.1:3003/foo", success: iterate, verbose:false } );
            };
            Logger.warning("iterate done");
        }
        iterate();
        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (i <= 0) {
                break;
            }
        }
    }

    obj.run = function () {
         
        obj.myServer = new Async.HttpServer();
        var myServer = {
            foo: function(theMethod, theBody) {
                     return "ok"; 
                 }
        };
        obj.myServer.start("0.0.0.0", "3003");
        obj.myServer.registerCallback("/foo", myServer, myServer.foo);
        
        testFireAndForget();

        testError();
        testSmallRequests();
        // testBigRequest();

        obj.myServer.close();
    };

};

var myTestName = "testHttpClient.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new HttpClientUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
