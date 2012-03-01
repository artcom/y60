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

#include "MultiAdapter.h"
#include "Client.h"
#include "../NetAsync.h"

#include <asl/base/Logger.h>

using namespace asl;

namespace y60 {
namespace async {
namespace http {
namespace curl {
        


MultiAdapter::MultiAdapter(boost::asio::io_service & theIOService) :
    io(theIOService),
    _strand(theIOService)
{
    _curlMulti = curl_multi_init(); 
    CURLMcode myStatus = curl_multi_setopt(_curlMulti, CURLMOPT_SOCKETFUNCTION, &MultiAdapter::curl_socket_callback);
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
    myStatus = curl_multi_setopt(_curlMulti, CURLMOPT_SOCKETDATA, this); 
    checkCurlStatus(myStatus, PLUS_FILE_LINE);

    myStatus = curl_multi_setopt(_curlMulti, CURLMOPT_TIMERFUNCTION, &MultiAdapter::curl_timer_callback);
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
    myStatus = curl_multi_setopt(_curlMulti, CURLMOPT_TIMERDATA, this); 
    checkCurlStatus(myStatus, PLUS_FILE_LINE);

};
MultiAdapter::~MultiAdapter() {
    AC_TRACE << "~MultiAdapter done";
};

void
MultiAdapter::shutdown() {
    AC_TRACE << "MultiAdapter::shutdown";
    
    while (!_allClients.empty()) {
        (*_allClients.begin())->onDone(CURLE_ABORTED_BY_CALLBACK );
        removeClient(*_allClients.begin());
    }
    
    SocketAdapter::abort();
    CURLMcode myStatus = curl_multi_cleanup(_curlMulti); 
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
    if (timeout_timer) {
        timeout_timer->cancel();
    }
    AC_TRACE << "MultiAdapter::shutdown done";
};

void
MultiAdapter::checkCurlStatus(CURLMcode theStatusCode, const std::string & theWhere) {
    if (theStatusCode != CURLM_OK) {
        throw asl::Exception(curl_multi_strerror(theStatusCode), theWhere);
    }
};

void 
MultiAdapter::addClient(boost::shared_ptr<Client> theClient) { 
    AC_DEBUG << "adding client " << theClient;
    CURLMcode myStatus = curl_multi_add_handle(_curlMulti,  theClient->_curlHandle);
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
    _allClients.insert(theClient);
    //int i;
    //myStatus = curl_multi_socket_action(_curlMulti, 0, 0, &i);
    //checkCurlStatus(myStatus, PLUS_FILE_LINE);
};
void 
MultiAdapter::removeClient(boost::shared_ptr<Client> theClient ){ 
    AC_DEBUG << "removeClient client " << theClient;
    CURLMcode myStatus = curl_multi_remove_handle(_curlMulti,  theClient->_curlHandle); 
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
    _allClients.erase(theClient);
};


int 
MultiAdapter::curl_socket_callback(CURL *easy, /* easy handle */   
                               curl_socket_t theCurlSocket, /* socket */   
                               int action, /* see values below */   
                               void *userp, /* private callback pointer */   
                               void *socketp) /* private socket pointer */ 
{
    Client * curClient = 0;
    curl_easy_getinfo(easy, CURLINFO_PRIVATE, &curClient);
    AC_DEBUG << "Curl Socket "<< theCurlSocket << " Callback: " << action << " on " << userp << "," << curClient;
    SocketAdapter::Ptr s = SocketAdapter::find(theCurlSocket);
    if (s) {
        s->readyState = action;
        SocketAdapter::handleOperations(s, theCurlSocket);
    }
    return 0;
};

int 
MultiAdapter::curl_timer_callback(CURLM *multi,  long timeout_ms, MultiAdapter * self) {
    AC_TRACE << "multi_timer_cb: Setting timeout to " << timeout_ms << " ms";
    if ( ! self->timeout_timer) {
        self->timeout_timer = boost::shared_ptr<boost::asio::deadline_timer>(new boost::asio::deadline_timer(self->io)); 
    }
    self->timeout_timer->expires_from_now(boost::posix_time::milliseconds(timeout_ms));
    // self->onTimeout(boost::asio::error::operation_aborted);
    self->timeout_timer->async_wait(self->_strand.wrap(bind(&MultiAdapter::onTimeout, self, boost::asio::placeholders::error)));
    return 0;
};

void
MultiAdapter::onTimeout(const boost::system::error_code& error) {
    AC_TRACE << "onTimeout " << error;
    if (error != 0) {
        return;
    }
    int i;
    CURLMcode myStatus = curl_multi_socket_action(_curlMulti, CURL_SOCKET_TIMEOUT, 0, &i);
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
};

void
MultiAdapter::processCallbacks() {
    for (std::set<boost::shared_ptr<Client> >::iterator it = _allClients.begin(); it != _allClients.end(); ++it) {
        (*it)->onProgress();
    }
};

void 
MultiAdapter::setSocketInfo(curl_socket_t s, void * data) {
    CURLMcode myStatus = curl_multi_assign(_curlMulti, s, data);
    checkCurlStatus(myStatus, PLUS_FILE_LINE);
};

void
MultiAdapter::processCompleted() {
    // take care of completed requests
    int myMessageCount = 0;
    CURLMsg * myMessage = 0;
    do {
        myMessage = curl_multi_info_read(_curlMulti, &myMessageCount);
        if (myMessage) {
            CURL * myEasyHandle = myMessage->easy_handle;
            Client * curClient = 0;
            curl_easy_getinfo(myEasyHandle, CURLINFO_PRIVATE, &curClient);
            if (myMessage->msg == CURLMSG_DONE) {
                AC_DEBUG << "calling onDone for " << curClient;
                curClient->onDone(myMessage->data.result);
                removeClient(curClient->shared_from_this());
            } else {
                throw asl::Exception("Unknown CURL message encountered");
            }
        }
    } while (myMessage);
};


}
}
}
}
