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
//   $RCSfile: testRendererExtension.tst.js,v $
//   $Author: martin $
//   $Revision: 1.7 $
//   $Date: 2005/01/03 10:46:01 $
//
//
//=============================================================================

use("SceneViewer.js");
plug("Network")

window = new RenderWindow();

function TimeServer(theArguments) {
    this.Constructor(this, theArguments);
}

TimeServer.prototype.Constructor = function(self, theArguments) {

    //////////////////////////////////////////////////////////////////////
    //
    // Constructor
    //
    //////////////////////////////////////////////////////////////////////

    SceneViewer.prototype.Constructor(self, theArguments);
    var Base = [];

    //////////////////////////////////////////////////////////////////////
    //
    // public members
    //
    //////////////////////////////////////////////////////////////////////

    var _myServerSocket = null;
    var _mySockets = [];
    var _myRefTime = 0;
    
    // setup
    Base.setup = self.setup;
    self.setup = function(theWidth, theHeight, theTitle) {
        Base.setup(theWidth, theHeight, false, theTitle);
       
        _myServerSocket = new TCPServer("0.0.0.0", 3456); 
        _myServerSocket.setBlockingMode(false);
        
        window.swapInterval = 0;
    }

    Base.onFrame = self.onFrame;
    self.onFrame = function(theTime) {
        Base.onFrame(theTime);
        
        var mySocket = _myServerSocket.waitForConnection();
        if (mySocket) {
            _mySockets.push(mySocket); 
            mySocket.setBlockingMode(true);

            print("added new Socket: " + mySocket);
        }

        var i=0;
        // wait for ready messages for everyone
        for (i=0; i<_mySockets.length; i++) {
            try {
                mySocket = _mySockets[i];
                // get READY message from clients
                var myMessage = mySocket.read();
                
            } catch(ex) {
                print("removing socket " + mySocket);
                mySocket.close();
                _mySockets.splice(i,1);
            }
        }
        
        for (i=0; i<_mySockets.length; i++) {
            try {
                mySocket = _mySockets[i];
                var myTimeFrame = new Node("<time>" + (theTime - _myRefTime) +"</time>");
                mySocket.write("<?xml version='1.0' encoding='utf-8'?>"+myTimeFrame);

            } catch(ex) {
                print("removing socket " + mySocket);
                mySocket.close();
                _mySockets.splice(i,1);
            }
        }
    }

    // Base.onKey = self.onKey;
    // self.onKey = function() {
    //     _myRefTime  = self.getCurrentTime();
    // }
    self.onMouseButton = function() {
        _myRefTime  = self.getCurrentTime();
    }

    Base.onExit = self.onExit;
    self.onExit = function() {
        
        for (var i=0; i<_mySockets.length; i++) {
            mySocket = _mySockets[i];
            mySocket.close();
        }
        
        Base.onExit();
    }
}

//
// main
//

var ourTimeServer = new TimeServer();
ourTimeServer.setup(320, 200, "RendererExtensionTest");
ourTimeServer.go();
