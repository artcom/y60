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
//============================================================================= */

#include "TimeSyncBarrier.h"
#include <y60/glutil/GLUtils.h>

using namespace std;
using namespace asl;
using namespace jslib;
using namespace y60;


TimeSyncBarrier::TimeSyncBarrier(DLHandle theDLHandle) :
    PlugInBase(theDLHandle),
    IRendererExtension("TimeSyncBarrier"),
    _mySocket(0),
    _myAddress("localhost"),
    _myPort(3456)
{
}

void
TimeSyncBarrier::foo() {
}

void
TimeSyncBarrier::onStartup(jslib::AbstractRenderWindow * theWindow) {
    // TODO:: 
    //  - try to connect to the sync master
    //  - setup blocking socket
    
    
    // set abstractrenderwindow to external time mode
    theWindow->setUseExternalTimeSource(true);

    unsigned long myRemoteHostAddress = asl::hostaddress(_myAddress);
    _mySocket = inet::TCPClientSocketPtr(new inet::TCPClientSocket());
    _mySocket->setRemoteAddr(myRemoteHostAddress, _myPort);
    _mySocket->connect();
}

bool
TimeSyncBarrier::onSceneLoaded(jslib::AbstractRenderWindow * theWindow) {
    return false;
}

void
TimeSyncBarrier::handle(AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {
}

void
TimeSyncBarrier::onFrame(AbstractRenderWindow * theWindow , double t) {
}

void
TimeSyncBarrier::onPreRender(AbstractRenderWindow * theRenderer) {
}

void
TimeSyncBarrier::onPostRender(AbstractRenderWindow * theRenderer) {
    // TODO:: 
    //  - tell syncmaster that the current frame has been rendered 
    //  - wait for master reply
    //  - set application time
   
    std::string ready = "<?xml version='1.0' encoding='utf-8'?><ready/>";
    _mySocket->send(ready.c_str(), ready.size());
    
    char myBuffer[2048];
    unsigned int readBytes = _mySocket->receive(&myBuffer, sizeof(myBuffer));
    
    std::string serverTime(myBuffer, readBytes); 
    
    glFlush();


    dom::NodePtr myNode = dom::NodePtr(new dom::Node());
    if (myNode->parseAll(serverTime)) {
        dom::NodePtr myTimeNode = myNode->childNode("time", 0);
        double myTime = asl::as<double>(myTimeNode->firstChild()->nodeValue());
        theRenderer->setFrameTime(myTime);
    
    } else {
        // TODO: process error
    }

}

static JSBool
Foo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("");
    DOC_END;
    asl::Ptr<TimeSyncBarrier> myNative = getNativeAs<TimeSyncBarrier>(cx, obj);
    if (myNative) {
        myNative->foo();
    }
    
    return JS_TRUE;
}

JSFunctionSpec *
TimeSyncBarrier::Functions() {
    AC_DEBUG << "TimeSyncBarrier::Functions";
    static JSFunctionSpec myFunctions[] = {
        {"foo", Foo, 0},
        {0}
    };
    return myFunctions;
}


extern "C"
EXPORT PlugInBase* TimeSyncBarrier_instantiatePlugIn(DLHandle myDLHandle) {
    return new TimeSyncBarrier(myDLHandle);
}

