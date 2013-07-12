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
// Description: Base class for WebSocketClient  
//
 */

#ifndef _ac_y60_async_websocket_client_h
#define _ac_y60_async_websocket_client_h

#include "../y60_netasync_settings.h" 
#include "Message.h"


#include <asl/dom/Nodes.h>
#include <asl/base/Block.h>
#include <asl/base/ReadWriteLock.h>

#include <netsrc/spidermonkey/jsapi.h>

#include <y60/jsbase/JSWrapper.h>
#include <y60/jsbase/JSBlock.h>

#include <map>

#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <curl/curl.h>

namespace y60 {
namespace async {
namespace websocket {
    struct Event : public boost::noncopyable {
        virtual JSObject * newJSObject(JSContext * cx, JSObject * theWrapper) {
            JSObject *obj = JS_NewObject(cx, NULL, NULL, NULL);
            jsval v = OBJECT_TO_JSVAL(theWrapper); 
            JS_SetProperty(cx, obj, "target", &v);
            return obj;
        };
        virtual ~Event() {} // ensure polymorphism
    };
    typedef boost::shared_ptr<Event> EventPtr; 

    struct OpenEvent : public Event {
        virtual JSObject * newJSObject(JSContext * cx, JSObject * theWrapper) {
            JSObject *obj = Event::newJSObject(cx, theWrapper);
            jsval v = jslib::as_jsval(cx, "open");
            JS_SetProperty(cx, obj, "type", &v);
            return obj;
        };
    };
    struct CloseEvent : public Event {
        CloseEvent(bool theCleanFlag, unsigned short theCode, const std::string & theReason) :
            wasClean(theCleanFlag), code(theCode), reason(theReason) {};
        bool wasClean;
        unsigned short code;
        std::string reason;
        virtual JSObject * newJSObject(JSContext * cx, JSObject * theWrapper) {
            JSObject *obj = Event::newJSObject(cx, theWrapper);
            jsval v = jslib::as_jsval(cx, "close");
            JS_SetProperty(cx, obj, "type", &v);

            v = jslib::as_jsval(cx, code);
            JS_SetProperty(cx, obj, "code", &v);
            v = jslib::as_jsval(cx, reason);
            JS_SetProperty(cx, obj, "reason", &v);
            return obj;
        };
    };

    struct MessageEvent : public Event {
        MessageEvent(MessagePtr theMessage) :
            m(theMessage) {};
        MessagePtr m;
        virtual JSObject * newJSObject(JSContext * cx, JSObject * theWrapper) {
            JSObject *obj = Event::newJSObject(cx, theWrapper);
            jsval v = jslib::as_jsval(cx, "message");
            JS_SetProperty(cx, obj, "type", &v);

            if (boost::shared_ptr<TextMessage> t = boost::dynamic_pointer_cast<TextMessage>(m)) {
                jsval v = jslib::as_jsval(cx, t->data);
                JS_SetProperty(cx, obj, "data", &v);
            } else if (boost::shared_ptr<BinaryMessage> b = boost::dynamic_pointer_cast<BinaryMessage>(m)) {
                jsval v = jslib::as_jsval(cx, b->data);
                JS_SetProperty(cx, obj, "data", &v);
            } else {
                AC_ERROR << "Unsupported Message type";
            }

            return obj;
        };
    };

    struct Frame : public boost::noncopyable {
        static const unsigned char CONTINUATION = 0x00; 
        static const unsigned char TEXT = 0x01; 
        static const unsigned char BINARY = 0x02; 
        static const unsigned char CONNECTION_CLOSE = 0x08; 
        static const unsigned char PING = 0x09; 
        static const unsigned char PONG = 0x0A;

        Frame(unsigned char theOpCode, bool theFinalFlag = true) :
            opcode(theOpCode), final(theFinalFlag),
            disconnect_after_sending(false)
        {};
        bool final;
        unsigned char rsv; 
        unsigned char opcode;
        bool masked;
        unsigned char masking_key[4];

        // implementation flags
        bool disconnect_after_sending;

        std::vector<asl::Unsigned8> payload; 
    };
    typedef boost::shared_ptr<Frame> FramePtr; 

    class Client : public boost::enable_shared_from_this<Client>, boost::noncopyable {
        public:
            static const unsigned short CONNECTING = 0;
            static const unsigned short OPEN = 1;
            static const unsigned short CLOSING = 2;
            static const unsigned short CLOSED = 3;
        private:
            JSContext * _jsContext;
            JSObject * _jsWrapper;
            JSObject * _jsOptsObject;
            unsigned short _readyState;
            std::string _host;
            std::string _port;
            std::string _hostport;
            std::string _path;
            boost::asio::ip::tcp::resolver _resolver;
            boost::asio::ip::tcp::socket _socket;
            boost::asio::streambuf _recv_buffer;
            boost::asio::streambuf _send_buffer;
            boost::asio::io_service::strand _writeStrand;
            std::map<std::string, std::string> _replyHeaders;
            std::string _secWebSocketKey;

            std::string closing_reason;
            unsigned int closing_code;

            // current fragment data
            FramePtr _incomingFrame;
            MessagePtr _incomingMessage;

            // JS interface
            asl::ReadWriteLock _lockEventQueue; // lock for _eventQueue;
            std::deque<EventPtr> _eventQueue;
            
        public:
            /// creates a new HttpClient
            Client(JSContext * cx, JSObject * theOpts, boost::asio::io_service & io);
            virtual ~Client();
            void connect();
            std::string debugIdentifier;
            void setWrapper(JSObject * theWrapper);
            void releaseJSWrapper();
            jsval getJSOption(const char * theName) const;
            JSBool setJSOption(const char * theName, jsval * vp);
            void processCallbacks();

            void send(const std::string& data);
            void send(const asl::Block & data);
            
        private:
            Client();
            void async_read_if_needed(std::size_t needed, void (Client::*contfunc)(const boost::system::error_code &, std::size_t)); 
            void onResolved(const boost::system::error_code& err, boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
            void onTCPConnection(const boost::system::error_code& error);
            void onHeadersSent(const boost::system::error_code& error);
            void onHeadersRead(const boost::system::error_code& error);
            void onFrameHeaderRead(const boost::system::error_code& error, std::size_t bytes_transferred);
            void onPayloadRead(const boost::system::error_code& error, std::size_t bytes_transferred);

            void processFinalFragment();
            void processMessageFrame();
            void processControlFrame();
            void processCloseFrame(const std::vector<unsigned char> & payload);

            // we use a boost::strand to prevent interleaving write calls
            // all members prefixed with _w_ should only be called/accessed from the
            // _writeStrand.
            std::deque<FramePtr> _w_messageQueue;
            std::deque<FramePtr> _w_controlQueue;
            FramePtr _w_outgoingFrame;
            void _w_sendNextFrame();
            void _w_sendCloseFrame(bool disconnect, int theCode = 0, const std::string & theReason = 0);
            void _w_sendPong(const std::vector<unsigned char> & payload);
            void _w_onFrameSent(const boost::system::error_code& error, std::size_t bytes_transferred);
            void _w_injectFrame(FramePtr f);
            void _w_queueFrame(FramePtr f);

            void failTheWebSocketConnection(unsigned int theCode, const char * theMessage = 0);
            bool hasCallback(const char * theName);

    };

} // websocket
} // async
} // y60
#endif
