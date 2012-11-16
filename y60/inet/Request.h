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
*/

#ifndef _Y60_INET_REQUEST_INCLUDED_
#define _Y60_INET_REQUEST_INCLUDED_

#include "y60_inet_settings.h"

#ifdef _WIN32
    #include <winsock2.h>
    // undefine the windows macro max (thanx bill) [jb]
    #undef max
#endif

#include <asl/base/buildinfo.h>
#include <asl/base/Exception.h>
#include <asl/base/Enum.h>
#include <asl/base/Block.h>
#include <asl/base/Ptr.h>

#include <string>
#include <curl/curl.h>
#include <vector>
#include <map>

namespace inet {

    typedef std::map<std::string, std::string> CookieJar;

    DEFINE_EXCEPTION(INetException, asl::Exception);

    enum AuthentTypeEnum {
        BASIC,
        DIGEST,
        ANY,
        AuthentTypeEnum_MAX
    };

    const char * const AuthentTypeStrings[] = {
        "BASIC",
        "DIGEST",
        "ANY",
        ""
    };

    DEFINE_ENUM(AuthentType, AuthentTypeEnum, Y60_INET_DECL);

    class RequestManager;

    /**
     * @ingroup y60inet
     * HTTP Request.
     *
     */
    class Y60_INET_DECL Request {
        friend class RequestManager;

        public:
            Request(const std::string & theURL, const std::string & theUserAgent = std::string("Y60/")+asl::build_information::get().executable().history_id());
            virtual ~Request();
            CURL * getHandle() const;
            long getResponseCode() const;
            std::string getResponseString() const;
            const asl::Block & getResponseBlock() const;
            asl::Ptr<asl::Block> getResponseBlockPtr() const;
            std::string getErrorString() const;
            const std::string & getURL() const;
            void setLowSpeedLimit(unsigned theBytesPerSec);
            void setLowSpeedTimeout(unsigned theSeconds);
            void setTimeoutParams(unsigned theBytesPerSec, unsigned theSeconds);
            void setVerbose(bool theVerboseFlag);
            bool getVerbose() const { return _myVerboseFlag; }
            bool getVerifyPeer() const { return _myVerifyPeerFlag; }
            void setSSLCertificate(const std::string & theCertificateFilename);
            void verifyPeer(bool theFlag);
            void setCookies(const CookieJar & theCookies);
            void setCookie(const std::string & theCookie);
            void setResume(long theResumeOffset);
            void setProxy(const std::string & theProxyServer, bool theTunnelFlag = false);
            void setCredentials(const std::string & theUsername, const std::string & thePassword,
                                AuthentType theAuthentType = ANY);
            void setFollowLocation(bool theFollowFlag);
            void verifySSLPeer(bool theVerifyFlag);

            // request-method type methods
            size_t put(const std::string & thePutData);
            size_t putBlock(const asl::Ptr<asl::Block> & theBlock);
            size_t post(const std::string & thePostData);
            size_t postFile(const std::string & theFilename);
            size_t postBlock(const asl::Ptr<asl::ReadableBlock> & theBlock);
            void http_delete();
            void get();
            void head();

            const std::multimap<std::string,std::string> & getResponseHeaders() const;
            std::string getResponseHeader(const std::string & theHeader) const;
            std::vector<std::string> getAllResponseHeaders(const std::string & theHeader) const;
            time_t getResponseHeaderAsDate(const std::string & theHeader) const;
            unsigned getLowSpeedLimit(void) const;
            unsigned getLowSpeedTimeout(void) const;

            void addHttpHeader(const std::string & theKey, const std::string & theValue);
            void addHttpHeaderAsDate(const std::string & theKey, const time_t & theValue);

            //TODO: I'm not sure if this function is in the right place...
            static std::string urlEncode(const std::string & theUrl);
            static std::string urlDecode(const std::string & theUrl);

            static time_t getTimeFromHTTPDate(const std::string & theHTTPDate );

        protected:
            // callback hooks
            virtual void onStart();
            virtual size_t onData(const char * theData, size_t theLength);
            virtual void onError(CURLcode theError, long theHttpStatus);
            virtual bool onProgress(double theDownloadTotal, double theCurrentDownload,
                double theUploadTotal, double theCurrentUpload);  // return false to abort transfer
            virtual void onDone();
            virtual bool onResponseHeader(const std::string & theHeader);

        private:
            // static callback hooks
            static size_t curlWriteCallback( void *theData, size_t theBlockCount,
                                    size_t theBlockSize, void *theRequestObject);
            static int curlProgressCallback( void *theRequestObject, double theDownloadTotal,
                                    double theCurrentDownload, double theUploadTotal,
                                    double theCurrentUpdate);
            static size_t curlHeaderCallback( void *theData, size_t theBlockCount,
                                    size_t theBlockSize, void *theRequestObject);
            //
            Request();
            void checkCurlStatus(CURLcode theStatusCode, const std::string & theWhere) const;

        private:
            std::string         _myURL;
            std::string         _myProxy;
            std::string         _myUserAgent;
            CURL *              _myCurlHandle;
            unsigned            _myLowSpeedLimit;
            unsigned            _myLowSpeedTimeout;
            struct curl_slist * _myHttpHeaderList;
            std::string         _myPostBuffer;
            asl::Ptr<asl::ReadableBlock> _myPostBlock;
            asl::Ptr<asl::Block> _myResponseBlock;
            std::vector<char>   _myErrorBuffer;
            std::string         _mySSLCertificateFilename;
            std::multimap<std::string, std::string> _myResponseHeaders;
            std::vector<std::string> _myHttpHeaderBuffers;
            std::string         _myCookieBuffer;
            std::string         _myAuthentData;
            bool                _myVerboseFlag;
            bool                _myVerifyPeerFlag;
    };

    typedef asl::Ptr<Request> RequestPtr;
    typedef asl::WeakPtr<Request> RequestWeakPtr;

}

#endif
