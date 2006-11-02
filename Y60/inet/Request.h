//=============================================================================
// Copyright (C) 1993-2006, ART+COM AG Berlin
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM AG Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM AG Berlin.
//=============================================================================

#ifndef _Y60_INET_REQUEST_INCLUDED_
#define _Y60_INET_REQUEST_INCLUDED_

#ifdef WIN32
    #include <winsock2.h>
    // undefine the windows macro max (thanx bill) [jb]
    #undef max
#endif

#include <asl/Exception.h>
#include <asl/Enum.h>
#include <asl/Revision.h>
#include <asl/Block.h>
#include <asl/Ptr.h>

#include <string>
#include <curl/curl.h>
#include <vector>
#include <map>

namespace inet {

    DEFINE_EXCEPTION(INetException, asl::Exception);

    enum AuthentTypeEnum {
        BASIC,
        DIGEST,
        ANY,
        AuthentTypeEnum_MAX
    };
    
    static const char * AuthentTypeStrings[] = {
        "BASIC",
        "DIGEST",
        "ANY",
        ""
    };

    DEFINE_ENUM(AuthentType, AuthentTypeEnum);

    class RequestManager;

    /**
     * @ingroup y60inet
     * HTTP Request.
     *
     */    class Request {
        friend class RequestManager;

        public:
            Request(const std::string & theURL, const std::string & theUserAgent = std::string("Y60/")+asl::ourRevision);
            virtual ~Request();
            CURL * getHandle() const;
            long getResponseCode() const;
            std::string getResponseString() const;
            const asl::Block & getResponseBlock() const;
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
            void setCookie(const std::string & theCookie, bool theSessionCookieFlag = false);
            void setResume(long theResumeOffset);
            void setProxy(const std::string & theProxyServer, bool theTunnelFlag = false);
            void setCredentials(const std::string & theUsername, const std::string & thePassword,
                                AuthentType theAuthentType = ANY);
            void setFollowLocation(bool theFollowFlag);
            
            // request-method type methods
            size_t post(const std::string & thePostData);
            size_t postFile(const std::string & theFilename);
            size_t postBlock(const asl::Ptr<asl::ReadableBlock> & theBlock);
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
            virtual void onError(CURLcode theCode);
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
            void checkCurlStatus(CURLcode theStatusCode, const std::string & theWhere) const ;
            std::string         _myURL;
            std::string         _myUserAgent;
            CURL *              _myCurlHandle;
            unsigned            _myLowSpeedLimit;
            unsigned            _myLowSpeedTimeout;
            struct curl_slist * _myHttpHeaderList;
            std::string         _myPostBuffer;
            asl::Ptr<asl::ReadableBlock> _myPostBlock;
            asl::Block          _myResponseBlock;
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
