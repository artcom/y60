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
#include <boost/regex.hpp>

#include <netsrc/spidermonkey/jsapi.h>

#ifdef __APPLE__
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include <openssl/sha.h>
#include <openssl/rand.h>

using namespace std;
using namespace asl;
using namespace jslib;
using boost::asio::ip::tcp;

namespace y60 {
namespace async {
namespace websocket {

#define Magic_UUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11" 

    Client::Client(JSContext * cx, JSObject * theOpts) :
        _jsContext(cx),
        _resolver(dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName))->io_service()),
        _socket(dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName))->io_service()),
        _jsOptsObject(theOpts),
        _readyState(CONNECTING)
    {
        ostringstream s;
        s << "WebSocketClient::" << hex << this;
        jsval propValue;
        std::string theURL;

        JS_GetProperty(_jsContext, _jsOptsObject, "url", &propValue);
        if (JSVAL_IS_STRING(propValue)) {
            jslib::convertFrom(_jsContext, propValue, theURL);
        }
        s << " '" << theURL << "'";
        debugIdentifier = s.str();
        AC_TRACE << "creating client " << debugIdentifier;

        /*
        if(!JS_AddNamedRoot(_jsContext, &_jsOptsObject, debugIdentifier.c_str())) {
            AC_WARNING << "failed to root request object!";
        }
        */

        // parse the URI and start an async name resolver query
        string myProtocol, myLogin;
        asl::parseURI(theURL, &myProtocol, &myLogin, &_path);
        if (myProtocol != "ws") {
            throw asl::NotYetImplemented("Only ws:// supported for now", PLUS_FILE_LINE);
        };
        string::size_type myCredentialServerDelimiter = myLogin.find("@");
        if (myCredentialServerDelimiter != string::npos) {
            throw asl::NotYetImplemented("Authetification not yet supported for websockets", PLUS_FILE_LINE);
        };
        string::size_type myHostPortDelimiter = myLogin.find(":");
        if (myHostPortDelimiter == string::npos) {
            _host = myLogin;
            _port = "80";
        } else {
            _host = myLogin.substr(0, myHostPortDelimiter);
            _port = myLogin.substr(myHostPortDelimiter+1);
        }

        AC_TRACE << "Resolving " << _host << " on Port " << _port << "...";
        
        tcp::resolver::query query(_host, _port);
        _resolver.async_resolve(query,
            boost::bind(&Client::onResolved, this, 
                boost::asio::placeholders::error, 
                    boost::asio::placeholders::iterator));
    }

    void 
    Client::onResolved(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator) {
        if (!error) {
            AC_TRACE << "resolved!" << endpoint_iterator->host_name() << ":" << endpoint_iterator->service_name();
            AC_TRACE << "now established TCP connection...";
            async_connect(_socket, endpoint_iterator,
                boost::bind(&Client::onTCPConnection, this,
                boost::asio::placeholders::error));
            // prepare the secure key
            //
            unsigned char nonce[16]; 
            RAND_bytes(nonce, sizeof(nonce));
            binToBase64(nonce, sizeof(nonce), _secWebSocketKey, cb64);
        } else {
             AC_ERROR << error.message();
             // TODO: call onError
        }
    }

    void 
    Client::onTCPConnection(const boost::system::error_code& error) {
        if (!error) {
            AC_TRACE << "TCP connection established. Sending WS Handshake...";
            std::ostream request_stream(&_send_buffer);
            request_stream << "GET " << _path << " HTTP/1.1\r\n";
            request_stream << "Host: " << _host << "\r\n";
            request_stream << "Upgrade: websocket\r\n";
            request_stream << "Connection: Upgrade\r\n";
            request_stream << "Sec-WebSocket-Key: " << _secWebSocketKey << "\r\n";
            request_stream << "Sec-WebSocket-Version: 13\r\n";
            request_stream << "\r\n";

            // send the request
            boost::asio::async_write(_socket, _send_buffer,
                boost::bind(&Client::onHeadersSent, this,
                boost::asio::placeholders::error));
        } else {
             AC_ERROR << error.message();
             // TODO: call onError
        }
    }
                
    void 
    Client::onHeadersSent(const boost::system::error_code& error) {
        if (!error) {
            AC_TRACE << "WS Handshake sent...waiting for response...";
            // Read the response headers, which are terminated by a blank line.
            boost::asio::async_read_until(_socket, _recv_buffer, "\r\n",
                boost::bind(&Client::onHeadersRead, this,
                boost::asio::placeholders::error));
        } else {
             AC_ERROR << error.message();
             // TODO: call onError
        }
    }


    void 
    Client::onHeadersRead(const boost::system::error_code& error) {
        if (!error) {
            // parse the response headers.
            std::istream response_stream(&_recv_buffer);
            std::string header;
            int statusCode = -1;
            while (std::getline(response_stream, header)) {
                // first line should be status line
                if (statusCode == -1) {
                    boost::regex expr("HTTP/([^\\s]*)\\s([^\\s]*)\\s([\\w\\s]+)[\r\n]+");
                    boost::smatch what;
                    if (boost::regex_match(header, what, expr)) {
                        statusCode = as<int>(what[2]);
                        AC_TRACE << "Received HTTP status code " << statusCode << " from server";
                    }
                } else {
                    boost::regex expr("([^\\s]*): (.*)[\r\n]+");
                    boost::smatch what;
                    if (boost::regex_match(header, what, expr)) {
                        AC_TRACE << "Received Header: '" << what[1] << "' -> '" << what[2] << "'";
                        _replyHeaders.insert(make_pair(what[1], what[2]));
                    }
                }
            }
            // validate the connection
            if (statusCode != 101) {
                AC_ERROR << "Unsupported status code received from server: " << statusCode;
                // TODO: call onError, close connection
            }
            string concatenatedKey = _secWebSocketKey + Magic_UUID;
            unsigned char sha1_digest[SHA_DIGEST_LENGTH];
            SHA1(reinterpret_cast<const unsigned char*>(concatenatedKey.c_str()),concatenatedKey.size(),sha1_digest);
            string expectedValue;
            binToBase64(sha1_digest, sizeof(sha1_digest), expectedValue, cb64);
            if (expectedValue != _replyHeaders["Sec-WebSocket-Accept"]) {
                AC_ERROR << "Expected " << expectedValue << " from Server, received " << _replyHeaders["Sec-WebSocket-Accept"];
            }
            _readyState = OPEN;
            AC_DEBUG << "WebSocket connection OPEN to " << debugIdentifier;

            // wait for incoming frame. The header will be at least 2 bytes, possibly more.
            // We won't know how long the header will be until we have the first two bytes. 
            // boost::asio::async_read(_socket, _recv_buffer, boost::asio::transfer_at_least(2), 
            boost::asio::async_read(_socket, _recv_buffer, boost::asio::transfer_at_least(2), 
                boost::bind(&Client::onFrameHeaderRead, this,
                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

        } else {
             AC_ERROR << error.message();
             // TODO: call onError, close connection
        }
    }
   
#if 0
      0                   1                   2                   3
      0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
     +-+-+-+-+-------+-+-------------+-------------------------------+
     |F|R|R|R| opcode|M| Payload len |    Extended payload length    |
     |I|S|S|S|  (4)  |A|     (7)     |             (16/64)           |
     |N|V|V|V|       |S|             |   (if payload len==126/127)   |
     | |1|2|3|       |K|             |                               |
     +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
     |     Extended payload length continued, if payload len == 127  |
     + - - - - - - - - - - - - - - - +-------------------------------+
     |                               |Masking-key, if MASK set to 1  |
     +-------------------------------+-------------------------------+
     | Masking-key (continued)       |          Payload Data         |
     +-------------------------------- - - - - - - - - - - - - - - - +
     :                     Payload Data continued ...                :
     + - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - +
     |                     Payload Data continued ...                |
     +---------------------------------------------------------------+
#endif    
    void 
    Client::onFrameHeaderRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            AC_TRACE << "Transferred " << bytes_transferred << " bytes";
            AC_TRACE << "Buffer contains " << _recv_buffer.size() << " bytes";
            // assume we have at least two bytes. Calculate how long the header really is
            boost::asio::streambuf::const_buffers_type bufs = _recv_buffer.data();

            boost::asio::buffers_iterator<boost::asio::streambuf::const_buffers_type> header = boost::asio::buffers_begin(bufs);
            Unsigned8 firstByte = *header;
            Unsigned8 secondByte = header[1];
            Unsigned8 tempPayloadLength = (secondByte & 0x7f);
            int payloadLength;
            
            bool payloadMask = (secondByte & 0x80);
            int frameHeaderSize = 2 + (tempPayloadLength == 126? 2 : 0) + (tempPayloadLength == 127? 6 : 0) + (payloadMask? 4 : 0);
            if (_recv_buffer.size() < frameHeaderSize) {
                AC_TRACE << "Need " << frameHeaderSize << " bytes, waiting for more.";
                boost::asio::async_read(_socket, _recv_buffer, boost::asio::transfer_at_least(frameHeaderSize-_recv_buffer.size()), 
                        boost::bind(&Client::onFrameHeaderRead, this,
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
                return;
            }
            // now calculate payload length
            int i;
            if (tempPayloadLength < 126) {
                payloadLength = tempPayloadLength;
                i = 2;
            }
            else if (tempPayloadLength == 126) {
                payloadLength = 0;
                payloadLength |= ((uint64_t) header[2]) << 8;
                payloadLength |= ((uint64_t) header[3]) << 0;
                i = 4;
            }
            else if (tempPayloadLength == 127) {
                payloadLength = 0;
                payloadLength |= ((uint64_t) header[2]) << 56;
                payloadLength |= ((uint64_t) header[3]) << 48;
                payloadLength |= ((uint64_t) header[4]) << 40;
                payloadLength |= ((uint64_t) header[5]) << 32;
                payloadLength |= ((uint64_t) header[6]) << 24;
                payloadLength |= ((uint64_t) header[7]) << 16;
                payloadLength |= ((uint64_t) header[8]) << 8;
                payloadLength |= ((uint64_t) header[9]) << 0;
                i = 10;
            }
            /*
            if (ws.mask) {
                ws.masking_key[0] = ((uint8_t) header[i+0]) << 0;
                ws.masking_key[1] = ((uint8_t) header[i+1]) << 0;
                ws.masking_key[2] = ((uint8_t) header[i+2]) << 0;
                ws.masking_key[3] = ((uint8_t) header[i+3]) << 0;
            }
            else {
                ws.masking_key[0] = 0;
                ws.masking_key[1] = 0;
                ws.masking_key[2] = 0;
                ws.masking_key[3] = 0;
            */
            AC_DEBUG << "Payload length is " << static_cast<unsigned int>(payloadLength) << " i=" << i;
            _recv_buffer.consume(frameHeaderSize);
            AC_TRACE << _recv_buffer.size() << " bytes left in buffer after reading header";
            boost::asio::async_read(_socket, _recv_buffer, boost::asio::transfer_at_least(payloadLength-_recv_buffer.size()), 
                    boost::bind(&Client::onPayloadRead, this,
                        boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

        } else {
             AC_ERROR << error.message();
             // TODO: call onError, close connection
        }
    }

    void 
    Client::onPayloadRead(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error) {
            AC_TRACE << "Transferred " << bytes_transferred << " bytes";
            AC_TRACE << "Payload Buffer contains " << _recv_buffer.size() << " bytes";
        } else {
             AC_ERROR << error.message();
             // TODO: call onError, close connection
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
        AC_TRACE << "~Client " << this;
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
        AC_TRACE << "cx:" << cx << ", this:" << theThisObject << ", obj:" << theObject << ", theName:" << theName << ", argc:" << argc << ", argv:" << argv << ", rval:" << rval;
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
