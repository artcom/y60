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
// Description: Base class for HttpServer  
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

#ifndef INCL_Y60_HttpServer
#define INCL_Y60_HttpServer

#ifdef WIN32
#include <winsock2.h>
#endif

#include <asl/net/UDPConnection.h>
#include <asl/dom/Nodes.h>

#include <boost/thread.hpp>

#include <map>

#include "Server.h"
#include "Y60Request.h"

#include <netsrc/spidermonkey/jsapi.h>

namespace y60 {

    struct JSCallback {
        JSContext*  context;
        JSObject*   object;
        JSFunction* function;
        jsval functionValue;
        std::string contentType;
    };

    typedef asl::Ptr<http::server::server, dom::ThreadingModel> HttpServerPtr;
    typedef asl::Ptr<boost::thread, dom::ThreadingModel> HttpServerThreadPtr;

    class HttpServer {

        public:

            /// creates a new HttpServer
            HttpServer();
            bool start( std::string theServerAddress,  std::string theServerPort );
            virtual ~HttpServer();

            void close();

            void registerCallback( const std::string & theUri, const JSCallback & myCallback )
            {
                if (_myCallbacks.find(theUri) != _myCallbacks.end()) {
                    // the old callback can now be garbage collected
                    JS_RemoveRoot( _myCallbacks[theUri].context, &(_myCallbacks[theUri].functionValue));
                }
                _myCallbacks[theUri] = myCallback;
                // root callback value to prevent garbage collection of anonymous callback functions
                JS_AddRoot( _myCallbacks[theUri].context, &(_myCallbacks[theUri].functionValue));
            }

            void unregisterCallback( std::string theUri )
            {
                if (_myCallbacks.find(theUri) != _myCallbacks.end()) {
                    JS_RemoveRoot( _myCallbacks[theUri].context, &(_myCallbacks[theUri].functionValue));
                    _myCallbacks.erase( theUri );
                }
            }

            void handleRequest();
            bool requestsPending();


        private:
    
            y60::Y60Response invokeCallback(const JSCallback & theCallback, 
                                            const y60::Y60Request & theRequest,
                                            const std::string & theURI ); 

            HttpServerPtr                     _myHttpServer;
            HttpServerThreadPtr               _myHttpServerThread;
            std::size_t                       _myNumThreads;
            Y60RequestQueuePtr                _myRequestQueue;
            Y60ResponseQueuePtr               _myResponseQueue;

            std::map<std::string, JSCallback> _myCallbacks;

    };

}
#endif
