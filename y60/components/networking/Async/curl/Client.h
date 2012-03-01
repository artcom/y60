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
 */

#ifndef _ac_y60_async_http_client_h
#define _ac_y60_async_http_client_h

#include "SocketAdapter.h"

#ifdef WIN32
#include <winsock2.h>
#endif

#include <asl/net/UDPConnection.h>
#include <asl/dom/Nodes.h>

#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <map>

#include <curl/curl.h>
#include <asl/base/ReadWriteLock.h>
#include <netsrc/spidermonkey/jsapi.h>
#include <y60/jsbase/JSWrapper.h>

namespace y60 {
namespace async {
namespace http {
namespace curl {
    
    /*
    struct JSCallback {
        JSContext*  context;
        JSObject*   object;
        jsval functionValue;
        std::string contentType;
    };
    */

    class MultiAdapter;

    class Client : public boost::enable_shared_from_this<Client> {
        public:
            CURL * _curlHandle;
        private:
            JSContext * _jsContext;
            JSObject * _jsWrapper;
            JSObject * _jsOptsObject;
            std::vector<char>   _myErrorBuffer;
            asl::Ptr<asl::Block> _privateResponseBuffer; // filled in io_service thread, emptied in JS thread
            asl::ReadWriteLock _lockResponseBuffer; // lock for _privateResponseBuffer;
            asl::Ptr<asl::Block> _myResponseBlock; // used only in JS thread. 
            bool _read_in_progress;
            bool _write_in_progress;
            std::string _debugIdentifier;
        public:
            /// creates a new HttpClient
            Client(JSContext * cx, JSObject * theOpts);
            virtual ~Client();
            void setWrapper(JSObject * theWrapper);
            curl_socket_t getCurlSocket();
            void get();
            void onDone(MultiAdapter * parent, CURLcode result);
            void onProgress();
            std::string getResponseString() const;
            const asl::Ptr<asl::Block> & getResponseBlock() const;
            
            template<typename T>
            bool setCurlOption(JSObject* opts, std::string theProperty, CURLoption theCurlOption, T * theValue) {
                jsval propValue;
                T nativeValue;

                JS_GetProperty(_jsContext, opts, theProperty.c_str(), &propValue);
                if (JSVAL_IS_VOID(propValue)) {
                    return false;
                }
                if (!jslib::convertFrom(_jsContext, propValue, nativeValue)) {
                    JS_ReportError(_jsContext, "Type mismatch on %s", theProperty.c_str());
                }
                CURLcode myStatus = curl_easy_setopt(_curlHandle, theCurlOption, nativeValue);
                checkCurlStatus(myStatus, PLUS_FILE_LINE);
                AC_DEBUG << "set " << nativeValue;
                if (theValue) {
                    *theValue = nativeValue;
                }
                return true;
            };
        private:
            Client();

            void checkCurlStatus(CURLcode theStatusCode, const std::string & theWhere); 
            bool hasCallback(const char * theName);

            size_t writeFunction(const unsigned char *ptr, size_t size);
            static size_t _writeFunction(unsigned char *ptr, size_t size, size_t nmemb, Client *self) {
                AC_DEBUG << "calling writefunction for " << self;
                if (self) {
                    return self->writeFunction(ptr, size*nmemb);
                } else {
                    return 0;
                }
            };

            curl_socket_t openSocket(curlsocktype purpose, struct curl_sockaddr *addr);
            static curl_socket_t _openSocket(Client *self, curlsocktype purpose, struct curl_sockaddr *addr) {
                return self->openSocket(purpose, addr);
            };
            
            static int _closeSocket(Client *self, curl_socket_t item);
    };
    template<>
    bool Client::setCurlOption<std::string>(JSObject* opts, std::string theProperty, CURLoption theCurlOption, std::string * theValue);

} // curl
} // http
} // async
} // y60
#endif
