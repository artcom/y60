/* __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
// Copyright (C) 1993-2001, ART+COM Berlin GmbH
//
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information of ART+COM GmbH Berlin, and
// are copy protected by law. They may not be disclosed to third parties
// or copied or duplicated in any form, in whole or in part, without the
// specific, prior written permission of ART+COM GmbH Berlin.
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
//
//    $RCSnumeric: test_numeric_functions.tst.cpp,v $
//
//   $Revision: 1.6 $
//
// Description: Test for Request
//
//
// __ ___ ____ _____ ______ _______ ________ _______ ______ _____ ____ ___ __
*/

#include "RequestManager.h"
#include "Request.h"

#include <asl/Time.h>
#include <asl/UnitTest.h>
#include <asl/ConduitAcceptor.h>
#include <asl/TCPPolicy.h>
#include <asl/net.h>
#include <iostream>
#include <sstream>

using namespace inet;
using namespace asl;
using namespace std;

#define DB(x) x


class TestRequest : public Request {
    public:
        TestRequest(const std::string & theURL, bool test=false) :
            Request(theURL), _myDataReceivedFlag(false),
        _myErrorCalledFlag(false), _myProgressCalledFlag(false),
        _myDoneCalledFlag(false) {};

    virtual size_t onData(const char * theData, size_t theReceivedByteCount) {
        _myDataReceivedFlag = true;
        return Request::onData(theData, theReceivedByteCount);
    };
    void onError(CURLcode theErrorCode) {
        cerr << "CURL ERROR: '" << getErrorString() << "' (CODE: "  << theErrorCode << ")" << endl;
        _myErrorCalledFlag = true;
        _myErrorCode = theErrorCode;
    };
    bool onProgress(double theDownloadTotal, double theCurrentDownload,
                double theUploadTotal, double theCurrentUpload) 
    {
        _myProgressCalledFlag = true;
        return true;
    };
    void onDone() {
        // cerr << "********************* RESPONSE **********************" << endl;
        // cerr << getResponseString() << endl;
        // cerr << "********************* RESPONSE END ******************" << endl;
        _myDoneCalledFlag = true;
    }
    bool _myDoneCalledFlag;
    bool _myDataReceivedFlag;
    bool _myErrorCalledFlag;
    bool _myProgressCalledFlag;
    bool _myTimeoutFlag;
    unsigned _myErrorCode;
};
typedef asl::Ptr<TestRequest> TestRequestPtr;

size_t eatWebsite(void* buffer, size_t size, size_t memb, void* userp) {
    return size*memb;
}

class TestServer : public ConduitServer<TCPPolicy> {
    public:
    static ConduitServer<TCPPolicy>::Ptr create(TCPPolicy::Handle theHandle) {
            return ConduitServer<TCPPolicy>::Ptr(new TestServer(theHandle));
        }
    private:
    TestServer(TCPPolicy::Handle theHandle) :
        ConduitServer<TCPPolicy>(theHandle) {}; 

    void sendSlowly(const std::string theData) {
        this->sendData(theData.c_str(), theData.length());
    }

    void sendResponseHeader(int theResponseCode) {
        std::stringstream myDate;
        myDate << Time();
        sendSlowly("HTTP/1.1 "+as_string(theResponseCode)+"\n");
        sendSlowly("Date: "+myDate.str()+"\n");
        sendSlowly("Server: ASL Conduit\n");
        sendSlowly("Content-Type: text/plain\n");
    }

    void sendResponseBody(const std::string & theBody) {
        sendSlowly("Content-Length: "+as_string(theBody.length())+"\n\n");
        sendSlowly(theBody);
    }

    std::string getUrl(const std::string & theRequest) {
        std::string::size_type getPos = theRequest.find("GET");
        std::string::size_type httpPos = theRequest.find("HTTP");
        std::string myUrl = theRequest.substr(getPos+5, httpPos-getPos-6);
        return myUrl;
    }
    
    virtual bool processData() {
        CharBuffer myInputBuffer;
        if (this->receiveData(myInputBuffer)) {
            std::string s(&myInputBuffer[0], myInputBuffer.size());
            std::string myURL = getUrl(s);
            if (myURL == "ShortRequest") {
                sendResponseHeader(200);
                sendResponseBody("SHORT");
            } else if (myURL == "LongRequest") {
                sendResponseHeader(200);
                sendResponseBody(string(65000,'L'));
            } else if (myURL == "Timeout") {
                asl::msleep(10*1000);
                sendResponseHeader(200);
                sendResponseBody("timeout");
            } else {
                sendResponseHeader(404);
                sendResponseBody("not found");
            }
            return false;
        }
        return true;
    }
};




class RequestTest : public UnitTest {

    public:
        // overloaded Request methods
        RequestTest() : UnitTest("RequestTest") {};
        
        void run() {
            const int PORT = 2346;
            inet::initSockets();
            ConduitAcceptor<TCPPolicy> myTestAcceptor(TCPPolicy::Endpoint("localhost",PORT), TestServer::create);
            myTestAcceptor.start();
            std::string myServer = "http://localhost:"+as_string(PORT);
            
            DPRINT(curl_version());
            RequestManager myRequestManager;
            TestRequestPtr myLongRequest = TestRequestPtr(new TestRequest(myServer+"/LongRequest"));
            myLongRequest->setVerbose(false);
            TestRequestPtr myShortRequest = TestRequestPtr(new TestRequest(myServer+"/ShortRequest"));
            TestRequestPtr myPageNotFoundRequest = TestRequestPtr(new TestRequest(myServer+"/404"));
            TestRequestPtr myNoServerRequest = TestRequestPtr(new TestRequest("http://server.invalid"));
            TestRequestPtr myServerTimeoutRequest = TestRequestPtr(new TestRequest(myServer+"/Timeout"));
            myServerTimeoutRequest->setTimeoutParams(1, 5); // 5 sec timeout

            /*
            TestRequestPtr myPostRequest = TestRequestPtr(new TestRequest("http://himmel/~martin/upload.php"));
            myPostRequest->addHttpHeader("X-Filename", "myFilename");
            myPostRequest->setVerbose(true);
            myPostRequest->post("Hello World");
            
            myRequestManager.performRequest(myPostRequest);
            */
            myRequestManager.performRequest(myNoServerRequest);
            myRequestManager.performRequest(myPageNotFoundRequest);
            myRequestManager.performRequest(myLongRequest);
            myRequestManager.performRequest(myShortRequest);
            myRequestManager.performRequest(myServerTimeoutRequest);

            {
                TestRequestPtr myFireAndForgetRequest = TestRequestPtr(new TestRequest("http://www.web.de"));
                myRequestManager.performRequest(myFireAndForgetRequest);
            }

            int myRunningCount = 0;
            do {
                myRunningCount = myRequestManager.handleRequests(); 
                msleep(10);
            } while (myRunningCount);

            ENSURE(myLongRequest->_myDataReceivedFlag);
            ENSURE(myLongRequest->_myErrorCalledFlag == false);
            ENSURE(myLongRequest->_myDoneCalledFlag == true);
            ENSURE(myLongRequest->getResponseCode() == 200);
            
            ENSURE(myShortRequest->_myDataReceivedFlag);
            ENSURE(myShortRequest->_myErrorCalledFlag == false);
            ENSURE(myShortRequest->_myDoneCalledFlag == true);
            ENSURE(myShortRequest->getResponseCode() == 200);
            
            ENSURE(myPageNotFoundRequest->_myDataReceivedFlag);
            ENSURE(myPageNotFoundRequest->_myErrorCalledFlag == false);
            ENSURE(myPageNotFoundRequest->_myDoneCalledFlag == true);
            ENSURE(myPageNotFoundRequest->getResponseCode() == 404);
            
            ENSURE(myNoServerRequest->_myDataReceivedFlag == false);
            ENSURE(myNoServerRequest->_myErrorCalledFlag == true);
            ENSURE(myNoServerRequest->_myDoneCalledFlag == false);
            DPRINT(myNoServerRequest->getResponseCode());
            ENSURE(myNoServerRequest->getResponseCode() == 0);
            
            ENSURE(myServerTimeoutRequest->_myDataReceivedFlag == false);
            ENSURE(myServerTimeoutRequest->_myErrorCalledFlag == true);
            ENSURE(myServerTimeoutRequest->_myErrorCode == CURLE_OPERATION_TIMEOUTED);
            ENSURE(myServerTimeoutRequest->_myDoneCalledFlag == false);
            DPRINT(myServerTimeoutRequest->getResponseCode());
            ENSURE(myServerTimeoutRequest->getResponseCode() == 0);

            ENSURE(myRequestManager.getActiveCount() ==0);
        }

        bool isInternetAvailable() {
            CURL * myCurl = curl_easy_init();
            curl_easy_setopt(myCurl, CURLOPT_URL, "http://www.artcom.de");
            curl_easy_setopt(myCurl, CURLOPT_WRITEFUNCTION, eatWebsite);
            CURLcode res = curl_easy_perform(myCurl);

            bool isOk= (res == CURLE_OK ||
                res == CURLE_TOO_MANY_REDIRECTS);
            curl_easy_cleanup(myCurl);

            return isOk;
        }
};


class MyTestSuite : public UnitTestSuite {
public:
    MyTestSuite(const char * myName) : UnitTestSuite(myName) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new RequestTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0]);

    try {
        mySuite.run();
    } catch (const asl::Exception & ex) {
        cerr << "Exception during test execution: " << ex << endl;
        return -1;
    }

    cerr << ">> Finished test suite '" << argv[0] << "'"
         << ", return status = " << mySuite.returnStatus() << endl;

    return mySuite.returnStatus();

}
