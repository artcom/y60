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

#ifndef _ac_y60_async_http_curl_socket_info_h
#define _ac_y60_async_http_curl_socket_info_h

#include <asl/base/Logger.h>
#include <curl/curl.h>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

namespace y60 {
namespace async {
namespace http {
    
class CurlSocketInfo : public boost::noncopyable, public boost::enable_shared_from_this<CurlSocketInfo> {
    public:
        typedef boost::shared_ptr<CurlSocketInfo> Ptr;
        CurlSocketInfo(CURLM * theCurlMultihandle);
        ~CurlSocketInfo();
        curl_socket_t native() { return boost_socket.native(); };
        boost::asio::ip::tcp::socket boost_socket;
        int readyState;
        bool read_in_progress;
        bool write_in_progress;
        void handleRead(const boost::system::error_code& error);
        void handleWrite(const boost::system::error_code& error);
        static void handleOperations(Ptr s, curl_socket_t theCurlSocket);
        static void add(Ptr s) {
            _allSockets.insert(std::make_pair(s->native(), s));
        };
        static void release(curl_socket_t t) {
            int n = _allSockets.erase(t);
            AC_TRACE << "erased " << n << " socket " << t << " from _allSockets.";
        };
        static Ptr find(curl_socket_t s) {
            std::map<curl_socket_t, Ptr>::iterator it = _allSockets.find(s);
            if (it != _allSockets.end()) {
                return it->second;
            }
            return Ptr();
        }
    private:
        CURLM * _curlMulti;
        CurlSocketInfo();
        static std::map<curl_socket_t, Ptr> _allSockets;
};

typedef CurlSocketInfo::Ptr SocketPtr;

}
}
}
#endif
