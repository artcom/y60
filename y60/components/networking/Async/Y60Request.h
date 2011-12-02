#ifndef Y60_REQUEST_RESPONSE
#define Y60_REQUEST_RESPONSE

#include <asl/base/Ptr.h>
#include <asl/dom/ThreadingModel.h>
#include "Reply.h"
#include "ConcurrentQueue.h"
#include "HttpHeader.h"

namespace y60 {

    struct Y60Request {
        std::string uri;
        std::string method;
        std::string body;
    };

    struct Y60Response {
        std::string payload;
        std::string content_type;
        http::server::reply::status_type return_code;
        std::vector<http::server::header> headers;
    };

    typedef ConcurrentQueue<Y60Request> Y60RequestQueue;
    typedef ConcurrentQueue<Y60Response> Y60ResponseQueue;

    typedef asl::Ptr<ConcurrentQueue<Y60Request>,dom::ThreadingModel> Y60RequestQueuePtr;
    typedef asl::Ptr<ConcurrentQueue<Y60Response>,dom::ThreadingModel> Y60ResponseQueuePtr;

}

#endif