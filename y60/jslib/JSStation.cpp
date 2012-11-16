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

// own header
#include "JSStation.h"

#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSWrapper.impl>
#include <y60/jsbase/JSBlock.h>
#include <asl/net/net_functions.h>

#include <iostream>

using namespace std;
using namespace asl;

namespace jslib {

    template class JSWrapper<asl::Station, asl::Ptr<asl::Station>, StaticAccessProtocol>;
    template struct Y60_JSLIB_DECL ArgumentHolder<JSBlock const&>;
    template struct Y60_JSLIB_DECL ArgumentHolder<asl::Block>;

    static JSBool
    closeStation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Close the station");
        DOC_END;
        return Method<JSStation::NATIVE>::call(&JSStation::NATIVE::closeStation,cx,obj,argc,argv,rval);
    }

    static JSBool 
    broadcastBlock(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Broadcast datablock");
        DOC_END;
        ensureParamCount(argc,1);
        
        asl::Ptr<asl::Block> myData;
        if (!convertFrom(cx, argv[0], myData)) {
            JS_ReportError(cx, "JSStation::broadcastBlock(): Argument #1 must be block (myData)!");
            return JS_FALSE;
        }
        JSStation::getJSWrapper(cx, obj).openNative().broadcast( *myData );
        JSStation::getJSWrapper(cx, obj).closeNative();
        return JS_TRUE;
    }

    static JSBool
    broadcastString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Broadcast a string");
        DOC_END;
        ensureParamCount(argc, 1);

        std::string myString;
        if (!convertFrom(cx, argv[0], myString)) {
            JS_ReportError(cx, "JSStation::broadcastString(): Argument #1 must be a string (myString)!");
            return JS_FALSE;
        }

        asl::Block myData;
        myData.appendString( myString );
        
        JSStation::getJSWrapper(cx, obj).openNative().broadcast( myData );
        JSStation::getJSWrapper(cx, obj).closeNative();

        return JS_TRUE;

    }

    static JSBool
    receive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("receive data from the station");
        DOC_RVAL("Object containing three properties: sender - the sender ip address, "
                 "id - the sender station id, data - a block containing the sent data", DOC_TYPE_OBJECT);
        DOC_END;

        ensureParamCount(argc, 0);

        asl::Ptr<asl::Block> myReplyData = asl::Ptr<asl::Block>(new asl::Block());
        unsigned long mySender;
        unsigned long mySenderStationID;

        bool myResult = JSStation::getJSWrapper(cx, obj).openNative().receive( *myReplyData, 
                                                                               mySender, 
                                                                               mySenderStationID );
        JSStation::getJSWrapper(cx, obj).closeNative();

        if (!myResult) {
            *rval = JSVAL_VOID;
        } else {

            JSObject * myReturnObject = JS_NewArrayObject(cx, 0, NULL);
            *rval = OBJECT_TO_JSVAL( myReturnObject );
            if (!JS_DefineProperty( cx, myReturnObject, "sender", 
                                    as_jsval( cx, asl::as_dotted_address(mySender) ), 
                                    0, 0, JSPROP_ENUMERATE )) 
            {
                JS_ReportError(cx, "Coulnd't create property \"sender\"" ); 
                return JS_FALSE;
            }
            if (!JS_DefineProperty( cx, myReturnObject, "id", 
                                    as_jsval( cx, mySenderStationID ), 0, 0, JSPROP_ENUMERATE )) 
            {
                JS_ReportError(cx, "Coulnd't create property \"id\"" ); 
                return JS_FALSE;
            }
            if (!JS_DefineProperty( cx, myReturnObject, "data",
                                    as_jsval( cx, myReplyData ), 0, 0, JSPROP_ENUMERATE ))
            {
                JS_ReportError(cx, "Coulnd't create property \"data\"" ); 
                return JS_FALSE;
            }
        }

        return JS_TRUE;
    }

    JSFunctionSpec *
    JSStation::Functions() {
        static JSFunctionSpec myFunctions[] = {
            // name                native                   nargs
            {"closeStation",       closeStation,                0},
            {"broadcastBlock",     broadcastBlock,              1},
            {"broadcastString",    broadcastString,             1},
            {"receive",            receive,                     0},
            {0}
        };
        return myFunctions;
    }

    JSPropertySpec *
    JSStation::Properties() {
        static JSPropertySpec myProperties[] = {
            {0}
        };
        return myProperties;
    }

    // getproperty handling
    JSBool
    JSStation::getPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSStation::getProperty: index %d out of range", theID);
        return JS_FALSE;
    }

    // setproperty handling
    JSBool
    JSStation::setPropertySwitch(unsigned long theID, JSContext *cx, JSObject *obj, jsval id, jsval *vp) {
        JS_ReportError(cx,"JSStation::setPropertySwitch: index %d out of range", theID);
        return JS_FALSE;
    }

    JSBool
    JSStation::Constructor(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
        DOC_BEGIN("Creates a new perlin noise object");
        DOC_PARAM("Broadcast Address", "IP-Broadcast Address", DOC_TYPE_STRING);
        DOC_PARAM("Broadcast Port", "Port of the remote stations", DOC_TYPE_INTEGER);
        DOC_PARAM("Receive Port", "IP Port of the local station", DOC_TYPE_INTEGER);
        DOC_PARAM("Own IP Address", "supply own address to avoid receiving self-sent packets", DOC_TYPE_STRING);
        DOC_PARAM("Station ID", "arbitrary unique ID to distinguish different stations", DOC_TYPE_INTEGER);
        DOC_PARAM("Network ID", "arbitrary group id, must be the same number for all participating nodes", DOC_TYPE_INTEGER);
        DOC_END;

        if (JSA_GetClass(cx,obj) != Class()) {
            JS_ReportError(cx,"Constructor for %s  bad object; did you forget a 'new'?",ClassName());
            return JS_FALSE;
        }

        OWNERPTR myNewStation = OWNERPTR(0);
        JSStation * myNewObject = 0;
                         
        if (argc == 0) {
            myNewStation = OWNERPTR(new Station());
        } else if (argc == 6) {
            std::string myBroadcastAddress;
            if (JSVAL_IS_VOID(argv[0]) || !convertFrom(cx, argv[0], myBroadcastAddress)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #1 must be string (myBroadcastAddress)!");
                return JS_FALSE;
            }
            unsigned short myBroadcastPort;
            if (JSVAL_IS_VOID(argv[1]) || !convertFrom(cx, argv[1], myBroadcastPort)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #2 must be Number (myBroadcastPort)!");
                return JS_FALSE;
            }
            unsigned short myReceivePort;
            if (JSVAL_IS_VOID(argv[2]) || !convertFrom(cx, argv[2], myReceivePort)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #3 must be Number (myReceivePort)!");
                return JS_FALSE;
            }
            std::string myOwnIPAddress;
            if (JSVAL_IS_VOID(argv[3]) || !convertFrom(cx, argv[3], myOwnIPAddress)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #4 must be String (myOwnIPAddress)!");
                return JS_FALSE;
            }
            unsigned long myStationID;
            if (JSVAL_IS_VOID(argv[4]) || !convertFrom(cx, argv[4], myStationID)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #5 must be Number (myStationID)!");
                return JS_FALSE;
            }
            unsigned long myNetworkID;
            if (JSVAL_IS_VOID(argv[5]) || !convertFrom(cx, argv[5], myNetworkID)) {
                JS_ReportError(cx, "JSStation::Constructor(): Argument #6 must be Number (myNetworkID)!");
                return JS_FALSE;
            }
            myNewStation = OWNERPTR(new Station( asl::hostaddress(myBroadcastAddress), 
                                                 myBroadcastPort,
                                                 myReceivePort, 
                                                 asl::hostaddress(myOwnIPAddress), 
                                                 myStationID,
                                                 myNetworkID ));
        } else {
            JS_ReportError(cx, "JSStation::Constructor(): Wrong number of arguments, expected 0 or 6, got %d.", 
                           argc);
            return JS_FALSE;
        }

        myNewObject = new JSStation(myNewStation, myNewStation.get());

        if (myNewObject) {
            JS_SetPrivate(cx,obj,myNewObject);
            return JS_TRUE;
        }
        JS_ReportError(cx,"JSStation::Constructor: bad parameters");
        return JS_FALSE;
    }

    JSConstIntPropertySpec *
    JSStation::ConstIntProperties() {
        static JSConstIntPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSPropertySpec *
    JSStation::StaticProperties() {
        static JSPropertySpec myProperties[] = {{0}};
        return myProperties;
    }

    JSFunctionSpec *
    JSStation::StaticFunctions() {
        static JSFunctionSpec myFunctions[] = {{0}};
        return myFunctions;
    }

    JSObject *
    JSStation::initClass(JSContext *cx, JSObject *theGlobalObject) {
        JSObject * myClass = Base::initClass(cx, theGlobalObject, ClassName(), Constructor, Properties(), 
                             Functions(), ConstIntProperties());
        DOC_MODULE_CREATE("Global", JSStation);
        return myClass;
    }

    bool convertFrom(JSContext *cx, jsval theValue, JSStation::NATIVE & theStation) {
        if (JSVAL_IS_OBJECT(theValue)) {
            JSObject * myArgument;
            if (JS_ValueToObject(cx, theValue, &myArgument)) {
                if (JSA_GetClass(cx,myArgument) == JSClassTraits<JSStation::NATIVE >::Class()) {
                    theStation = JSClassTraits<JSStation::NATIVE>::getNativeRef(cx,myArgument);
                    return true;
                }
            }
        }
        return false;
    }

    jsval as_jsval(JSContext *cx, JSStation::OWNERPTR theOwner) {
        JSObject * myReturnObject = JSStation::Construct(cx, theOwner, theOwner.get());
        return OBJECT_TO_JSVAL(myReturnObject);
    }

    jsval as_jsval(JSContext *cx, JSStation::OWNERPTR theOwner, JSStation::NATIVE * theStation) {
        JSObject * myObject = JSStation::Construct(cx, theOwner, theStation);
        return OBJECT_TO_JSVAL(myObject);
    }

}
