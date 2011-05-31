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
#include "HttpServer.h"

#include <string>
#include <y60/jsbase/JScppUtils.h>
#include <boost/bind.hpp>

using namespace std;
using namespace asl;
using namespace inet;

namespace y60 {
    
    HttpServer::HttpServer() :
        _myHttpServer(0),
        _myHttpServerThread(0),
        _myNumThreads(5)
    {
        _myRequestQueue  = Y60RequestQueuePtr(new Y60RequestQueue);
        _myResponseQueue = Y60ResponseQueuePtr(new Y60ResponseQueue);
    }

    HttpServer::~HttpServer()
    {
        close();
    }
    
    bool HttpServer::start( string theServerAddress, string theServerPort ) {

        _myHttpServer = 
            HttpServerPtr(new http::server::server(  theServerAddress, 
                                                     theServerPort, 
                                                     ".", _myNumThreads,
                                                     _myRequestQueue,
                                                     _myResponseQueue) );

        _myHttpServerThread = 
            HttpServerThreadPtr(new boost::thread( boost::bind( &http::server::server::run,
                                                                &(*_myHttpServer) ) ) );

        return true; 

    }

    void HttpServer::close() {
        _myHttpServer->stop();
        _myHttpServerThread->join();
    }

    y60::Y60Response HttpServer::invokeCallback( const JSCallback & theCallback, 
                                            const y60::Y60Request & theRequest,
                                            const std::string & thePath ) 
    {
        std::string myResponseString;
        y60::Y60Response myResponse;
        
        try {
            jsval argv[3], rval;

            argv[0] = jslib::as_jsval(theCallback.context, theRequest.method);
            argv[1] = jslib::as_jsval(theCallback.context, theRequest.body);
            argv[2] = jslib::as_jsval(theCallback.context, thePath); 

            jslib::JSA_CallFunctionValue(theCallback.context, theCallback.object, theCallback.functionValue, 
                                         3, argv, &rval);

            // check if rval is array-like or string-like...
            // sadly the convertFrom will also succeed on arrays and objects since the
            // implement toString() -> use JS_IsArrayObject
            
            if (JSVAL_IS_VOID(rval)) {
                // TODO return default no-content response (inkl. status)
                //myResponse.headers.

                return myResponse;
            }
            if (JSVAL_IS_OBJECT(rval)) {
                JSObject * myJsObject;
                if(JS_ValueToObject(theCallback.context, rval, &myJsObject)) {
                    AC_PRINT << theCallback.context << " - " << myJsObject;
                    if (JS_IsArrayObject(theCallback.context, myJsObject)) {
                        AC_PRINT << "return value is of type Array (via JS_IsArrayObject)";
                        //JS_GetElement(JSContext *cx, JSObject *obj, jsint index, jsval *vp);

                        jsval curElement;
                        
                        // Body
                        JS_GetElement(theCallback.context, myJsObject, 0, &curElement);
                        if (!JSVAL_IS_VOID(curElement)) {
                            jslib::convertFrom(theCallback.context, curElement, myResponse.payload);
                        }
                        
                        // status
                        
                        JS_GetElement(theCallback.context, myJsObject, 1, &curElement);
                        if (!JSVAL_IS_VOID(curElement)) {
                            int myStatusCode;
                            jslib::convertFrom(theCallback.context, curElement, myStatusCode);
                            myResponse.return_code = static_cast<http::server::reply::status_type>(myStatusCode);
                        } else {
                            myResponse.return_code = http::server::reply::ok;
                        }
                        
                        
                        
                        
                        
                        return myResponse;
                    }
                }
            }
            
            if (!jslib::convertFrom(theCallback.context, rval, myResponseString)) {
                JS_ReportError(theCallback.context, 
                         "HttpServer::handleRequest: Callback does not return a string!");
            }
            
            myResponse.payload      = myResponseString;
            myResponse.return_code  = http::server::reply::ok;
            myResponse.content_type = theCallback.contentType;
            return myResponse;
            
        } catch (Exception e) {
            myResponse.return_code  = http::server::reply::internal_server_error;
            myResponse.content_type = "text/plain";
            myResponse.payload      = std::string("An internal error occured: ") + asl::compose_message(e);
            AC_ERROR << e;
            return myResponse;
        };
    }

    bool HttpServer::requestsPending() {
        return !(_myRequestQueue->empty());
    }

    void HttpServer::handleRequest() {

        try {
            y60::Y60Request myRequest;
            bool hasRequest = _myRequestQueue->try_pop(myRequest);
            if (!hasRequest) { 
                return;
            }
            
            std::string myResponseString;
            std::string myPath = myRequest.uri.substr(0, myRequest.uri.find_first_of("?"));  
            
            y60::Y60Response myResponse;
            
            //invoke callback will not only return a string...
            //if it returns an array - how do the headers look like?
            //they are basically just key-value pairs (see HttpHeader.h) so in
            //js they could just be objects?!
            
            if (_myCallbacks.find(myPath) != _myCallbacks.end()) {
                JSCallback myCallback = _myCallbacks[myPath]; 
                myResponse = invokeCallback( myCallback, myRequest, myRequest.uri );
                //myResponseString = invokeCallback( myCallback, myRequest, myRequest.uri );
                //myResponse.payload      = myResponseString; 
                //myResponse.return_code  = http::server::reply::ok; 
                //myResponse.content_type = myCallback.contentType;
            } else if (_myCallbacks.find("*") != _myCallbacks.end()) {
                JSCallback myCallback = _myCallbacks["*"]; 
                myResponse = invokeCallback( myCallback, myRequest, myRequest.uri );
                //myResponseString = invokeCallback( myCallback, myRequest, myRequest.uri );
                //myResponse.payload      = myResponseString; 
                //myResponse.return_code  = http::server::reply::ok; 
                //myResponse.content_type = "text/plain";
            } else {
                myResponse.return_code  = http::server::reply::not_found; 
                AC_ERROR << "No callback registered for path: \"" << myPath << "\"!";
            }

            _myResponseQueue->push( myResponse );

        } catch (Exception e) {
            AC_ERROR << e;
        };
    }
}