

#include "SocketAdapter.h"
#include "../NetAsync.h"

namespace y60 {
namespace async {
namespace http {
namespace curl {
    
std::map<curl_socket_t, SocketPtr> SocketAdapter::_allSockets;

SocketAdapter::SocketAdapter(MultiAdapter * pParent, CURLM * theCurlMultihandle) :
    boost_socket(pParent->io),
    readyState(0),
    read_in_progress(false),
    write_in_progress(false),
    _parent(pParent)
{ 
    AC_DEBUG << "creating socket " << this;
    boost_socket.open(boost::asio::ip::tcp::v4());
    boost::asio::ip::tcp::socket::non_blocking_io non_blocking_io(true);
    boost_socket.io_control(non_blocking_io);
    AC_TRACE << "         socket is " << native();
};

SocketAdapter::~SocketAdapter() {
    AC_TRACE << "SocketAdapter::DTOR socket " << this;
};

void
SocketAdapter::handleOperations(SocketPtr s, curl_socket_t theCurlSocket) {
    AC_TRACE << "handleOperations: socket " << theCurlSocket << " is " << s->readyState;
    if (!s->boost_socket.is_open()) {
        return;
    }
    // NOTE: this will be called from one of io_service's threads
    switch (s->readyState) {
        case CURL_POLL_OUT:
            if (s->write_in_progress == false) {
                s->write_in_progress = true;
                AC_TRACE << "queuing write " << s->native();
                s->boost_socket.async_write_some(
                        boost::asio::null_buffers(),
                        s->_parent->_strand.wrap(
                        boost::bind(&SocketAdapter::handleWrite, s,
                            boost::asio::placeholders::error)));
            }
            break;
        case CURL_POLL_IN:
            if (s->read_in_progress == false) {
                s->read_in_progress = true;
                AC_TRACE << "queuing read " << s->native();
                s->boost_socket.async_read_some(
                        boost::asio::null_buffers(),
                        s->_parent->_strand.wrap(
                        boost::bind(&SocketAdapter::handleRead, s,
                            boost::asio::placeholders::error)));

            }
            break;
        case CURL_POLL_INOUT:
            if (s->read_in_progress == false) {
                s->read_in_progress = true;
                AC_TRACE << "queuing read " << s->native();
                s->boost_socket.async_read_some(
                        boost::asio::null_buffers(),
                        s->_parent->_strand.wrap(
                        boost::bind(&SocketAdapter::handleRead, s,
                            boost::asio::placeholders::error)));

            }
            if (s->write_in_progress == false) {
                s->write_in_progress = true;
                AC_TRACE << "queuing write " << s->native();
                s->boost_socket.async_write_some(
                        boost::asio::null_buffers(),
                        s->_parent->_strand.wrap(
                        boost::bind(&SocketAdapter::handleWrite, s,
                            boost::asio::placeholders::error)));
            }
            break;
        case CURL_POLL_REMOVE:
            // we don't need to do anything here. We'll keep the connection open 
            // so it can be reused by later clients
            break;
        default:
            AC_ERROR << "readyState: " << s->readyState;
            throw asl::Exception("Unknown Socket State "+asl::as_string(s->readyState)); 
    };
}


void 
SocketAdapter::handleRead(const boost::system::error_code& error) {
    Ptr self(shared_from_this());
    // NOTE: this will be called from one of io_service's threads
    AC_TRACE << "  doing read " << this << " on socket " << native() << " with error " << error;
    read_in_progress = false;
    if (error != 0) {
        if (error == boost::asio::error::operation_aborted) {
            AC_TRACE << "Read aborted";
        } else {
            AC_TRACE << "Read aborted with unknown error " << error;
        }
        return;
    }
    int i;
    CURLMcode myStatus = curl_multi_socket_action(_parent->_curlMulti, native(), CURL_CSELECT_IN, &i);
    MultiAdapter::checkCurlStatus(myStatus, PLUS_FILE_LINE);
    AC_TRACE << "   done read " << this << " socket " << native();
    SocketAdapter::handleOperations(self, native());
};

void 
SocketAdapter::handleWrite(const boost::system::error_code& error) {
    Ptr self(shared_from_this());
    // NOTE: this will be called from one of io_service's threads
    AC_TRACE << "  doing write " << this << " with error " << error << " socket is " << native();
    write_in_progress = false;
    if (boost_socket.is_open() && error == 0) {
        int i;
        CURLMcode myStatus = curl_multi_socket_action(_parent->_curlMulti, native(), CURL_CSELECT_OUT, &i);
        MultiAdapter::checkCurlStatus(myStatus, PLUS_FILE_LINE);
        AC_TRACE << "   done write " << this;
    }

    if (boost_socket.is_open() && error == 0) {
        SocketAdapter::handleOperations(self, native());
    } 
};

}
}
}
}
