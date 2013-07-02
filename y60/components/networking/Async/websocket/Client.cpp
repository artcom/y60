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
#include "Client.h"
#include "../NetAsync.h"

#include <string>
#include <y60/jsbase/JScppUtils.h>
#include <y60/jsbase/JSBlock.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <netsrc/spidermonkey/jsapi.h>

using namespace std;
using namespace asl;
using namespace jslib;

namespace y60 {
namespace async {
namespace websocket {

    Client::Client(JSContext * cx, JSObject * theURL) :
        _jsContext(cx)
    {
        ostringstream s;
        s << "WebSocketClient::" << hex << this;
        // TODO get URL from theURL
        std::string url = "ws://echo.websocket.org";
        s << " " << url;
        AC_DEBUG << "creating client " << this << " with _jsOptsObject " << _jsOptsObject;
        debugIdentifier = s.str();

        if(!JS_AddNamedRoot(_jsContext, &_jsOptsObject, debugIdentifier.c_str())) {
            AC_WARNING << "failed to root request object!";
        }
    
    }

    void 
    Client::setWrapper(JSObject * theWrapper) {
        _jsWrapper = theWrapper;
        // add root to prevent garbage collection of client and its callbacks 
        if(!JS_AddNamedRoot(_jsContext, &_jsWrapper, debugIdentifier.c_str())) {
            AC_WARNING << "failed to root request object!";
        }
    }

    Client::~Client()
    {
        AC_DEBUG << "~Client " << this;
    }
   
JSBool
JSA_CallFunctionName(JSContext * cx, JSObject * theThisObject, JSObject * theObject, const char * theName, uintN argc, jsval argv[], jsval *rval) {
    jsval myValue;
    if (JS_GetProperty(cx, theObject, theName, &myValue)) {
        if (JS_TypeOfValue(cx, myValue) != JSTYPE_FUNCTION) {
            AC_WARNING << "Property '" << theName << "' is not a function: type=" << JS_TypeOfValue(cx, myValue);
            return false;
        }
    }
    try {
        AC_DEBUG << "cx:" << cx << ", this:" << theThisObject << ", obj:" << theObject << ", theName:" << theName << ", argc:" << argc << ", argv:" << argv << ", rval:" << rval;
        JSBool ok = JS_CallFunctionValue(cx, theThisObject, myValue, argc, argv, rval);
        if (!ok) {
            AC_DEBUG << "Exception while calling js function '" << theName << "'" << endl;
        }
        return ok;
    } HANDLE_CPP_EXCEPTION;
};


    bool
    Client::hasCallback(const char * theName) {
        jsval myValue;
        if (JS_GetProperty(_jsContext, _jsOptsObject, theName, &myValue)) {
            if (JS_TypeOfValue(_jsContext, myValue) == JSTYPE_FUNCTION) {
                return true;
            }
        }
        return false;
    }

}
}
}
