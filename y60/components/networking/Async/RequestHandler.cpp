//
// request_handler.cpp
// ~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2008 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "RequestHandler.h"
#include <fstream>
#include <sstream>
#include <string>
#include <boost/lexical_cast.hpp>
#include "MimeTypes.h"
#include "Reply.h"
#include "Request.h"
#include <y60/base/CommonTags.h>


namespace http {
    namespace server {

        request_handler::request_handler(const y60::Y60RequestQueuePtr & theRequestQueue)
              : _myRequestQueue(theRequestQueue)
        {
        }

        void request_handler::handle_request(const request& req)
        {
            // Decode url to path.
            std::string request_path;
            if (!url_decode(req.uri, request_path))
            {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            // Request path must be absolute and not contain "..".
            if (request_path.empty() || request_path[0] != '/'
                    || request_path.find("..") != std::string::npos)
            {
                rep = reply::stock_reply(reply::bad_request);
                return;
            }

            // If path ends in slash (i.e. is a directory) then add "index.html".
            if (request_path[request_path.size() - 1] == '/')
            {
                request_path += "index.html";
            }

            // Determine the file extension.
            std::size_t last_slash_pos = request_path.find_last_of("/");
            std::size_t last_dot_pos = request_path.find_last_of(".");
            std::string extension;
            if (last_dot_pos != std::string::npos && last_dot_pos > last_slash_pos)
            {
                extension = request_path.substr(last_dot_pos + 1);
            }

            y60::Y60Request myRequest;
            myRequest.uri    = request_path;
            myRequest.method = req.method;
            myRequest.body   = req.body;

            _myRequestQueue->push( myRequest );
            /*
             * TODO move to response
             *
            y60::Y60Response myResponse;
            _myResponseQueue->wait_and_pop(myResponse);

            // Fill out the reply to be sent to the client.
            rep.status = reply::ok;
            rep.content.append(myResponse.payload.c_str(), myResponse.payload.length());
            rep.status = myResponse.return_code;
            rep.headers.clear();
            
            bool hasContentLengthFlag = false;
            bool hasContentTypeFlag = false;
            for (std::vector<header>::size_type i = 0; i < myResponse.headers.size(); ++i) {
                const header &myHeader = myResponse.headers[i];
                hasContentLengthFlag = hasContentLengthFlag || (myHeader.name == "Content-Length");
                hasContentTypeFlag   = hasContentTypeFlag || (myHeader.name == "Content-Type");
                rep.headers.push_back(myHeader);
            }
            
            if (!hasContentLengthFlag) {
                rep.headers.push_back(header("Content-Length",
                                             boost::lexical_cast<std::string>(rep.content.size())));
            }
            
            if (!hasContentTypeFlag) {
                header myContentType("Content-Type", "text/html");
                if ( myResponse.content_type.length() > 0 ) {
                    myContentType.value = myResponse.content_type;
                } else {
                    myContentType.value = mime_types::extension_to_type(extension);
                }
                rep.headers.push_back(myContentType);
            }
            */
        }
        
        bool request_handler::url_decode(const std::string& in, std::string& out)
        {
            out.clear();
            out.reserve(in.size());
            for (std::size_t i = 0; i < in.size(); ++i)
            {
                if (in[i] == '%')
                {
                    if (i + 3 <= in.size())
                    {
                        int value;
                        std::istringstream is(in.substr(i + 1, 2));
                        if (is >> std::hex >> value)
                        {
                            out += static_cast<char>(value);
                            i += 2;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }
                }
                else if (in[i] == '+')
                {
                    out += ' ';
                }
                else
                {
                    out += in[i];
                }
            }
            return true;
        }
        
    } // namespace server
} // namespace http
