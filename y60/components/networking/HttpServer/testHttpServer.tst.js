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

/*jslint white : false*/
/*globals use plug UnitTest millisec Socket HttpServer ENSURE UnitTestSuite
          print exit RequestManager Request*/

use("UnitTest.js");

plug("Network");
plug("HttpServer");

var TIMEOUT = 1000;

function HttpServerUnitTest() {
    this.Constructor(this, "HttpServerUnitTest");
}

HttpServerUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    obj.run = function () {

        obj.callback_answer = "callback";
        obj.fallback_answer = "fallback";

        var myObj = {
            test : function (theMethod, theBody) {
                return obj.callback_answer;
            },
            fallback : function (theMethod, theBody) {
                return obj.fallback_answer;
            },
            detailed_response : function (theMethod, theBody, thePath) {
                return ["I have headers", "304", {
                    "Content-Length" : "5",
                    "X-PRODUCED-BY" : "Y60"
                }];
            },
            detailed_response_2 : function (theMethod, theBody, thePath) {
                    return ["I am Content", "201"];
            },
            detailed_response_3 : function (theMethod, theBody, thePath) {
                    return [];
            },
            detailed_response_4 : function (theMethod, theBody, thePath) {
                    return;
            }

        };

        obj.myServer = new HttpServer();

        obj.myServer.registerCallback("/test", myObj, myObj.test);
        obj.myServer.registerCallback("/foo", myObj, myObj.detailed_response);
        obj.myServer.registerCallback("/bar", myObj, myObj.detailed_response_2);
        obj.myServer.registerCallback("/krokodil", myObj, myObj.detailed_response_3);
        obj.myServer.registerCallback("/versicherung", myObj, myObj.detailed_response_4);
        obj.myServer.registerCallback("*", myObj, myObj.fallback);

        obj.myServer.start("0.0.0.0", "4042");

        obj.socket = new Socket(Socket.TCPCLIENT, "4041", "INADDR_ANY");
        obj.socket.connect("localhost", "4042");

        var myRequest = "GET /test HTTP/1.1\r\nHost: localhost:4042\r\n\r\n";
        obj.socket.write(myRequest);

        var time = millisec();
        while (!obj.myServer.requestsPending() && (millisec() - time < TIMEOUT)) {
        }

        obj.myServer.handleRequests();

        time = millisec();
        //UGLY
        while (!(obj.socket.peek(1) > 0) && (millisec() - time < TIMEOUT)) {
        }

        obj.response = obj.socket.read();
        obj.response = obj.response.substr(obj.response.search(/\r\n\r\n/) + 4);
        ENSURE("obj.response == obj.callback_answer");

        myRequest = "GET /somethingelse HTTP/1.1\r\nHost: localhost:4042\r\n\r\n";
        obj.socket.connect("localhost", "4042");
        obj.socket.write(myRequest);

        time = millisec();
        while (!obj.myServer.requestsPending() && (millisec() - time < TIMEOUT)) {
        }

        obj.myServer.handleRequests();

        time = millisec();
        // ARGS UGLY!
        while (!(obj.socket.peek(1) > 0) && (millisec() - time < TIMEOUT)) {
        }

        obj.response = obj.socket.read();
        obj.response = obj.response.substr(obj.response.search(/\r\n\r\n/) + 4);
        ENSURE("obj.response == obj.fallback_answer");
        
        // detailed responses
        
        // path: bar
        var myRequestManager  = new RequestManager();
        myRequest = new Request("http://localhost:4042/bar");
        myRequest.onDone = function () {
            obj.testResponse = this;
            ENSURE("obj.testResponse.responseCode == '201'");
            ENSURE("obj.testResponse.responseString == 'I am Content'");
            ENSURE("obj.testResponse.getResponseHeader('Content-Type') == 'text/plain'");
            ENSURE("obj.testResponse.getResponseHeader('Content-Length') == obj.testResponse.responseString.length");
        };
        myRequest.get();

        myRequestManager.performRequest(myRequest);
        while (myRequestManager.activeCount > 0) {
            myRequestManager.handleRequests();
            obj.myServer.handleRequests();
        }

        // path: foo
        myRequest = new Request("http://localhost:4042/foo");
        myRequest.onError = function () {
            obj.testResponse = this;
            ENSURE("obj.testResponse.responseCode == '304'");
            ENSURE("obj.testResponse.getResponseHeader('Content-Type') == 'text/plain'");
            ENSURE("obj.testResponse.getResponseHeader('X-PRODUCED-BY') == 'Y60'");
        };
        myRequest.get();
        
        myRequestManager.performRequest(myRequest);
        while (myRequestManager.activeCount > 0) {
            myRequestManager.handleRequests();
            obj.myServer.handleRequests();
        }

        // path: krokodil
        myRequest = new Request("http://localhost:4042/krokodil");
        myRequest.onDone = function () {
            obj.testResponse = this;
            ENSURE("obj.testResponse.responseString == ''");
            ENSURE("obj.testResponse.responseCode == '200'");
            ENSURE("obj.testResponse.getResponseHeader('Content-Type') == 'text/plain'");
        };
        myRequest.get();
        
        myRequestManager.performRequest(myRequest);
        while (myRequestManager.activeCount > 0) {
            myRequestManager.handleRequests();
            obj.myServer.handleRequests();
        }
        
        // path: versicherung
        myRequest = new Request("http://localhost:4042/versicherung");
        myRequest.onDone = function () {
            obj.testResponse = this;
            ENSURE("obj.testResponse.responseString == ''");
            ENSURE("obj.testResponse.responseCode == '204'");
            ENSURE("obj.testResponse.getResponseHeader('Content-Type') == 'text/plain'");
        };
        myRequest.get();
        
        myRequestManager.performRequest(myRequest);
        while (myRequestManager.activeCount > 0) {
            myRequestManager.handleRequests();
            obj.myServer.handleRequests();
        }
        
        obj.myServer.close();
    };

};

var myTestName = "testHttpServer.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new HttpServerUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + "");
exit(mySuite.returnStatus());
