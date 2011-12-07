//
// Request.h
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_REQUEST_HPP
#define HTTP_SERVER_REQUEST_HPP

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "HttpHeader.h"

namespace y60 {
namespace async {
namespace http {

class connection;

/// A request received from a client.
struct request
{
  std::string method;
  std::string uri;
  int http_version_major;
  int http_version_minor;
  std::vector<header> headers;
  std::string body;
  boost::shared_ptr<connection> conn;
};

} // namespace http
} // namespace async
} // namespace y60

#endif // HTTP_SERVER_REQUEST_HPP
