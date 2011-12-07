//
// MimeTypes.h
// ~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_SERVER_MIME_TYPES_HPP
#define HTTP_SERVER_MIME_TYPES_HPP

#include <string>

namespace y60 {
namespace async {
namespace mime_types {

/// Convert a file extension into a MIME type.
std::string extension_to_type(const std::string& extension);

} // namespace mime_types
} // namespace async
} // namespace y60

#endif // HTTP_SERVER_MIME_TYPES_HPP
