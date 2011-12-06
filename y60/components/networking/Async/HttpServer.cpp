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
#include "Y60Request.h"

#include <string>
#include <y60/jsbase/JScppUtils.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace asl;
using namespace inet;

namespace y60 {
    
    HttpServer::HttpServer() :
        _myHttpServer(0),
        _myHttpServerThread(0),
        _myNumThreads(5)
    {
    }

    HttpServer::~HttpServer()
    {
        close();
        std::map<std::string, JSCallback>::iterator it;

        for (it = _myCallbacks.begin(); it != _myCallbacks.end(); ++it) {
            AC_WARNING << "deleting " << it->first;
            JS_RemoveRoot( it->second.context, &(it->second.functionValue));
        };
        _myCallbacks.clear();
    }
    
    bool HttpServer::start( string theServerAddress, string theServerPort ) {

        _myHttpServer = 
            HttpServerPtr(new http::server::server(  theServerAddress, 
                                                     theServerPort, 
                                                     ".", _myNumThreads,
                                                     _myRequestQueue) );

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
            
            if (JSVAL_IS_VOID(rval)) {
                myResponse.return_code = http::server::reply::no_content;
                myResponse.payload = "";
                myResponse.content_type = "text/plain";
                return myResponse;
            }
            if (JSVAL_IS_OBJECT(rval)) {
                JSObject * myJsObject;
                if(JS_ValueToObject(theCallback.context, rval, &myJsObject)) {
                    if (JS_IsArrayObject(theCallback.context, myJsObject)) {
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
                        
                        // headers
                        JS_GetElement(theCallback.context, myJsObject, 2, &curElement);
                        if (JSVAL_IS_OBJECT(curElement)) {
                            JSObject *headers = JSVAL_TO_OBJECT(curElement);
                            JSIdArray *props = JS_Enumerate(theCallback.context, headers);
                            for (int i = 0; props && i < props->length; ++i) {
                                jsid propid = props->vector[i];
                                jsval propname;
                                if (!JS_IdToValue(theCallback.context, propid, &propname)) {
                                    AC_WARNING << "Weird case";
                                    continue;
                                }
                                
                                std::string header_name;
                                if (!jslib::convertFrom(theCallback.context, propname, header_name)) {
                                    JS_ReportError(theCallback.context, 
                                             "HttpServer::handleRequest: header_name is not a string!");
                                }
                                
                                jsval propval;
                                if (!JS_GetProperty(theCallback.context, headers, header_name.c_str(), &propval)) {
                                    AC_WARNING << "Weird case";
                                    continue;
                                }
                                
                                std::string header_value;
                                if (!jslib::convertFrom(theCallback.context, propval, header_value)) {
                                    JS_ReportError(theCallback.context, 
                                             "HttpServer::handleRequest: header_value is not a a string!");
                                }
                                myResponse.headers.push_back(http::server::header(header_name, header_value));
                            }
                        }
                        return myResponse;
                    }
                }
            }
            
            // default (backwards-compatible): treat jsval as string...
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
        return !(_myRequestQueue.empty());
    }

    void HttpServer::handleRequest() {
        try {
            http::server::request rawRequest;
            while (_myRequestQueue.try_pop(rawRequest)) {
                std::string myPath = rawRequest.uri.substr(0, rawRequest.uri.find_first_of("?"));  
            
                y60::Y60Response myResponse;
                y60::Y60Request myRequest;
                myRequest.uri    = rawRequest.uri;
                myRequest.method = rawRequest.method;
                myRequest.body   = rawRequest.body;
            
                if (_myCallbacks.find(myPath) != _myCallbacks.end()) {
                    JSCallback myCallback = _myCallbacks[myPath]; 
                    myResponse = invokeCallback( myCallback, myRequest, myRequest.uri );
                } else if (_myCallbacks.find("*") != _myCallbacks.end()) {
                    JSCallback myCallback = _myCallbacks["*"]; 
                    myResponse = invokeCallback( myCallback, myRequest, myRequest.uri );
                } else {
                    myResponse.return_code  = http::server::reply::not_found; 
                    AC_ERROR << "No callback registered for path: \"" << myPath << "\"!";
                }
                // Fill out the reply to be sent to the client.
                http::server::reply rep;
                rep.content.append(myResponse.payload.c_str(), myResponse.payload.length());
                rep.status = myResponse.return_code;
                rep.headers.clear();

                bool hasContentLengthFlag = false;
                bool hasContentTypeFlag = false;
                for (std::vector<http::server::header>::size_type i = 0; i < myResponse.headers.size(); ++i) {
                    const http::server::header &myHeader = myResponse.headers[i];
                    hasContentLengthFlag = hasContentLengthFlag || (myHeader.name == "Content-Length");
                    hasContentTypeFlag   = hasContentTypeFlag || (myHeader.name == "Content-Type");
                    rep.headers.push_back(myHeader);
                }

                if (!hasContentLengthFlag) {
                    rep.headers.push_back(http::server::header("Content-Length",
                                boost::lexical_cast<std::string>(rep.content.size())));
                }
            
                if (!hasContentTypeFlag) {
                    http::server::header myContentType("Content-Type", "text/plain");
                    if ( myResponse.content_type.length() > 0 ) {
                        myContentType.value = myResponse.content_type;
                    }
                    rep.headers.push_back(myContentType);
                }
                rawRequest.conn->async_respond(rep); // this is threadsafe
                rawRequest.conn.reset(); // free shared_ptr
            }
        } catch (Exception e) {
            AC_ERROR << e;
        };
    }
}
