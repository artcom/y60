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

#include "NetAsync.h"
#include "JSHttpClient.h"
#include <y60/jsbase/JSScriptablePlugin.h>
#include <y60/jsbase/JSWrapper.h>

using namespace jslib;
using namespace y60;

namespace y60 {
    
static JSClass Package = {
    "Package",
    JSCLASS_HAS_PRIVATE,
    JS_PropertyStub, JS_PropertyStub,
    JS_PropertyStub, JS_PropertyStub,
    JS_EnumerateStub, JS_ResolveStub,
    JS_ConvertStub, JS_FinalizeStub
};
    	
NetAsync::NetAsync(asl::DLHandle theDLHandle) : 
                asl::PlugInBase(theDLHandle),
                IRendererExtension(ClassName()) 
{
    _myAsioThread = AsioThreadPtr(new boost::thread( boost::bind( &NetAsync::run, this, 10) ) );
    _curlMulti = curl_multi_init(); 
    curl_multi_setopt(_curlMulti, CURLMOPT_SOCKETFUNCTION, &NetAsync::curl_socket_callback); 
    curl_multi_setopt(_curlMulti, CURLMOPT_SOCKETDATA, this); 
};

NetAsync::~NetAsync() {
    stop();
    curl_multi_cleanup(_curlMulti); 
};

boost::asio::io_service & 
NetAsync::io_service() {
    return io;
};

int 
NetAsync::curl_socket_callback(CURL *easy, /* easy handle */   
                               curl_socket_t s, /* socket */   
                               int action, /* see values below */   
                               void *userp, /* private callback pointer */   
                               void *socketp) /* private socket pointer */ 
{
    async::http::Client * curClient = 0;
    curl_easy_getinfo(easy, CURLINFO_PRIVATE, &curClient);
    AC_DEBUG << "Curl Socket "<< s << " Callback: " << action << " on " << userp << "," << curClient;
    curClient->onSocketState(action);
    return 0;
}
void 
NetAsync::addClient(async::http::Client * theClient) { 
    AC_DEBUG << "adding client " << theClient;
    curl_multi_add_handle(_curlMulti,  theClient->_curlHandle);
    int i;
    curl_multi_socket_action(_curlMulti, theClient->_socket.native_handle(), 0, &i);
};
void 
NetAsync::removeClient(async::http::Client * theClient ){ 
    AC_DEBUG << "removeClient client " << theClient;
    if (theClient->_socket.is_open()) {
        AC_DEBUG << "    shutting down " << theClient;
        theClient->_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both);
        theClient->_socket.close();
    }
    // curl_multi_remove_handle(_curlMulti,  theClient->_curlHandle); 
};
void 
NetAsync::doSocketRead(curl_socket_t theSocket) {
    int i;
    AC_TRACE << "start curl_multi_socket_action(IN)";
    curl_multi_socket_action(_curlMulti, theSocket, CURL_CSELECT_IN, &i);
    AC_TRACE << " done curl_multi_socket_action(IN)";
};
void 
NetAsync::doSocketWrite(curl_socket_t theSocket) {
    int i;
    AC_DEBUG << "start curl_multi_socket_action(OUT)";
    curl_multi_socket_action(_curlMulti, theSocket, CURL_CSELECT_OUT, &i);
    AC_DEBUG << " done curl_multi_socket_action(OUT)";
};

void 
NetAsync::initClasses(JSContext * theContext, JSObject *theGlobalObject) {
    IScriptablePlugin::initClasses(theContext, theGlobalObject);
    // start javascript namespace
    JSObject *asyncNamespace = JS_DefineObject(theContext, theGlobalObject, "Async", &Package, NULL, JSPROP_PERMANENT | JSPROP_READONLY);
    JSA_AddFunctions(theContext, asyncNamespace, Functions());
    JSHttpServer::initClass(theContext, asyncNamespace);
    JSHttpClient::initClass(theContext, asyncNamespace);
};

void
NetAsync::run(std::size_t thread_pool_size) {
    AC_DEBUG << "starting asio threads";
    
    // Create a pool of threads to run all of the io_services.
    std::vector<boost::shared_ptr<boost::thread> > threads;
    for (std::size_t i = 0; i < thread_pool_size; ++i)
    {
        boost::shared_ptr<boost::thread> thread(new boost::thread(
                    boost::bind(&boost::asio::io_service::run, &io)));
        threads.push_back(thread);
    }

    // Wait for all threads in the pool to exit.
    for (std::size_t i = 0; i < threads.size(); ++i)
        threads[i]->join();

    AC_DEBUG << "asio threads terminated";
};

void 
NetAsync::onFrame(jslib::AbstractRenderWindow * theWindow , double t) {
    std::map<const void*, onFrameHandler>::iterator it;
    for (it = _onFrameHandlers.begin(); it != _onFrameHandlers.end(); ++it) {
        (it->second)();
    }
    
    // take care of completed requests
    int myMessageCount = 0;
    CURLMsg * myMessage = 0;
    do {
        myMessage = curl_multi_info_read(_curlMulti, &myMessageCount);
        if (myMessage) {
            CURL * myEasyHandle = myMessage->easy_handle;
            async::http::Client * curClient = 0;
            curl_easy_getinfo(myEasyHandle, CURLINFO_PRIVATE, &curClient);
            if (myMessage->msg == CURLMSG_DONE) {
                AC_DEBUG << "calling onDone for " << curClient;
                curClient->onDone();
            } else {
                AC_ERROR << "curl request failed with error: " << curl_easy_strerror(myMessage->data.result);
                // myRequest->onError(myResult, 0);
            }
            curl_multi_remove_handle(_curlMulti,  curClient->_curlHandle); 
        }
    } while (myMessage);

};


void 
NetAsync::stop() {
    io.stop();
};

static JSBool
OnFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("Triggers any waiting JS callbacks. Will be called automatically if a render window is rendering."); DOC_END;
    asl::Ptr<NetAsync> parentPlugin = dynamic_cast_Ptr<NetAsync>(asl::Singleton<asl::PlugInManager>::get().getPlugIn(NetAsync::PluginName));
    parentPlugin->onFrame(0,0);
    return JS_TRUE;
}

static JSBool
GetExtension(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval) {
    DOC_BEGIN("returns a IRendererExtension suitable for adding to a RenderWindow."); DOC_END;
    IScriptablePluginPtr parentPlugin = dynamic_cast_Ptr<IScriptablePlugin>(asl::Singleton<asl::PlugInManager>::get().getPlugIn(NetAsync::PluginName));
    *rval = as_jsval(cx, parentPlugin);
    return JS_TRUE;
}

JSFunctionSpec *
NetAsync::Functions() {
    IF_REG(cerr << "Registering class '"<<ClassName()<<"'"<<endl);
    static JSFunctionSpec myFunctions[] = {
        // name                  native                   nargs
        {"onFrame",             OnFrame,                0},
        {"getExtension",        GetExtension,           0},
        {0}
    };
    return myFunctions;
}

};

// static initializer
boost::asio::io_service y60::NetAsync::io;
boost::asio::io_service::work y60::NetAsync::keep_busy(io);
const char * y60::NetAsync::PluginName = "NetAsync";

extern "C"
EXPORT asl::PlugInBase * NetAsync_instantiatePlugIn(asl::DLHandle myDLHandle) {
	return new y60::NetAsync(myDLHandle);
}
