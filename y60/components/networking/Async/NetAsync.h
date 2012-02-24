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
*/


#ifndef _ac_y60_async_net_async_h
#define _ac_y60_async_net_async_h

#include "JSHttpServer.h"
#include "CurlMultiAdapter.h"

#include <boost/asio.hpp>

#include <asl/base/PlugInBase.h>
#include <y60/jsbase/IScriptablePlugin.h>
#include <y60/jslib/IRendererExtension.h>

namespace y60 {

	class NetAsync : 
        public asl::PlugInBase, 
        public IRendererExtension, 
        public jslib::IScriptablePlugin,
        public async::http::CurlMultiAdapter
    {
    public:
        typedef asl::Ptr<boost::thread, dom::ThreadingModel> AsioThreadPtr;
        typedef boost::function<void()> onFrameHandler;
        static const char * PluginName;

        NetAsync(asl::DLHandle theDLHandle);
        virtual ~NetAsync();
        static boost::asio::io_service & io_service();
        virtual void initClasses(JSContext * theContext, JSObject *theGlobalObject);
        virtual JSFunctionSpec * Functions();

        const char * ClassName() {
            return PluginName;
        }
        virtual void onStartup(jslib::AbstractRenderWindow * theWindow) {}
        virtual bool onSceneLoaded(jslib::AbstractRenderWindow * theWindow) { return true; }

        virtual void handle(jslib::AbstractRenderWindow * theWindow, y60::EventPtr theEvent) {}
        virtual void onFrame(jslib::AbstractRenderWindow * theWindow , double t);

        virtual void onPreRender(jslib::AbstractRenderWindow * theRenderer) {}
        virtual void onPostRender(jslib::AbstractRenderWindow * theRenderer) {}
        void registerHandler(const void * theInstance, onFrameHandler & theHandler) {
            _onFrameHandlers.insert(std::make_pair(theInstance, theHandler));
        }
        void unregisterHandler(const void * theInstance) {
            std::map<const void*, onFrameHandler>::iterator it = _onFrameHandlers.find(theInstance);
            if (it != _onFrameHandlers.end()) {
                _onFrameHandlers.erase(it);
            }
        }
    private:
        std::map<const void*, onFrameHandler> _onFrameHandlers;  
        void run(std::size_t thread_pool_size);
        void stop();
        /// The io_service used to perform asynchronous operations.
        static boost::asio::io_service io;
        // fictional work item to prevent our io_service from being out of work and terminating
        static boost::asio::io_service::work keep_busy;


    private:
        AsioThreadPtr _myAsioThread;
	};
}

#endif
