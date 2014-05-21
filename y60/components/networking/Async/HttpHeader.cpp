//
// HttpHeader.h
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "HttpHeader.h"

namespace jslib {
    Y60_NETASYNC_DECL jsval as_jsval(JSContext *cx, const y60::async::http::Headers theValue) {
 		JSObject* headersObject = JS_NewObject(cx, NULL, NULL, NULL);
 		y60::async::http::Headers::const_iterator myIt = theValue.begin();
 		y60::async::http::Headers::const_iterator myItEnd = theValue.end();
 		for (; myIt != myItEnd; ++myIt) {
 			jsval myHeaderValue = as_jsval(cx, myIt->second);
			JS_SetProperty(cx, headersObject, myIt->first.c_str(), &myHeaderValue); 			
 		}
 		return OBJECT_TO_JSVAL(headersObject);
     }
} // namespace jslib

