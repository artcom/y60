

#include "CurlSocketInfo.h"
#include "NetAsync.h"

namespace y60 {
namespace async {
namespace http {
    
std::map<curl_socket_t, SocketPtr> CurlSocketInfo::_allSockets;

CurlSocketInfo::CurlSocketInfo(CURLM * theCurlMultihandle) :
    boost_socket(NetAsync::io_service()),
    readyState(0),
    _curlMulti(theCurlMultihandle)
{ 
    AC_TRACE << "creating socket " << this;
    boost_socket.open(boost::asio::ip::tcp::v4());
    boost::asio::ip::tcp::socket::non_blocking_io non_blocking_io(true);
    boost_socket.io_control(non_blocking_io);
    AC_TRACE << "         socket is " << native();
};

CurlSocketInfo::~CurlSocketInfo() {
    AC_TRACE << "destroying socket " << this;
};

void
CurlSocketInfo::handleOperations(SocketPtr s, curl_socket_t theCurlSocket) {
    AC_TRACE << "handleOperations: socket " << theCurlSocket << " is " << s->readyState;
    if (!s->boost_socket.is_open()) {
        return;
    }
    // NOTE: this will be called from one of io_service's threads
    switch (s->readyState) {
        case CURL_POLL_OUT:
            if (s->write_in_progress.try_lock()) {
                AC_TRACE << "queuing write " << s;
                s->boost_socket.async_write_some(
                        boost::asio::null_buffers(),
                        boost::bind(&CurlSocketInfo::handleWrite, s,
                            boost::asio::placeholders::error));
            }
            break;
        case CURL_POLL_IN:
            if (s->read_in_progress.try_lock()) {
                AC_TRACE << "queuing read " << s;
                s->boost_socket.async_read_some(
                        boost::asio::null_buffers(),
                        boost::bind(&CurlSocketInfo::handleRead, s,
                            boost::asio::placeholders::error));

            }
            break;
        case CURL_POLL_REMOVE:
            // we don't need to do anything here. We'll keep the connection open 
            // so it can be reused by later clients
            break;
        default:
            throw asl::Exception("Unknown Socket State "+s->readyState); 
    };
}


void 
CurlSocketInfo::handleRead(const boost::system::error_code& error) {
    // NOTE: this will be called from one of io_service's threads
    read_in_progress.unlock();
    AC_TRACE << "doing read " << this;
    int i;
    CURLMcode myStatus = curl_multi_socket_action(_curlMulti, native(), CURL_CSELECT_IN, &i);
    CurlMultiAdapter::checkCurlStatus(myStatus, PLUS_FILE_LINE);
    AC_TRACE << " done read " << this;
    CurlSocketInfo::handleOperations(shared_from_this(), native());
};

void 
CurlSocketInfo::handleWrite(const boost::system::error_code& error) {
    // NOTE: this will be called from one of io_service's threads
    write_in_progress.unlock();
    AC_TRACE << "doing write " << this;
    int i;
    CURLMcode myStatus = curl_multi_socket_action(_curlMulti, native(), CURL_CSELECT_OUT, &i);
    CurlMultiAdapter::checkCurlStatus(myStatus, PLUS_FILE_LINE);
    AC_TRACE << " done write " << this;
    CurlSocketInfo::handleOperations(shared_from_this(), native());
};

}
}
}
