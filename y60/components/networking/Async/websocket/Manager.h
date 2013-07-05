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
*/

#ifndef _ac_y60_async_websocket_manager_h
#define _ac_y60_async_websocket_manager_h

#include "../y60_netasync_settings.h" 

#include <asl/base/Logger.h>

#include <set>
#include <boost/asio.hpp>

namespace y60 {
namespace async {
namespace websocket {
    
class Client; // forward declaration

class Manager {
    public:
        friend class SocketAdapter;
        Manager(boost::asio::io_service & theIOService);
        virtual ~Manager();
        void addClient(boost::shared_ptr<Client> theClient);
        void removeClient(boost::shared_ptr<Client> theClient);
        void processCompleted();
        void processCallbacks();
        void shutdown();
    private:
        Manager();
        // curl stuff

        std::set<boost::shared_ptr<Client> > _allClients;
        void onTimeout(const boost::system::error_code& error);

        // Boost IO stuff
        boost::asio::io_service & io; // owned by NetAsync
        boost::asio::io_service::strand _strand;
};

}
}
}

#endif
