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


plug("NetAsync");
use("JSON.js");

var wsuri = "ws://localhost:9001";
var webSocket = null;
var currentCaseId = null;
var caseCount = null;
var agent = "Y60";
var done = false;

function updateStatus(s) { print(s);}

function openWebSocket(ws_uri)
{
    webSocket = new Async.WebSocket(ws_uri);
    return webSocket;
} 


function getCaseCount(cont)
{
  var ws_uri = wsuri + "/getCaseCount";
  webSocket = openWebSocket(ws_uri);
  webSocket.onmessage =
    function(e)
    {
      caseCount = JSON.parse(e.data);
      updateStatus("Will run " + caseCount + " cases ..");
    };
  webSocket.onclose =
    function(e)
    {
      cont();
    };
} 

function runNextCase()
{
  var ws_uri = wsuri + "/runCase?case=" + currentCaseId + "&agent=" + agent;
  webSocket = openWebSocket(ws_uri);
  webSocket.binaryType = "arraybuffer";
  webSocket.onopen =
    function(e)
    {
      updateStatus("Executing test case " + currentCaseId + "/" + caseCount);
    };
  webSocket.onclose =
    function(e)
    {
      webSocket = null;
      currentCaseId = currentCaseId + 1;
      if (currentCaseId <= caseCount)
      {
        runNextCase();
      }
      else
      {
        updateStatus("All test cases executed.");
        done = true;
        updateReports();
      }
    };
  webSocket.onerror = webSocket.onclose;
  webSocket.onmessage =
    function(e)
    {
      webSocket.send(e.data);
    };
} 

function updateReports()
{
    var ws_uri = wsuri + "/updateReports?agent=" + agent;
    webSocket = openWebSocket(ws_uri);
    webSocket.onopen =
        function(e)
        {
            updateStatus("Updating reports ..");
        }
    webSocket.onclose =
        function(e)
        {
            webSocket = null;
            updateStatus("Reports updated.");
            updateStatus("Test suite finished!");
        }
} 



currentCaseId = 1;
getCaseCount(runNextCase);

        while (true) {
            Async.onFrame();
            gc();
            msleep(1);
            if (done) {
                break;
            }
        }
