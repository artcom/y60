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

#include "RequestManager.h"
#include "Request.h"
#include "TestServer.h"

#include <asl/base/Time.h>
#include <asl/base/UnitTest.h>
#include <asl/net/net.h>
#include <asl/base/file_functions.h>
#include <asl/base/os_functions.h>
#include <iostream>
#include <sstream>

using namespace inet;
using namespace asl;
using namespace std;

#define DB(x) // x

class TestRequest : public Request {
    public:
        TestRequest(const std::string & theURL, bool test=false)
        : Request(theURL),
        _myDoneCalledFlag(false),
        _myDataReceivedFlag(false),
        _myErrorCalledFlag(false),
        _myProgressCalledFlag(false),
        _myTimeoutFlag(false),
        _myErrorCode(CURLcode(0)),
        _myHttpStatus(0)
    {}

    virtual size_t onData(const char * theData, size_t theReceivedByteCount) {
        _myDataReceivedFlag = true;
        return Request::onData(theData, theReceivedByteCount);
    };
    void onError(CURLcode theError, long theHttpStatus) {
        cerr << "CURL ERROR: '" << getErrorString() << "' (CODE: "  << theError << "), HTTP status " << theHttpStatus << endl;
        _myErrorCalledFlag = true;
        _myErrorCode = theError;
        _myHttpStatus = theHttpStatus;
    };
    bool onProgress(double theDownloadTotal, double theCurrentDownload,
                    double theUploadTotal, double theCurrentUpload) {
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
    CURLcode _myErrorCode;
    long _myHttpStatus;
};
typedef asl::Ptr<TestRequest> TestRequestPtr;

size_t eatWebsite(void* buffer, size_t size, size_t memb, void* userp) {
    return size*memb;
}

class DateTest : public UnitTest {

    public:
        DateTest() : UnitTest("DateTest") {};

        void run() {
            string myDateString = "Wed, 02 Aug 2006 14:16:26 GMT";
            time_t myTime = Request::getTimeFromHTTPDate(myDateString);
            DPRINT(myDateString);
            DPRINT(myTime);
            struct tm * myTimeStruct = gmtime(&myTime);
            char myBuffer[128];
            strftime(myBuffer, 128, "%a, %d %b %Y %H:%M:%S GMT", myTimeStruct);
            string myTimeString(myBuffer);
            // this failed in curl 7.12, windows
            ENSURE_EQUAL(myTimeString, myDateString);
        }
};

class RequestTest : public UnitTest {

    public:
        // overloaded Request methods
        RequestTest() : UnitTest("RequestTest") {};

        void run() {
            const asl::Unsigned16 PORT = asl::getenv<asl::Unsigned16>("TEST_PORT", 2346);

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
            TestRequestPtr myUTF8Request = TestRequestPtr(new TestRequest(myServer+"/utf8"));
            myUTF8Request->setVerbose(true);
            TestRequestPtr myServerTimeoutRequest = TestRequestPtr(new TestRequest(myServer+"/Timeout"));
            myServerTimeoutRequest->setTimeoutParams(1, 5); // 5 sec timeout

            /*
            TestRequestPtr myBaseAuthentRequest = TestRequestPtr(
                    new TestRequest("http://bacon.intern.artcom.de/testBaseAuthent/index.html"));
            myBaseAuthentRequest->setCredentials("Aladdin", "open sesame", BASIC);

            TestRequestPtr myPostRequest = TestRequestPtr(new TestRequest("http://himmel/~martin/upload.php"));
            myPostRequest->addHttpHeader("X-Filename", "myFilename");
            myPostRequest->setVerbose(true);
            myPostRequest->post("Hello World");

            myRequestManager.performRequest(myPostRequest);
            myRequestManager.performRequest(myBaseAuthentRequest);
            */
            myRequestManager.performRequest(myNoServerRequest);
            myRequestManager.performRequest(myUTF8Request);
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
            /*
            ENSURE(myBaseAuthentRequest->_myDataReceivedFlag);
            ENSURE(myBaseAuthentRequest->_myErrorCalledFlag == false);
            ENSURE(myBaseAuthentRequest->_myDoneCalledFlag == true);
            ENSURE(myBaseAuthentRequest->getResponseCode() == 200);
            */
            ENSURE(myLongRequest->_myDataReceivedFlag);
            ENSURE(myLongRequest->_myErrorCalledFlag == false);
            ENSURE(myLongRequest->_myDoneCalledFlag == true);
            ENSURE(myLongRequest->getResponseCode() == 200);

            ENSURE(myShortRequest->_myDataReceivedFlag);
            ENSURE(myShortRequest->_myErrorCalledFlag == false);
            ENSURE(myShortRequest->_myDoneCalledFlag == true);
            ENSURE(myShortRequest->getResponseCode() == 200);
            ENSURE(myShortRequest->getResponseString().size());

            ENSURE(myPageNotFoundRequest->_myDataReceivedFlag);
            ENSURE(myPageNotFoundRequest->_myErrorCalledFlag == true);
            ENSURE(myPageNotFoundRequest->_myDoneCalledFlag == false);
            ENSURE(myPageNotFoundRequest->getResponseCode() == 404);
            ENSURE(myPageNotFoundRequest->_myHttpStatus == myPageNotFoundRequest->getResponseCode());

            ENSURE(myNoServerRequest->_myDataReceivedFlag == false);
            ENSURE(myNoServerRequest->_myErrorCalledFlag == true);
            ENSURE(myNoServerRequest->_myDoneCalledFlag == false);
            DPRINT(myNoServerRequest->getResponseCode());
            ENSURE(myNoServerRequest->getResponseCode() == 0);
            ENSURE(myNoServerRequest->_myHttpStatus == myNoServerRequest->getResponseCode());

            DPRINT(myUTF8Request->getResponseString().size());
            ENSURE(myUTF8Request->getResponseString().size() == 2);

            ENSURE(myServerTimeoutRequest->_myDataReceivedFlag == false);
            ENSURE(myServerTimeoutRequest->_myErrorCalledFlag == true);
            ENSURE(myServerTimeoutRequest->_myErrorCode == CURLE_OPERATION_TIMEOUTED);
            ENSURE(myServerTimeoutRequest->_myDoneCalledFlag == false);
            DPRINT(myServerTimeoutRequest->getResponseCode());
            ENSURE(myServerTimeoutRequest->getResponseCode() == 0);
            ENSURE(myServerTimeoutRequest->_myHttpStatus == myServerTimeoutRequest->getResponseCode());

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
    MyTestSuite(const char * myName, int argc, char *argv[]) : UnitTestSuite(myName, argc, argv) {}
    void setup() {
        UnitTestSuite::setup(); // called to print a launch message
        addTest(new RequestTest);
        addTest(new DateTest);
    }
};


int main(int argc, char *argv[]) {

    MyTestSuite mySuite(argv[0], argc, argv);

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
