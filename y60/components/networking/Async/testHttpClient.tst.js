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

/*globals use, plug, UnitTest, millisec, msleep, Async, ENSURE, SUCCESS, FAILURE, gc, UnitTestSuite,
          print, exit, RequestManager, Request, Logger */

use("UnitTest.js");

plug("NetAsync");

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
            url: "http://127.0.0.1:3003/foo",
            success: function () {
                SUCCESS("FireAndForget");
                done = true;
            },
            error: function () {
                FAILURE("FireAndForget");
                done = true;
            }
        });
        myClient = null;
        gc();

        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }
    }
    function testBlocking() {
        var done = false;
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:88/foo",
            async: false,
            verbose: false,
            success: function() {
                done = true;
            },
            error: function() {
                done = true;
            }
        });

        if (done) {
            SUCCESS("testBlocking");
        } else {
            FAILURE("testBlocking");
        }
    }
    function testError() {
        var done = false;
        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:8765/foobar",
            connecttimeout: 5,
            success: function() {
                FAILURE("testError");
                done = true;
            },
            error: function() {
                SUCCESS("testError");
                done = true;
            }
        });


        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }
    }
    function testBigRequest() {
        var done = false;

        var received = 0;
        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/big",
            // url: "http://files.t-gallery.act/data/repository/original/vol0/24/vater_der_braut_de_hd_eff5390ebdbdf0bd62f86b716f8f8adf3d9512d6.mp4",
            progress: function(theBlock) {
                received += theBlock.size;
                theBlock.resize(0);
                return true;
            },
            error: function() {
                FAILURE("testBigRequest");
                done = true;
            },
            success: function(data, code, response) {
                DPRINT2("received in Progress: ", received);
                DPRINT2("last responseBlock:", response.responseBlock.size);
                done = true;
                if (received + response.responseBlock.size === 1024*1024*5) {
                    SUCCESS("testBigRequest");
                } else {
                    FAILURE("testBigRequest");
                }
            }
        });


        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
    }

    function testRequestProgressError() {
        var done = false;

        var received = 0;
        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/big",
            progress: function(theBlock) {
                return false;
            },
            error: function() {
                SUCCESS("testRequestProgressError");
                done = true;
            },
            success: function() {
                FAILURE("testRequestProgressError");
                done = true;
            }
        });


        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
    }
    
    function testRequestAbort() {
        var done = false;

        var received = 0;
        Logger.info("creating client");
        obj.myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/big",
            error: function() {
                SUCCESS("testRequestAbort");
                done = true;
            },
            success: function() {
                FAILURE("testRequestAbort");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            obj.myClient.abort();
            if (done) {
                break;
            }
        }
    };

    function testGetRequest() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/echo",
            type: "GET",
            verbose: true,
            success: function (data, code, response) {
                obj.responseString = response.responseString;
                obj.code = code;
                obj.content_type = response.getResponseHeader("Content-Type");
                SUCCESS("testGetRequest");
                done = true;
            },
            error: function () {
                FAILURE("testGetRequest");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
        ENSURE_EQUAL("GET:", obj.responseString);
        ENSURE_EQUAL("200", obj.code);
        ENSURE_EQUAL("text/plain", obj.content_type);
    };

    function testDeleteRequest() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/echo",
            type: "DELETE",
            success: function (data, code, response) {
                obj.responseString = response.responseString;
                SUCCESS("testDeleteRequest");
                done = true;
            },
            error: function () {
                FAILURE("testDeleteRequest");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
        ENSURE_EQUAL("DELETE:", obj.responseString);
    };

    function testPostRequest() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/echo",
            type: "POST",
            verbose: true,
            data: "Hello World!",
            contentType: "text/plain",
            success: function (data, code, response) {
                obj.responseString = response.responseString;
                SUCCESS("testPostRequest");
                done = true;
            },
            error: function () {
                FAILURE("testPostRequest");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
        ENSURE_EQUAL("POST:Hello World!", obj.responseString);
    };

    function testPutRequest() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/echo",
            type: "PUT",
            data: "Hello World!",
            verbose: true,
            contentType: "text/plain",
            success: function (data, code, response) {
                obj.responseString = response.responseString;
                SUCCESS("testPutRequest");
                done = true;
            },
            error: function () {
                FAILURE("testPutRequest");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
        ENSURE_EQUAL("PUT:Hello World!", obj.responseString);
    };

    function testHeadRequest() {
        var done = false;

        Logger.info("creating client");
        var myClient = new Async.HttpClient({
            url: "http://127.0.0.1:3003/echo",
            type: "HEAD",
            success: function (data, code, response) {
                obj.responseString = response.responseString;
                SUCCESS("testHeadRequest");
                done = true;
            },
            error: function () {
                FAILURE("testHeadRequest");
                done = true;
            }
        });

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
        ENSURE_EQUAL("", obj.responseString);
    };

    function testSmallRequests() {
        // test many small requests
        var i = 10;

        var iterate = function() {
            if (i>0) {
                obj.myClient = new Async.HttpClient({
                    url: "http://127.0.0.1:3003/foo",
                    success: function() {
                        SUCCESS("testSmallRequests");
                        i--;
                        iterate();
                    },
                    error: function() {
                        FAILURE("testSmallRequests");
                        i = 0;
                    }
                });
            }
        };
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
                 },
            big: function(theMethod, theBody) {
                return new Array(1024*1024+1).join("Fubar"); // 100 MB
                 },
            echo: function(theMethod, theBody) {
                return theMethod+":"+theBody;
                 }

        };
        obj.myServer.start("0.0.0.0", "3003");
        obj.myServer.registerCallback("/foo", myServer, myServer.foo);
        obj.myServer.registerCallback("/big", myServer, myServer.big);
        obj.myServer.registerCallback("/echo", myServer, myServer.echo);

        testFireAndForget();
        testGetRequest();
        testDeleteRequest();
        testHeadRequest();
        testPostRequest();
        testPutRequest();
        testBlocking();
        testError();
        testSmallRequests();
        testBigRequest();
        testRequestProgressError();
        testRequestAbort();

        obj.myServer = null;
        gc();
    };

};

var myTestName = "testHttpClient.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new HttpClientUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + ".");
exit(mySuite.returnStatus());
