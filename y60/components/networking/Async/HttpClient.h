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
// Description: Base class for HttpClient  
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

#ifndef INCL_Y60_HttpClient
#define INCL_Y60_HttpClient

#ifdef WIN32
#include <winsock2.h>
#endif

#include <asl/net/UDPConnection.h>
#include <asl/dom/Nodes.h>

#include <boost/thread.hpp>
#include <boost/asio.hpp>

#include <map>

#include "Connection.h"

#include <curl/curl.h>
#include <netsrc/spidermonkey/jsapi.h>

namespace y60 {
namespace async {
namespace http {

    /*
    struct JSCallback {
        JSContext*  context;
        JSObject*   object;
        jsval functionValue;
        std::string contentType;
    };
    */

    class Client {
        public:
            CURL * _curlHandle;
            boost::asio::ip::tcp::socket _socket;
        private:
            const std::string _myURI;
            JSContext * _jsContext;
            JSObject * _jsObject;
            std::vector<char>   _myErrorBuffer;
            int _socketState;
            bool _read_in_progress;
            bool _write_in_progress;
        public:
            /// creates a new HttpClient
            Client(const std::string & theURI);
            virtual ~Client();
            void setJSObject(JSContext * cx, JSObject * theObject) { _jsContext = cx; _jsObject = theObject; };
            void get();
            void onDone();
            void onSocketState(int theAction);
        private:
            Client(); // disbale default CTOR

            void handleRead(const boost::system::error_code& error);
            void handleWrite(const boost::system::error_code& error);
            void handleOperations();

            void checkCurlStatus(CURLcode theStatusCode, const std::string & theWhere) const;
            bool hasCallback(const char * theName);

            size_t writeFunction( char *ptr, size_t size, size_t nmemb);
            static size_t _writeFunction( char *ptr, size_t size, size_t nmemb, Client *self) {
                return self->writeFunction(ptr, size, nmemb);
            };

            curl_socket_t openSocket(curlsocktype purpose, struct curl_sockaddr *addr);
            static curl_socket_t _openSocket(Client *self, curlsocktype purpose, struct curl_sockaddr *addr) {
                return self->openSocket(purpose, addr);
            };
            /*
            int closeSocket(curl_socket_t item);
            static int _closeSocket(Client *self, curl_socket_t item) {
                // return self->closeSocket(item);
            };
            */
    };

} // http
} // async
} // y60
#endif
