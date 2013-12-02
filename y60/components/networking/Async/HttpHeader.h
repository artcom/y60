//
// HttpHeader.h
// ~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_HEADER_HPP
#define HTTP_SERVER_HEADER_HPP

#include "y60_netasync_settings.h" 

#include <y60/jsbase/JSWrapper.h>
#include <string>
#include <map>

namespace y60 {
namespace async {
namespace http {

    class Headers : public std::map<std::string, std::string> {};

} // namespace http
} // namespace async
} // namespace y60

namespace jslib {
    Y60_NETASYNC_DECL jsval as_jsval(JSContext *cx, const y60::async::http::Headers theValue);
} // namespace jslib

#endif // HTTP_SERVER_HEADER_HPP
