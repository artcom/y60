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

#ifndef INCL_Y60_HttpServer
#define INCL_Y60_HttpServer

#ifdef WIN32
#include <winsock2.h>
#endif

#include <asl/net/UDPConnection.h>
#include <asl/dom/Nodes.h>

#include <boost/thread.hpp>

#include <map>

#include "Connection.h"

#include <netsrc/spidermonkey/jsapi.h>

namespace y60 {
namespace async {
namespace http {

    struct JSCallback {
        JSContext*  context;
        JSObject*   object;
        jsval functionValue;
        std::string contentType;
    };

    class Server {

        public:
            /// creates a new HttpServer
            Server(JSContext * cx, boost::asio::io_service & theIOService);
            bool start( std::string theServerAddress,  std::string theServerPort );
            virtual ~Server();

            void close();

            void registerCallback(JSObject * theJSHttpServer, const std::string & theUri, JSCallback & myCallback )
            {

                // we protect the callbacks (which are JS Function objects) from the GC
                // by adding them as properties to a Function-Container object. This object is then attached 
                // as a property to the JSHttpServer wrapper object.
                // This will ensure that the callbacks are not GC'ed while the server still exists.
                jsval funcContainerProp;
               
                // get the 'callback' property if it already exists
                JS_GetProperty(_jsContext, theJSHttpServer, "callbacks", &funcContainerProp);

                if (JSVAL_IS_VOID(funcContainerProp)) { // property doesn't exists yet - create it
                    JSObject * funcContainerObject = JS_NewObject(_jsContext, 0,0,0);
                    funcContainerProp = OBJECT_TO_JSVAL(funcContainerObject);

                    // now add it as a property to the wrapper
                    JS_SetProperty(_jsContext, theJSHttpServer, "callbacks", &funcContainerProp);
                }
                // now store the function object as a property of the callback object
                JS_SetProperty(_jsContext, JSVAL_TO_OBJECT(funcContainerProp), theUri.c_str(), &myCallback.functionValue);

                // now add the callback to our internal map
                _myCallbacks[theUri] = myCallback;
            }

            void unregisterCallback( JSObject * theJSHttpServer, std::string theUri )
            {
                // remove the callback from our javascript Function-Container object,
                // so that it can be GC'ed
                jsval funcContainerProp;
                JS_GetProperty(_jsContext, theJSHttpServer, "callbacks", &funcContainerProp);
                if (JSVAL_IS_OBJECT(funcContainerProp)) {
                    JS_DeleteProperty(_jsContext, JSVAL_TO_OBJECT(funcContainerProp), theUri.c_str());
                }

                // remove the callback from our internal map
                if (_myCallbacks.find(theUri) != _myCallbacks.end()) {
                    _myCallbacks.erase( theUri );
                }
            }

            void handleRequest();
            bool requestsPending();

        private:
            Server(); // disbale default CTOR
    
            void invokeCallback(const JSCallback & theCallback, 
                                            const request & theRequest, reply & theReply); 

            JSContext * _jsContext;
            std::map<std::string, JSCallback> _myCallbacks;

            // this queue is the communication between the two threads
            ConcurrentQueue<request>          _myRequestQueue;

            /*
             * the following members run in the async's io_server thread
             */

            /// Handle completion of an asynchronous accept operation.
            void handle_accept(const boost::system::error_code& e);
            /// Acceptor used to listen for incoming connections.
            boost::asio::ip::tcp::acceptor acceptor_;

            /// The next connection to be accepted.
            connection_ptr new_connection_;
    };

} // http
} // async
} // y60
#endif
