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

function WebSocketUnitTest() {
    this.Constructor(this, "WebSocketUnitTest");
}

WebSocketUnitTest.prototype.Constructor = function (obj, theName) {

    UnitTest.prototype.Constructor(obj, theName);

    function performTest(opts) {
        var done;

        if ('onclose' in opts) {
            var f = opts.onclose;
            opts.onclose = function(evt) { f(evt); done = true; };
        } else {
            opts.onclose = function(evt) { done = true; };
        }

        var websocket = new Async.WebSocket(opts);
        done = false;
        while (true) {
            Async.onFrame();
            gc();
            msleep(50);
            if (done) {
                break;
            }
        }

    }

    function testConnectionRefused() {

        performTest( { url: "ws://localhost:1234/timeinfo" ,
                       onclose: function(evt) { obj.code = evt.code; } 
                   });
        ENSURE("obj.code == 1006");

    }

    function testEcho() {
        performTest( { url: "ws://echo.websocket.org" ,
                       onopen: function(evt) { this.send("Hello World"); }, 
                       onmessage: function(m) { obj.data = m.data; this.close(3000,"hello"); },
                       onclose: function(evt) { obj.code = evt.code; } 
                   });
        ENSURE('obj.data == "Hello World"');
        ENSURE('obj.code == 3000');
        
        performTest( { url: "ws://echo.websocket.org" ,
                       onopen: function(evt) { this.send("Hello World"); }, 
                       onmessage: function(m) { obj.data = m.data; this.close(); },
                       onclose: function(evt) { obj.code = evt.code; } 
                   });
        ENSURE('obj.data == "Hello World"');
        ENSURE('obj.code == 1000');
    }

    obj.run = function () {
        testConnectionRefused();
        // testEcho();
        gc();
    };

};

var myTestName = "testWebSocket.tst.js";
var mySuite = new UnitTestSuite(myTestName);

mySuite.addTest(new WebSocketUnitTest());
mySuite.run();

print(">> Finished test suite '" + myTestName + "', return status = " + mySuite.returnStatus() + ".");
exit(mySuite.returnStatus());
