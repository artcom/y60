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

#include <string>

namespace y60 {
namespace async {
namespace http {

struct header
{
    header(const std::string &theName, const std::string &theValue):
        name(theName), value(theValue) {}
  std::string name;
  std::string value;
};



} // namespace http
} // namespace async
} // namespace y60

#endif // HTTP_SERVER_HEADER_HPP
