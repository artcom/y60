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
#include "HttpServer.h"
#include "NetAsync.h"

#include <string>
#include <y60/jsbase/JScppUtils.h>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace asl;
using namespace inet;

namespace y60 {
namespace async {
namespace http {
    
    Server::Server(JSContext * cx, boost::asio::io_service & theIOService) :
        _jsContext(cx),
        acceptor_(theIOService),
        new_connection_(new connection(theIOService, _myRequestQueue))
    {
    }

    Server::~Server()
    {
        AC_TRACE << "DTOR ~Server";
        close();
        _myCallbacks.clear();
    }
    
    bool Server::start( string theServerAddress, string theServerPort ) {
        AC_DEBUG << "starting http::Server on " << theServerAddress << ":" << theServerPort;
        boost::asio::ip::tcp::resolver resolver(acceptor_.get_io_service());
        boost::asio::ip::tcp::resolver::query query(theServerAddress, theServerPort);
        boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen();
        acceptor_.async_accept(new_connection_->socket(),
                boost::bind(&Server::handle_accept, this,
                    boost::asio::placeholders::error));
        
        asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName));
        boost::function<void()> handler = boost::bind(&Server::handleRequest, this);
        parentPlugin->registerHandler(this, handler);
        return true;
    }

    void Server::close() {
        acceptor_.close();
        asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(Singleton<PlugInManager>::get().getPlugIn(NetAsync::PluginName));
        parentPlugin->unregisterHandler(this);
    }

    void Server::invokeCallback( const JSCallback & theCallback, 
                                            const request & theRequest,
                                            reply & theReply) 
    {
        std::string myResponseString;
        try {
            jsval argv[3], rval;

            argv[0] = jslib::as_jsval(theCallback.context, theRequest.method);
            argv[1] = jslib::as_jsval(theCallback.context, theRequest.body);
            argv[2] = jslib::as_jsval(theCallback.context, theRequest.uri ); 

            jslib::JSA_CallFunctionValue(theCallback.context, theCallback.object, theCallback.functionValue, 
                                         3, argv, &rval);
            
            if (JSVAL_IS_VOID(rval)) {
                theReply.status = reply::no_content;
                return;
            }
            if (JSVAL_IS_OBJECT(rval)) {
                JSObject * myJsObject;
                if(JS_ValueToObject(theCallback.context, rval, &myJsObject)) {
                    if (JS_IsArrayObject(theCallback.context, myJsObject)) {
                        jsval curElement;
                        
                        // Body
                        JS_GetElement(theCallback.context, myJsObject, 0, &curElement);
                        if (!JSVAL_IS_VOID(curElement)) {
                            jslib::convertFrom(theCallback.context, curElement, theReply.content);
                        }
                        
                        // status
                        JS_GetElement(theCallback.context, myJsObject, 1, &curElement);
                        if (!JSVAL_IS_VOID(curElement)) {
                            int myStatusCode;
                            jslib::convertFrom(theCallback.context, curElement, myStatusCode);
                            theReply.status = static_cast<reply::status_type>(myStatusCode);
                        } else {
                            theReply.status = reply::ok;
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
                                             "Server::handleRequest: header_name is not a string!");
                                }
                                
                                jsval propval;
                                if (!JS_GetProperty(theCallback.context, headers, header_name.c_str(), &propval)) {
                                    AC_WARNING << "Weird case";
                                    continue;
                                }
                                
                                std::string header_value;
                                if (!jslib::convertFrom(theCallback.context, propval, header_value)) {
                                    JS_ReportError(theCallback.context, 
                                             "Server::handleRequest: header_value is not a a string!");
                                }
                                theReply.headers.insert(make_pair(header_name, header_value));
                            }
                        }
                        return;
                    }
                }
            }
            
            // default (backwards-compatible): treat jsval as string...
            if (!jslib::convertFrom(theCallback.context, rval, myResponseString)) {
                JS_ReportError(theCallback.context, 
                         "Server::handleRequest: Callback does not return a string!");
            }
            theReply.content = myResponseString;
            theReply.status  = reply::ok;
            theReply.headers.insert(make_pair("Content-Type", theCallback.contentType));
            return;
        } catch (Exception e) {
            theReply.status  = reply::internal_server_error;
            theReply.content = std::string("An internal error occured: ") + asl::compose_message(e);
            AC_ERROR << e;
            return;
        };
    }

    bool Server::requestsPending() {
        return !(_myRequestQueue.empty());
    }

    void Server::handleRequest() {
        try {
            request curRequest;
            while (_myRequestQueue.try_pop(curRequest)) {
                std::string myPath = curRequest.uri.substr(0, curRequest.uri.find_first_of("?"));  
            
                reply curReply;
            
                if (_myCallbacks.find(myPath) != _myCallbacks.end()) {
                    JSCallback myCallback = _myCallbacks[myPath]; 
                    invokeCallback( myCallback, curRequest, curReply);
                } else if (_myCallbacks.find("*") != _myCallbacks.end()) {
                    JSCallback myCallback = _myCallbacks["*"]; 
                    invokeCallback( myCallback, curRequest, curReply);
                } else {
                    curReply.status  = reply::not_found; 
                    AC_ERROR << "No callback registered for path: \"" << myPath << "\"!";
                }
                // Fill out the reply to be sent to the client.
                if (curReply.headers.find("Content-Length") == curReply.headers.end()) {
                    curReply.headers.insert(make_pair("Content-Length",
                                boost::lexical_cast<std::string>(curReply.content.size())));
                }
            
                if (curReply.headers.find("Content-Type") == curReply.headers.end()) {
                    curReply.headers.insert(make_pair("Content-Type", "text/plain"));
                }
                AC_TRACE << "JS Handler returned, queuing write for " << curRequest.conn;
                curRequest.conn->async_respond(curReply); // this is threadsafe
                curRequest.conn.reset(); // free shared_ptr
            }
        } catch (Exception e) {
            AC_ERROR << e;
        };
    }

    // Note: this will be called from the io_service thread.
    void Server::handle_accept(const boost::system::error_code& e) {
        if (!e)
        {
            new_connection_->start();
            new_connection_.reset(new connection(acceptor_.get_io_service(), _myRequestQueue));
            acceptor_.async_accept(new_connection_->socket(),
                    boost::bind(&Server::handle_accept, this,
                        boost::asio::placeholders::error));
        }
    }
}
}
}
